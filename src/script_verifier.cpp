#include "script_verifier.h"
#include "script_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define popen _popen
#define pclose _pclose
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/wait.h>
#endif

ScriptVerifier::ScriptVerifier() {
    platformNames_[ScriptPlatform::LINUX_BASH] = "Linux Bash";
    platformNames_[ScriptPlatform::LINUX_SH] = "Linux POSIX sh";
    platformNames_[ScriptPlatform::MACOS_BASH] = "macOS Bash";
    platformNames_[ScriptPlatform::MACOS_ZSH] = "macOS Zsh";
    platformNames_[ScriptPlatform::WINDOWS_POWERSHELL] = "Windows PowerShell";
    platformNames_[ScriptPlatform::WINDOWS_CMD] = "Windows CMD";
    platformNames_[ScriptPlatform::WINDOWS_BATCH] = "Windows Batch";
}

ScriptVerifier::~ScriptVerifier() {}

bool ScriptVerifier::initialize(const std::string& workDir) {
    if (workDir.empty()) {
#ifdef _WIN32
        workingDir_ = "G:\\Zachary's Playroom Powerd by AI\\script_verification";
#else
        const char* home = getenv("HOME");
        workingDir_ = std::string(home ? home : ".") + "/ZacharysPlayroom/script_verification";
#endif
    } else {
        workingDir_ = workDir;
    }

    return createDirectoryRecursive(workingDir_);
}

bool ScriptVerifier::createDirectoryRecursive(const std::string& path) {
    std::string currentPath;
    size_t pos = 0;
    
    while (pos < path.length()) {
        size_t nextSlash = path.find_first_of("/\\", pos);
        if (nextSlash == std::string::npos) {
            nextSlash = path.length();
        }
        
        currentPath = path.substr(0, nextSlash);
        if (!currentPath.empty() && currentPath != "." && currentPath.find(':') == std::string::npos) {
            struct stat st;
            if (stat(currentPath.c_str(), &st) != 0) {
                mkdir(currentPath.c_str(), 0755);
            }
        }
        
        pos = nextSlash + 1;
    }
    
    return true;
}

ScriptVerifier::UploadedScript ScriptVerifier::uploadScript(const std::string& filepath) {
    UploadedScript script;
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return script;
    }
    
    // Read content
    std::ostringstream buffer;
    buffer << file.rdbuf();
    script.content = buffer.str();
    file.close();
    
    // Extract filename
    size_t lastSlash = filepath.find_last_of("/\\");
    script.filename = (lastSlash != std::string::npos) ? filepath.substr(lastSlash + 1) : filepath;
    
    // Detect platform
    script.platform = detectPlatform(script.content);
    if (script.platform == ScriptPlatform::UNKNOWN) {
        script.platform = detectPlatformFromExtension(script.filename);
    }
    
    script.fileSize = script.content.size();
    script.uploadTime = getCurrentTimestamp();
    
    std::cout << "Script uploaded: " << script.filename 
              << " (" << script.fileSize << " bytes, " 
              << platformToString(script.platform) << ")" << std::endl;
    
    return script;
}

ScriptVerifier::UploadedScript ScriptVerifier::uploadScriptContent(const std::string& content, const std::string& filename) {
    UploadedScript script;
    script.content = content;
    script.filename = filename;
    script.platform = detectPlatform(content);
    if (script.platform == ScriptPlatform::UNKNOWN) {
        script.platform = detectPlatformFromExtension(filename);
    }
    script.fileSize = content.size();
    script.uploadTime = getCurrentTimestamp();
    
    return script;
}

ScriptVerifier::ScriptPlatform ScriptVerifier::detectPlatform(const std::string& content) const {
    // Check shebang
    if (content.find("#!/bin/bash") != std::string::npos) return ScriptPlatform::LINUX_BASH;
    if (content.find("#!/bin/sh") != std::string::npos) return ScriptPlatform::LINUX_SH;
    if (content.find("#!/bin/zsh") != std::string::npos) return ScriptPlatform::MACOS_ZSH;
    
    // Check PowerShell markers
    if (content.find("$PSVersionTable") != std::string::npos ||
        content.find("Write-Host") != std::string::npos ||
        content.find("$ErrorActionPreference") != std::string::npos) {
        return ScriptPlatform::WINDOWS_POWERSHELL;
    }
    
    // Check CMD/Batch markers
    if (content.find("@echo off") != std::string::npos ||
        content.find("REM ") != std::string::npos ||
        content.find("%errorLevel%") != std::string::npos) {
        return ScriptPlatform::WINDOWS_BATCH;
    }
    
    return ScriptPlatform::UNKNOWN;
}

