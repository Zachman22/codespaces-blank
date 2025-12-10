#ifndef PROMPT_VERIFIER_H
#define PROMPT_VERIFIER_H

#include <string>
#include <vector>

struct VerificationResult {
    bool valid;
    std::string message;
    std::vector<std::string> warnings;
};

class PromptVerifier {
public:
    static VerificationResult verifyBuildRequest(const std::string& sourceFile,
                                                  const std::string& outputFile);
    static VerificationResult verifyRunRequest(const std::string& executablePath);
    static bool fileExists(const std::string& path);
    static bool isValidSourceFile(const std::string& path);
    static bool isCompilerAvailable();
    
private:
    static std::vector<std::string> supportedExtensions;
};

#endif // PROMPT_VERIFIER_H
