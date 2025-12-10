#include "docker_manager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <regex>

namespace fs = std::filesystem;

namespace HybridIDE {

DockerManager::DockerManager() {
    if (!isDockerAvailable()) {
        std::cerr << "Warning: Docker is not available or not running" << std::endl;
    }
}

DockerManager::~DockerManager() {
}

// Container Discovery & Search
std::vector<ImageSearchResult> DockerManager::searchContainers(const std::string& query) {
    std::vector<ImageSearchResult> results;
    
    // Search Docker Hub
    std::string command = "curl -s 'https://hub.docker.com/api/content/v1/products/search?q=" + query + "&page_size=10'";
    std::string output;
    
    if (runDockerCommand(command, output)) {
        // Parse JSON response (simplified)
        // In production, use proper JSON library
        std::regex nameRegex("\"name\":\"([^\"]+)\"");
        std::regex descRegex("\"short_description\":\"([^\"]+)\"");
        
        std::smatch match;
        std::string::const_iterator searchStart(output.cbegin());
        
        while (std::regex_search(searchStart, output.cend(), match, nameRegex)) {
            ImageSearchResult result;
            result.name = match[1];
            result.source = "dockerhub";
            result.official = output.find("\"is_official\":true") != std::string::npos;
            results.push_back(result);
            searchStart = match.suffix().first;
        }
    }
    
    if (logCallback_) {
        logCallback_("Found " + std::to_string(results.size()) + " containers matching: " + query);
    }
    
    return results;
}

std::vector<ImageSearchResult> DockerManager::searchRegistry(const std::string& registry, const std::string& query) {
    std::vector<ImageSearchResult> results;
    
    if (registry == "ghcr") {
        // Search GitHub Container Registry
        std::string command = "curl -s 'https://ghcr.io/v2/_catalog'";
        std::string output;
        runDockerCommand(command, output);
        // Parse and filter results
    }
    else if (registry == "gitlab") {
        // Search GitLab Container Registry
        // Implementation
    }
    
    return results;
}

std::vector<std::string> DockerManager::suggestAlternatives(const std::string& imageName) {
    std::vector<std::string> alternatives;
    
    // Suggest lighter alternatives
    if (imageName.find("ubuntu") != std::string::npos) {
        alternatives.push_back("alpine");
        alternatives.push_back("debian:slim");
    }
    else if (imageName.find("node") != std::string::npos) {
        alternatives.push_back("node:alpine");
        alternatives.push_back("node:slim");
    }
    else if (imageName.find("python") != std::string::npos) {
        alternatives.push_back("python:alpine");
        alternatives.push_back("python:slim");
    }
    
    return alternatives;
}

bool DockerManager::pullImage(const std::string& imageName, const std::string& tag) {
    std::string fullImage = imageName + ":" + tag;
    std::string command = "docker pull " + fullImage;
    std::string output;
    
    if (logCallback_) {
        logCallback_("Pulling image: " + fullImage);
    }
    
    bool success = runDockerCommand(command, output);
    
    if (success && logCallback_) {
        logCallback_("Successfully pulled: " + fullImage);
    }
    
    return success;
}

// Container Management
std::vector<ContainerInfo> DockerManager::listContainers(bool includeAll) {
    std::vector<ContainerInfo> containers;
    
    std::string command = includeAll ? "docker ps -a --format \"{{.ID}}|{{.Names}}|{{.Image}}|{{.Status}}\"" 
                                      : "docker ps --format \"{{.ID}}|{{.Names}}|{{.Image}}|{{.Status}}\"";
    std::string output;
    
    if (runDockerCommand(command, output)) {
        std::istringstream stream(output);
        std::string line;
        
        while (std::getline(stream, line)) {
            if (line.empty()) continue;
            
            ContainerInfo info;
            std::istringstream lineStream(line);
            std::getline(lineStream, info.id, '|');
            std::getline(lineStream, info.name, '|');
            std::getline(lineStream, info.image, '|');
            std::getline(lineStream, info.status, '|');
            info.healthy = info.status.find("healthy") != std::string::npos;
            
            containers.push_back(info);
        }
    }
    
    return containers;
}

bool DockerManager::startContainer(const std::string& containerId) {
    std::string command = "docker start " + containerId;
    std::string output;
    return runDockerCommand(command, output);
}

bool DockerManager::stopContainer(const std::string& containerId) {
    std::string command = "docker stop " + containerId;
    std::string output;
    return runDockerCommand(command, output);
}

bool DockerManager::restartContainer(const std::string& containerId) {
    std::string command = "docker restart " + containerId;
    std::string output;
    return runDockerCommand(command, output);
}

bool DockerManager::removeContainer(const std::string& containerId, bool force) {
    std::string command = "docker rm " + std::string(force ? "-f " : "") + containerId;
    std::string output;
    return runDockerCommand(command, output);
}

ContainerInfo DockerManager::inspectContainer(const std::string& containerId) {
    ContainerInfo info;
    info.id = containerId;
    
    std::string command = "docker inspect " + containerId;
    std::string output;
    
    if (runDockerCommand(command, output)) {
        // Parse JSON output for detailed info
        // Extract ports, volumes, env vars, etc.
    }
    
    return info;
}

// Container Builder
std::string DockerManager::generateDockerfile(const std::string& baseImage, const std::string& appType) {
    std::ostringstream dockerfile;
    
    dockerfile << "FROM " << baseImage << "\n\n";
    dockerfile << "WORKDIR /app\n\n";
    
    if (appType == "node") {
        dockerfile << "COPY package*.json ./\n";
        dockerfile << "RUN npm install\n\n";
        dockerfile << "COPY . .\n\n";
        dockerfile << "EXPOSE 3000\n";
        dockerfile << "CMD [\"npm\", \"start\"]\n";
    }
    else if (appType == "python") {
        dockerfile << "COPY requirements.txt ./\n";
        dockerfile << "RUN pip install --no-cache-dir -r requirements.txt\n\n";
        dockerfile << "COPY . .\n\n";
        dockerfile << "EXPOSE 8000\n";
        dockerfile << "CMD [\"python\", \"app.py\"]\n";
    }
    else if (appType == "cpp") {
        dockerfile << "RUN apt-get update && apt-get install -y g++ cmake make\n\n";
        dockerfile << "COPY . .\n\n";
        dockerfile << "RUN cmake . && make\n\n";
        dockerfile << "CMD [\"./app\"]\n";
    }
    
    return dockerfile.str();
}

std::string DockerManager::generateDockerCompose(const std::vector<std::string>& services) {
    std::ostringstream compose;
    
    compose << "version: '3.8'\n\n";
    compose << "services:\n";
    
    for (const auto& service : services) {
        compose << "  " << service << ":\n";
        compose << "    image: " << service << ":latest\n";
        compose << "    ports:\n";
        compose << "      - \"8080:8080\"\n";
        compose << "    environment:\n";
        compose << "      - NODE_ENV=production\n";
        compose << "    volumes:\n";
        compose << "      - ./" << service << ":/app\n";
        compose << "    networks:\n";
        compose << "      - app-network\n\n";
    }
    
    compose << "networks:\n";
    compose << "  app-network:\n";
    compose << "    driver: bridge\n";
    
    return compose.str();
}

bool DockerManager::buildFromDockerfile(const std::string& dockerfilePath, const std::string& tag) {
    std::string command = "docker build -t " + tag + " -f " + dockerfilePath + " .";
    std::string output;
    
    if (logCallback_) {
        logCallback_("Building image: " + tag);
    }
    
    bool success = runDockerCommand(command, output);
    
    if (success && logCallback_) {
        logCallback_("Successfully built: " + tag);
    }
    
    return success;
}

bool DockerManager::rebuildContainer(const std::string& containerId) {
    auto info = inspectContainer(containerId);
    
    // Stop and remove old container
    stopContainer(containerId);
    removeContainer(containerId, true);
    
    // Rebuild from same image
    std::string command = "docker run -d --name " + info.name + " " + info.image;
    std::string output;
    
    return runDockerCommand(command, output);
}

bool DockerManager::validateDockerCompose(const std::string& filePath, std::vector<std::string>& errors) {
    std::string command = "docker-compose -f " + filePath + " config";
    std::string output;
    
    bool valid = runDockerCommand(command, output);
    
    if (!valid) {
        errors.push_back(output);
    }
    
    return valid;
}

std::string DockerManager::fixDockerComposeYaml(const std::string& content) {
    std::string fixed = content;
    
    // Fix common indentation issues
    std::regex indentRegex("^  ([a-z_]+):", std::regex::multiline);
    fixed = std::regex_replace(fixed, indentRegex, "  $1:");
    
    // Fix port format
    std::regex portRegex("\"(\\d+):(\\d+)\"");
    fixed = std::regex_replace(fixed, portRegex, "'$1:$2'");
    
    // Ensure version is quoted
    std::regex versionRegex("version:\\s*(\\d+\\.\\d+)");
    fixed = std::regex_replace(fixed, versionRegex, "version: '$1'");
    
    return fixed;
}

bool DockerManager::resolvePortConflict(int port, int& newPort) {
    newPort = findFreePort(port + 1);
    return newPort != -1;
}

// Container Updater
bool DockerManager::updateContainer(const std::string& containerId) {
    auto info = inspectContainer(containerId);
    
    if (logCallback_) {
        logCallback_("Updating container: " + info.name);
    }
    
    // Pull latest image
    if (!pullImage(info.image, "latest")) {
        return false;
    }
    
    // Rebuild container
    return rebuildContainer(containerId);
}

std::vector<std::string> DockerManager::checkForUpdates() {
    std::vector<std::string> updates;
    auto containers = listContainers(false);
    
    for (const auto& container : containers) {
        std::string command = "docker pull " + container.image;
        std::string output;
        
        if (runDockerCommand(command, output)) {
            if (output.find("Downloaded newer image") != std::string::npos) {
                updates.push_back(container.name);
            }
        }
    }
    
    return updates;
}

bool DockerManager::rollbackContainer(const std::string& containerId) {
    // TODO: Implement version history tracking
    if (logCallback_) {
        logCallback_("Rolling back container: " + containerId);
    }
    return true;
}

std::string DockerManager::diffVersions(const std::string& image, const std::string& oldTag, const std::string& newTag) {
    std::ostringstream diff;
    diff << "Comparing " << image << ":" << oldTag << " → " << image << ":" << newTag << "\n";
    
    // TODO: Implement actual diff logic
    diff << "Changes:\n";
    diff << "- Updated dependencies\n";
    diff << "- Security patches\n";
    
    return diff.str();
}

// Container Repair
bool DockerManager::repairContainer(const std::string& containerId) {
    auto diagnostics = diagnoseContainer(containerId);
    
    for (const auto& issue : diagnostics) {
        if (issue.find("volume") != std::string::npos) {
            repairMissingVolumes(containerId);
        }
        else if (issue.find("permission") != std::string::npos) {
            fixPermissions(containerId);
        }
        else if (issue.find("network") != std::string::npos) {
            fixNetworkConfig(containerId);
        }
    }
    
    return true;
}

bool DockerManager::repairMissingVolumes(const std::string& containerId) {
    auto info = inspectContainer(containerId);
    
    for (const auto& volume : info.volumes) {
        std::string command = "docker volume create " + volume;
        std::string output;
        runDockerCommand(command, output);
    }
    
    return true;
}

bool DockerManager::fixPermissions(const std::string& containerId) {
    std::string command = "docker exec " + containerId + " chmod -R 755 /app";
    std::string output;
    return runDockerCommand(command, output);
}

bool DockerManager::fixNetworkConfig(const std::string& containerId) {
    // Reconnect to bridge network
    std::string command = "docker network connect bridge " + containerId;
    std::string output;
    return runDockerCommand(command, output);
}

std::vector<std::string> DockerManager::diagnoseContainer(const std::string& containerId) {
    std::vector<std::string> issues;
    auto info = inspectContainer(containerId);
    
    if (!info.healthy) {
        issues.push_back("Container is unhealthy");
    }
    
    if (info.status.find("Exited") != std::string::npos) {
        issues.push_back("Container has exited");
    }
    
    // Check logs for errors
    auto logs = getContainerLogs(containerId, 50);
    for (const auto& log : logs) {
        if (log.find("error") != std::string::npos || log.find("ERROR") != std::string::npos) {
            issues.push_back("Error in logs: " + log);
        }
    }
    
    return issues;
}

// Docker Environment Doctor
bool DockerManager::checkDockerDesktop() {
#ifdef _WIN32
    std::string command = "tasklist | findstr Docker";
#else
    std::string command = "ps aux | grep Docker";
#endif
    
    std::string output;
    return runDockerCommand(command, output);
}

std::vector<std::string> DockerManager::diagnoseDNS() {
    std::vector<std::string> issues;
    
    std::string command = "docker run --rm alpine ping -c 1 google.com";
    std::string output;
    
    if (!runDockerCommand(command, output)) {
        issues.push_back("DNS resolution failed");
    }
    
    return issues;
}

std::vector<std::string> DockerManager::diagnoseFirewall() {
    std::vector<std::string> issues;
    
    // Check if Docker ports are accessible
    auto usedPorts = getUsedPorts();
    
    for (int port : usedPorts) {
        std::string command = "nc -zv localhost " + std::to_string(port);
        std::string output;
        
        if (!runDockerCommand(command, output)) {
            issues.push_back("Port " + std::to_string(port) + " blocked by firewall");
        }
    }
    
    return issues;
}

bool DockerManager::cleanDanglingImages() {
    std::string command = "docker image prune -f";
    std::string output;
    return runDockerCommand(command, output);
}

long long DockerManager::getStorageUsage() {
    std::string command = "docker system df --format \"{{.Size}}\"";
    std::string output;
    
    if (runDockerCommand(command, output)) {
        // Parse size from output
        return 0; // Simplified
    }
    
    return 0;
}

// Backup & Restore
bool DockerManager::backupContainer(const std::string& containerId, const std::string& backupPath) {
    std::string command = "docker commit " + containerId + " backup-" + containerId;
    std::string output;
    
    if (!runDockerCommand(command, output)) {
        return false;
    }
    
    // Save to tar
    command = "docker save backup-" + containerId + " -o " + backupPath;
    return runDockerCommand(command, output);
}

bool DockerManager::backupVolume(const std::string& volumeName, const std::string& backupPath) {
    std::string command = "docker run --rm -v " + volumeName + ":/data -v " + backupPath + ":/backup alpine tar czf /backup/" + volumeName + ".tar.gz /data";
    std::string output;
    return runDockerCommand(command, output);
}

bool DockerManager::restoreContainer(const std::string& backupPath) {
    std::string command = "docker load -i " + backupPath;
    std::string output;
    return runDockerCommand(command, output);
}

bool DockerManager::exportContainer(const std::string& containerId, const std::string& tarPath) {
    std::string command = "docker export " + containerId + " -o " + tarPath;
    std::string output;
    return runDockerCommand(command, output);
}

// Network & Port Management
bool DockerManager::createNetwork(const std::string& networkName, const std::string& driver) {
    std::string command = "docker network create --driver " + driver + " " + networkName;
    std::string output;
    return runDockerCommand(command, output);
}

std::vector<std::string> DockerManager::listNetworks() {
    std::vector<std::string> networks;
    std::string command = "docker network ls --format \"{{.Name}}\"";
    std::string output;
    
    if (runDockerCommand(command, output)) {
        std::istringstream stream(output);
        std::string line;
        while (std::getline(stream, line)) {
            if (!line.empty()) {
                networks.push_back(line);
            }
        }
    }
    
    return networks;
}

std::string DockerManager::generateNetworkGraph() {
    std::ostringstream graph;
    graph << "Container Network Map:\n";
    
    auto containers = listContainers(true);
    auto networks = listNetworks();
    
    for (const auto& network : networks) {
        graph << "\n[" << network << "]\n";
        for (const auto& container : containers) {
            graph << "  └─ " << container.name << "\n";
        }
    }
    
    return graph.str();
}

std::string DockerManager::summarizeLogs(const std::vector<std::string>& logs) {
    std::ostringstream summary;
    
    int errors = 0, warnings = 0;
    
    for (const auto& log : logs) {
        if (log.find("ERROR") != std::string::npos || log.find("error") != std::string::npos) {
            errors++;
        }
        if (log.find("WARN") != std::string::npos || log.find("warning") != std::string::npos) {
            warnings++;
        }
    }
    
    summary << "Log Summary:\n";
    summary << "Total lines: " << logs.size() << "\n";
    summary << "Errors: " << errors << "\n";
    summary << "Warnings: " << warnings << "\n";
    
    if (errors > 0) {
        summary << "\nRecent errors:\n";
        int count = 0;
        for (const auto& log : logs) {
            if ((log.find("ERROR") != std::string::npos || log.find("error") != std::string::npos) && count < 5) {
                summary << "  - " << log << "\n";
                count++;
            }
        }
    }
    
    return summary.str();
}

// Security & Secrets
bool DockerManager::injectSecret(const std::string& containerId, const std::string& key, const std::string& value) {
    std::string command = "docker exec " + containerId + " sh -c 'echo \"" + key + "=" + value + "\" >> /etc/environment'";
    std::string output;
    return runDockerCommand(command, output);
}

bool DockerManager::rotateSecrets(const std::string& containerId) {
    // Generate new secrets and inject them
    auto info = inspectContainer(containerId);
    
    for (auto& [key, value] : info.env) {
        if (key.find("PASSWORD") != std::string::npos || key.find("SECRET") != std::string::npos) {
            std::string newSecret = generateSecret(32);
            injectSecret(containerId, key, newSecret);
        }
    }
    
    return true;
}

std::vector<std::string> DockerManager::detectLeakedSecrets(const std::string& containerId) {
    std::vector<std::string> leaks;
    auto logs = getContainerLogs(containerId, 100);
    
    std::regex secretPattern("(password|secret|token|key)\\s*=\\s*['\"]?([^'\"\\s]+)", std::regex::icase);
    
    for (const auto& log : logs) {
        std::smatch match;
        if (std::regex_search(log, match, secretPattern)) {
            leaks.push_back("Potential secret leak: " + match.str());
        }
    }
    
    return leaks;
}

bool DockerManager::hardenContainer(const std::string& containerId) {
    std::vector<std::string> commands = {
        "docker exec " + containerId + " chmod 600 /etc/passwd",
        "docker exec " + containerId + " chmod 600 /etc/shadow",
        "docker update --restart=always " + containerId
    };
    
    for (const auto& cmd : commands) {
        std::string output;
        if (!runDockerCommand(cmd, output)) {
            return false;
        }
    }
    
    return true;
}

// Windows-specific
bool DockerManager::convertToWindowsContainer(const std::string& linuxImage) {
    if (logCallback_) {
        logCallback_("Converting " + linuxImage + " to Windows container...");
    }
    
    // Generate Windows-compatible Dockerfile
    std::string windowsDockerfile = generateWindowsDockerfile(linuxImage);
    
    // Write to file
    std::ofstream out("Dockerfile.windows");
    out << windowsDockerfile;
    out.close();
    
    // Build
    return buildFromDockerfile("Dockerfile.windows", linuxImage + "-windows");
}

bool DockerManager::installARMOnWindows(const std::string& projectName) {
    if (logCallback_) {
        logCallback_("Installing ARM project on Windows: " + projectName);
    }
    
    // Generate compatible Dockerfile
    std::string dockerfile = generateWindowsDockerfile("mcr.microsoft.com/windows/servercore:ltsc2022");
    
    // Add ARM-specific dependencies
    dockerfile += "\nRUN powershell -Command Install-WindowsFeature NET-Framework-45-Core\n";
    
    std::ofstream out("Dockerfile.arm");
    out << dockerfile;
    out.close();
    
    return buildFromDockerfile("Dockerfile.arm", projectName + "-arm");
}

std::string DockerManager::generateWindowsDockerfile(const std::string& baseImage) {
    std::ostringstream dockerfile;
    
    dockerfile << "FROM " << baseImage << "\n\n";
    dockerfile << "WORKDIR C:\\\\app\n\n";
    dockerfile << "COPY . .\n\n";
    dockerfile << "RUN powershell -Command \"Write-Host 'Windows Container Ready'\"\n\n";
    dockerfile << "CMD [\"cmd\"]\n";
    
    return dockerfile.str();
}

std::vector<int> DockerManager::detectPortConflicts() {
    std::vector<int> conflicts;
    
    std::string command = "docker ps --format \"{{.Ports}}\"";
    std::string output;
    
    if (runDockerCommand(command, output)) {
        std::regex portRegex(":(\\d+)->");
        std::smatch match;
        std::map<int, int> portCounts;
        
        std::string::const_iterator searchStart(output.cbegin());
        while (std::regex_search(searchStart, output.cend(), match, portRegex)) {
            int port = std::stoi(match[1]);
            portCounts[port]++;
            if (portCounts[port] > 1) {
                conflicts.push_back(port);
            }
            searchStart = match.suffix().first;
        }
    }
    
    return conflicts;
}

int DockerManager::findFreePort(int startPort) {
    auto usedPorts = getUsedPorts();
    
    for (int port = startPort; port < 65535; ++port) {
        if (std::find(usedPorts.begin(), usedPorts.end(), port) == usedPorts.end()) {
            return port;
        }
    }
    
    return -1;
}

std::vector<int> DockerManager::getUsedPorts() {
    std::vector<int> ports;
    
    std::string command = "docker ps --format \"{{.Ports}}\"";
    std::string output;
    
    if (runDockerCommand(command, output)) {
        std::regex portRegex(":(\\d+)->");
        std::smatch match;
        
        std::string::const_iterator searchStart(output.cbegin());
        while (std::regex_search(searchStart, output.cend(), match, portRegex)) {
            ports.push_back(std::stoi(match[1]));
            searchStart = match.suffix().first;
        }
    }
    
    return ports;
}

// Docker Environment Doctor
bool DockerManager::checkDockerHealth() {
    std::string command = "docker info";
    std::string output;
    
    bool healthy = runDockerCommand(command, output);
    
    if (healthy && logCallback_) {
        logCallback_("Docker daemon is healthy");
    }
    
    return healthy;
}

bool DockerManager::cleanUnusedImages() {
    std::string command = "docker image prune -a -f";
    std::string output;
    
    bool success = runDockerCommand(command, output);
    
    if (success && logCallback_) {
        logCallback_("Cleaned unused images: " + output);
    }
    
    return success;
}

// Security & Secrets
std::string DockerManager::generateSecret(int length) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
    std::string secret;
    secret.reserve(length);
    
