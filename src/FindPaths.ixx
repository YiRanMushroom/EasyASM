export module FindPaths;

import std;
import <args.hxx>;

export class ProgramPaths {
public:
    ProgramPaths(int argc, char* argv[]) {
        args::ArgumentParser parser(
    "A simple assembler architecture",
    "Usage example:\n  EasyASM -l ../../PicoBlaze -i ../../tests/test_compile_psm.psm -o ./out");
        args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
        args::ValueFlag<std::string> languageRootDirFlag(
            parser, "languageRootDir", "Path to the library root directory of the language",
            {'l', "language-root-dir"});
        args::ValueFlag<std::string> sourceFilePathFlag(
            parser, "Input file",
            "Path to the source file to compile", {'i', "input"});
        args::ValueFlag<std::string> outputDirFlag(
            parser, "Output directory",
            "Path to the directory where the output will be written", {'o', "output"});

        try {
            parser.ParseCLI(argc, argv);
        } catch (args::Help&) {
            std::cout << parser;
            std::exit(0);
        } catch (args::ParseError& e) {
            std::cerr << e.what() << "\n" << parser;
            std::exit(1);
        }

        if (!languageRootDirFlag || !sourceFilePathFlag) {
            std::cerr << "Error: Missing required arguments." << std::endl;
            parser.Help(std::cerr);
            std::exit(1);
        }
        std::string languageRootDirStr = args::get(languageRootDirFlag);
        languageRootDir = std::filesystem::path(languageRootDirStr);

        if (!std::filesystem::exists(languageRootDir)) {
            std::cerr << "Error: Language root directory does not exist: " << languageRootDirStr << "\n";
            std::exit(1);
        }
        std::string sourceFilePathStr = args::get(sourceFilePathFlag);
        sourceFilePath = std::filesystem::path(sourceFilePathStr);
        if (!std::filesystem::exists(sourceFilePath)) {
            std::cerr << "Error: Source file does not exist: " << sourceFilePathStr << "\n";
            std::exit(1);
        }
        if (outputDirFlag) {
            std::string outputDirStr = args::get(outputDirFlag);
            outputDir = std::filesystem::path(outputDirStr);
            if (!std::filesystem::exists(outputDir)) {
                std::filesystem::create_directories(outputDir);
            }
        } else {
            outputDir = sourceFilePath.parent_path();
        }

        outputFileName = sourceFilePath.filename().string();
        if (outputFileName.find_last_of('.') != std::string::npos) {
            outputFileName = outputFileName.substr(0, outputFileName.find_last_of('.')) + ".mem";
        }
    }

    const std::filesystem::path& GetLanguageRootDir() const {
        return languageRootDir;
    }

    const std::filesystem::path& GetSourceFilePath() const {
        return sourceFilePath;
    }

    const std::filesystem::path& GetOutputDir() const {
        return outputDir;
    }

    const std::string& GetOutputFileName() const {
        return outputFileName;
    }

private:
    std::filesystem::path languageRootDir;
    std::filesystem::path sourceFilePath;
    std::filesystem::path outputDir;
    std::string outputFileName;
};
