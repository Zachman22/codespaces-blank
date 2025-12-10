#include "license_verifier.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

LicenseVerifier::LicenseVerifier() : verified_(false) {
    // Initialize
}

LicenseVerifier::~LicenseVerifier() {
    // Cleanup
}

bool LicenseVerifier::verifyLicense(const std::string& repoPath) {
    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              License & Permission Verification                 ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::string licensePath = repoPath + "/LICENSE";
    std::string readmePath = repoPath + "/README.md";
    
    // Check if LICENSE file exists
    if (!fs::exists(licensePath)) {
        std::cout << "⚠️  No LICENSE file found\n";
        std::cout << "   Checking for license information in README...\n\n";
        
        if (fs::exists(readmePath)) {
            return checkReadmeForLicense(readmePath);
        } else {
            std::cout << "❌ No license information found\n";
            std::cout << "   This repository may not be licensed for use.\n\n";
            return promptForPermission();
        }
    }
    
    // Parse LICENSE file
    LicenseInfo license = parseLicense(licensePath);
    
    std::cout << "📄 License detected: " << license.type << "\n";
    std::cout << "   Permissions:\n";
    
    for (const auto& perm : license.permissions) {
        std::cout << "   ✅ " << perm << "\n";
    }
    
    if (!license.conditions.empty()) {
        std::cout << "\n   Conditions:\n";
        for (const auto& cond : license.conditions) {
            std::cout << "   📋 " << cond << "\n";
        }
    }
    
    if (!license.limitations.empty()) {
        std::cout << "\n   Limitations:\n";
        for (const auto& limit : license.limitations) {
            std::cout << "   ⚠️  " << limit << "\n";
        }
    }
    
    std::cout << "\n";
    
    // Check if license allows cloning/use
    if (isOpenSourceLicense(license.type)) {
        std::cout << "✅ This is an OPEN SOURCE license\n";
        std::cout << "   You may clone and use this repository!\n\n";
        
        // Show any requirements
        if (hasAttribution(license.type)) {
            std::cout << "📌 REQUIRED: You must include attribution to the original author\n";
        }
        if (hasShareAlike(license.type)) {
            std::cout << "📌 REQUIRED: Derivative works must use the same license\n";
        }
        if (hasDisclosure(license.type)) {
            std::cout << "📌 REQUIRED: Source code must be disclosed if distributed\n";
        }
        
        verified_ = true;
        licenseInfo_ = license;
        return true;
    } else {
        std::cout << "⚠️  This license has restrictions\n";
        std::cout << "   Please verify you have permission to use this code.\n\n";
        return promptForPermission();
    }
}

LicenseVerifier::LicenseInfo LicenseVerifier::parseLicense(const std::string& path) {
    LicenseInfo info;
    info.type = "Unknown";
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return info;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Convert to lowercase for comparison
    std::string lower = content;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Detect license type
    if (lower.find("mit license") != std::string::npos) {
        info.type = "MIT License";
        info.permissions = {"Commercial use", "Modification", "Distribution", "Private use"};
        info.conditions = {"License and copyright notice"};
        info.limitations = {"Liability", "Warranty"};
    }
    else if (lower.find("apache license") != std::string::npos) {
        info.type = "Apache License 2.0";
        info.permissions = {"Commercial use", "Modification", "Distribution", "Patent use", "Private use"};
        info.conditions = {"License and copyright notice", "State changes"};
        info.limitations = {"Trademark use", "Liability", "Warranty"};
    }
    else if (lower.find("gnu general public license") != std::string::npos || 
             lower.find("gpl") != std::string::npos) {
        if (lower.find("version 3") != std::string::npos || lower.find("v3") != std::string::npos) {
            info.type = "GNU GPL v3";
        } else {
            info.type = "GNU GPL v2";
        }
        info.permissions = {"Commercial use", "Modification", "Distribution", "Patent use", "Private use"};
        info.conditions = {"Disclose source", "License and copyright notice", "Same license", "State changes"};
        info.limitations = {"Liability", "Warranty"};
    }
    else if (lower.find("bsd") != std::string::npos) {
        info.type = "BSD License";
        info.permissions = {"Commercial use", "Modification", "Distribution", "Private use"};
        info.conditions = {"License and copyright notice"};
        info.limitations = {"Liability", "Warranty"};
    }
    else if (lower.find("creative commons") != std::string::npos) {
        info.type = "Creative Commons";
        info.permissions = {"Reproduction", "Distribution", "Display"};
        info.conditions = {"Attribution"};
        if (lower.find("sharealike") != std::string::npos || lower.find("sa") != std::string::npos) {
            info.conditions.push_back("Share alike");
        }
        if (lower.find("noderivatives") != std::string::npos || lower.find("nd") != std::string::npos) {
            info.limitations.push_back("No derivatives");
        }
        if (lower.find("noncommercial") != std::string::npos || lower.find("nc") != std::string::npos) {
            info.limitations.push_back("Non-commercial use only");
        }
    }
    else if (lower.find("unlicense") != std::string::npos) {
        info.type = "The Unlicense";
        info.permissions = {"Commercial use", "Modification", "Distribution", "Private use"};
        info.conditions = {};
        info.limitations = {"Liability", "Warranty"};
    }
    else if (lower.find("isc license") != std::string::npos) {
        info.type = "ISC License";
        info.permissions = {"Commercial use", "Modification", "Distribution", "Private use"};
        info.conditions = {"License and copyright notice"};
        info.limitations = {"Liability", "Warranty"};
    }
    else {
        info.type = "Custom/Proprietary License";
        info.permissions = {"Unknown - Manual review required"};
    }
    
    return info;
}

