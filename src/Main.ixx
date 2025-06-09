export module Main;

import Vendor.sol;
import Vendor.yaml;
import <cassert>;

import Core.Compiler;
import std;

export int main() {
    std::filesystem::path picoblazeRootDir = "../../picoblaze";
    std::string source = R"(
        ; Example Picoblaze source code
        TestNonIns
        add 0, 1, 2, 3, 4
    )";
    try {
        auto compiler = Core::Compiler{picoblazeRootDir};

        auto sourceCompiler = compiler.CreateSourceCompiler(source);

        while (!sourceCompiler.CompileOneLine());

        sourceCompiler.Link();

    } catch (std::exception &e) {
        std::cerr << std::format("Compilation failed due to an error:\n{}\n",
                                 e.what());
    }

    return 0;
}
