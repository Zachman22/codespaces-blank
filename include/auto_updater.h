#ifndef AUTO_UPDATER_H
#define AUTO_UPDATER_H

#include <string>
#include <functional>
#include <chrono>

namespace HybridIDE {

struct UpdateInfo {
    std::string version;
    std::string releaseNotes;
    std::string downloadUrl;
    std::string checksum;
    long long size;
    bool critical;
    std::string publishedDate;
};

enum class UpdateChannel {
    STABLE,
    BETA,
    DEV,
    NIGHTLY
};

class AutoUpdater {
public:
    AutoUpdater();
    ~AutoUpdater();

    // Update checking
    bool checkForUpdates();
    bool checkForUpdates(UpdateChannel channel);
    UpdateInfo getLatestVersion();
    bool isUpdateAvailable();
    std::string getCurrentVersion();
    
    // Update installation
    bool downloadUpdate(const UpdateInfo& update);
    bool verifyUpdate(const std::string& filePath, const std::string& checksum);
    bool installUpdate(const std::string& filePath);
    bool applyUpdate(bool restartRequired = true);
    
    // Automatic updates
    void enableAutoUpdates(bool enable);
    void setUpdateChannel(UpdateChannel channel);
    void setUpdateCheckInterval(std::chrono::hours interval);
    
    // Rollback
    bool createBackup();
    bool rollbackUpdate();
    std::vector<std::string> getUpdateHistory();
    
    // Callbacks
    void setUpdateAvailableCallback(std::function<void(const UpdateInfo&)> callback);
    void setDownloadProgressCallback(std::function<void(int, long long, long long)> callback);
    void setUpdateCompleteCallback(std::function<void(bool)> callback);

private:
    std::string updateServerUrl_;
    std::string currentVersion_;
    UpdateChannel channel_;
    bool autoUpdateEnabled_;
    std::chrono::hours checkInterval_;
    
    std::function<void(const UpdateInfo&)> updateAvailableCallback_;
    std::function<void(int, long long, long long)> downloadProgressCallback_;
    std::function<void(bool)> updateCompleteCallback_;
    
    bool compareVersions(const std::string& v1, const std::string& v2);
    std::string fetchUpdateManifest(UpdateChannel channel);
    bool downloadFile(const std::string& url, const std::string& destination);
    std::string calculateChecksum(const std::string& filePath);
    bool extractUpdate(const std::string& archivePath, const std::string& destination);
};

} // namespace HybridIDE

#endif // AUTO_UPDATER_H
