export module Main;

import Vendor.sol;
import Vendor.yaml;
import <cassert>;

import Core.Compiler;
import std;

export int main() {
    std::filesystem::path picoblazeRootDir = "../../picoblaze";
    try {
        auto compiler = Core::Compiler{picoblazeRootDir};
        const auto& function = compiler.m_InstructionProcessorMap->at(Core::SourceElements::Token("add"));
        std::vector<Core::SourceElements::Token> tokens = {
            Core::SourceElements::Token("add"),
            // Core::SourceElements::Token("r0"),
            // Core::SourceElements::Token("r1")
        };
        auto result = function(tokens);
        if (result.has_value()) {
            auto instructionBinary = result.value();
            std::cout << "Instruction processed successfully." << std::endl;
            // You can add more logic to handle the instructionBinary as needed
        } else {
            std::cerr << "Error processing instruction: " << result.error() << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
