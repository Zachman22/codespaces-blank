#ifndef CODE_SIGNER_H
#define CODE_SIGNER_H

#include <string>
#include <vector>

class CodeSigner {
public:
    CodeSigner();
    ~CodeSigner();
    
    // Generate self-signed certificate
    bool generateCertificate(
        const std::string& commonName,
        const std::string& organization,
        const std::string& country = "US",
        int validityDays = 365
    );
    
    // Sign an executable
    bool signExecutable(
        const std::string& exePath,
        const std::string& description = "Zachary's Playroom Powered By AI"
    );
    
    // Check if certificate exists
    bool hasCertificate() const;
    
    // Get certificate info
    std::string getCertificateInfo() const;
    
private:
    std::string certPath;
    std::string keyPath;
    std::string pfxPath;
    
    bool createSelfSignedCert(
        const std::string& subject,
        int validityDays
    );
    
    bool signWithSignTool(
        const std::string& exePath,
        const std::string& description
    );
    
    bool signWithOsslSignCode(
        const std::string& exePath,
        const std::string& description
    );
    
    std::string executeCommand(const std::string& command);
};

#endif // CODE_SIGNER_H
