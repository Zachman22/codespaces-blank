#include "../include/file_operations.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iomanip>

namespace fs = std::filesystem;

bool FileOperations::readFile(const std::string& path, std::string& content) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        
        return true;
    } catch (...) {
        return false;
    }
}

bool FileOperations::writeFile(const std::string& path, const std::string& content) {
    try {
        // Create parent directories if they don't exist
        fs::path filePath(path);
        if (filePath.has_parent_path()) {
            fs::create_directories(filePath.parent_path());
        }
        
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<FileInfo> FileOperations::listDirectory(const std::string& path) {
    std::vector<FileInfo> result;
    
    try {
        if (!fs::exists(path) || !fs::is_directory(path)) {
            return result;
        }
        
        for (const auto& entry : fs::directory_iterator(path)) {
            FileInfo info = getFileInfo(entry.path().string());
            result.push_back(info);
        }
    } catch (...) {
        // Return empty vector on error
    }
    
    return result;
}

bool FileOperations::createDirectory(const std::string& path) {
    try {
        return fs::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool FileOperations::deleteFile(const std::string& path) {
    try {
        return fs::remove(path);
    } catch (...) {
        return false;
    }
}

bool FileOperations::exists(const std::string& path) {
    try {
        return fs::exists(path);
    } catch (...) {
        return false;
    }
}

bool FileOperations::isDirectory(const std::string& path) {
    try {
        return fs::is_directory(path);
    } catch (...) {
        return false;
    }
}

FileInfo FileOperations::getFileInfo(const std::string& path) {
    FileInfo info;
    info.name = fs::path(path).filename().string();
    info.path = path;
    info.type = isDirectory(path) ? "folder" : "file";
    info.size = 0;
    info.modified = "";
    
    try {
        if (!isDirectory(path)) {
            info.size = fs::file_size(path);
        }
        
        auto ftime = fs::last_write_time(path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        auto cftime = std::chrono::system_clock::to_time_t(sctp);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %H:%M:%S");
        info.modified = ss.str();
    } catch (...) {
        // Keep defaults on error
    }
    
    return info;
}

std::string FileOperations::fileInfoToJSON(const FileInfo& info) {
    std::stringstream json;
    json << "{";
    json << "\"name\":\"" << info.name << "\",";
    json << "\"path\":\"" << info.path << "\",";
    json << "\"type\":\"" << info.type << "\",";
    json << "\"size\":" << info.size << ",";
    json << "\"modified\":\"" << info.modified << "\"";
    json << "}";
    return json.str();
}

std::string FileOperations::fileListToJSON(const std::vector<FileInfo>& files) {
    std::stringstream json;
    json << "[";
    
    for (size_t i = 0; i < files.size(); i++) {
        json << fileInfoToJSON(files[i]);
        if (i < files.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    return json.str();
}
