module Core.Compiler;

import std;
import Core.Lib;
import <cassert>;
import Core.Exceptions;

namespace Core {
    void SourceCompiler::AddLibToState(sol::state &state) {
        Lib::AddLibToState(state);
        Exceptions::AddLibToState(state);
        TokenStream::AddLibToState(state);

        state.new_usertype<SourceCompiler>("SourceCompiler",
                                           "GetCompilerContext", &SourceCompiler::GetCompilerContext,
                                           "GetLinkerContext", &SourceCompiler::GetLinkerContext,
                                           "GetTokenStream", &SourceCompiler::GetTokenStream,
                                           "GetBitBuffer", &SourceCompiler::GetBitBuffer,
                                           "WriteBit", &SourceCompiler::WriteBit,
                                           "WriteBits", &SourceCompiler::WriteBits,
                                           "WriteNumber", &SourceCompiler::WriteNumber,
                                           "GetBitBufferSize", &SourceCompiler::GetBitBufferSize
        );
    }

    void SourceCompiler::WriteBit(bool bit) {
        m_BitBuffer.push_back(bit);
    }

    void SourceCompiler::WriteBits(const std::vector<bool> &bits) {
        m_BitBuffer.insert(m_BitBuffer.end(), bits.begin(), bits.end());
    }

    void SourceCompiler::WriteNumber(uint64_t number, size_t bits) {
        if (std::log2(number) >= bits) {
            throw Exceptions::CompileError(
                std::format("Compile Error({}): Number {} exceeds the bit limit of {} bits",
                            m_TokenStream.GetApproxCurrentLocation(), number, bits));
        }
        for (size_t i = 0; i < bits; ++i) {
            m_BitBuffer.push_back((number >> (bits - 1 - i)) & 1);
        }
    }

    size_t SourceCompiler::GetBitBufferSize() const {
        return m_BitBuffer.size();
    }

    bool SourceCompiler::CompileOneLine() {
        auto token = m_TokenStream.PeekCurrent();
        if (!token)
            return true;

        auto unwrapped = std::move(token.value());
        if (!m_InstructionProcessorMap->contains(unwrapped)) {
            m_InstructionProcessorMap->at("Compiler@NonInstructionHandler")(*this);
        } else {
            m_TokenStream.SkipCurrent();
            m_InstructionProcessorMap->at(unwrapped)(*this);
        }

        return false;
    }

    void SourceCompiler::Link() {
        m_Linker(*this);
    }

    Compiler::Compiler(const std::filesystem::path &languageRootDir) {
        YAML::Node config{};
        try {
            config = YAML::LoadFile((languageRootDir
                                     / "Language_Specification.yaml").string());
        } catch (std::exception &e) {
            throw std::runtime_error("Failed to load configuration file");
        }

        // initialize state

        m_SharedState = CreateSharedState();

        std::string languageLoadPath =
                ParseConfigOptional<std::string>(
                    config, "LanguageLoadPath").value_or("LanguageInstructionLib");

        InitParticularState(std::filesystem::directory_iterator(
            languageRootDir / languageLoadPath));

        InitNameToFunctionMap(config);

        InitLinker(config);
    }

    std::shared_ptr<sol::state> Compiler::CreateSharedState() {
        auto state = std::make_shared<sol::state>(sol::state{});
        state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::string);

        SourceCompiler::AddLibToState(*state);

        return state;
    }

    void Compiler::InitParticularState(std::filesystem::directory_iterator languageRootDir) {
        for (const auto &luaSource: languageRootDir) {
            if (luaSource.is_regular_file() && luaSource.path().extension() == ".lua") {
                m_SharedState->script_file(luaSource.path().string());
            }
        }
    }

    void Compiler::InitNameToFunctionMap(const YAML::Node &config) {
        std::unordered_map<std::string, std::string> instructionToLuaFunctionNameMap
                = ParseConfigOrThrow<std::unordered_map<std::string, std::string>>(
                    config, "InstructionToLuaFunctionNameMap");

        std::unordered_map<
            std::string,
            std::function<void(SourceCompiler &)>> instructionProcessorMap;

        for (auto &&[key, value]: std::move(instructionToLuaFunctionNameMap)) {
            instructionProcessorMap[Core::Lib::ToLowerCase(key)] =
                    [value, sharedState = m_SharedState](
                SourceCompiler &compiler) {
                        sol::function function = sharedState->get<sol::function>(value);
                        if (!function.valid()) {
                            throw std::runtime_error("Function not found: " + value);
                        }
                        auto exception = function(compiler);
                        if (!exception.valid()) {
                            sol::error err = exception;
                            throw std::runtime_error(
                                "Error executing function '" + value + "': " +
                                std::string(err.what()));
                        } else {
                            sol::type resultType = exception.get_type();
                            if (resultType != sol::type::nil) {
                                if (resultType == sol::lua_type_of_v<Exceptions::WrappedGenericException>) {
                                    exception.get<Exceptions::WrappedGenericException>().ThrowIfNotNull();
                                }
                            }
                        }
                    };
        }

        std::string NonInstructionHandlerName =
                ParseConfigOptional<std::string>(config, "NonInstructionHandlerName")
                        .value_or("ProcessNonInstruction");

        instructionProcessorMap["Compiler@NonInstructionHandler"] =
            [NonInstructionHandlerName, sharedState = m_SharedState](
                SourceCompiler &compiler) {
                sol::function function = sharedState->get<sol::function>(NonInstructionHandlerName);
                if (!function.valid()) {
                    throw std::runtime_error("Function not found: " + NonInstructionHandlerName);
                }
                auto exception = function(compiler);
                if (!exception.valid()) {
                    sol::error err = exception;
                    throw std::runtime_error(
                        "Error executing non-instruction handler '" + NonInstructionHandlerName + "': " +
                        std::string(err.what()));
                } else {
                    sol::type resultType = exception.get_type();
                    if (resultType != sol::type::nil) {
                        if (resultType == sol::lua_type_of_v<Exceptions::WrappedGenericException>) {
                            exception.get<Exceptions::WrappedGenericException>().ThrowIfNotNull();
                        }
                    }
                }
            };

        m_InstructionProcessorMap =
                std::make_shared<const std::unordered_map<
                    std::string,
                    std::function<void(SourceCompiler &)>>>(std::move(instructionProcessorMap));
    }

    void Compiler::InitLinker(const YAML::Node &config) {
        auto linkerName = ParseConfigOptional<std::string>(config, "LinkerName")
                .value_or("Linker");

        m_Linker = [linkerName](SourceCompiler &compiler) {
            sol::function function = compiler.m_SharedState->get<sol::function>(linkerName);
            if (!function.valid()) {
                throw std::runtime_error("Linker function not found: " + linkerName);
            }
            auto exception = function(compiler);
            if (!exception.valid()) {
                sol::error err = exception;
                throw std::runtime_error(
                    "Error executing linker '" + linkerName + "': " +
                    std::string(err.what()));
            } else {
                sol::type resultType = exception.get_type();
                if (resultType != sol::type::nil) {
                    if (resultType == sol::lua_type_of_v<Exceptions::WrappedGenericException>) {
                        exception.get<Exceptions::WrappedGenericException>().ThrowIfNotNull();
                    }
                }
            }
        };
    }

    SourceCompiler Compiler::CreateSourceCompiler(std::string source) const {
        return SourceCompiler{
            m_SharedState,
            m_InstructionProcessorMap,
            m_Linker,
            std::move(source),
        };
    }
}
