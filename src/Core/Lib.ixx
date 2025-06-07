export module Core.Lib;

import std;
import Vendor.sol;

namespace Core::Lib {
    export void AddLibToState(sol::state &state);

    export std::string ToLowerCase(std::string str);
    export std::string ToUpperCase(std::string str);
}
