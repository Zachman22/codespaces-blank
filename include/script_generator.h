#ifndef SCRIPT_GENERATOR_H
#define SCRIPT_GENERATOR_H

#include <string>
#include <vector>
#include <map>

/**
 * ScriptGenerator - Multi-platform script generation with automatic code signing
 * 
 * Generates scripts for:
 * - Linux (bash, sh): Ubuntu, Debian, Fedora, CentOS, Arch, RHEL, SUSE
 * - macOS (bash, zsh): All current versions
 * - Windows (PowerShell, CMD, Batch): Windows 10, 11, Server 2019/2022
 */
class ScriptGenerator {
public:
    enum class Platform {
        LINUX_BASH,
        LINUX_SH,
        MACOS_BASH,
        MACOS_ZSH,
        WINDOWS_POWERSHELL,
        WINDOWS_CMD,
        WINDOWS_BATCH,
        ALL_PLATFORMS
    };

    enum class ScriptType {
        INSTALLER,
        UPDATER,
        BACKUP,
        DEPLOYMENT,
        AUTOMATION,
        SYSTEM_ADMIN,
        BUILD,
        TEST,
        DOCKER,
        CUSTOM
    };

    struct ScriptRequest {
        std::string description;
        ScriptType type;
        Platform platform;
        std::vector<std::string> commands;
        std::vector<std::string> dependencies;
        bool requiresAdmin;
        bool autoSign;
        std::string customName;
    };

    struct GeneratedScript {
        std::string filename;
        std::string content;
        Platform platform;
        std::string filepath;
        bool isSigned;
        std::string signature;
    };

    ScriptGenerator();
    ~ScriptGenerator();

    // Initialize the generator
    bool initialize(const std::string& outputDir = "");

    // Generate scripts based on request
    std::vector<GeneratedScript> generateScripts(const ScriptRequest& request);

    // Generate script for specific platform
    GeneratedScript generateForPlatform(const ScriptRequest& request, Platform platform);

    // Auto-generate script name based on type and description
    std::string generateScriptName(const ScriptRequest& request, Platform platform);

    // Sign script with CA
    bool signScript(GeneratedScript& script);

    // Save script to disk
    bool saveScript(const GeneratedScript& script);

    // Get list of supported platforms
    std::vector<std::string> getSupportedPlatforms() const;

    // Get list of supported Linux distributions
    std::vector<std::string> getSupportedLinuxDistros() const;

    // Get list of supported macOS versions
    std::vector<std::string> getSupportedMacOSVersions() const;

    // Get list of supported Windows versions
    std::vector<std::string> getSupportedWindowsVersions() const;

    // Validate script syntax
    bool validateScript(const GeneratedScript& script);

    // Set output directory
    void setOutputDirectory(const std::string& dir);

    // Get output directory
    std::string getOutputDirectory() const;

private:
    std::string outputDirectory_;
    std::map<Platform, std::string> platformExtensions_;
    std::map<Platform, std::string> platformShebangs_;

    // Platform-specific generators
    std::string generateLinuxBashScript(const ScriptRequest& request);
    std::string generateLinuxShScript(const ScriptRequest& request);
    std::string generateMacOSBashScript(const ScriptRequest& request);
    std::string generateMacOSZshScript(const ScriptRequest& request);
    std::string generateWindowsPowerShellScript(const ScriptRequest& request);
    std::string generateWindowsCMDScript(const ScriptRequest& request);
    std::string generateWindowsBatchScript(const ScriptRequest& request);

    // Helper functions
    std::string getFileExtension(Platform platform) const;
    std::string getShebang(Platform platform) const;
    std::string getCurrentTimestamp() const;
    std::string sanitizeFilename(const std::string& name) const;
    bool createDirectoryRecursive(const std::string& path);

    // Script components
    std::string generateHeader(const ScriptRequest& request, Platform platform);
    std::string generatePrerequisiteChecks(const ScriptRequest& request, Platform platform);
    std::string generateDependencyInstall(const ScriptRequest& request, Platform platform);
    std::string generateMainLogic(const ScriptRequest& request, Platform platform);
    std::string generateErrorHandling(Platform platform);
    std::string generateFooter(Platform platform);
};

#endif // SCRIPT_GENERATOR_H
