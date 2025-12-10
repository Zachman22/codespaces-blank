#ifndef AUTO_INSTALLER_H
#define AUTO_INSTALLER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace HybridIDE {

enum class DependencyType {
    SYSTEM_PACKAGE,
    RUNTIME,
    LIBRARY,
    TOOL,
    CONFIGURATION
};

struct Dependency {
    std::string name;
    std::string version;
    DependencyType type;
    std::vector<std::string> platforms;
    std::string installCommand;
    std::string verifyCommand;
    bool required;
};

class AutoInstaller {
public:
    AutoInstaller();
    ~AutoInstaller();

    // Dependency management
    bool detectMissingDependencies(std::vector<Dependency>& missing);
    bool installDependency(const Dependency& dep);
    bool installAllDependencies();
    bool verifyDependency(const Dependency& dep);
    
    // Runtime detection and installation
    bool detectPlatform();
    bool installRuntime(const std::string& runtime, const std::string& version);
    std::vector<std::string> getInstalledRuntimes();
    
    // Configuration
    bool configureEnvironment();
    bool setupPaths();
    bool createConfigFiles();
    bool downloadAssets(const std::vector<std::string>& urls);
    
    // Post-build automation
    bool executePostBuildSteps();
    bool packageApplication();
    
    // Progress reporting
    void setProgressCallback(std::function<void(const std::string&, int)> callback);

private:
    std::vector<Dependency> dependencies_;
    std::string platform_;
    std::string architecture_;
    std::function<void(const std::string&, int)> progressCallback_;
    
    bool runCommand(const std::string& command, std::string& output);
    bool isAdmin();
    bool elevatePrivileges();
    std::string getPackageManager();
};

} // namespace HybridIDE

#endif // AUTO_INSTALLER_H
