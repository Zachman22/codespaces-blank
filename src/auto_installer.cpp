#include "auto_installer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
#endif

namespace fs = std::filesystem;

namespace HybridIDE {

AutoInstaller::AutoInstaller() {
    detectPlatform();
    
    // Define default dependencies
    dependencies_ = {
        {"g++", "latest", DependencyType::TOOL, {"linux", "macos"}, "apt-get install -y g++", "g++ --version", true},
        {"cmake", "3.20+", DependencyType::TOOL, {"linux", "macos", "windows"}, "apt-get install -y cmake", "cmake --version", true},
        {"node", "18+", DependencyType::RUNTIME, {"linux", "macos", "windows"}, "curl -fsSL https://deb.nodesource.com/setup_18.x | bash - && apt-get install -y nodejs", "node --version", true},
        {"npm", "9+", DependencyType::TOOL, {"linux", "macos", "windows"}, "", "npm --version", true},
        {"webkit2gtk", "latest", DependencyType::LIBRARY, {"linux"}, "apt-get install -y libwebkit2gtk-4.0-dev", "pkg-config --exists webkit2gtk-4.0", false},
        {"git", "latest", DependencyType::TOOL, {"linux", "macos", "windows"}, "apt-get install -y git", "git --version", true}
    };
}

AutoInstaller::~AutoInstaller() {
}

bool AutoInstaller::detectPlatform() {
#ifdef _WIN32
    platform_ = "windows";
    architecture_ = "x64";
#elif __APPLE__
    platform_ = "macos";
    #ifdef __aarch64__
    architecture_ = "arm64";
    #else
    architecture_ = "x64";
    #endif
#elif __linux__
    platform_ = "linux";
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        std::string machine(buffer.machine);
        architecture_ = (machine.find("64") != std::string::npos) ? "x64" : "x86";
    }
#else
    platform_ = "unknown";
    architecture_ = "unknown";
#endif

    std::cout << "Detected platform: " << platform_ << " (" << architecture_ << ")" << std::endl;
    return platform_ != "unknown";
}

bool AutoInstaller::detectMissingDependencies(std::vector<Dependency>& missing) {
    missing.clear();
    
    for (const auto& dep : dependencies_) {
        // Check if dependency is for current platform
        if (std::find(dep.platforms.begin(), dep.platforms.end(), platform_) == dep.platforms.end()) {
            continue;
        }

        if (!verifyDependency(dep)) {
            missing.push_back(dep);
            std::cout << "Missing dependency: " << dep.name << std::endl;
        } else {
            std::cout << "Dependency satisfied: " << dep.name << std::endl;
        }
    }

    return true;
}

bool AutoInstaller::verifyDependency(const Dependency& dep) {
    if (dep.verifyCommand.empty()) {
        return true;
    }

    std::string output;
    bool result = runCommand(dep.verifyCommand, output);
    return result;
}

bool AutoInstaller::installDependency(const Dependency& dep) {
    if (dep.installCommand.empty()) {
        std::cerr << "No install command for dependency: " << dep.name << std::endl;
        return false;
    }

    std::cout << "Installing " << dep.name << "..." << std::endl;
    
    if (progressCallback_) {
        progressCallback_("Installing " + dep.name, 0);
    }

    std::string output;
    bool result = runCommand(dep.installCommand, output);
    
    if (result) {
        std::cout << "Successfully installed " << dep.name << std::endl;
        if (progressCallback_) {
            progressCallback_("Installed " + dep.name, 100);
        }
    } else {
        std::cerr << "Failed to install " << dep.name << std::endl;
        std::cerr << "Output: " << output << std::endl;
    }

    return result;
}

bool AutoInstaller::installAllDependencies() {
    std::vector<Dependency> missing;
    detectMissingDependencies(missing);

    if (missing.empty()) {
        std::cout << "All dependencies are satisfied!" << std::endl;
        return true;
    }

    std::cout << "Installing " << missing.size() << " missing dependencies..." << std::endl;

    bool allSuccessful = true;
    int current = 0;
    int total = missing.size();

    for (const auto& dep : missing) {
        current++;
        
        if (progressCallback_) {
            progressCallback_("Installing dependencies", (current * 100) / total);
        }

        if (dep.required) {
            if (!installDependency(dep)) {
                std::cerr << "Failed to install required dependency: " << dep.name << std::endl;
                allSuccessful = false;
                break;
            }
        } else {
            installDependency(dep); // Optional, continue even if fails
        }
    }

    return allSuccessful;
}

bool AutoInstaller::installRuntime(const std::string& runtime, const std::string& version) {
    std::cout << "Installing runtime: " << runtime << " version " << version << std::endl;

    if (runtime == "node" || runtime == "nodejs") {
        Dependency nodeDep = {"node", version, DependencyType::RUNTIME, {platform_}, 
                              "curl -fsSL https://deb.nodesource.com/setup_18.x | bash - && apt-get install -y nodejs",
                              "node --version", true};
        return installDependency(nodeDep);
    }
    else if (runtime == "python") {
        Dependency pyDep = {"python", version, DependencyType::RUNTIME, {platform_},
                            "apt-get install -y python3 python3-pip",
                            "python3 --version", true};
        return installDependency(pyDep);
    }

    return false;
}

std::vector<std::string> AutoInstaller::getInstalledRuntimes() {
    std::vector<std::string> runtimes;
    
    std::string output;
    if (runCommand("node --version", output)) {
        runtimes.push_back("node");
    }
    if (runCommand("python3 --version", output)) {
        runtimes.push_back("python");
    }
    if (runCommand("java --version", output)) {
        runtimes.push_back("java");
    }

    return runtimes;
}

