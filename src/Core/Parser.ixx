export module Core.Parser;

import std;
import Vendor.sol;
import Core.Exceptions;

namespace Core {
    export class TokenStream {
    public:
        TokenStream(const std::string& source)
            : m_Source(source), m_Current(this->m_Source.begin()) {
            SkipToNextToken();
        }

        TokenStream(const TokenStream&) = delete;
        TokenStream& operator=(const TokenStream&) = delete;
        TokenStream(TokenStream&&) = default;
        TokenStream& operator=(TokenStream&&) = default;

        std::optional<std::string> ParseCurrent();
        std::optional<std::string> PeekCurrent();
        void SkipCurrent();
        std::string GetApproxCurrentLocation();
        std::optional<Exceptions::WrappedGenericException> AssertIsNewLine();
        void SetNewLine(bool isNewLine);
        bool IsNewLine() const { return m_IsNewLine; }

        static void AddLibToState(sol::state& state) {
            state.new_usertype<TokenStream>("TokenStream",
                sol::constructors<TokenStream(const std::string&)>(),
                "ParseCurrent", &TokenStream::ParseCurrent,
                "PeekCurrent", &TokenStream::PeekCurrent,
                "SkipCurrent", &TokenStream::SkipCurrent,
                "GetApproxCurrentLocation", &TokenStream::GetApproxCurrentLocation,
                "AssertIsNewLine", &TokenStream::AssertIsNewLine,
                "SetNewLine", &TokenStream::SetNewLine,
                "IsNewLine", &TokenStream::IsNewLine
            );
        }

    private:
        void SkipWhitespace();
        void SkipToNextToken();
        std::optional<std::string> ParseString();

        std::string m_Source;
        std::string::const_iterator m_Current;
        size_t m_NumberOfLines = 1;
        bool m_IsNewLine = true;
    };
}