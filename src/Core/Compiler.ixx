module;

export module Core.Compiler;

import std;
import Vendor.yaml;
import Vendor.sol;
import Core.SourceElements.Token;
import Core.InstructionBinary;

namespace Core {
    std::string AllLowerCase(std::string str) {
        std::ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
        return str;
    }

    export class SourceCompiler {
    private:
        SourceCompiler() = default;

    public:
        SourceCompiler(size_t perInstructionBits,
                       std::shared_ptr<const std::unordered_map<SourceElements::Token,
                           SourceElements::Token>> preProcessorTokenMap,
                       std::shared_ptr<const std::unordered_map<SourceElements::Token,
                           std::function<
                               std::expected<InstructionBinary, std::string>
                               (const std::span<SourceElements::Token> &)>>> instructionProcessorMap,
                       std::shared_ptr<const std::unordered_set<SourceElements::Token>> macroDefineTokens)
            : m_PerInstructionBits(perInstructionBits),
              m_PreProcessorTokenMap(std::move(preProcessorTokenMap)),
              m_InstructionProcessorMap(std::move(instructionProcessorMap)),
              m_MacroDefineTokens(std::move(macroDefineTokens)) {
        }

    private:
        size_t m_PerInstructionBits;
        // std::shared_ptr<const sol::state> m_SharedState;
        std::shared_ptr<const std::unordered_map<SourceElements::Token,
            SourceElements::Token>> m_PreProcessorTokenMap;

        std::shared_ptr<const std::unordered_set<SourceElements::Token>>
        m_MacroDefineTokens;

        std::shared_ptr<const std::unordered_map<
        SourceElements::Token,
        std::function<std::expected<InstructionBinary, std::string>(
        std::span<SourceElements::Token> const &)>>>
        m_InstructionProcessorMap;
    };

    export class Compiler {
    public:
        Compiler(const std::filesystem::path &languageRootDir) {
            YAML::Node config{};
            try {
                config = YAML::LoadFile((languageRootDir
                                         / "Language_Specification.yaml").string());
            } catch (std::exception &e) {
                throw std::runtime_error("Failed to load configuration file");
            }

            m_PerInstructionBits = ParseConfigOrThrow<size_t>(config, "PerInstructionBits");

            // parse macro define tokens
            {
                auto macroDefineTokens = ParseConfigOrThrow<std::vector<std::string>>(
                    config, "MacroDefineTokens");

                m_MacroDefineTokens = std::make_shared<const std::unordered_set<SourceElements::Token>>(
                    std::move(macroDefineTokens)
                    | std::views::transform([](std::string &token) {
                        return SourceElements::Token{std::move(token)};
                    }) | std::ranges::to<std::unordered_set<SourceElements::Token>>()
                );
            }

            // initialize state
            {
                std::string languageLoadPath =
                    ParseConfigOptional<std::string>(
                    config,
                    "LanguageLoadPath"
                    ).value_or("LanguageInstructionLib");

                m_SharedState = std::make_shared<sol::state>(sol::state{});
                m_SharedState->open_libraries(sol::lib::base, sol::lib::package, sol::lib::string);

                m_SharedState->new_usertype<InstructionBinary>(
                    "InstructionBinary",
                    sol::constructors<InstructionBinary(std::vector<bool>), InstructionBinary(size_t)>(),
                    "SetBit", &InstructionBinary::SetBit,
                    "GetBits", &InstructionBinary::GetBits
                );

                for (const auto &luaSource : std::filesystem::directory_iterator(
                        languageRootDir / languageLoadPath)) {
                    if (luaSource.is_regular_file() && luaSource.path().extension() == ".lua") {
                        m_SharedState->load_file(luaSource.path().string());
                    }
                }

                std::unordered_map<std::string, std::string> instructionToLuaFunctionNameMap
                    = ParseConfigOrThrow<std::unordered_map<std::string, std::string>>(
                        config, "InstructionToLuaFunctionNameMap");

                std::unordered_map<
                    SourceElements::Token,
                    std::function<std::expected<InstructionBinary, std::string>(
                        std::span<SourceElements::Token> const &)>> instructionProcessorMap;

                for (auto&& [key, value] : std::move(instructionToLuaFunctionNameMap)) {
                    instructionProcessorMap[AllLowerCase(key)] =
                        [sharedState = m_SharedState, value = std::move(value)](
                            std::span<SourceElements::Token> const &tokens) -> std::expected<InstructionBinary, std::string> {
                            sol::function luaFunction = (*sharedState)[value];
                            auto result = luaFunction(tokens);
                            if (result.valid()) {
                                sol::type type = result.get_type();
                                 try {
                                     auto instructionBinary = result.get<InstructionBinary>();
                                     return instructionBinary;
                                 } catch (...) {
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
            }
        }

    private:
        template<typename ExpectedType>
        inline static ExpectedType ParseConfig(const YAML::Node &config,
                                               std::string_view firstKey,
                                               const std::convertible_to<std::string_view> auto &... keys) {
            if constexpr (sizeof...(keys) == 0) {
                return config[firstKey].as<ExpectedType>();
            } else {
                return ParseConfig<ExpectedType>(config[firstKey], keys...);
            }
        }

        template<typename ExpectedType>
        inline static ExpectedType ParseConfigOrThrow(const YAML::Node &config,
                                                      const auto &... keys) {
            try {
                return ParseConfig<ExpectedType>(config, keys...);
            } catch (const YAML::Exception &e) {
                throw std::runtime_error("Error parsing configuration: " + std::string(e.what()));
            }
        }

        template<typename ExpectedType>
        inline static std::optional<ExpectedType> ParseConfigOptional(const YAML::Node &config,
                                                      const auto &... keys) {
            try {
                return ParseConfig<ExpectedType>(config, keys...);
            } catch (const YAML::Exception &e) {
                return std::optional<ExpectedType>{};
            }
        }

        size_t m_PerInstructionBits;

        std::shared_ptr<sol::state> m_SharedState;

        std::shared_ptr<const std::unordered_set<SourceElements::Token>>
        m_MacroDefineTokens;

        std::shared_ptr<const std::unordered_map<
            SourceElements::Token,
            std::function<std::expected<InstructionBinary, std::string>(
                std::span<SourceElements::Token> const &)>>>
        m_InstructionProcessorMap;
    };
}
