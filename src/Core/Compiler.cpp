module Core.Compiler;

import std;
import Core.Lib;

namespace Core {
    void SourceCompiler::AddLibToState(sol::state &state) {
        auto Lib= state.create_named_table("Lib");
        Lib.set_function("ToLowerCase", &Core::Lib::ToLowerCase);
    }

    Compiler::Compiler(const std::filesystem::path &languageRootDir)  {
            YAML::Node config{};
            try {
                config = YAML::LoadFile((languageRootDir
                                         / "Language_Specification.yaml").string());
            } catch (std::exception &e) {
                throw std::runtime_error("Failed to load configuration file");
            }

            m_PerInstructionBits = ParseConfigOrThrow<size_t>(config, "PerInstructionBits");

            InitMacroDefineTokens(config);

            // initialize state

            m_SharedState = CreateSharedState();

            std::string languageLoadPath =
                ParseConfigOptional<std::string>(
                config,
                "LanguageLoadPath").value_or("LanguageInstructionLib");

            InitParticularState(std::filesystem::directory_iterator(
                languageRootDir / languageLoadPath));

            InitNameToFuntionMap(config);
        }

    std::shared_ptr<sol::state> Compiler::CreateSharedState() {
        auto state = std::make_shared<sol::state>(sol::state{});
        state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::string);

        SourceElements::Token::AddLibToState(*state);
        ASMElements::InstructionBinary::AddLibToState(*state);
        SourceCompiler::AddLibToState(*state);

        return state;
    }

    void Compiler::InitParticularState(std::filesystem::directory_iterator languageRootDir) {
        for (const auto &luaSource : languageRootDir) {
            if (luaSource.is_regular_file() && luaSource.path().extension() == ".lua") {
                m_SharedState->script_file(luaSource.path().string());
            }
        }
    }

    void Compiler::InitNameToFuntionMap(const YAML::Node &config) {
        std::unordered_map<std::string, std::string> instructionToLuaFunctionNameMap
                    = ParseConfigOrThrow<std::unordered_map<std::string, std::string>>(
                        config, "InstructionToLuaFunctionNameMap");

                std::unordered_map<
                    SourceElements::Token,
                    std::function<std::expected<ASMElements::InstructionBinary, std::string>(
                        std::span<SourceElements::Token> const &)>> instructionProcessorMap;

                for (auto&& [key, value] : std::move(instructionToLuaFunctionNameMap)) {
                    instructionProcessorMap[Core::Lib::ToLowerCase(key)] =
                        [sharedState = m_SharedState, value = std::move(value)](
                            std::span<SourceElements::Token> const &tokens) -> std::expected<ASMElements::InstructionBinary, std::string> {
                            sol::function luaFunction = (*sharedState)[value];
                            auto result = luaFunction(tokens);
                            if (result.valid()) {
                                sol::type type = result.get_type();

                                if (type == sol::lua_type_of_v<ASMElements::InstructionBinary>) {
                                    return result.get<ASMElements::InstructionBinary>();
                                }

                                if (type == sol::type::string) {
                                    auto errMsg = result.get<std::string>();
                                    return std::unexpected(errMsg);
                                }
                                return std::unexpected("Unexpected Lua function return type");
                            }
                            return std::unexpected("Lua function call failed");
                        };
                }

                m_InstructionProcessorMap =
                    std::make_shared<const std::unordered_map<
                        SourceElements::Token,
                        std::function<std::expected<ASMElements::InstructionBinary, std::string>(
                            std::span<SourceElements::Token> const &)>>>(std::move(instructionProcessorMap));
    }

    void Compiler::InitMacroDefineTokens(const YAML::Node &config) {
        auto macroDefineTokens = ParseConfigOrThrow<std::vector<std::string>>(
            config, "MacroDefineTokens");

        m_MacroDefineTokens = std::make_shared<const std::unordered_set<SourceElements::Token>>(
            std::move(macroDefineTokens)
            | std::views::transform([](std::string &token) {
                return SourceElements::Token{std::move(token)};
            }) | std::ranges::to<std::unordered_set<SourceElements::Token>>()
        );
    }
}