bool LicenseVerifier::checkReadmeForLicense(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    std::string lower = content;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower.find("open source") != std::string::npos ||
        lower.find("free to use") != std::string::npos ||
        lower.find("public domain") != std::string::npos) {
        std::cout << "✅ README indicates this is OPEN SOURCE\n\n";
        return true;
    }
    
    if (lower.find("license") != std::string::npos) {
        std::cout << "⚠️  License mentioned in README - please review manually\n\n";
        return promptForPermission();
    }
    
    return false;
}

bool LicenseVerifier::isOpenSourceLicense(const std::string& type) {
    std::vector<std::string> openSourceLicenses = {
        "MIT License",
        "Apache License",
        "GNU GPL",
        "BSD License",
        "The Unlicense",
        "ISC License",
        "Mozilla Public License",
        "Eclipse Public License"
    };
    
    for (const auto& oss : openSourceLicenses) {
        if (type.find(oss) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool LicenseVerifier::hasAttribution(const std::string& type) {
    return type.find("MIT") != std::string::npos ||
           type.find("Apache") != std::string::npos ||
           type.find("BSD") != std::string::npos ||
           type.find("GPL") != std::string::npos ||
           type.find("Creative Commons") != std::string::npos;
}

bool LicenseVerifier::hasShareAlike(const std::string& type) {
    return type.find("GPL") != std::string::npos ||
           (type.find("Creative Commons") != std::string::npos && 
            type.find("SA") != std::string::npos);
}

bool LicenseVerifier::hasDisclosure(const std::string& type) {
    return type.find("GPL") != std::string::npos ||
           type.find("AGPL") != std::string::npos;
}

bool LicenseVerifier::promptForPermission() {
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "⚠️  PERMISSION CHECK\n\n";
    std::cout << "Do you have explicit permission to clone and use this repository?\n\n";
    std::cout << "You have permission if:\n";
    std::cout << "  ✅ You are the repository owner\n";
    std::cout << "  ✅ You have been granted access by the owner\n";
    std::cout << "  ✅ This is a public, open-source repository\n";
    std::cout << "  ✅ The license explicitly allows cloning\n\n";
    std::cout << "You DO NOT have permission if:\n";
    std::cout << "  ❌ This is private code without authorization\n";
    std::cout << "  ❌ The license restricts use or distribution\n";
    std::cout << "  ❌ You are unsure about the licensing\n\n";
    std::cout << "════════════════════════════════════════════════════════════════\n\n";
    
    std::cout << "Do you confirm you have permission? (yes/no): ";
    std::string response;
    std::getline(std::cin, response);
    
    std::transform(response.begin(), response.end(), response.begin(), ::tolower);
    
    if (response == "yes" || response == "y") {
        std::cout << "\n✅ Permission confirmed by user\n";
        std::cout << "   You may proceed with cloning and using this repository.\n\n";
        
        // Log the confirmation
        logPermission(true, "User confirmed permission");
        
        verified_ = true;
        return true;
    } else {
        std::cout << "\n❌ Permission denied or not confirmed\n";
        std::cout << "   Please contact the repository owner for authorization.\n";
        std::cout << "   Or verify the license terms before proceeding.\n\n";
        
        logPermission(false, "User did not confirm permission");
        
        return false;
    }
}

void LicenseVerifier::logPermission(bool granted, const std::string& reason) {
    std::string logDir;
    
#ifdef _WIN32
    char* appdata = getenv("APPDATA");
    logDir = appdata ? std::string(appdata) + "\\HybridIDE\\logs" : ".\\logs";
#else
    char* home = getenv("HOME");
    logDir = home ? std::string(home) + "/.hybridide/logs" : "./logs";
#endif
    
    fs::create_directories(logDir);
    std::string logPath = logDir + "/license_verification.log";
    
    std::ofstream log(logPath, std::ios::app);
    if (log.is_open()) {
        std::time_t now = std::time(nullptr);
        log << "===========================================\n";
        log << "Timestamp: " << std::ctime(&now);
        log << "Permission: " << (granted ? "GRANTED" : "DENIED") << "\n";
        log << "Reason: " << reason << "\n";
        if (verified_) {
            log << "License Type: " << licenseInfo_.type << "\n";
        }
        log << "===========================================\n\n";
        log.close();
    }
}

bool LicenseVerifier::isVerified() const {
    return verified_;
}

LicenseVerifier::LicenseInfo LicenseVerifier::getLicenseInfo() const {
    return licenseInfo_;
}

void LicenseVerifier::displayLicenseRequirements() const {
    if (!verified_) {
        std::cout << "⚠️  License not verified\n";
        return;
    }
    
    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                 License Requirements Summary                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "License: " << licenseInfo_.type << "\n\n";
    
    std::cout << "✅ YOU CAN:\n";
    for (const auto& perm : licenseInfo_.permissions) {
        std::cout << "   • " << perm << "\n";
    }
    
    if (!licenseInfo_.conditions.empty()) {
        std::cout << "\n📋 YOU MUST:\n";
        for (const auto& cond : licenseInfo_.conditions) {
            std::cout << "   • " << cond << "\n";
        }
    }
    
    if (!licenseInfo_.limitations.empty()) {
        std::cout << "\n❌ YOU CANNOT:\n";
        for (const auto& limit : licenseInfo_.limitations) {
            std::cout << "   • " << limit << "\n";
        }
    }
    
    std::cout << "\n";
}
