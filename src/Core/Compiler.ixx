module;

export module Core.Compiler;

import std;
import Vendor.yaml;
import Vendor.sol;
export import Core.SourceElements.Token;
export import Core.ASMElements.InstructionBinary;

namespace Core {
    export class SourceCompiler {
    private:
        SourceCompiler() = default;

    public:
        SourceCompiler(size_t perInstructionBits,
                       std::shared_ptr<const std::unordered_map<SourceElements::Token,
                           SourceElements::Token>> preProcessorTokenMap,
                       std::shared_ptr<const std::unordered_map<SourceElements::Token,
                           std::function<
                               std::expected<ASMElements::InstructionBinary, std::string>
                               (const std::span<SourceElements::Token> &)>>> instructionProcessorMap,
                       std::shared_ptr<const std::unordered_set<SourceElements::Token>> macroDefineTokens)
            : m_PerInstructionBits(perInstructionBits),
              m_PreProcessorTokenMap(std::move(preProcessorTokenMap)),
              m_InstructionProcessorMap(std::move(instructionProcessorMap)),
              m_MacroDefineTokens(std::move(macroDefineTokens)) {
        }

        static void AddLibToState(sol::state& state);

    public:
        size_t m_PerInstructionBits;
        // std::shared_ptr<const sol::state> m_SharedState;
        std::shared_ptr<const std::unordered_map<SourceElements::Token,
            SourceElements::Token>> m_PreProcessorTokenMap;

        std::shared_ptr<const std::unordered_set<SourceElements::Token>>
        m_MacroDefineTokens;

        std::shared_ptr<const std::unordered_map<
        SourceElements::Token,
        std::function<std::expected<ASMElements::InstructionBinary, std::string>(
        std::span<SourceElements::Token> const &)>>>
        m_InstructionProcessorMap;
    };

    export class Compiler {
    public:
        Compiler(const std::filesystem::path &languageRootDir);

    private:
        static std::shared_ptr<sol::state> CreateSharedState();
        void InitParticularState(std::filesystem::directory_iterator languageRootDir);
        void InitNameToFuntionMap(const YAML::Node &config);
        void InitMacroDefineTokens(const YAML::Node &config);

    public:
        template<typename ExpectedType>
        static ExpectedType ParseConfig(const YAML::Node &config,
                                        std::string_view firstKey,
                                        const std::convertible_to<std::string_view> auto &... keys);

        template<typename ExpectedType>
        static ExpectedType ParseConfigOrThrow(const YAML::Node &config,
                                               const auto &... keys);

        template<typename ExpectedType>
        static std::optional<ExpectedType> ParseConfigOptional(const YAML::Node &config,
                                                               const auto &... keys);

        size_t m_PerInstructionBits;

        std::shared_ptr<sol::state> m_SharedState;

        std::shared_ptr<const std::unordered_set<SourceElements::Token>>
        m_MacroDefineTokens;

        std::shared_ptr<const std::unordered_map<
            SourceElements::Token,
            std::function<std::expected<ASMElements::InstructionBinary, std::string>(
                std::span<SourceElements::Token> const &)>>>
        m_InstructionProcessorMap;
    };

    template<typename ExpectedType>
    ExpectedType Compiler::ParseConfig(const YAML::Node &config, std::string_view firstKey,
        const std::convertible_to<std::string_view> auto &...keys) {
        if constexpr (sizeof...(keys) == 0) {
            return config[firstKey].as<ExpectedType>();
        } else {
            return ParseConfig<ExpectedType>(config[firstKey], keys...);
        }
    }

    template<typename ExpectedType>
    ExpectedType Compiler::ParseConfigOrThrow(const YAML::Node &config, const auto &...keys) {
        try {
            return ParseConfig<ExpectedType>(config, keys...);
        } catch (const YAML::Exception &e) {
            throw std::runtime_error("Error parsing configuration: " + std::string(e.what()));
        }
    }

    template<typename ExpectedType>
    std::optional<ExpectedType> Compiler::ParseConfigOptional(const YAML::Node &config, const auto &...keys) {
        try {
            return ParseConfig<ExpectedType>(config, keys...);
        } catch (const YAML::Exception &e) {
            return std::optional<ExpectedType>{};
        }
    }
}