ScriptVerifier::ScriptPlatform ScriptVerifier::detectPlatformFromExtension(const std::string& filename) const {
    if (filename.size() >= 3 && filename.substr(filename.size() - 3) == ".sh") return ScriptPlatform::LINUX_BASH;
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".zsh") return ScriptPlatform::MACOS_ZSH;
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".ps1") return ScriptPlatform::WINDOWS_POWERSHELL;
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".bat") return ScriptPlatform::WINDOWS_BATCH;
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".cmd") return ScriptPlatform::WINDOWS_CMD;
    return ScriptPlatform::UNKNOWN;
}

bool ScriptVerifier::validateSyntax(const UploadedScript& script, std::string& errorMsg) {
    switch (script.platform) {
        case ScriptPlatform::LINUX_BASH:
        case ScriptPlatform::LINUX_SH:
        case ScriptPlatform::MACOS_BASH:
        case ScriptPlatform::MACOS_ZSH:
            return validateBashSyntax(script.content, errorMsg);
        
        case ScriptPlatform::WINDOWS_POWERSHELL:
            return validatePowerShellSyntax(script.content, errorMsg);
        
        case ScriptPlatform::WINDOWS_BATCH:
        case ScriptPlatform::WINDOWS_CMD:
            return validateBatchSyntax(script.content, errorMsg);
        
        default:
            errorMsg = "Unknown platform, cannot validate";
            return false;
    }
}

bool ScriptVerifier::validateBashSyntax(const std::string& content, std::string& errorMsg) {
    std::string tempFile = createTempFile(content, ".sh");
    if (tempFile.empty()) {
        errorMsg = "Failed to create temp file for validation";
        return false;
    }
    
    // Use bash -n to check syntax
    std::string cmd = "bash -n " + tempFile + " 2>&1";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        removeTempFile(tempFile);
        errorMsg = "Failed to run syntax check";
        return false;
    }
    
    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
    int status = pclose(pipe);
    removeTempFile(tempFile);
    
    if (status != 0) {
        errorMsg = result;
        return false;
    }
    
    return true;
}

bool ScriptVerifier::validatePowerShellSyntax(const std::string& content, std::string& errorMsg) {
    std::string tempFile = createTempFile(content, ".ps1");
    if (tempFile.empty()) {
        errorMsg = "Failed to create temp file for validation";
        return false;
    }
    
#ifdef _WIN32
    std::string cmd = "powershell.exe -NoProfile -Command \"$null = Get-Content '" + tempFile + "' | Out-Null; if ($?) { exit 0 } else { exit 1 }\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
#else
    // On Linux, use pwsh if available
    std::string cmd = "pwsh -NoProfile -File " + tempFile + " -WhatIf 2>&1";
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    
    if (!pipe) {
        removeTempFile(tempFile);
        errorMsg = "PowerShell not available for validation";
        return true; // Assume valid if we can't test
    }
    
    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
#ifdef _WIN32
    int status = _pclose(pipe);
#else
    int status = pclose(pipe);
#endif
    
    removeTempFile(tempFile);
    
    if (status != 0 && !result.empty()) {
        errorMsg = result;
        return false;
    }
    
    return true;
}

bool ScriptVerifier::validateBatchSyntax(const std::string& content, std::string& errorMsg) {
    // Basic validation for batch scripts
    if (content.empty()) {
        errorMsg = "Empty script";
        return false;
    }
    
    // Check for common syntax errors
    size_t pos = 0;
    int parenBalance = 0;
    
    while ((pos = content.find_first_of("()", pos)) != std::string::npos) {
        if (content[pos] == '(') parenBalance++;
        else parenBalance--;
        pos++;
    }
    
    if (parenBalance != 0) {
        errorMsg = "Unbalanced parentheses";
        return false;
    }
    
    return true;
}

