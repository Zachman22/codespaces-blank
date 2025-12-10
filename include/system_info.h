#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <string>
#include <map>

class SystemInfo {
public:
    static std::string getOSVersion();
    static std::string getArchitecture();
    static std::string getCPUModel();
    static int getCPUCores();
    static unsigned long long getTotalRAM();
    static std::map<std::string, std::string> getAllInfo();
    static std::string toJSON();
    
private:
    static std::string executeCommand(const std::string& command);
};

#endif // SYSTEM_INFO_H
