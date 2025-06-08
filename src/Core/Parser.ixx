export module Core.Parser;

import std;
import Vendor.sol;

namespace Core {
    export class TokenStream {
    public:
        TokenStream(const std::string& source)
            : source(source), current(this->source.begin()) {
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


        static void AddLibToState(sol::state& state) {
            state.new_usertype<TokenStream>("TokenStream",
                sol::constructors<TokenStream(const std::string&)>(),
                "ParseCurrent", &TokenStream::ParseCurrent,
                "PeekCurrent", &TokenStream::PeekCurrent,
                "SkipCurrent", &TokenStream::SkipCurrent,
                "GetApproxCurrentLocation", &TokenStream::GetApproxCurrentLocation
            );
        }

    private:
        void SkipWhitespace();
        void SkipToNextToken();
        std::optional<std::string> ParseString();

        std::string source;
        std::string::const_iterator current;
        size_t numberOfLines = 1;
    };
}