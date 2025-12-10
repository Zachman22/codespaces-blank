#include <iostream>
#include "github_auth.h"

int main(int argc, char* argv[]) {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════════╗
║                           HYBRID IDE - SETUP                                ║
║                      GitHub Authentication Required                         ║
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
        
        if (choice == 2) {
            auth.logout();
            std::cout << "\nPlease authenticate again:\n\n";
            if (!auth.startDeviceFlow()) {
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
        if (!auth.startDeviceFlow()) {
            std::cerr << "❌ Authentication failed\n";
            std::cerr << "\nYou can skip authentication and use offline features only.\n";
            std::cerr << "Run this setup again anytime to authenticate.\n";
            return 1;
        }
    }
    
    std::cout << "\n╔════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                        ✅ SETUP COMPLETE                                    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "You can now use all Hybrid IDE features!\n\n";
    std::cout << "Quick Start:\n";
    std::cout << "  • Run HybridIDE: ./HybridIDE\n";
    std::cout << "  • Open a project: ./HybridIDE /path/to/project\n";
    std::cout << "  • AI Code Chat: ./AutoCoderChatbot\n";
    std::cout << "  • Generate Scripts: ./ScriptGeneratorChatbot\n\n";
    std::cout << "Documentation: See README.md for full usage guide\n\n";
    
    return 0;
}
