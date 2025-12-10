#include "script_verifier.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <script_file>" << std::endl;
        return 1;
    }

    ScriptVerifier verifier;
    if (!verifier.initialize()) {
        std::cerr << "Failed to initialize verifier" << std::endl;
        return 1;
    }

    std::cout << "Working directory: " << verifier.getWorkingDirectory() << std::endl;
    
    // Upload script
    auto script = verifier.uploadScript(argv[1]);
    if (script.content.empty()) {
        std::cerr << "Failed to upload script" << std::endl;
        return 1;
    }

    // Validate syntax
    std::string syntaxError;
    if (verifier.validateSyntax(script, syntaxError)) {
        std::cout << "✓ Syntax validation passed" << std::endl;
    } else {
        std::cout << "✗ Syntax errors found:\n" << syntaxError << std::endl;
    }

    // Test execution (dry run)
    std::cout << "\nTesting script (dry run)..." << std::endl;
    auto report = verifier.testScript(script, true);
    
    std::cout << "Exit code: " << report.exitCode << std::endl;
    std::cout << "Execution time: " << report.executionTime << "s" << std::endl;
    
    if (report.result == ScriptVerifier::TestResult::SUCCESS) {
        std::cout << "✓ Script test PASSED" << std::endl;
    } else {
        std::cout << "✗ Script test FAILED" << std::endl;
        if (!report.errorMessage.empty()) {
            std::cout << "Error: " << report.errorMessage << std::endl;
        }
    }

    return 0;
}
