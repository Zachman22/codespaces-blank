# Code Signing Integration

## Overview

The Hybrid IDE includes **automatic code signing** for all compiled executables. When you build a C++ program, the IDE will automatically:

1. Generate a self-signed certificate (if not already present)
2. Sign the compiled executable with the certificate
3. Display signing status in the build panel

## Certificate Details

- **Common Name**: Zachary's Playroom Powered By AI
- **Organization**: Zachary's Playroom
- **Country**: US
- **Validity**: 365 days
- **Type**: Self-signed code signing certificate

## Certificate Storage

Certificates are stored in:
```
%APPDATA%\HybridIDE\certs\
├── cert.pem       (Certificate)
├── key.pem        (Private key)
└── cert.pfx       (Windows certificate store format)
```

## How It Works

### 1. Certificate Generation

On first build, the IDE will automatically generate a certificate using one of these methods:

**Method A: OpenSSL** (if available)
```bash
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365
openssl pkcs12 -export -out cert.pfx -inkey key.pem -in cert.pem
```

**Method B: PowerShell** (Windows 11+)
```powershell
New-SelfSignedCertificate `
  -Subject "CN=Zachary's Playroom Powered By AI" `
  -Type CodeSigningCert `
  -CertStoreLocation "Cert:\CurrentUser\My"
```

### 2. Executable Signing

After successful compilation, the IDE signs your executable using one of these methods:

**Method A: SignTool** (Windows SDK)
```bash
signtool sign /f cert.pfx /p password /t http://timestamp.digicert.com /fd SHA256 program.exe
```

**Method B: osslsigncode** (Cross-platform)
```bash
osslsigncode sign -pkcs12 cert.pfx -pass password -n "Program Name" program.exe
```

## Build Panel Output

When building with code signing, you'll see:

```
[INFO] Starting compilation...
[INFO] Using compiler: C:\MinGW\bin\g++.exe
[CMD] g++ -std=c++17 -O2 hello.cpp -o hello.exe
[SUCCESS] Compilation completed successfully
[INFO] Code signing executable...
[INFO] Generating code signing certificate...
[SUCCESS] Certificate generated successfully
[SUCCESS] Executable signed successfully
```

## Manual Certificate Management

### View Certificate Info

The IDE provides `CodeSigner::getCertificateInfo()` to check certificate status:
- Certificate path
- Validity dates
- Subject details

### Regenerate Certificate

Simply delete the certificate files and rebuild:
```powershell
Remove-Item -Recurse "$env:APPDATA\HybridIDE\certs"
```

The IDE will automatically create a new certificate on the next build.

## Trusted Certificates

Self-signed certificates are not trusted by Windows by default. To make Windows trust your signed executables:

### Option 1: Add to Trusted Publishers

1. Double-click a signed executable
2. Windows SmartScreen will appear
3. Click "More info" → "Run anyway"
4. Windows will remember this publisher

### Option 2: Import to Certificate Store

1. Open `certmgr.msc`
2. Right-click "Trusted Publishers"
3. Import → Browse to `%APPDATA%\HybridIDE\certs\cert.pfx`
4. All signed executables will now be trusted

### Option 3: Enterprise Deployment

For organizations, import the certificate to Group Policy:
```
Computer Configuration → Policies → Windows Settings → 
Security Settings → Public Key Policies → Trusted Publishers
```

## Technical Implementation

### CodeSigner Class (code_signer.h/cpp)

```cpp
class CodeSigner {
public:
    CodeSigner();
    
    // Generate a new certificate
    bool generateCertificate(
        const std::string& commonName = "Zachary's Playroom Powered By AI",
        const std::string& organization = "Zachary's Playroom",
        const std::string& country = "US",
        int validityDays = 365
    );
    
    // Sign an executable
    bool signExecutable(
        const std::string& exePath,
        const std::string& description = "Zachary's Playroom Powered By AI"
    );
    
