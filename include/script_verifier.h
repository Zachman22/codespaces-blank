#ifndef SCRIPT_VERIFIER_H
#define SCRIPT_VERIFIER_H

#include <string>
#include <vector>
#include <map>

/**
 * ScriptVerifier - Upload, test, and auto-fix scripts
 * 
 * Features:
 * - Upload script files (bash, PowerShell, batch, etc.)
 * - Syntax validation
 * - Automated testing in isolated environment
 * - AI-powered auto-fixing for failed scripts
 * - Iterative testing until script works
 * - Support for all platforms
 */
class ScriptVerifier {
public:
    enum class ScriptPlatform {
        LINUX_BASH,
        LINUX_SH,
        MACOS_BASH,
        MACOS_ZSH,
        WINDOWS_POWERSHELL,
        WINDOWS_CMD,
        WINDOWS_BATCH,
        UNKNOWN
    };

    enum class TestResult {
        SUCCESS,
        SYNTAX_ERROR,
        RUNTIME_ERROR,
        PERMISSION_ERROR,
        DEPENDENCY_MISSING,
        TIMEOUT,
        UNKNOWN_ERROR
    };

    struct UploadedScript {
        std::string filename;
        std::string content;
        ScriptPlatform platform;
        size_t fileSize;
        std::string uploadTime;
    };

    struct TestReport {
        TestResult result;
        std::string output;
        std::string errorMessage;
        int exitCode;
        double executionTime;
        std::vector<std::string> missingDependencies;
        std::vector<std::string> suggestions;
    };

    struct FixAttempt {
        int attemptNumber;
        std::string modifiedContent;
        TestReport testReport;
        std::string fixDescription;
    };

    ScriptVerifier();
    ~ScriptVerifier();

    // Initialize verifier
    bool initialize(const std::string& workDir = "");

    // Upload and analyze script
    UploadedScript uploadScript(const std::string& filepath);
    UploadedScript uploadScriptContent(const std::string& content, const std::string& filename);

    // Detect platform from script content
    ScriptPlatform detectPlatform(const std::string& content) const;
    ScriptPlatform detectPlatformFromExtension(const std::string& filename) const;

    // Validate script syntax
    bool validateSyntax(const UploadedScript& script, std::string& errorMsg);

    // Test script execution
    TestReport testScript(const UploadedScript& script, bool dryRun = true);

    // Ask user for script purpose (for AI fixing)
    std::string getUserPurpose();

    // Auto-fix script using AI
    std::string fixScript(
        const UploadedScript& script,
        const TestReport& report,
        const std::string& userPurpose
    );

    // Iterative testing and fixing until it works
    bool verifyAndFixUntilWorks(
        UploadedScript& script,
        int maxAttempts = 5,
        std::vector<FixAttempt>* history = nullptr
    );

    // Get verification history
    std::vector<FixAttempt> getFixHistory() const;

    // Save fixed script
    bool saveFixedScript(const UploadedScript& script, const std::string& outputPath = "");

    // Get working directory
    std::string getWorkingDirectory() const;

private:
    std::string workingDir_;
    std::vector<FixAttempt> fixHistory_;
    std::map<ScriptPlatform, std::string> platformNames_;

    // Platform-specific syntax validators
    bool validateBashSyntax(const std::string& content, std::string& errorMsg);
    bool validatePowerShellSyntax(const std::string& content, std::string& errorMsg);
    bool validateBatchSyntax(const std::string& content, std::string& errorMsg);

    // Platform-specific test executors
    TestReport testBashScript(const std::string& content, bool dryRun);
    TestReport testPowerShellScript(const std::string& content, bool dryRun);
    TestReport testBatchScript(const std::string& content, bool dryRun);

    // Script analysis
    std::vector<std::string> extractDependencies(const std::string& content);
    std::vector<std::string> checkMissingCommands(const std::vector<std::string>& commands);
    
    // AI-powered fixing
    std::string generateFix(
        const std::string& content,
        const TestReport& report,
        const std::string& purpose
    );

    // Utility functions
    std::string createTempFile(const std::string& content, const std::string& extension);
    bool removeTempFile(const std::string& filepath);
    std::string getCurrentTimestamp() const;
    std::string platformToString(ScriptPlatform platform) const;
    bool createDirectoryRecursive(const std::string& path);
};

#endif // SCRIPT_VERIFIER_H