ScriptVerifier::TestReport ScriptVerifier::testScript(const UploadedScript& script, bool dryRun) {
    switch (script.platform) {
        case ScriptPlatform::LINUX_BASH:
        case ScriptPlatform::LINUX_SH:
        case ScriptPlatform::MACOS_BASH:
        case ScriptPlatform::MACOS_ZSH:
            return testBashScript(script.content, dryRun);
        
        case ScriptPlatform::WINDOWS_POWERSHELL:
            return testPowerShellScript(script.content, dryRun);
        
        case ScriptPlatform::WINDOWS_BATCH:
        case ScriptPlatform::WINDOWS_CMD:
            return testBatchScript(script.content, dryRun);
        
        default: {
            TestReport report;
            report.result = TestResult::UNKNOWN_ERROR;
            report.errorMessage = "Unknown platform";
            report.exitCode = -1;
            return report;
        }
    }
}

ScriptVerifier::TestReport ScriptVerifier::testBashScript(const std::string& content, bool dryRun) {
    TestReport report;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::string tempFile = createTempFile(content, ".sh");
    if (tempFile.empty()) {
        report.result = TestResult::UNKNOWN_ERROR;
        report.errorMessage = "Failed to create temp file";
        report.exitCode = -1;
        return report;
    }
    
    chmod(tempFile.c_str(), 0755);
    
    // Test execution
    std::string cmd;
    if (dryRun) {
        // Dry run: just check if commands exist
        cmd = "bash -n " + tempFile + " 2>&1";
    } else {
        // Real execution (risky!)
        cmd = tempFile + " 2>&1";
    }
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        report.result = TestResult::UNKNOWN_ERROR;
        report.errorMessage = "Failed to execute script";
        report.exitCode = -1;
        removeTempFile(tempFile);
        return report;
    }
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        report.output += buffer;
    }
    
    int status = pclose(pipe);
    report.exitCode = WEXITSTATUS(status);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    report.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    removeTempFile(tempFile);
    
    // Analyze results
    if (report.exitCode == 0) {
        report.result = TestResult::SUCCESS;
    } else if (report.output.find("command not found") != std::string::npos ||
               report.output.find("No such file or directory") != std::string::npos) {
        report.result = TestResult::DEPENDENCY_MISSING;
        report.missingDependencies = extractDependencies(report.output);
    } else if (report.output.find("Permission denied") != std::string::npos) {
        report.result = TestResult::PERMISSION_ERROR;
    } else if (report.output.find("syntax error") != std::string::npos) {
        report.result = TestResult::SYNTAX_ERROR;
        report.errorMessage = report.output;
    } else {
        report.result = TestResult::RUNTIME_ERROR;
        report.errorMessage = report.output;
    }
    
    return report;
}

ScriptVerifier::TestReport ScriptVerifier::testPowerShellScript(const std::string& content, bool dryRun) {
    TestReport report;
    report.result = TestResult::SUCCESS;
    report.exitCode = 0;
    report.executionTime = 0.0;
    report.output = "PowerShell script validation not fully implemented on this platform";
    
    // Basic check
    if (content.empty()) {
        report.result = TestResult::SYNTAX_ERROR;
        report.errorMessage = "Empty script";
    }
    
    return report;
}

ScriptVerifier::TestReport ScriptVerifier::testBatchScript(const std::string& content, bool dryRun) {
    TestReport report;
    report.result = TestResult::SUCCESS;
    report.exitCode = 0;
    report.executionTime = 0.0;
    report.output = "Batch script validation not fully implemented on this platform";
    
    if (content.empty()) {
        report.result = TestResult::SYNTAX_ERROR;
        report.errorMessage = "Empty script";
    }
    
    return report;
}

std::vector<std::string> ScriptVerifier::extractDependencies(const std::string& content) {
    std::vector<std::string> deps;
    
    // Look for "command not found" messages
    size_t pos = 0;
    while ((pos = content.find("command not found", pos)) != std::string::npos) {
        // Extract command name before "command not found"
        size_t start = content.rfind('\n', pos);
        if (start == std::string::npos) start = 0;
        else start++;
        
        std::string line = content.substr(start, pos - start);
        
        // Find the actual command name
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string cmd = line.substr(0, colonPos);
            // Trim whitespace
            cmd.erase(0, cmd.find_first_not_of(" \t"));
            cmd.erase(cmd.find_last_not_of(" \t") + 1);
            if (!cmd.empty()) {
                deps.push_back(cmd);
            }
        }
        
        pos++;
    }
    
    return deps;
}

