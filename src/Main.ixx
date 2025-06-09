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
        add
    )";
    try {
        auto compiler = Core::Compiler{picoblazeRootDir};

        auto sourceCompiler = compiler.CreateSourceCompiler(source);

        sourceCompiler.CompileAll();
        sourceCompiler.Link();


    } catch (std::exception &e) {
        std::cerr << std::format("Compilation failed due to an error:\n{}\n",
                                 e.what());
    }

    return 0;
}
