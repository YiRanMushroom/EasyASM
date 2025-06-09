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
                                           "WriteSignedNumber", &SourceCompiler::WriteSignedNumber,
                                           "WriteUnsignedNumber", &SourceCompiler::WriteUnsignedNumber,
                                           "GetBitBufferSize", &SourceCompiler::GetBitBufferSize,
                                           "AlignStartAddress", &SourceCompiler::AlignStartAddress,
                                           "ReplaceUnsignedNumber", &SourceCompiler::ReplaceUnsignedNumber
        );
    }

    void SourceCompiler::WriteBit(bool bit) {
        m_BitBuffer.push_back(bit);
    }

    void SourceCompiler::WriteBits(const std::vector<bool> &bits) {
        m_BitBuffer.insert(m_BitBuffer.end(), bits.begin(), bits.end());
    }

    void SourceCompiler::WriteSignedNumber(int64_t number, size_t bits) {
        int64_t min_value = -(1ll << (bits - 1));
        int64_t max_value = (1ll << (bits - 1)) - 1;

        if (number < min_value || number > max_value) {
            throw Exceptions::CompileError(
                std::format("Compile Error({}): Signed number {} exceeds the bit limit of {} bits (valid range: [{}, {}])",
                            m_TokenStream.GetApproxCurrentLocation(), number, bits, min_value, max_value));
        }

        // Two's complement encoding, masked to lower 'bits' bits
        uint64_t encoded = static_cast<uint64_t>(number) & ((1ull << bits) - 1);

        for (size_t i = 0; i < bits; ++i) {
            m_BitBuffer.push_back((encoded >> i) & 1);  // LSB first
        }
    }

    void SourceCompiler::WriteUnsignedNumber(uint64_t number, size_t bits) {
        if (number >= (1ull << bits)) {
            throw Exceptions::CompileError(
                std::format("Compile Error({}): Number {} exceeds the bit limit of {} bits",
                            m_TokenStream.GetApproxCurrentLocation(), number, bits));
        }

        for (size_t i = 0; i < bits; ++i) {
            bool bit = (number >> i) & 1;  // LSB first
            m_BitBuffer.push_back(bit);
        }
    }

    void SourceCompiler::ReplaceUnsignedNumber(uint64_t number, size_t bits, size_t startIndex) {
        if (number >= (1ull << bits)) {
            throw Exceptions::CompileError(
                std::format("Compile Error({}): Number {} exceeds the bit limit of {} bits",
                            m_TokenStream.GetApproxCurrentLocation(), number, bits));
        }

        if (startIndex + bits > m_BitBuffer.size()) {
            throw Exceptions::CompileError(
                std::format("Compile Error({}): Attempted to replace bits beyond the current buffer size",
                            m_TokenStream.GetApproxCurrentLocation()));
        }

        for (size_t i = 0; i < bits; ++i) {
            m_BitBuffer[startIndex + i] = (number >> i) & 1;  // LSB first
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
        auto lower = Lib::ToLowerCase(unwrapped);
        if (!m_NameToFunctionMap->contains(lower)) {
            m_NameToFunctionMap->at("Compiler@NonInstructionHandler")(*this);
        } else {
            m_TokenStream.SkipCurrent();
            m_NameToFunctionMap->at(lower)(*this);
        }

        return false;
    }

    void SourceCompiler::CompileAll() {
        m_NameToFunctionMap->at("Compiler@BeforeCompile")(*this);

        while (!CompileOneLine()) {}
    }

    void SourceCompiler::Link() {
        m_NameToFunctionMap->at("Compiler@BeforeLink")(*this);
        m_NameToFunctionMap->at("Compiler@Linker")(*this);
        m_NameToFunctionMap->at("Compiler@AfterLink")(*this);
    }

    void SourceCompiler::AlignStartAddress() {
        while (m_BitBuffer.size() % m_StartAddressAlignment != 0) {
            m_BitBuffer.push_back(false); // pad with zeros to align
        }
    }

    std::string SourceCompiler::GenerateOutput() {
        m_NameToFunctionMap->at("Compiler@Output")(*this);

        return m_Output.value();
    }

    Compiler::Compiler(const std::filesystem::path &languageRootDir) {
        YAML::Node config{};
        try {
            config = YAML::LoadFile((languageRootDir
                                     / "Language_Specification.yaml").string());
        } catch (std::exception &e) {
            throw std::runtime_error("Failed to load configuration file");
        }

        m_SharedState = CreateSharedState();

        m_StartAddressAlignment = ParseConfigOptional<size_t>(
            config, "StartAddressAlignment").value_or(1);

        std::string languageLoadPath =
                ParseConfigOptional<std::string>(
                    config, "LanguageLoadPath").value_or("LanguageInstructionLib");

        InitParticularState(std::filesystem::directory_iterator(
            languageRootDir / languageLoadPath));

        InitNameToFunctionMap(config);

        m_SharedConfig = std::make_shared<YAML::Node>(std::move(config));
    }

    std::shared_ptr<sol::state> Compiler::CreateSharedState() {
        auto state = std::make_shared<sol::state>(sol::state{});
        state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::bit32);

        SourceCompiler::AddLibToState(*state);

        return state;
    }

    void Compiler::InitParticularState(std::filesystem::directory_iterator languageRootDir) {
        for (const auto &luaSourceOrSubDir: languageRootDir) {
            if (luaSourceOrSubDir.is_regular_file() && luaSourceOrSubDir.path().extension() == ".lua") {
                m_SharedState->script_file(luaSourceOrSubDir.path().string());
            } else if (luaSourceOrSubDir.is_directory()) {
                // Recursively load Lua files in subdirectories
                InitParticularState(std::filesystem::directory_iterator(luaSourceOrSubDir.path()));
            }
        }
    }

    void HandlePossibleLuaError(const auto &exception, const std::string &value) {
        if (exception.get_type() == sol::lua_type_of_v<Exceptions::WrappedGenericException>) {
            exception.get<Exceptions::WrappedGenericException>().ThrowIfNotNull();
        } else {
            if (exception.get_type() != sol::type::none) {
                throw Exceptions::CompilerImplementationError(
                    std::format(
                        "Function '{}' did not return none or an exception, this is probably a bug in the implementation. Please report this issue to your compiler vendor.",
                        value));
            }
        }
    }

    void CheckFunctionValid(const sol::function &function) {
        if (!function.valid()) {
            throw Exceptions::CompilerImplementationError(
                "Function not found in shared state, this is a compiler implementation error. Please report this to your compiler vendor.");
        }
    }

    void CheckResultValid(const auto &result, const std::string &functionName) {
        if (!result.valid()) {
            sol::error err = result;
            throw Exceptions::CompilerImplementationError(
                std::format(
                    "Lua failed when executing function '{}', this is a compiler implementation error. Please report this to your compiler vendor:\n{}",
                    functionName, err.what()));
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
            instructionProcessorMap[Lib::ToLowerCase(key)] =
                    [value, function = m_SharedState->get<sol::function>(value)](
                SourceCompiler &compiler) {
                        CheckFunctionValid(function);

                        auto exception = function(compiler);

                        CheckResultValid(exception, value);

                        HandlePossibleLuaError(exception, value);
                    };
        }

        std::string NonInstructionHandlerName =
                ParseConfigOptional<std::string>(config, "NonInstructionHandlerName")
                .value_or("ProcessNonInstruction");

        instructionProcessorMap["Compiler@NonInstructionHandler"] =
                [NonInstructionHandlerName, function = m_SharedState->get<sol::function>(NonInstructionHandlerName)](
            SourceCompiler &compiler) {
                    CheckFunctionValid(function);

                    auto exception = function(compiler);

                    CheckResultValid(exception, NonInstructionHandlerName);

                    HandlePossibleLuaError(exception, NonInstructionHandlerName);
                };

        m_NameToFunctionMap =
                std::make_shared<std::unordered_map<
                    std::string,
                    std::function<void(SourceCompiler &)>>>(std::move(instructionProcessorMap));

        InitLinker(config);
        InitEventFunctions(config);
        InitOutputFunction(config);
    }

    void Compiler::InitOutputFunction(const YAML::Node &config) {
        auto outputFunctionName = ParseConfigOptional<std::string>(config, "OutputFunctionName")
                .value_or("GenerateOutput");

        (*m_NameToFunctionMap)["Compiler@Output"] =
                [outputFunctionName, function = m_SharedState->get<sol::function>(outputFunctionName)](
            SourceCompiler &compiler) {
                    CheckFunctionValid(function);

                    auto result = function(compiler);

                    CheckResultValid(result, outputFunctionName);

                    if (result.get_type() == sol::type::string) {
                        compiler.m_Output = result.get<std::string>();
                        return;
                    }

                    HandlePossibleLuaError(result, outputFunctionName);

                    throw Exceptions::CompilerImplementationError(
                        std::format(
                            "Output function '{}' did not return a string, this is probably a bug in the implementation. Please report this issue to your compiler vendor.",
                            outputFunctionName));
                };
    }

    void Compiler::InitLinker(const YAML::Node &config) {
        auto linkerName = ParseConfigOptional<std::string>(config, "LinkerName")
                .value_or("Linker");

        (*m_NameToFunctionMap)["Compiler@Linker"] =
                [linkerName, function = m_SharedState->get<sol::function>(linkerName)](
            SourceCompiler &compiler) {
                    CheckFunctionValid(function);

                    auto exception = function(compiler);

                    CheckResultValid(exception, linkerName);

                    HandlePossibleLuaError(exception, linkerName);
                };
    }

    void Compiler::InitEventFunctions(const YAML::Node &config) {
        auto BeforeCompileFunctionName =
                ParseConfigOptional<std::string>(config, "BeforeCompileFunctionName");

        auto BeforeLinkFunctionName =
                ParseConfigOptional<std::string>(config, "BeforeLinkFunctionName");

        auto AfterLinkFunctionName =
                ParseConfigOptional<std::string>(config, "AfterLinkFunctionName");

        if (BeforeCompileFunctionName) {
            (*m_NameToFunctionMap)["Compiler@BeforeCompile"] =
                    [function = m_SharedState->get<sol::function>(*BeforeCompileFunctionName), BeforeCompileFunctionName
                    ](
                SourceCompiler &compiler) {
                        CheckFunctionValid(function);

                        auto exception = function(compiler);

                        CheckResultValid(exception, *BeforeCompileFunctionName);

                        HandlePossibleLuaError(exception, *BeforeCompileFunctionName);
                    };
        } else {
            (*m_NameToFunctionMap)["Compiler@BeforeCompile"] = [](SourceCompiler &) {};
        }

        if (BeforeLinkFunctionName) {
            (*m_NameToFunctionMap)["Compiler@BeforeLink"] =
                    [function = m_SharedState->get<sol::function>(*BeforeLinkFunctionName), BeforeLinkFunctionName](
                SourceCompiler &compiler) {
                        CheckFunctionValid(function);

                        auto exception = function(compiler);

                        CheckResultValid(exception, *BeforeLinkFunctionName);

                        HandlePossibleLuaError(exception, *BeforeLinkFunctionName);
                    };
        } else {
            (*m_NameToFunctionMap)["Compiler@BeforeLink"] = [](SourceCompiler &) {};
        }

        if (AfterLinkFunctionName) {
            (*m_NameToFunctionMap)["Compiler@AfterLink"] =
                    [function = m_SharedState->get<sol::function>(*AfterLinkFunctionName), AfterLinkFunctionName](
                SourceCompiler &compiler) {
                        CheckFunctionValid(function);

                        auto exception = function(compiler);

                        CheckResultValid(exception, *AfterLinkFunctionName);

                        HandlePossibleLuaError(exception, *AfterLinkFunctionName);
                    };
        } else {
            (*m_NameToFunctionMap)["Compiler@AfterLink"] = [](SourceCompiler &) {};
        }
    }

    SourceCompiler Compiler::CreateSourceCompiler(std::string source) const {
        return SourceCompiler{
            m_SharedState,
            m_NameToFunctionMap,
            std::move(source),
            m_StartAddressAlignment,
            m_SharedConfig
        };
    }
}
