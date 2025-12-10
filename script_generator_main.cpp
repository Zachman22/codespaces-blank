#include "script_generator.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

class ScriptGeneratorChatbot {
private:
    ScriptGenerator generator;
    bool running;

    void displayWelcome() {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "║      Script Generator Chatbot - Auto Multi-Platform     ║\n";
        std::cout << "║                   Code Signing Enabled                   ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════╝\n";
        std::cout << "\nGenerates scripts for:\n";
        std::cout << "  • Linux: Ubuntu, Debian, Fedora, CentOS, Arch, RHEL\n";
        std::cout << "  • macOS: All current versions (Bash/Zsh)\n";
        std::cout << "  • Windows: 10, 11, Server 2019/2022 (PS/CMD/Batch)\n";
        std::cout << "\nFeatures:\n";
        std::cout << "  ✓ Automatic code signing with CA\n";
        std::cout << "  ✓ Smart auto-naming (customizable)\n";
        std::cout << "  ✓ Cross-platform compatibility\n";
        std::cout << "  ✓ Error handling built-in\n";
        std::cout << "  ✓ Admin privilege detection\n";
        std::cout << "\nCommands:\n";
        std::cout << "  generate    - Create a new script\n";
        std::cout << "  platforms   - List supported platforms\n";
        std::cout << "  help        - Show detailed help\n";
        std::cout << "  exit        - Quit chatbot\n";
        std::cout << "\n";
    }

    void displayHelp() {
        std::cout << "\n=== Script Generator Help ===\n\n";
        std::cout << "Script Types:\n";
        std::cout << "  1. installer   - Installation scripts\n";
        std::cout << "  2. updater     - Update/upgrade scripts\n";
        std::cout << "  3. backup      - Backup and restore scripts\n";
        std::cout << "  4. deployment  - Deployment automation\n";
        std::cout << "  5. automation  - General automation tasks\n";
        std::cout << "  6. sysadmin    - System administration\n";
        std::cout << "  7. build       - Build and compilation\n";
        std::cout << "  8. test        - Testing scripts\n";
        std::cout << "  9. docker      - Docker/container scripts\n";
        std::cout << "  10. custom     - Custom scripts\n\n";
        
        std::cout << "Platforms:\n";
        std::cout << "  1. linux_bash       - Linux Bash\n";
        std::cout << "  2. linux_sh         - Linux POSIX sh\n";
        std::cout << "  3. macos_bash       - macOS Bash\n";
        std::cout << "  4. macos_zsh        - macOS Zsh\n";
        std::cout << "  5. windows_ps       - Windows PowerShell\n";
        std::cout << "  6. windows_cmd      - Windows CMD\n";
        std::cout << "  7. windows_batch    - Windows Batch\n";
        std::cout << "  8. all              - All platforms\n\n";
        
        std::cout << "Example Usage:\n";
        std::cout << "  > generate\n";
        std::cout << "  Description: Install Docker on server\n";
        std::cout << "  Type: installer\n";
        std::cout << "  Platform: all\n";
        std::cout << "  Commands: curl -fsSL https://get.docker.com | sh\n";
        std::cout << "  Requires admin: yes\n";
        std::cout << "  Auto-sign: yes\n";
        std::cout << "  Custom name: docker_install\n\n";
    }

    void displayPlatforms() {
        std::cout << "\n=== Supported Platforms ===\n\n";
        
        std::cout << "Linux Distributions:\n";
        for (const auto& distro : generator.getSupportedLinuxDistros()) {
            std::cout << "  • " << distro << "\n";
        }
        
        std::cout << "\nmacOS Versions:\n";
        for (const auto& version : generator.getSupportedMacOSVersions()) {
            std::cout << "  • " << version << "\n";
        }
        
        std::cout << "\nWindows Versions:\n";
        for (const auto& version : generator.getSupportedWindowsVersions()) {
            std::cout << "  • " << version << "\n";
        }
        std::cout << "\n";
    }

