export module Main;

import Vendor.sol;
import Vendor.yaml;
import <cassert>;

import Core.Compiler;
import std;
import FindPaths;

export int main(int argc, char *argv[]) {
    ProgramPaths paths{argc, argv};
    try {
        Core::Compiler compiler{paths.GetLanguageRootDir()};
        auto sourceFile = std::ifstream(paths.GetSourceFilePath());
        std::string source((std::istreambuf_iterator<char>(sourceFile)),
             std::istreambuf_iterator<char>());

        Core::SourceCompiler sourceCompiler{compiler.CreateSourceCompiler(source)};
        sourceCompiler.CompileAll();
        sourceCompiler.Link();
        std::ofstream outputFile(paths.GetOutputDir() / paths.GetOutputFileName());
        outputFile << sourceCompiler.GenerateOutput();
        std::cout << std::format("Compilation successful. Output written to {}\n",
                         (paths.GetOutputDir() / paths.GetOutputFileName()).string());
    } catch (const std::exception &e) {
        std::cerr << std::format("Compilation failed due to an error:\n{}\n",
                         e.what());
        return 1;
    } catch (...) {
        std::cerr << "Compilation failed due to an unknown error.\n";
        return 1;
    }

    return 0;
}
