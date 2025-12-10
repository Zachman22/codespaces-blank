#include "code_signer.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <ctime>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

CodeSigner::CodeSigner() {
    // Set up certificate paths in user's home directory
#ifdef _WIN32
    char* appdata = getenv("APPDATA");
    std::string certDir = appdata ? std::string(appdata) + "\\HybridIDE\\certs" : ".\\certs";
    fs::create_directories(certDir);
    certPath = certDir + "\\zachary_playroom.crt";
    keyPath = certDir + "\\zachary_playroom.key";
    pfxPath = certDir + "\\zachary_playroom.pfx";
#else
    char* home = getenv("HOME");
    std::string certDir = home ? std::string(home) + "/.hybridide/certs" : "./certs";
    fs::create_directories(certDir);
    certPath = certDir + "/zachary_playroom.crt";
    keyPath = certDir + "/zachary_playroom.key";
    pfxPath = certDir + "/zachary_playroom.pfx";
#endif
}

CodeSigner::~CodeSigner() {
}

bool CodeSigner::hasCertificate() const {
    return fs::exists(pfxPath) || fs::exists(certPath);
}

std::string CodeSigner::executeCommand(const std::string& command) {
    std::string result;
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) return "";
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return result;
}

bool CodeSigner::generateCertificate(
    const std::string& commonName,
    const std::string& organization,
    const std::string& country,
    int validityDays
) {
    std::cout << "[CODE SIGNER] Generating certificate for: " << organization << std::endl;
    
    // Check if OpenSSL is available
    std::string opensslCheck = executeCommand("openssl version 2>nul");
    bool hasOpenSSL = !opensslCheck.empty();
    
    if (hasOpenSSL) {
        return createSelfSignedCert(
            "C=" + country + 
            ", O=" + organization + 
            ", CN=" + commonName,
            validityDays
        );
    } else {
        std::cout << "[CODE SIGNER] OpenSSL not found. Trying PowerShell certificate generation..." << std::endl;
        
        // Use PowerShell to create a self-signed certificate
        std::stringstream ps;
        ps << "powershell -Command \"";
        ps << "$cert = New-SelfSignedCertificate ";
        ps << "-Type CodeSigningCert ";
        ps << "-Subject 'CN=" << commonName << ", O=" << organization << "' ";
        ps << "-CertStoreLocation 'Cert:\\CurrentUser\\My' ";
        ps << "-NotAfter (Get-Date).AddDays(" << validityDays << "); ";
        ps << "$pwd = ConvertTo-SecureString -String 'hybrid-ide-2025' -Force -AsPlainText; ";
        ps << "Export-PfxCertificate -Cert $cert -FilePath '" << pfxPath << "' -Password $pwd";
        ps << "\"";
        
        std::string result = executeCommand(ps.str());
        
        if (fs::exists(pfxPath)) {
            std::cout << "[CODE SIGNER] Certificate created successfully!" << std::endl;
            std::cout << "[CODE SIGNER] Location: " << pfxPath << std::endl;
            return true;
        }
    }
    
    std::cout << "[CODE SIGNER] Warning: Could not generate certificate automatically" << std::endl;
    std::cout << "[CODE SIGNER] The executable will be unsigned (still functional)" << std::endl;
    return false;
}

bool CodeSigner::createSelfSignedCert(
    const std::string& subject,
    int validityDays
) {
    std::cout << "[CODE SIGNER] Creating self-signed certificate with OpenSSL..." << std::endl;
    
    // Generate private key
    std::stringstream keyCmd;
    keyCmd << "openssl genrsa -out \"" << keyPath << "\" 2048 2>nul";
    executeCommand(keyCmd.str());
    
    if (!fs::exists(keyPath)) {
        std::cerr << "[CODE SIGNER] Failed to generate private key" << std::endl;
        return false;
    }
    
    // Generate certificate
    std::stringstream certCmd;
    certCmd << "openssl req -new -x509 ";
    certCmd << "-key \"" << keyPath << "\" ";
    certCmd << "-out \"" << certPath << "\" ";
    certCmd << "-days " << validityDays << " ";
    certCmd << "-subj \"/" << subject << "\" 2>nul";
    executeCommand(certCmd.str());
    
    if (!fs::exists(certPath)) {
        std::cerr << "[CODE SIGNER] Failed to generate certificate" << std::endl;
        return false;
    }
    
    // Convert to PFX format
    std::stringstream pfxCmd;
    pfxCmd << "openssl pkcs12 -export ";
    pfxCmd << "-out \"" << pfxPath << "\" ";
    pfxCmd << "-inkey \"" << keyPath << "\" ";
    pfxCmd << "-in \"" << certPath << "\" ";
    pfxCmd << "-passout pass:hybrid-ide-2025 2>nul";
    executeCommand(pfxCmd.str());
    
    if (fs::exists(pfxPath)) {
        std::cout << "[CODE SIGNER] Certificate generated successfully!" << std::endl;
        return true;
    }
    
    return false;
}