    ScriptGenerator::ScriptType parseScriptType(const std::string& input) {
        std::string lower = input;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "installer" || lower == "1") return ScriptGenerator::ScriptType::INSTALLER;
        if (lower == "updater" || lower == "2") return ScriptGenerator::ScriptType::UPDATER;
        if (lower == "backup" || lower == "3") return ScriptGenerator::ScriptType::BACKUP;
        if (lower == "deployment" || lower == "4") return ScriptGenerator::ScriptType::DEPLOYMENT;
        if (lower == "automation" || lower == "5") return ScriptGenerator::ScriptType::AUTOMATION;
        if (lower == "sysadmin" || lower == "6") return ScriptGenerator::ScriptType::SYSTEM_ADMIN;
        if (lower == "build" || lower == "7") return ScriptGenerator::ScriptType::BUILD;
        if (lower == "test" || lower == "8") return ScriptGenerator::ScriptType::TEST;
        if (lower == "docker" || lower == "9") return ScriptGenerator::ScriptType::DOCKER;
        return ScriptGenerator::ScriptType::CUSTOM;
    }

    ScriptGenerator::Platform parsePlatform(const std::string& input) {
        std::string lower = input;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "linux_bash" || lower == "1") return ScriptGenerator::Platform::LINUX_BASH;
        if (lower == "linux_sh" || lower == "2") return ScriptGenerator::Platform::LINUX_SH;
        if (lower == "macos_bash" || lower == "3") return ScriptGenerator::Platform::MACOS_BASH;
        if (lower == "macos_zsh" || lower == "4") return ScriptGenerator::Platform::MACOS_ZSH;
        if (lower == "windows_ps" || lower == "5") return ScriptGenerator::Platform::WINDOWS_POWERSHELL;
        if (lower == "windows_cmd" || lower == "6") return ScriptGenerator::Platform::WINDOWS_CMD;
        if (lower == "windows_batch" || lower == "7") return ScriptGenerator::Platform::WINDOWS_BATCH;
        if (lower == "all" || lower == "8") return ScriptGenerator::Platform::ALL_PLATFORMS;
        return ScriptGenerator::Platform::ALL_PLATFORMS;
    }

    bool parseYesNo(const std::string& input) {
        std::string lower = input;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return (lower == "yes" || lower == "y" || lower == "true" || lower == "1");
    }

    std::vector<std::string> splitCommands(const std::string& input) {
        std::vector<std::string> commands;
        std::istringstream stream(input);
        std::string line;
        
        while (std::getline(stream, line)) {
            if (!line.empty() && line != "done") {
                commands.push_back(line);
            }
        }
        
        return commands;
    }

    void handleGenerate() {
        ScriptGenerator::ScriptRequest request;
        std::string input;

        // Get description
        std::cout << "\nScript Description: ";
        std::getline(std::cin, request.description);
        if (request.description.empty()) request.description = "Auto-generated script";

        // Get script type
        std::cout << "Script Type (installer/updater/backup/deployment/automation/sysadmin/build/test/docker/custom): ";
        std::getline(std::cin, input);
        request.type = parseScriptType(input);

        // Get platform
        std::cout << "Target Platform (linux_bash/linux_sh/macos_bash/macos_zsh/windows_ps/windows_cmd/windows_batch/all): ";
        std::getline(std::cin, input);
        request.platform = parsePlatform(input);

        // Get commands
        std::cout << "Enter commands (one per line, type 'done' when finished):\n";
        std::string allCommands;
        while (true) {
            std::cout << "  > ";
            std::getline(std::cin, input);
            if (input == "done" || input.empty()) break;
            request.commands.push_back(input);
        }

        if (request.commands.empty()) {
            std::cout << "Warning: No commands provided. Adding placeholder command.\n";
            request.commands.push_back("echo \"Script executed\"");
        }

        // Get dependencies (optional)
        std::cout << "Dependencies (comma-separated, or press Enter to skip): ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            std::istringstream stream(input);
            std::string dep;
            while (std::getline(stream, dep, ',')) {
                // Trim whitespace
                dep.erase(0, dep.find_first_not_of(" \t"));
                dep.erase(dep.find_last_not_of(" \t") + 1);
                if (!dep.empty()) {
                    request.dependencies.push_back(dep);
                }
            }
        }

        // Requires admin
        std::cout << "Requires admin privileges? (yes/no): ";
        std::getline(std::cin, input);
        request.requiresAdmin = parseYesNo(input);

        // Auto-sign
        std::cout << "Auto-sign with CA? (yes/no) [default: yes]: ";
        std::getline(std::cin, input);
        request.autoSign = input.empty() ? true : parseYesNo(input);

        // Custom name
        std::cout << "Custom script name (or press Enter for auto-generated): ";
        std::getline(std::cin, request.customName);

        // Generate scripts
        std::cout << "\n🔄 Generating scripts...\n";
        auto scripts = generator.generateScripts(request);

        // Display results
        std::cout << "\n✅ Generated " << scripts.size() << " script(s):\n";
        for (const auto& script : scripts) {
            std::cout << "  ✓ " << script.filename;
            if (script.isSigned) {
                std::cout << " [SIGNED]";
            }
            std::cout << "\n    Path: " << script.filepath << "\n";
        }
        std::cout << "\nScripts saved to: " << generator.getOutputDirectory() << "\n\n";
    }

public:
    ScriptGeneratorChatbot() : running(false) {}

    void run() {
        if (!generator.initialize()) {
            std::cerr << "Failed to initialize script generator\n";
            return;
        }

        std::cout << "Script Generator initialized successfully!\n";
        std::cout << "Output directory: " << generator.getOutputDirectory() << "\n";

        displayWelcome();
        running = true;

        while (running) {
            std::cout << "script-gen> ";
            std::string command;
            std::getline(std::cin, command);

            if (command.empty()) continue;

            // Convert to lowercase for comparison
            std::string cmd = command;
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

            if (cmd == "exit" || cmd == "quit" || cmd == "bye") {
                running = false;
                std::cout << "Goodbye!\n";
            }
            else if (cmd == "help" || cmd == "?") {
                displayHelp();
            }
            else if (cmd == "platforms") {
                displayPlatforms();
            }
            else if (cmd == "generate" || cmd == "create" || cmd == "new") {
                handleGenerate();
            }
            else if (cmd == "clear" || cmd == "cls") {
                #ifdef _WIN32
                system("cls");
                #else
                system("clear");
                #endif
                displayWelcome();
            }
            else {
                std::cout << "Unknown command. Type 'help' for available commands.\n";
            }
        }
    }
};

int main() {
    ScriptGeneratorChatbot chatbot;
    chatbot.run();
    return 0;
}
