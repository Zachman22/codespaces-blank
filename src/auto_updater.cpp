#include "auto_updater.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <thread>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace HybridIDE {

AutoUpdater::AutoUpdater() 
    : updateServerUrl_("https://api.github.com/repos/hybrid-ide/hybrid-ide/releases"),
      currentVersion_("1.0.0"),
      channel_(UpdateChannel::STABLE),
      autoUpdateEnabled_(true),
      checkInterval_(std::chrono::hours(24)) {
}

AutoUpdater::~AutoUpdater() {
}

bool AutoUpdater::checkForUpdates() {
    return checkForUpdates(channel_);
}

bool AutoUpdater::checkForUpdates(UpdateChannel channel) {
    std::cout << "Checking for updates..." << std::endl;

    try {
        std::string manifest = fetchUpdateManifest(channel);
        if (manifest.empty()) {
            std::cout << "No updates available" << std::endl;
            return false;
        }

        // Parse manifest and check version
        UpdateInfo latest = getLatestVersion();
        
        if (compareVersions(latest.version, currentVersion_) > 0) {
            std::cout << "Update available: " << latest.version << std::endl;
            
            if (updateAvailableCallback_) {
                updateAvailableCallback_(latest);
            }
            
            return true;
        } else {
            std::cout << "Already on latest version: " << currentVersion_ << std::endl;
            return false;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error checking for updates: " << e.what() << std::endl;
        return false;
    }
}

UpdateInfo AutoUpdater::getLatestVersion() {
    UpdateInfo info;
    
    // Fetch from GitHub API or update server
    std::string command = "curl -s " + updateServerUrl_ + "/latest";
    FILE* pipe = popen(command.c_str(), "r");
    
    if (!pipe) {
        return info;
    }

    std::stringstream response;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        response << buffer;
    }
    pclose(pipe);

    // Simple parsing (in production, use JSON library)
    std::string data = response.str();
    
    // Extract version (tag_name)
    size_t tagPos = data.find("\"tag_name\":");
    if (tagPos != std::string::npos) {
        size_t start = data.find("\"", tagPos + 11) + 1;
        size_t end = data.find("\"", start);
        info.version = data.substr(start, end - start);
        
        // Remove 'v' prefix if present
        if (!info.version.empty() && info.version[0] == 'v') {
            info.version = info.version.substr(1);
        }
    }

    // Extract download URL
    size_t urlPos = data.find("\"browser_download_url\":");
    if (urlPos != std::string::npos) {
        size_t start = data.find("\"", urlPos + 23) + 1;
        size_t end = data.find("\"", start);
        info.downloadUrl = data.substr(start, end - start);
    }

    // Extract release notes
    size_t bodyPos = data.find("\"body\":");
    if (bodyPos != std::string::npos) {
        size_t start = data.find("\"", bodyPos + 7) + 1;
        size_t end = data.find("\"", start);
        info.releaseNotes = data.substr(start, end - start);
    }

    return info;
}

bool AutoUpdater::isUpdateAvailable() {
    UpdateInfo latest = getLatestVersion();
    return compareVersions(latest.version, currentVersion_) > 0;
}

std::string AutoUpdater::getCurrentVersion() {
    return currentVersion_;
}

bool AutoUpdater::downloadUpdate(const UpdateInfo& update) {
    if (update.downloadUrl.empty()) {
        std::cerr << "No download URL provided" << std::endl;
        return false;
    }

    std::cout << "Downloading update from: " << update.downloadUrl << std::endl;

    // Create updates directory
    fs::create_directories("./updates");
    
    std::string filename = "./updates/update-" + update.version + ".tar.gz";
    
    // Download with progress
    if (!downloadFile(update.downloadUrl, filename)) {
        std::cerr << "Failed to download update" << std::endl;
        return false;
    }

    std::cout << "Update downloaded successfully: " << filename << std::endl;
    return true;
}

bool AutoUpdater::verifyUpdate(const std::string& filePath, const std::string& checksum) {
    if (checksum.empty()) {
        std::cout << "No checksum provided, skipping verification" << std::endl;
        return true;
    }

    std::string calculated = calculateChecksum(filePath);
    
    if (calculated == checksum) {
        std::cout << "Update verification successful" << std::endl;
        return true;
    } else {
        std::cerr << "Update verification failed!" << std::endl;
        std::cerr << "Expected: " << checksum << std::endl;
        std::cerr << "Got: " << calculated << std::endl;
        return false;
    }
}

