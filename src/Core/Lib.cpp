module Core.Lib;

import std;
import Vendor.sol;

namespace Core::Lib {
    void AddLibToState(sol::state &state) {
        auto Lib = state.create_named_table("Lib");
        Lib.set_function("ToLowerCase", &ToLowerCase);
        Lib.set_function("ToUpperCase", &ToUpperCase);
    }

    std::string ToLowerCase(std::string str)  {
        std::ranges::transform(str, str.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return str;
    }

    std::string ToUpperCase(std::string str) {
        std::ranges::transform(str, str.begin(),
                               [](unsigned char c) { return std::toupper(c); });
        return str;
    }
}
