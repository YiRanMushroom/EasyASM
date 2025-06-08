module;

#define SOL_EXCEPTIONS_SAFE_PROPAGATION 1
#include <sol/sol.hpp>

export module Main;

// import Vendor.sol;
import Vendor.yaml;
import <cassert>;

import Core.Compiler;
import std;

export int main() {
    std::filesystem::path picoblazeRootDir = "../../picoblaze";
    std::string source = R"(
        ; Example Picoblaze source code
        add 0, 1
    )";
    try {
        auto compiler = Core::Compiler{picoblazeRootDir};

        auto sourceCompiler = compiler.CreateSourceCompiler(source);

        sourceCompiler.CompileOneLine();

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
