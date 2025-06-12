export module Main;

import Vendor.sol;
import Vendor.yaml;
import <cassert>;

import Core.Compiler;
import std;
import FindPaths;
import Core.Exceptions;

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
        std::u8string u8str = (paths.GetOutputDir() / paths.GetOutputFileName()).u8string();
        std::string out(reinterpret_cast<const char*>(u8str.c_str()), u8str.size());
        std::cout << "Compilation successful. Output has been written to: "
                  << out << "\n";
    } catch (const std::exception &e) {
        std::cerr << std::format("Compilation failed due to an error:\n{}\n",
                         e.what());
        if (auto wrapped = dynamic_cast<const Core::Exceptions::WrappedGenericException*>(&e)) {
            if (auto implementationError = dynamic_cast<const Core::Exceptions::CompilerImplementationError*>(wrapped->GetPointer())) {
                return -1;
            }
        }
    } catch (...) {
        std::cerr << "Compilation failed due to an unknown error.\n";
        return 1;
    }

    return 0;
}
