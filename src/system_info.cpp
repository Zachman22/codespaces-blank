#include "../include/system_info.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <sstream>
#include <array>
#include <sys/utsname.h>

std::string SystemInfo::executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) {
        return "ERROR: Could not execute command";
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return result;
}

std::string SystemInfo::getOSVersion() {
#ifdef _WIN32
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    
    // Note: GetVersionEx is deprecated but works for basic detection
    // For production, use RtlGetVersion or Version Helper APIs
    #pragma warning(push)
    #pragma warning(disable: 4996)
    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        std::stringstream ss;
        
        if (osvi.dwMajorVersion == 10 && osvi.dwBuildNumber >= 22000) {
            ss << "Windows 11";
        } else if (osvi.dwMajorVersion == 10) {
            ss << "Windows 10";
        } else {
            ss << "Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;
        }
        
        ss << " (Build " << osvi.dwBuildNumber << ")";
        return ss.str();
    }
    #pragma warning(pop)
    
    // Fallback: use systeminfo command
    std::string result = executeCommand("wmic os get Caption /value");
    size_t pos = result.find("Caption=");
    if (pos != std::string::npos) {
        std::string caption = result.substr(pos + 8);
        // Remove trailing whitespace
        caption.erase(caption.find_last_not_of(" \n\r\t") + 1);
        return caption;
    }
    
    return "Windows (Unknown Version)";
#else
    // Linux/Unix
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        std::stringstream ss;
        ss << unameData.sysname << " " << unameData.release << " " << unameData.machine;
        return ss.str();
    }
    return "Unknown OS";
#endif
}

std::string SystemInfo::getArchitecture() {
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    
    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            return "x64";
        case PROCESSOR_ARCHITECTURE_ARM64:
            return "ARM64";
        case PROCESSOR_ARCHITECTURE_INTEL:
            return "x86";
        case PROCESSOR_ARCHITECTURE_ARM:
            return "ARM";
        default:
            return "Unknown";
    }
#else
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        return std::string(unameData.machine);
    }
    return "Unknown";
#endif
}

std::string SystemInfo::getCPUModel() {
#ifdef _WIN32
    std::string result = executeCommand("wmic cpu get name /value");
    size_t pos = result.find("Name=");
    if (pos != std::string::npos) {
        std::string name = result.substr(pos + 5);
        // Remove trailing whitespace
        name.erase(name.find_last_not_of(" \n\r\t") + 1);
        return name;
    }
    return "Unknown CPU";
#else
    std::string result = executeCommand("cat /proc/cpuinfo | grep 'model name' | head -n1");
    size_t pos = result.find(": ");
    if (pos != std::string::npos) {
        std::string name = result.substr(pos + 2);
        name.erase(name.find_last_not_of(" \n\r\t") + 1);
        return name;
    }
    return "Unknown CPU";
#endif
}

int SystemInfo::getCPUCores() {
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
#else
    std::string result = executeCommand("nproc");
    try {
        return std::stoi(result);
    } catch (...) {
        return 1;
    }
#endif
}

unsigned long long SystemInfo::getTotalRAM() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPhys / (1024 * 1024); // Return in MB
#else
    std::string result = executeCommand("cat /proc/meminfo | grep MemTotal");
    size_t pos = result.find(":");
    if (pos != std::string::npos) {
        std::string ramStr = result.substr(pos + 1);
        try {
            // MemTotal is in kB, convert to MB
            return std::stoull(ramStr) / 1024;
        } catch (...) {
            return 0;
        }
    }
    return 0;
#endif
}

std::map<std::string, std::string> SystemInfo::getAllInfo() {
    std::map<std::string, std::string> info;
    
    info["os"] = getOSVersion();
    info["architecture"] = getArchitecture();
    info["cpu"] = getCPUModel();
    info["cores"] = std::to_string(getCPUCores());
    info["ram"] = std::to_string(getTotalRAM()) + " MB";
    
    return info;
}

std::string SystemInfo::toJSON() {
    auto info = getAllInfo();
    
    std::stringstream json;
    json << "{\n";
    json << "  \"os\": \"" << info["os"] << "\",\n";
    json << "  \"architecture\": \"" << info["architecture"] << "\",\n";
    json << "  \"cpu\": \"" << info["cpu"] << "\",\n";
    json << "  \"cores\": " << info["cores"] << ",\n";
    json << "  \"ram\": \"" << info["ram"] << "\"\n";
    json << "}";
    
    return json.str();
}
