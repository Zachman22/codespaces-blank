#ifndef CLOUD_SYNC_H
#define CLOUD_SYNC_H

#include <string>
#include <vector>
#include <map>
#include <functional>

/**
 * Cloud Sync - Sync projects to cloud storage (AWS S3, Azure, GCP)
 */
class CloudSync {
public:
    enum class Provider {
        AWS_S3,
        AZURE_BLOB,
        GOOGLE_CLOUD,
        GITHUB,
        CUSTOM
    };

    enum class SyncStatus {
        IDLE,
        SYNCING,
        SUCCESS,
        ERROR,
        CONFLICT
    };

    struct SyncConfig {
        Provider provider;
        std::string endpoint;
        std::string bucket;
        std::string accessKey;
        std::string secretKey;
        bool autoSync;
        int syncIntervalSeconds;
    };

    struct FileChange {
        std::string path;
        std::string action;  // "added", "modified", "deleted"
        std::string timestamp;
        std::string hash;
    };

    struct SyncResult {
        SyncStatus status;
        int filesUploaded;
        int filesDownloaded;
        int filesDeleted;
        std::vector<std::string> errors;
        std::string message;
    };

    CloudSync();
    ~CloudSync();

    // Initialize
    bool initialize(const SyncConfig& config);
    bool testConnection();

    // Sync operations
    SyncResult syncUp(const std::string& localPath);
    SyncResult syncDown(const std::string& localPath);
    SyncResult fullSync(const std::string& localPath);

    // Auto-sync
    void enableAutoSync(bool enable);
    bool isAutoSyncEnabled() const;
    void setAutoSyncInterval(int seconds);

    // File operations
    bool uploadFile(const std::string& localPath, const std::string& remotePath);
    bool downloadFile(const std::string& remotePath, const std::string& localPath);
    bool deleteFile(const std::string& remotePath);
    
    // Directory operations
    bool uploadDirectory(const std::string& localPath, const std::string& remotePath);
    bool downloadDirectory(const std::string& remotePath, const std::string& localPath);
    
    // Listing
    std::vector<std::string> listRemoteFiles(const std::string& remotePath = "") const;
    
    // Change tracking
    std::vector<FileChange> getLocalChanges(const std::string& localPath) const;
    std::vector<FileChange> getRemoteChanges(const std::string& remotePath) const;
    
    // Conflict resolution
    std::vector<FileChange> detectConflicts(const std::string& localPath) const;
    bool resolveConflict(const FileChange& change, bool useLocal);

    // Callbacks
    void setProgressCallback(std::function<void(int current, int total)> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);

    // Status
    SyncStatus getStatus() const;
    std::string getLastError() const;

private:
    SyncConfig config_;
    SyncStatus status_;
    std::string lastError_;
    bool autoSyncEnabled_;
    
    std::function<void(int, int)> progressCallback_;
    std::function<void(const std::string&)> errorCallback_;
    
    std::map<std::string, std::string> fileHashes_;  // path -> hash

    // Provider-specific implementations
    bool uploadToS3(const std::string& localPath, const std::string& remotePath);
    bool downloadFromS3(const std::string& remotePath, const std::string& localPath);
    bool uploadToAzure(const std::string& localPath, const std::string& remotePath);
    bool downloadFromAzure(const std::string& remotePath, const std::string& localPath);
    bool uploadToGCS(const std::string& localPath, const std::string& remotePath);
    bool downloadFromGCS(const std::string& remotePath, const std::string& localPath);
    
    // Utilities
    std::string calculateFileHash(const std::string& filepath) const;
    bool compareFiles(const std::string& file1, const std::string& file2) const;
    std::vector<std::string> scanDirectory(const std::string& path) const;
};

#endif // CLOUD_SYNC_H
