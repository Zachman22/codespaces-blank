#ifndef DOCKER_MANAGER_H
#define DOCKER_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace HybridIDE {

struct ContainerInfo {
    std::string id;
    std::string name;
    std::string image;
    std::string status;
    std::vector<std::string> ports;
    std::vector<std::string> volumes;
    std::map<std::string, std::string> env;
    bool healthy;
};

struct ImageSearchResult {
    std::string name;
    std::string description;
    int stars;
    bool official;
    bool verified;
    std::string source; // dockerhub, ghcr, gitlab
};

struct DockerComposeConfig {
    std::string version;
    std::map<std::string, std::string> services;
    std::map<std::string, std::string> networks;
    std::map<std::string, std::string> volumes;
};

class DockerManager {
public:
    DockerManager();
    ~DockerManager();

    // Container Discovery & Search
    std::vector<ImageSearchResult> searchContainers(const std::string& query);
    std::vector<ImageSearchResult> searchRegistry(const std::string& registry, const std::string& query);
    std::vector<std::string> suggestAlternatives(const std::string& imageName);
    bool pullImage(const std::string& imageName, const std::string& tag = "latest");
    
    // Container Management
    std::vector<ContainerInfo> listContainers(bool includeAll = true);
    bool startContainer(const std::string& containerId);
    bool stopContainer(const std::string& containerId);
    bool restartContainer(const std::string& containerId);
    bool removeContainer(const std::string& containerId, bool force = false);
    ContainerInfo inspectContainer(const std::string& containerId);
    
    // Container Builder
    bool buildFromDockerfile(const std::string& dockerfilePath, const std::string& tag);
    std::string generateDockerfile(const std::string& baseImage, const std::string& appType);
    std::string generateDockerCompose(const std::vector<std::string>& services);
    bool rebuildContainer(const std::string& containerId);
    
    // Auto-fix & Validation
    bool validateDockerCompose(const std::string& filePath, std::vector<std::string>& errors);
    std::string fixDockerComposeYaml(const std::string& content);
    std::vector<int> detectPortConflicts();
    bool resolvePortConflict(int port, int& newPort);
    
    // Container Updater
    bool updateContainer(const std::string& containerId);
    std::vector<std::string> checkForUpdates();
    bool rollbackContainer(const std::string& containerId);
    std::string diffVersions(const std::string& image, const std::string& oldTag, const std::string& newTag);
    
    // Container Repair
    bool repairContainer(const std::string& containerId);
    bool repairMissingVolumes(const std::string& containerId);
    bool fixPermissions(const std::string& containerId);
    bool fixNetworkConfig(const std::string& containerId);
    std::vector<std::string> diagnoseContainer(const std::string& containerId);
    
    // Docker Environment Doctor
    bool checkDockerHealth();
    bool checkDockerDesktop();
    std::vector<std::string> diagnoseDNS();
    std::vector<std::string> diagnoseFirewall();
    bool cleanUnusedImages();
    bool cleanDanglingImages();
    long long getStorageUsage();
    
    // Backup & Restore
    bool backupContainer(const std::string& containerId, const std::string& backupPath);
    bool backupVolume(const std::string& volumeName, const std::string& backupPath);
    bool restoreContainer(const std::string& backupPath);
    bool exportContainer(const std::string& containerId, const std::string& tarPath);
    
    // Network & Port Management
    std::vector<int> getUsedPorts();
    int findFreePort(int startPort = 8000);
    bool createNetwork(const std::string& networkName, const std::string& driver = "bridge");
    std::vector<std::string> listNetworks();
    std::string generateNetworkGraph();
    
    // Monitoring
    struct ContainerStats {
        double cpuPercent;
        long long memoryUsage;
        long long memoryLimit;
        long long networkRx;
        long long networkTx;
        long long blockRead;
        long long blockWrite;
    };
    
    ContainerStats getContainerStats(const std::string& containerId);
    std::vector<std::string> getContainerLogs(const std::string& containerId, int lines = 100);
    std::string summarizeLogs(const std::vector<std::string>& logs);
    
    // Security & Secrets
    bool injectSecret(const std::string& containerId, const std::string& key, const std::string& value);
    std::string generateSecret(int length = 32);
    bool rotateSecrets(const std::string& containerId);
    std::vector<std::string> detectLeakedSecrets(const std::string& containerId);
    bool hardenContainer(const std::string& containerId);
    
    // Windows-specific
    bool convertToWindowsContainer(const std::string& linuxImage);
    bool installARMOnWindows(const std::string& projectName);
    std::string generateWindowsDockerfile(const std::string& baseImage);
    
    // Callbacks
    void setProgressCallback(std::function<void(const std::string&, int)> callback);
    void setLogCallback(std::function<void(const std::string&)> callback);

private:
    std::function<void(const std::string&, int)> progressCallback_;
    std::function<void(const std::string&)> logCallback_;
    
    bool runDockerCommand(const std::string& command, std::string& output);
    bool isDockerAvailable();
    std::string detectOS();
    std::vector<std::string> parseDockerOutput(const std::string& output);
};

} // namespace HybridIDE

#endif // DOCKER_MANAGER_H