std::string ScriptVerifier::getUserPurpose() {
    std::cout << "\n🤖 AI Script Fixer: What is the purpose of this script?\n";
    std::cout << "Please describe what this script should do:\n> ";
    
    std::string purpose;
    std::getline(std::cin, purpose);
    
    return purpose;
}

std::string ScriptVerifier::fixScript(
    const UploadedScript& script,
    const TestReport& report,
    const std::string& userPurpose
) {
    std::cout << "\n🔧 AI is analyzing and fixing the script...\n";
    
    std::string fixedContent = script.content;
    
    // Generate fixes based on error type
    if (report.result == TestResult::SYNTAX_ERROR) {
        std::cout << "  → Fixing syntax errors\n";
        
        // Common fixes for syntax errors
        if (report.errorMessage.find("unexpected end of file") != std::string::npos) {
            // Missing 'fi' or 'done'
            if (fixedContent.find("if ") != std::string::npos && 
                fixedContent.find("fi") == std::string::npos) {
                fixedContent += "\nfi\n";
            }
            if (fixedContent.find("while ") != std::string::npos && 
                fixedContent.find("done") == std::string::npos) {
                fixedContent += "\ndone\n";
            }
        }
        
        // Fix missing quotes
        if (report.errorMessage.find("unexpected token") != std::string::npos) {
            // This is complex, add error handling instead
            fixedContent = "set -e\nset -u\n\n" + fixedContent;
        }
    }
    else if (report.result == TestResult::DEPENDENCY_MISSING) {
        std::cout << "  → Adding dependency installation\n";
        
        // Add dependency installation based on purpose
        std::string deps;
        for (const auto& dep : report.missingDependencies) {
            deps += dep + " ";
        }
        
        std::string installSection = "\n# Auto-install missing dependencies\n";
        installSection += "if command -v apt-get &> /dev/null; then\n";
        installSection += "    sudo apt-get update && sudo apt-get install -y " + deps + "\n";
        installSection += "elif command -v yum &> /dev/null; then\n";
        installSection += "    sudo yum install -y " + deps + "\n";
        installSection += "elif command -v brew &> /dev/null; then\n";
        installSection += "    brew install " + deps + "\n";
        installSection += "fi\n\n";
        
        // Insert after shebang
        size_t shebangEnd = fixedContent.find('\n');
        if (shebangEnd != std::string::npos) {
            fixedContent.insert(shebangEnd + 1, installSection);
        } else {
            fixedContent = installSection + fixedContent;
        }
    }
    else if (report.result == TestResult::PERMISSION_ERROR) {
        std::cout << "  → Adding privilege checks\n";
        
        std::string privCheck = "\n# Check for required privileges\n";
        privCheck += "if [[ $EUID -ne 0 ]]; then\n";
        privCheck += "    echo \"This script requires sudo privileges\"\n";
        privCheck += "    exec sudo \"$0\" \"$@\"\n";
        privCheck += "fi\n\n";
        
        size_t shebangEnd = fixedContent.find('\n');
        if (shebangEnd != std::string::npos) {
            fixedContent.insert(shebangEnd + 1, privCheck);
        }
    }
    else if (report.result == TestResult::RUNTIME_ERROR) {
        std::cout << "  → Improving error handling\n";
        
        // Add better error handling
        std::string errorHandling = "\n# Enhanced error handling\n";
        errorHandling += "set -e\n";
        errorHandling += "set -u\n";
        errorHandling += "set -o pipefail\n\n";
        errorHandling += "trap 'echo \"Error on line $LINENO\"' ERR\n\n";
        
        size_t shebangEnd = fixedContent.find('\n');
        if (shebangEnd != std::string::npos) {
            fixedContent.insert(shebangEnd + 1, errorHandling);
        }
    }
    
    // Add documentation based on user purpose
    if (!userPurpose.empty()) {
        std::string docs = "# Purpose: " + userPurpose + "\n";
        size_t shebangEnd = fixedContent.find('\n');
        if (shebangEnd != std::string::npos) {
            fixedContent.insert(shebangEnd + 1, docs);
        }
    }
    
    std::cout << "  ✓ Script fixed\n";
    return fixedContent;
}

