#include <iostream>
#include "github_auth.h"
#include "license_verifier.h"

int main(int argc, char* argv[]) {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════════╗
║                           HYBRID IDE - SETUP                                ║
║              License Verification & GitHub Authentication                   ║
╚════════════════════════════════════════════════════════════════════════════╝

Welcome to Hybrid IDE!

To verify your installation and enable cloud features, please authenticate
with your GitHub account.

This one-time setup will:
  ✅ Verify your identity
  ✅ Enable cloud sync features
  ✅ Allow access to private repositories
  ✅ Enable collaboration features
  ✅ Unlock AI-powered code generation

Your authentication token is stored securely on your local machine only.

)" << std::endl;

    // Step 1: Verify license and permissions
    std::cout << "══════════════════════════════════════════════════════════════\n";
    std::cout << "STEP 1: Verifying license and clone permissions...\n";
    std::cout << "══════════════════════════════════════════════════════════════\n\n";
    
    LicenseVerifier licenseVerifier;
    std::string repoPath = "..";  // Parent directory (the repo root)
    
    if (!licenseVerifier.verifyLicense(repoPath)) {
        std::cerr << "\n❌ License verification failed\n";
        std::cerr << "   You may not have permission to use this repository.\n";
        std::cerr << "   Please contact the repository owner or verify licensing.\n\n";
        return 1;
    }
    
    std::cout << "══════════════════════════════════════════════════════════════\n";
    std::cout << "STEP 2: GitHub Authentication\n";
    std::cout << "══════════════════════════════════════════════════════════════\n\n";
    
    GitHubAuth auth;
    
    // Check if already authenticated
    if (auth.loadSavedToken()) {
        auto user = auth.getUserInfo();
        std::cout << "✅ Already authenticated as: " << user.login << "\n";
        std::cout << "📧 Email: " << user.email << "\n\n";
        
        std::cout << "Options:\n";
        std::cout << "  1. Continue with current authentication\n";
        std::cout << "  2. Re-authenticate with different account\n";
        std::cout << "  3. Logout\n\n";
        std::cout << "Choice (1-3): ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear newline
        
        if (choice == 2) {
            auth.logout();
            std::cout << "\nChoose authentication method:\n";
            std::cout << "  1. Browser login (standard)\n";
            std::cout << "  2. Numeric PIN login (easy)\n";
            std::cout << "Choice (1-2): ";
            
            int authChoice;
            std::cin >> authChoice;
            std::cin.ignore(); // Clear newline
            
            bool success = false;
            if (authChoice == 2) {
                success = auth.startNumericPinFlow();
            } else {
                success = auth.startDeviceFlow();
            }
            
            if (!success) {
                std::cerr << "❌ Authentication failed\n";
                return 1;
            }
        } else if (choice == 3) {
            auth.logout();
            std::cout << "Goodbye!\n";
            return 0;
        }
    } else {
        // Start authentication flow
        std::cout << "Choose authentication method:\n";
        std::cout << "  1. Browser login with verification code\n";
        std::cout << "  2. Numeric PIN login (easier, just numbers!)\n";
        std::cout << "Choice (1-2): ";
        
        int authChoice;
        std::cin >> authChoice;
        std::cin.ignore(); // Clear newline
        
        bool success = false;
        if (authChoice == 2) {
            std::cout << "\n🔢 Starting Numeric PIN authentication...\n";
            success = auth.startNumericPinFlow();
        } else {
            std::cout << "\n🌐 Starting browser authentication...\n";
            success = auth.startDeviceFlow();
        }
        
        if (!success) {
            std::cerr << "❌ Authentication failed\n";
            std::cerr << "\nYou can skip authentication and use offline features only.\n";
            std::cerr << "Run this setup again anytime to authenticate.\n";
            return 1;
        }
    }
    
    std::cout << "\n╔════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                        ✅ SETUP COMPLETE                                    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "✅ License verified and compliant\n";
    std::cout << "✅ GitHub authentication successful\n\n";
    
    // Display license requirements
    licenseVerifier.displayLicenseRequirements();
    
    std::cout << "You can now use all Hybrid IDE features!\n\n";
    std::cout << "Quick Start:\n";
    std::cout << "  • Run HybridIDE: ./HybridIDE\n";
    std::cout << "  • Open a project: ./HybridIDE /path/to/project\n";
    std::cout << "  • AI Code Chat: ./AutoCoderChatbot\n";
    std::cout << "  • Generate Scripts: ./ScriptGeneratorChatbot\n\n";
    std::cout << "Documentation: See README.md for full usage guide\n\n";
    
    return 0;
}
