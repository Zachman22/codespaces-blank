#include "../include/prompt_verifier.h"
#include "../include/compiler.h"
#include <fstream>
#include <algorithm>
#include <sstream>

std::vector<std::string> PromptVerifier::supportedExtensions = {
    ".cpp", ".cc", ".cxx", ".c++", ".c", ".h", ".hpp", ".hxx"
};

bool PromptVerifier::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool PromptVerifier::isValidSourceFile(const std::string& path) {
    // Check if file exists
    if (!fileExists(path)) {
        return false;
    }
    
    // Check extension
    for (const auto& ext : supportedExtensions) {
        if (path.length() >= ext.length()) {
            std::string fileExt = path.substr(path.length() - ext.length());
            std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
            
            if (fileExt == ext) {
                return true;
            }
        }
    }
    
    return false;
}

bool PromptVerifier::isCompilerAvailable() {
    Compiler compiler;
    return compiler.isAvailable();
}

VerificationResult PromptVerifier::verifyBuildRequest(const std::string& sourceFile,
                                                       const std::string& outputFile) {
    VerificationResult result;
    result.valid = true;
    
    // Check if source file exists
    if (!fileExists(sourceFile)) {
        result.valid = false;
        result.message = "Source file does not exist: " + sourceFile;
        return result;
    }
    
    // Check if source file has valid extension
    if (!isValidSourceFile(sourceFile)) {
        result.valid = false;
        result.message = "Invalid source file extension. Supported: .cpp, .c, .cc, .cxx, .h, .hpp";
        return result;
    }
    
    // Check if output file name is valid
    if (outputFile.empty()) {
        result.valid = false;
        result.message = "Output file name cannot be empty";
        return result;
    }
    
    // Check for invalid characters in output file name
    std::string invalidChars = "<>:\"|?*";
    for (char c : outputFile) {
        if (invalidChars.find(c) != std::string::npos) {
            result.valid = false;
            result.message = "Output file name contains invalid characters";
            return result;
        }
    }
    
    // Check if compiler is available
    if (!isCompilerAvailable()) {
        result.valid = false;
        result.message = "MinGW compiler (g++) not found. Please install MinGW-w64 and add it to PATH.";
        return result;
    }
    
    // Add warnings
    if (sourceFile == outputFile || sourceFile + ".exe" == outputFile) {
        result.warnings.push_back("Output file name is similar to source file. This may cause confusion.");
    }
    
    // Check file size
    std::ifstream file(sourceFile, std::ios::binary | std::ios::ate);
    if (file.good()) {
        std::streamsize size = file.tellg();
        if (size > 10 * 1024 * 1024) { // 10 MB
            result.warnings.push_back("Source file is very large (>10MB). Compilation may take a while.");
        }
    }
    
    result.message = "Build request validated successfully";
    return result;
}

VerificationResult PromptVerifier::verifyRunRequest(const std::string& executablePath) {
    VerificationResult result;
    result.valid = true;
    
    // Check if executable exists
    if (!fileExists(executablePath)) {
        result.valid = false;
        result.message = "Executable does not exist: " + executablePath;
        return result;
    }
    
    // Check if file has .exe extension
    if (executablePath.length() < 4 || 
        executablePath.substr(executablePath.length() - 4) != ".exe") {
        result.warnings.push_back("File does not have .exe extension. It may not be executable.");
    }
    
    result.message = "Run request validated successfully";
    return result;
}
