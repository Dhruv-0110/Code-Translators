#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "antlr4-runtime.h"
#include "AMSLexer.h"
#include "AMSParser.h"    
#include "ams/compiler/CodeGen.hpp"
#include "ams/interpreter/Evaluator.hpp"

using namespace antlr4;

int main(int argc, const char* argv[]) {
    // Help Verbose for the demonstration of Usage Options Available in AMS-Lang Engine 
    if (argc < 3) {
        std::cout << "AutomonScript Language Engine Usage:" << std::endl;
        std::cout << "  ams run <file.ams>   (Interpretation for Rapid Prototyping)" << std::endl;
        std::cout << "  ams build <file.ams> (Compile for Production Executable)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ams run examples\\hello_world.ams" << std::endl;
        std::cout << "  ams build examples\\brute_force.ams" << std::endl;
        return 1;
    }

    std::string mode = argv[1];     
    std::string filePath = argv[2]; 
    std::filesystem::path inputPath(filePath);
    std::ifstream stream(filePath);
    if (!stream.good()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    ANTLRInputStream input(stream);
    AMSLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    AMSParser parser(&tokens);

    // Build The Parse Tree of Source Code Using ANTLR4
    tree::ParseTree* tree = parser.program();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        std::cerr << "Syntax Error!" << std::endl;
        return 1;
    }

    // Execute ParseTree Based on Selected Mode of Execution
    if (mode == "run") {
        // Interpreted Execution Mode : For Prototyping & Testing
        Evaluator interpreter;
        interpreter.visit(tree); 
    } 
    else if (mode == "build") {
        // Compiled Execution Mode : For Production Deployment
        std::string tempCpp = "temp_output.cpp";
        std::filesystem::path exePath = inputPath;
        exePath.replace_extension(".exe");

        CodeGen generator(tempCpp);
        generator.visit(tree);
        generator.close();

        // Compile Generated C++ to Executable
        std::string compileCmd = "g++ " + tempCpp + " -o \"" + exePath.string() + "\" -mconsole";

        if (std::system(compileCmd.c_str()) == 0) {
            // Clean up intermediate code
            std::filesystem::remove(tempCpp); 
            std::cout << "[SUCCESS] Created: " << exePath.filename().string() << std::endl;
        } else {
            std::cerr << "[ERROR] Compilation failed." << std::endl;
        }
    }
    else {
        // Unknown Execution Mode
        std::cerr << "Unknown mode: " << mode << ". Use 'run' or 'build'." << std::endl;
        return 1;
    }

    return 0;
}