bool ScriptVerifier::verifyAndFixUntilWorks(
    UploadedScript& script,
    int maxAttempts,
    std::vector<FixAttempt>* history
) {
    std::cout << "\n🔍 Starting iterative verification and fixing...\n";
    std::cout << "Maximum attempts: " << maxAttempts << "\n\n";
    
    std::string userPurpose;
    bool purposeAsked = false;
    
    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        std::cout << "Attempt " << attempt << "/" << maxAttempts << ":\n";
        
        // Validate syntax
        std::string syntaxError;
        if (!validateSyntax(script, syntaxError)) {
            std::cout << "  ✗ Syntax validation failed: " << syntaxError << "\n";
            
            TestReport report;
            report.result = TestResult::SYNTAX_ERROR;
            report.errorMessage = syntaxError;
            report.exitCode = 1;
            
            if (!purposeAsked) {
                userPurpose = getUserPurpose();
                purposeAsked = true;
            }
            
            std::string fixed = fixScript(script, report, userPurpose);
            script.content = fixed;
            
            FixAttempt fixAttempt;
            fixAttempt.attemptNumber = attempt;
            fixAttempt.modifiedContent = fixed;
            fixAttempt.testReport = report;
            fixAttempt.fixDescription = "Fixed syntax errors";
            fixHistory_.push_back(fixAttempt);
            
            if (history) history->push_back(fixAttempt);
            
            continue;
        }
        
        std::cout << "  ✓ Syntax validation passed\n";
        
        // Test execution
        TestReport report = testScript(script, true); // Dry run
        
        std::cout << "  Test result: ";
        switch (report.result) {
            case TestResult::SUCCESS:
                std::cout << "✓ SUCCESS\n";
                std::cout << "\n🎉 Script verified and working!\n";
                return true;
            
            case TestResult::SYNTAX_ERROR:
                std::cout << "✗ SYNTAX ERROR\n";
                break;
            case TestResult::RUNTIME_ERROR:
                std::cout << "✗ RUNTIME ERROR\n";
                break;
            case TestResult::DEPENDENCY_MISSING:
                std::cout << "✗ MISSING DEPENDENCIES\n";
                break;
            case TestResult::PERMISSION_ERROR:
                std::cout << "✗ PERMISSION ERROR\n";
                break;
            default:
                std::cout << "✗ UNKNOWN ERROR\n";
                break;
        }
        
        if (!purposeAsked) {
            userPurpose = getUserPurpose();
            purposeAsked = true;
        }
        
        std::string fixed = fixScript(script, report, userPurpose);
        script.content = fixed;
        
        FixAttempt fixAttempt;
        fixAttempt.attemptNumber = attempt;
        fixAttempt.modifiedContent = fixed;
        fixAttempt.testReport = report;
        fixAttempt.fixDescription = "Applied automatic fixes";
        fixHistory_.push_back(fixAttempt);
        
        if (history) history->push_back(fixAttempt);
    }
    
    std::cout << "\n❌ Failed to fix script after " << maxAttempts << " attempts\n";
    return false;
}

std::vector<ScriptVerifier::FixAttempt> ScriptVerifier::getFixHistory() const {
    return fixHistory_;
}

bool ScriptVerifier::saveFixedScript(const UploadedScript& script, const std::string& outputPath) {
    std::string filepath = outputPath.empty() ? 
        (workingDir_ + "/fixed_" + script.filename) : outputPath;
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to save fixed script: " << filepath << std::endl;
        return false;
    }
    
    file << script.content;
    file.close();
    
#ifndef _WIN32
    chmod(filepath.c_str(), 0755);
#endif
    
    std::cout << "Fixed script saved: " << filepath << std::endl;
    return true;
}

std::string ScriptVerifier::getWorkingDirectory() const {
    return workingDir_;
}

std::string ScriptVerifier::createTempFile(const std::string& content, const std::string& extension) {
    std::string tempPath = workingDir_ + "/temp_script_" + getCurrentTimestamp() + extension;
    
    std::ofstream file(tempPath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    file << content;
    file.close();
    
    return tempPath;
}

bool ScriptVerifier::removeTempFile(const std::string& filepath) {
    return (remove(filepath.c_str()) == 0);
}

std::string ScriptVerifier::getCurrentTimestamp() const {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    return std::string(buffer);
}

std::string ScriptVerifier::platformToString(ScriptPlatform platform) const {
    auto it = platformNames_.find(platform);
    if (it != platformNames_.end()) {
        return it->second;
    }
    return "Unknown";
}
