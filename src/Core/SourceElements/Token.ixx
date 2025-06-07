export module Core.SourceElements.Token;

import std;
import Vendor.sol;


namespace Core::SourceElements {
    export class Token {
    public:
        const std::string& GetContent() const {
            return m_Content;
        }

        bool Is(std::string_view other) const {
            return m_Content == other;
        }

        bool operator==(const Token &) const = default;

        Token(std::string content)
            : m_Content(std::move(content)) {}

        static void AddLibToState(sol::state& state);

    private:
        std::string m_Content;
    };
}

export template<>
struct std::hash<Core::SourceElements::Token> {
    std::size_t operator()(const Core::SourceElements::Token &token) const noexcept {
        return std::hash<std::string>()(token.GetContent());
    }
};
