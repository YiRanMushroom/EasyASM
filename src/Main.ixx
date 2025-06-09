export module Main;

import Vendor.sol;
import Vendor.yaml;
import <cassert>;

import Core.Compiler;
import std;

export int main() {
    std::filesystem::path picoblazeRootDir = "../../picoblaze";
    auto sourceFile = std::ifstream("../../tests/test_compile_psm.psm");
    std::string source((std::istreambuf_iterator<char>(sourceFile)),
                 std::istreambuf_iterator<char>());
    try {
        auto compiler = Core::Compiler{picoblazeRootDir};

        auto sourceCompiler = compiler.CreateSourceCompiler(source);

        sourceCompiler.CompileAll();
        sourceCompiler.Link();
        std::ofstream outputFile("result.txt");

        outputFile << sourceCompiler.GenerateOutput();

        std::cout << std::format("Compilation successful. Output written to result.txt\n");
    } catch (std::exception &e) {
        std::cerr << std::format("Compilation failed due to an error:\n{}\n",
                                 e.what());
    }

    return 0;
}