bool AutoInstaller::configureEnvironment() {
    std::cout << "Configuring environment..." << std::endl;

    // Setup paths
    if (!setupPaths()) {
        return false;
    }

    // Create config files
    if (!createConfigFiles()) {
        return false;
    }

    std::cout << "Environment configured successfully!" << std::endl;
    return true;
}

bool AutoInstaller::setupPaths() {
    std::vector<std::string> paths = {
        "./bin",
        "./lib",
        "./plugins",
        "./temp",
        "./cache"
    };

    for (const auto& path : paths) {
        try {
            fs::create_directories(path);
            std::cout << "Created directory: " << path << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to create directory " << path << ": " << e.what() << std::endl;
            return false;
        }
    }

    return true;
}

bool AutoInstaller::createConfigFiles() {
    // Create default config.json if it doesn't exist
    if (!fs::exists("config.json")) {
        std::ofstream config("config.json");
        config << "{\n";
        config << "  \"version\": \"1.0.0\",\n";
        config << "  \"autoUpdate\": true,\n";
        config << "  \"updateChannel\": \"stable\",\n";
        config << "  \"pluginsEnabled\": true,\n";
        config << "  \"theme\": \"dark\",\n";
        config << "  \"editor\": {\n";
        config << "    \"fontSize\": 14,\n";
        config << "    \"tabSize\": 4,\n";
        config << "    \"wordWrap\": true\n";
        config << "  }\n";
        config << "}\n";
        config.close();
        std::cout << "Created default config.json" << std::endl;
    }

    return true;
}

bool AutoInstaller::downloadAssets(const std::vector<std::string>& urls) {
    for (const auto& url : urls) {
        std::cout << "Downloading asset: " << url << std::endl;
        
        std::string filename = url.substr(url.find_last_of('/') + 1);
        std::string command = "curl -L -o ./cache/" + filename + " " + url;
        
        std::string output;
        if (!runCommand(command, output)) {
            std::cerr << "Failed to download: " << url << std::endl;
            return false;
        }
    }

    return true;
}

bool AutoInstaller::executePostBuildSteps() {
    std::cout << "Executing post-build steps..." << std::endl;

    // Install dependencies
    if (!installAllDependencies()) {
        std::cerr << "Failed to install dependencies" << std::endl;
        return false;
    }

    // Configure environment
    if (!configureEnvironment()) {
        std::cerr << "Failed to configure environment" << std::endl;
        return false;
    }

    // Install frontend dependencies
    std::cout << "Installing frontend dependencies..." << std::endl;
    std::string output;
    if (!runCommand("cd frontend && npm install", output)) {
        std::cerr << "Failed to install frontend dependencies" << std::endl;
        return false;
    }

    std::cout << "Post-build steps completed successfully!" << std::endl;
    return true;
}

bool AutoInstaller::packageApplication() {
    std::cout << "Packaging application..." << std::endl;

    // Create package directory
    fs::create_directories("./package");

    // Copy binaries
    if (fs::exists("./hybrid-ide")) {
        fs::copy("./hybrid-ide", "./package/hybrid-ide", fs::copy_options::overwrite_existing);
    }

    // Copy frontend build
    if (fs::exists("./frontend/dist")) {
        fs::copy("./frontend/dist", "./package/dist", fs::copy_options::recursive | fs::copy_options::overwrite_existing);
    }

    // Copy plugins
    if (fs::exists("./plugins")) {
        fs::copy("./plugins", "./package/plugins", fs::copy_options::recursive | fs::copy_options::overwrite_existing);
    }

    // Create archive
    std::string archiveName = "hybrid-ide-" + platform_ + "-" + architecture_ + ".tar.gz";
    std::string command = "tar -czf " + archiveName + " -C ./package .";
    
    std::string output;
    if (!runCommand(command, output)) {
        std::cerr << "Failed to create package archive" << std::endl;
        return false;
    }

    std::cout << "Application packaged successfully: " << archiveName << std::endl;
    return true;
}

void AutoInstaller::setProgressCallback(std::function<void(const std::string&, int)> callback) {
    progressCallback_ = callback;
}

bool AutoInstaller::runCommand(const std::string& command, std::string& output) {
    FILE* pipe = popen((command + " 2>&1").c_str(), "r");
    if (!pipe) {
        return false;
    }

    char buffer[128];
    output.clear();
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int exitCode = pclose(pipe);
    return exitCode == 0;
}

bool AutoInstaller::isAdmin() {
#ifdef _WIN32
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    
    return isAdmin != FALSE;
#else
    return geteuid() == 0;
#endif
}

bool AutoInstaller::elevatePrivileges() {
#ifdef _WIN32
    // Request UAC elevation on Windows
    return false; // Not implemented
#else
    if (!isAdmin()) {
        std::cerr << "This operation requires root privileges. Please run with sudo." << std::endl;
        return false;
    }
    return true;
#endif
}

std::string AutoInstaller::getPackageManager() {
    if (platform_ == "linux") {
        std::string output;
        if (runCommand("which apt-get", output)) return "apt-get";
        if (runCommand("which yum", output)) return "yum";
        if (runCommand("which dnf", output)) return "dnf";
        if (runCommand("which pacman", output)) return "pacman";
    }
    else if (platform_ == "macos") {
        return "brew";
    }
    else if (platform_ == "windows") {
        return "choco";
    }

    return "";
}

} // namespace HybridIDE
