module Core.SourceElements.Token;

import std;
namespace Core::SourceElements {
    void Token::AddLibToState(sol::state &state) {
        state.new_usertype<Token>(
            "Token",
            sol::constructors<Token(std::string)>(),
            "GetContent", &Token::GetContent,
            "Is", &Token::Is
        );
    }
}
