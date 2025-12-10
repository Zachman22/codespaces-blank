#ifndef LICENSE_VERIFIER_H
#define LICENSE_VERIFIER_H

#include <string>
#include <vector>

/**
 * License Verifier - Validates cloning permissions and license compliance
 */
class LicenseVerifier {
public:
    struct LicenseInfo {
        std::string type;
        std::vector<std::string> permissions;
        std::vector<std::string> conditions;
        std::vector<std::string> limitations;
    };

    LicenseVerifier();
    ~LicenseVerifier();

    // Verification
    bool verifyLicense(const std::string& repoPath);
    bool isVerified() const;
    LicenseInfo getLicenseInfo() const;
    
    // Display
    void displayLicenseRequirements() const;

private:
    bool verified_;
    LicenseInfo licenseInfo_;
    
    // Parsing
    LicenseInfo parseLicense(const std::string& path);
    bool checkReadmeForLicense(const std::string& path);
    
    // License detection
    bool isOpenSourceLicense(const std::string& type);
    bool hasAttribution(const std::string& type);
    bool hasShareAlike(const std::string& type);
    bool hasDisclosure(const std::string& type);
    
    // User interaction
    bool promptForPermission();
    
    // Logging
    void logPermission(bool granted, const std::string& reason);
};

#endif // LICENSE_VERIFIER_H
