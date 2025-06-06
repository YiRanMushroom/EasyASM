export module Main;

import Vendor.sol;
import Vendor.yaml;
import <cassert>;

export int main() {
    sol::state lua;
    lua.open_libraries();
    sol::table map = lua.create_table();
    lua.script(
        R"(
        function map_to_string(map)
            local result = {}
            for k, v in pairs(map) do
                table.insert(result, tostring(k) .. ": " .. tostring(v))
            end
            return table.concat(result, ", ")
        end
    )");
    map["key1"] = "value1";
    map["key2"] = "value2";
    map["key3"] = "value3";
    try {
        // std::string result = lua["map_to_string"](map);
        lua["map"] = map;
        sol::function map_to_string = lua["map_to_string"];
        std::cout << "Map to string: " << map_to_string(lua["map"]).get<std::string>() << std::endl;
    } catch (const sol::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;

    // int x = 0;
    // lua.set_function("beep", [&x] { ++x; });
    // lua.script("beep()");
    // assert(x == 1);
    //
    // YAML::Node lineup = YAML::Load("{1B: Prince Fielder, 2B: Rickie Weeks, LF: Ryan Braun}");
    // for (YAML::const_iterator it = lineup.begin(); it != lineup.end(); ++it) {
    //     std::cout << "Playing at " << it->first.as<std::string>() << " is " << it->second.as<std::string>() << "\n";
    // }
    //
    // lineup["RF"] = "Corey Hart";
    // lineup["C"] = "Jonathan Lucroy";
    // assert(lineup.size() == 5);
}
