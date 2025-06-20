module;

export module Core.Compiler;

import std;
import Vendor.yaml;
import Vendor.sol;
import Core.Parser;
import Core.Exceptions;

namespace Core {
    export class SourceCompiler;
    export class Compiler;

    class SourceCompiler {
    private:
        friend class Compiler;

    public:
        SourceCompiler(std::shared_ptr<sol::state> sharedState,
                       std::shared_ptr<const std::unordered_map<std::string,
                           std::function<
                               void(SourceCompiler &)
                           >>> instructionProcessorMap,
                       std::string source,
                       size_t startAddressAlignment,
                       std::shared_ptr<YAML::Node> sharedConfig)
            : m_SharedState(std::move(sharedState)),
              m_NameToFunctionMap(std::move(instructionProcessorMap)),
              m_TokenStream(std::move(source)),
              m_StartAddressAlignment(startAddressAlignment),
              m_SharedConfig(std::move(sharedConfig)) {
            m_CompilerContext = m_SharedState->create_table();
            m_LinkerContext = m_SharedState->create_table();
        }

        static void AddLibToState(sol::state &state);

    public:
        sol::table GetCompilerContext() const {
            return m_CompilerContext;
        }

        sol::table GetLinkerContext() const {
            return m_LinkerContext;
        }

        TokenStream &GetTokenStream() {
            return m_TokenStream;
        }

    public:
        std::vector<uint8_t> &GetBitBuffer() {
            return m_BitBuffer;
        }

        void WriteBit(bool bit);

        void WriteBits(const std::vector<bool> &bits);

        void WriteSignedNumber(int64_t number, size_t bits);

        void WriteUnsignedNumber(uint64_t number, size_t bits);

        void ReplaceUnsignedNumber(uint64_t number, size_t bits, size_t startIndex);

        size_t GetBitBufferSize() const;

        bool CompileOneLine();

        void CompileAll();

        void Link();

        void AlignStartAddress();

        std::string GenerateOutput();

    private:
        std::shared_ptr<sol::state> m_SharedState;

        std::shared_ptr<const std::unordered_map<
            std::string,
            std::function<void(SourceCompiler &)>>> m_NameToFunctionMap;

        sol::table m_CompilerContext;
        sol::table m_LinkerContext;
        TokenStream m_TokenStream;

        std::vector<uint8_t> m_BitBuffer;
        size_t m_StartAddressAlignment; // default alignment
        std::shared_ptr<YAML::Node> m_SharedConfig;
        std::optional<std::string> m_Output;
    };

    class Compiler {
    public:
        Compiler(const std::filesystem::path &languageRootDir);

        friend class SourceCompiler;

    private:
        static std::shared_ptr<sol::state> CreateSharedState();

        void InitParticularState(std::filesystem::directory_iterator languageRootDir);

        void InitNameToFunctionMap(const YAML::Node &config);

        void InitLinker(const YAML::Node &config);

        void InitEventFunctions(const YAML::Node &config);

        void InitOutputFunction(const YAML::Node &config);

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

        [[nodiscard]] SourceCompiler CreateSourceCompiler(std::string) const;

    private:
        std::shared_ptr<sol::state> m_SharedState;

        std::shared_ptr<std::unordered_map<
            std::string,
            std::function<void(SourceCompiler &)>>>
        m_NameToFunctionMap;

        size_t m_StartAddressAlignment; // default alignment
        std::shared_ptr<YAML::Node> m_SharedConfig;
    };

    template<typename ExpectedType>
    ExpectedType Compiler::ParseConfig(const YAML::Node &config, std::string_view firstKey,
                                       const std::convertible_to<std::string_view> auto &... keys) {
        if constexpr (sizeof...(keys) == 0) {
            return config[firstKey].as<ExpectedType>();
        } else {
            return ParseConfig<ExpectedType>(config[firstKey], keys...);
        }
    }

    template<typename ExpectedType>
    ExpectedType Compiler::ParseConfigOrThrow(const YAML::Node &config, const auto &... keys) {
        try {
            return ParseConfig<ExpectedType>(config, keys...);
        } catch (const YAML::Exception &e) {
            throw std::runtime_error("Error parsing configuration: " + std::string(e.what()));
        }
    }

    template<typename ExpectedType>
    std::optional<ExpectedType> Compiler::ParseConfigOptional(const YAML::Node &config, const auto &... keys) {
        try {
            return ParseConfig<ExpectedType>(config, keys...);
        } catch (const YAML::Exception &e) {
            return std::optional<ExpectedType>{};
        }
    }
}