bool AutoUpdater::installUpdate(const std::string& filePath) {
    std::cout << "Installing update from: " << filePath << std::endl;

    // Create backup
    if (!createBackup()) {
        std::cerr << "Failed to create backup" << std::endl;
        return false;
    }

    // Extract update
    std::string extractPath = "./updates/extracted";
    if (!extractUpdate(filePath, extractPath)) {
        std::cerr << "Failed to extract update" << std::endl;
        return false;
    }

    // Copy new files
    try {
        for (const auto& entry : fs::recursive_directory_iterator(extractPath)) {
            if (entry.is_regular_file()) {
                std::string relativePath = entry.path().string().substr(extractPath.length() + 1);
                std::string destPath = "./" + relativePath;
                
                fs::create_directories(fs::path(destPath).parent_path());
                fs::copy(entry.path(), destPath, fs::copy_options::overwrite_existing);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error installing update: " << e.what() << std::endl;
        return false;
    }

    std::cout << "Update installed successfully" << std::endl;
    return true;
}

bool AutoUpdater::applyUpdate(bool restartRequired) {
    std::cout << "Applying update..." << std::endl;

    if (restartRequired) {
        std::cout << "Restart required to complete update" << std::endl;
        std::cout << "Please restart the application" << std::endl;
    }

    if (updateCompleteCallback_) {
        updateCompleteCallback_(true);
    }

    return true;
}

void AutoUpdater::enableAutoUpdates(bool enable) {
    autoUpdateEnabled_ = enable;
    std::cout << "Auto-updates " << (enable ? "enabled" : "disabled") << std::endl;
}

void AutoUpdater::setUpdateChannel(UpdateChannel channel) {
    channel_ = channel;
    std::string channelName;
    switch (channel) {
        case UpdateChannel::STABLE: channelName = "stable"; break;
        case UpdateChannel::BETA: channelName = "beta"; break;
        case UpdateChannel::DEV: channelName = "dev"; break;
        case UpdateChannel::NIGHTLY: channelName = "nightly"; break;
    }
    std::cout << "Update channel set to: " << channelName << std::endl;
}

void AutoUpdater::setUpdateCheckInterval(std::chrono::hours interval) {
    checkInterval_ = interval;
    std::cout << "Update check interval set to: " << interval.count() << " hours" << std::endl;
}

bool AutoUpdater::createBackup() {
    std::cout << "Creating backup..." << std::endl;

    std::string backupDir = "./backups/backup-" + currentVersion_;
    fs::create_directories(backupDir);

    try {
        // Backup main executable
        if (fs::exists("./hybrid-ide")) {
            fs::copy("./hybrid-ide", backupDir + "/hybrid-ide", fs::copy_options::overwrite_existing);
        }

        // Backup important directories
        if (fs::exists("./lib")) {
            fs::copy("./lib", backupDir + "/lib", fs::copy_options::recursive);
        }
        if (fs::exists("./plugins")) {
            fs::copy("./plugins", backupDir + "/plugins", fs::copy_options::recursive);
        }

        std::cout << "Backup created: " << backupDir << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to create backup: " << e.what() << std::endl;
        return false;
    }
}

bool AutoUpdater::rollbackUpdate() {
    std::cout << "Rolling back update..." << std::endl;

    // Find most recent backup
    std::string latestBackup;
    for (const auto& entry : fs::directory_iterator("./backups")) {
        if (entry.is_directory()) {
            latestBackup = entry.path().string();
        }
    }

    if (latestBackup.empty()) {
        std::cerr << "No backup found for rollback" << std::endl;
        return false;
    }

    std::cout << "Restoring from backup: " << latestBackup << std::endl;

    try {
        // Restore files
        for (const auto& entry : fs::recursive_directory_iterator(latestBackup)) {
            if (entry.is_regular_file()) {
                std::string relativePath = entry.path().string().substr(latestBackup.length() + 1);
                std::string destPath = "./" + relativePath;
                
                fs::copy(entry.path(), destPath, fs::copy_options::overwrite_existing);
            }
        }

        std::cout << "Rollback completed successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Rollback failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> AutoUpdater::getUpdateHistory() {
    std::vector<std::string> history;
    
    if (fs::exists("./backups")) {
        for (const auto& entry : fs::directory_iterator("./backups")) {
            if (entry.is_directory()) {
                history.push_back(entry.path().filename().string());
            }
        }
    }

    return history;
}

void AutoUpdater::setUpdateAvailableCallback(std::function<void(const UpdateInfo&)> callback) {
    updateAvailableCallback_ = callback;
}

void AutoUpdater::setDownloadProgressCallback(std::function<void(int, long long, long long)> callback) {
    downloadProgressCallback_ = callback;
}

void AutoUpdater::setUpdateCompleteCallback(std::function<void(bool)> callback) {
    updateCompleteCallback_ = callback;
}

bool AutoUpdater::compareVersions(const std::string& v1, const std::string& v2) {
    // Simple version comparison (assumes semver format)
    auto parseVersion = [](const std::string& v) {
        std::vector<int> parts;
        std::stringstream ss(v);
        std::string part;
        while (std::getline(ss, part, '.')) {
            parts.push_back(std::stoi(part));
        }
        return parts;
    };

    auto parts1 = parseVersion(v1);
    auto parts2 = parseVersion(v2);

    for (size_t i = 0; i < std::min(parts1.size(), parts2.size()); ++i) {
        if (parts1[i] > parts2[i]) return 1;
        if (parts1[i] < parts2[i]) return -1;
    }

    if (parts1.size() > parts2.size()) return 1;
    if (parts1.size() < parts2.size()) return -1;
    return 0;
}

std::string AutoUpdater::fetchUpdateManifest(UpdateChannel channel) {
    std::string url = updateServerUrl_;
    
    switch (channel) {
        case UpdateChannel::BETA:
            url += "?prerelease=true";
            break;
        case UpdateChannel::DEV:
        case UpdateChannel::NIGHTLY:
            url += "/tags/dev";
            break;
        default:
            url += "/latest";
            break;
    }

    std::string command = "curl -s " + url;
    FILE* pipe = popen(command.c_str(), "r");
    
    if (!pipe) {
        return "";
    }

    std::stringstream manifest;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        manifest << buffer;
    }
    pclose(pipe);

    return manifest.str();
}

bool AutoUpdater::downloadFile(const std::string& url, const std::string& destination) {
    std::string command = "curl -L -o " + destination + " " + url;
    
    if (downloadProgressCallback_) {
        command += " --progress-bar";
    }

    int result = system(command.c_str());
    return result == 0;
}

std::string AutoUpdater::calculateChecksum(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return "";
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    SHA256_Update(&sha256, buffer, file.gcount());

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

bool AutoUpdater::extractUpdate(const std::string& archivePath, const std::string& destination) {
    fs::create_directories(destination);
    
    std::string command = "tar -xzf " + archivePath + " -C " + destination;
    int result = system(command.c_str());
    
    return result == 0;
}

} // namespace HybridIDE