bool CodeSigner::signExecutable(
    const std::string& exePath,
    const std::string& description
) {
    if (!fs::exists(exePath)) {
        std::cerr << "[CODE SIGNER] Executable not found: " << exePath << std::endl;
        return false;
    }
    
    std::cout << "[CODE SIGNER] Signing: " << exePath << std::endl;
    std::cout << "[CODE SIGNER] Description: " << description << std::endl;
    
    // Generate certificate if needed
    if (!hasCertificate()) {
        std::cout << "[CODE SIGNER] No certificate found. Generating..." << std::endl;
        generateCertificate(
            "Zachary's Playroom Powered By AI",
            "Zachary's Playroom",
            "US",
            365
        );
    }
    
    // Try SignTool (Windows SDK)
    std::string signToolCheck = executeCommand("signtool /? 2>nul");
    if (!signToolCheck.empty() && fs::exists(pfxPath)) {
        if (signWithSignTool(exePath, description)) {
            return true;
        }
    }
    
    // Try osslsigncode (open source alternative)
    std::string osslCheck = executeCommand("osslsigncode --version 2>nul");
    if (!osslCheck.empty() && fs::exists(pfxPath)) {
        if (signWithOsslSignCode(exePath, description)) {
            return true;
        }
    }
    
    std::cout << "[CODE SIGNER] Note: Executable is unsigned but fully functional" << std::endl;
    std::cout << "[CODE SIGNER] Install Windows SDK for automatic signing" << std::endl;
    return false;
}

bool CodeSigner::signWithSignTool(
    const std::string& exePath,
    const std::string& description
) {
    std::stringstream cmd;
    cmd << "signtool sign ";
    cmd << "/f \"" << pfxPath << "\" ";
    cmd << "/p hybrid-ide-2025 ";
    cmd << "/d \"" << description << "\" ";
    cmd << "/t http://timestamp.digicert.com ";
    cmd << "\"" << exePath << "\" 2>nul";
    
    std::string result = executeCommand(cmd.str());
    
    if (result.find("Successfully signed") != std::string::npos) {
        std::cout << "[CODE SIGNER] ✓ Executable signed successfully with SignTool!" << std::endl;
        return true;
    }
    
    return false;
}

bool CodeSigner::signWithOsslSignCode(
    const std::string& exePath,
    const std::string& description
) {
    // Create timestamped copy
    std::string signedPath = exePath + ".signed";
    
    std::stringstream cmd;
    cmd << "osslsigncode sign ";
    cmd << "-pkcs12 \"" << pfxPath << "\" ";
    cmd << "-pass hybrid-ide-2025 ";
    cmd << "-n \"" << description << "\" ";
    cmd << "-t http://timestamp.digicert.com ";
    cmd << "-in \"" << exePath << "\" ";
    cmd << "-out \"" << signedPath << "\" 2>nul";
    
    std::string result = executeCommand(cmd.str());
    
    if (fs::exists(signedPath)) {
        fs::remove(exePath);
        fs::rename(signedPath, exePath);
        std::cout << "[CODE SIGNER] ✓ Executable signed successfully with osslsigncode!" << std::endl;
        return true;
    }
    
    return false;
}

std::string CodeSigner::getCertificateInfo() const {
    if (!hasCertificate()) {
        return "No certificate found";
    }
    
    std::stringstream info;
    info << "Certificate: Zachary's Playroom Powered By AI\n";
    info << "Organization: Zachary's Playroom\n";
    info << "Location: " << pfxPath << "\n";
    info << "Status: Active\n";
    
    return info.str();
}
