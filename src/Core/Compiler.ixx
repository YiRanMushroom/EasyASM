export module Core.Compiler;

import std;
import Vendor.sol;
import Vendor.yaml;
import Core.SourceElements.Token;

namespace Core {
    export class SourceCompiler {
    private:
        SourceCompiler() = default;

    public:
        SourceCompiler(size_t perInstructionBits,
                       std::shared_ptr<const sol::state> sharedState,
                       std::shared_ptr<const std::unordered_map<SourceElements::Token,
                           SourceElements::Token>> preProcessorTokenMap,
                       std::shared_ptr<const std::unordered_map<SourceElements::Token,
                           std::function<
                               std::expected<std::vector<bool>, std::string>
                               (const std::vector<SourceElements::Token> &)>>> instructionProcessorMap,
                       std::shared_ptr<const std::unordered_set<SourceElements::Token>> macroDefineTokens)
            : m_PerInstructionBits(perInstructionBits),
              m_SharedState(sharedState),
              m_PreProcessorTokenMap(std::move(preProcessorTokenMap)),
              m_InstructionProcessorMap(std::move(instructionProcessorMap)),
              m_MacroDefineTokens(std::move(macroDefineTokens)) {
        }

    private:
        size_t m_PerInstructionBits;
        std::shared_ptr<const sol::state> m_SharedState;
        std::shared_ptr<const std::unordered_map<SourceElements::Token,
            SourceElements::Token>> m_PreProcessorTokenMap;

        std::shared_ptr<const std::unordered_set<SourceElements::Token>>
        m_MacroDefineTokens;
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

        size_t m_PerInstructionBits;

        std::shared_ptr<const sol::state> m_SharedState;

        std::shared_ptr<const std::unordered_set<SourceElements::Token>>
        m_MacroDefineTokens;
    };
}
