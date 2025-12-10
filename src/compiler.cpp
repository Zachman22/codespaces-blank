#include "../include/compiler.h"
#include "../include/code_signer.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <sstream>
#include <fstream>

Compiler::Compiler() : compilerPath("") {
    findCompiler();
}

bool Compiler::findCompiler() {
    // Try to find g++ in PATH
#ifdef _WIN32
    std::string checkCmd = "where g++";
    FILE* pipe = _popen(checkCmd.c_str(), "r");
#else
    std::string checkCmd = "which g++";
    FILE* pipe = popen(checkCmd.c_str(), "r");
#endif
    
    if (!pipe) {
        return false;
    }
    
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        compilerPath = buffer;
        // Remove newline
        compilerPath.erase(compilerPath.find_last_not_of(" \n\r\t") + 1);
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
        return true;
    }
    
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    
    // Try common MinGW installation paths
    std::vector<std::string> commonPaths = {
        "C:\\MinGW\\bin\\g++.exe",
        "C:\\MinGW-w64\\bin\\g++.exe",
        "C:\\msys64\\mingw64\\bin\\g++.exe",
        "C:\\msys64\\ucrt64\\bin\\g++.exe",
        "C:\\Program Files\\mingw-w64\\bin\\g++.exe"
    };
    
    for (const auto& path : commonPaths) {
        std::ifstream file(path);
        if (file.good()) {
            compilerPath = path;
            return true;
        }
    }
    
    return false;
}

bool Compiler::isAvailable() {
    if (compilerPath.empty()) {
        return findCompiler();
    }
    
    std::ifstream file(compilerPath);
    return file.good();
}

std::string Compiler::getVersion() {
    if (!isAvailable()) {
        return "Compiler not found";
    }
    
    std::string cmd = "\"" + compilerPath + "\" --version";
#ifdef _WIN32
    FILE* pipe = _popen(cmd.c_str(), "r");
#else
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe) {
        return "Error getting version";
    }
    
    char buffer[256];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
    }
    
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return result;
}

std::string Compiler::buildCommand(const CompileOptions& options) {
    std::stringstream cmd;
    
    // Use quotes for compiler path
    cmd << "\"" << compilerPath << "\"";
    
    // Add source file
    cmd << " \"" << options.sourceFile << "\"";
    
    // Add output file
    cmd << " -o \"" << options.outputFile << "\"";
    
    // Add C++ standard
    cmd << " -std=" << options.standard;
    
    // Add include paths
    for (const auto& includePath : options.includePaths) {
        cmd << " -I\"" << includePath << "\"";
    }
    
    // Add optimization flags
    if (options.optimize) {
        cmd << " -O2";
    } else {
        cmd << " -O0";
    }
    
    // Add debug flag
    if (options.debug) {
        cmd << " -g";
    }
    
    // Add custom flags
    for (const auto& flag : options.flags) {
        cmd << " " << flag;
    }
    
    // Add libraries
    for (const auto& lib : options.libraries) {
        cmd << " -l" << lib;
    }
    
    // Add common flags
    cmd << " -Wall";
    
    return cmd.str();
}

bool Compiler::executeCompile(const std::string& command,
                              std::function<void(const std::string&)> logCallback) {
    // Log the command
    logCallback("[CMD] " + command + "\n");
    
    // Create a temporary file for stderr
    std::string tempFile = std::tmpnam(nullptr);
    std::string fullCmd = command + " 2>&1";
    
#ifdef _WIN32
    FILE* pipe = _popen(fullCmd.c_str(), "r");
#else
    FILE* pipe = popen(fullCmd.c_str(), "r");
#endif
    if (!pipe) {
        logCallback("[ERROR] Failed to execute compiler\n");
        return false;
    }
    
    char buffer[256];
    bool hasErrors = false;
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line = buffer;
        logCallback(line);
        
        // Check for errors
        if (line.find("error:") != std::string::npos) {
            hasErrors = true;
        }
    }
    
#ifdef _WIN32
    int exitCode = _pclose(pipe);
#else
    int exitCode = pclose(pipe);
#endif
    
    if (exitCode == 0 && !hasErrors) {
        logCallback("[SUCCESS] Compilation completed successfully\n");
        return true;
    } else {
        logCallback("[FAILED] Compilation failed with exit code: " + 
                   std::to_string(exitCode) + "\n");
        return false;
    }
}

bool Compiler::compile(const CompileOptions& options,
                       std::function<void(const std::string&)> logCallback) {
    if (!isAvailable()) {
        logCallback("[ERROR] Compiler not found. Please install MinGW-w64.\n");
        return false;
    }
    
    logCallback("[INFO] Using compiler: " + compilerPath + "\n");
    logCallback("[INFO] Source file: " + options.sourceFile + "\n");
    logCallback("[INFO] Output file: " + options.outputFile + "\n");
    logCallback("[INFO] Starting compilation...\n");
    
    std::string command = buildCommand(options);
    bool success = executeCompile(command, logCallback);
    
    // Sign the executable if compilation was successful
    if (success) {
        logCallback("[INFO] Code signing executable...\n");
        
        // Initialize code signer
        CodeSigner signer;
        
        // Check if certificate exists, if not generate one
        if (!signer.hasCertificate()) {
            logCallback("[INFO] Generating code signing certificate...\n");
            if (signer.generateCertificate("Zachary's Playroom Powered By AI", 
                                          "Zachary's Playroom", "US", 365)) {
                logCallback("[SUCCESS] Certificate generated successfully\n");
            } else {
                logCallback("[WARNING] Failed to generate certificate, skipping code signing\n");
                return success; // Return success but skip signing
            }
        }
        
        // Sign the executable
        if (signer.signExecutable(options.outputFile, "Zachary's Playroom Powered By AI")) {
            logCallback("[SUCCESS] Executable signed successfully\n");
        } else {
            logCallback("[WARNING] Failed to sign executable (executable will still work)\n");
        }
    }
    
    return success;
}
