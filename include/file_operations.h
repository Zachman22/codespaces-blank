#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <vector>
#include <functional>

struct FileInfo {
    std::string name;
    std::string path;
    std::string type; // "file" or "folder"
    long long size;
    std::string modified;
};

class FileOperations {
public:
    // Read file content
    static bool readFile(const std::string& path, std::string& content);
    
    // Write file content
    static bool writeFile(const std::string& path, const std::string& content);
    
    // List directory contents
    static std::vector<FileInfo> listDirectory(const std::string& path);
    
    // Create directory
    static bool createDirectory(const std::string& path);
    
    // Delete file
    static bool deleteFile(const std::string& path);
    
    // Check if path exists
    static bool exists(const std::string& path);
    
    // Check if path is directory
    static bool isDirectory(const std::string& path);
    
    // Get file info
    static FileInfo getFileInfo(const std::string& path);
    
    // Convert to JSON string
    static std::string fileInfoToJSON(const FileInfo& info);
    static std::string fileListToJSON(const std::vector<FileInfo>& files);
};

#endif // FILE_OPERATIONS_H