    // Check certificate status
    bool hasCertificate();
    std::string getCertificateInfo();
    
private:
    std::string certDir;
    std::string certPath;
    std::string keyPath;
    std::string pfxPath;
    
    bool createSelfSignedCert(/* params */);
    bool signWithSignTool(/* params */);
    bool signWithOsslSignCode(/* params */);
};
```

### Compiler Integration (compiler.cpp)

```cpp
bool Compiler::compile(const CompileOptions& options,
                       std::function<void(const std::string&)> logCallback) {
    // ... compilation code ...
    
    if (success) {
        logCallback("[INFO] Code signing executable...\n");
        
        CodeSigner signer;
        
        if (!signer.hasCertificate()) {
            logCallback("[INFO] Generating code signing certificate...\n");
            if (signer.generateCertificate(
                "Zachary's Playroom Powered By AI",
                "Zachary's Playroom",
                "US",
                365)) {
                logCallback("[SUCCESS] Certificate generated successfully\n");
            }
        }
        
        if (signer.signExecutable(options.outputFile, 
            "Zachary's Playroom Powered By AI")) {
            logCallback("[SUCCESS] Executable signed successfully\n");
        }
    }
    
    return success;
}
```

## Troubleshooting

### Certificate Generation Fails

**Issue**: No OpenSSL or PowerShell certificate cmdlets available

**Solution**: Install OpenSSL:
```bash
# Windows (via Chocolatey)
choco install openssl

# Windows (via Scoop)
scoop install openssl

# Or download from: https://slproweb.com/products/Win32OpenSSL.html
```

### Signing Fails

**Issue**: SignTool or osslsigncode not found

**Solution**: 
- **SignTool**: Install Windows SDK
- **osslsigncode**: `choco install osslsigncode`

**Note**: The IDE will gracefully skip signing if tools are unavailable. The executable will still work but won't be signed.

### Certificate Expired

Certificates expire after 365 days. To renew:
```powershell
Remove-Item -Recurse "$env:APPDATA\HybridIDE\certs"
# Rebuild any program to generate a new certificate
```

## Security Considerations

### Self-Signed Certificates

- Self-signed certificates provide **authenticity** (proves you signed it)
- They do NOT provide **trustworthiness** (requires a trusted CA)
- Users will see "Unknown Publisher" until they trust your certificate

### Production Deployment

For commercial software, consider:

1. **Purchase a Code Signing Certificate**
   - DigiCert, GlobalSign, Sectigo, etc.
   - ~$400-800/year
   - Instantly trusted by Windows

2. **Extended Validation (EV) Certificate**
   - Highest trust level
   - No SmartScreen warnings
   - ~$600-1000/year

3. **Replace Self-Signed Certificate**
   - Export your purchased certificate as `.pfx`
   - Copy to `%APPDATA%\HybridIDE\certs\cert.pfx`
   - All builds will use your commercial certificate

## References

- [Microsoft Code Signing Best Practices](https://docs.microsoft.com/en-us/windows-hardware/drivers/dashboard/code-signing-best-practices)
- [SignTool Documentation](https://docs.microsoft.com/en-us/windows/win32/seccrypto/signtool)
- [OpenSSL Certificate Creation](https://www.openssl.org/docs/man1.1.1/man1/openssl-req.html)
- [Windows SmartScreen](https://docs.microsoft.com/en-us/windows/security/threat-protection/windows-defender-smartscreen/windows-defender-smartscreen-overview)

## FAQ

**Q: Will my programs run without signing?**
A: Yes! Signing is optional. If it fails, the executable still works.

**Q: Can I customize the certificate name?**
A: Yes! Edit `compiler.cpp` and change the parameters in `generateCertificate()`.

**Q: Does this work on Linux/Mac?**
A: Code signing is Windows-specific. On other platforms, it's skipped automatically.

**Q: How do I disable code signing?**
A: Comment out the code signing section in `compiler.cpp` (lines ~145-170).

**Q: Is the private key secure?**
A: The key is stored in `%APPDATA%` with user-level permissions. For production use, store keys in a secure location or use a hardware token.