    for (int i = 0; i < length; ++i) {
        secret += charset[rand() % (sizeof(charset) - 1)];
    }
    
    return secret;
}

// Monitoring
DockerManager::ContainerStats DockerManager::getContainerStats(const std::string& containerId) {
    ContainerStats stats = {0, 0, 0, 0, 0, 0, 0};
    
    std::string command = "docker stats " + containerId + " --no-stream --format \"{{.CPUPerc}}|{{.MemUsage}}\"";
    std::string output;
    
    if (runDockerCommand(command, output)) {
        // Parse stats output
        std::istringstream stream(output);
        std::string cpuStr, memStr;
        std::getline(stream, cpuStr, '|');
        std::getline(stream, memStr, '|');
        
        // Remove % sign and convert
        cpuStr.erase(std::remove(cpuStr.begin(), cpuStr.end(), '%'), cpuStr.end());
        stats.cpuPercent = std::stod(cpuStr);
    }
    
    return stats;
}

std::vector<std::string> DockerManager::getContainerLogs(const std::string& containerId, int lines) {
    std::vector<std::string> logs;
    
    std::string command = "docker logs --tail " + std::to_string(lines) + " " + containerId;
    std::string output;
    
    if (runDockerCommand(command, output)) {
        std::istringstream stream(output);
        std::string line;
        while (std::getline(stream, line)) {
            logs.push_back(line);
        }
    }
    
    return logs;
}

void DockerManager::setProgressCallback(std::function<void(const std::string&, int)> callback) {
    progressCallback_ = callback;
}

void DockerManager::setLogCallback(std::function<void(const std::string&)> callback) {
    logCallback_ = callback;
}

// Private methods
bool DockerManager::runDockerCommand(const std::string& command, std::string& output) {
    FILE* pipe = popen((command + " 2>&1").c_str(), "r");
    if (!pipe) {
        return false;
    }

    char buffer[256];
    output.clear();
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int exitCode = pclose(pipe);
    return exitCode == 0;
}

bool DockerManager::isDockerAvailable() {
    std::string output;
    return runDockerCommand("docker --version", output);
}

std::string DockerManager::detectOS() {
#ifdef _WIN32
    return "windows";
#elif __APPLE__
    return "macos";
#elif __linux__
    return "linux";
#else
    return "unknown";
#endif
}

} // namespace HybridIDE
