#include "ide_application.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sys/stat.h>

using namespace HybridIDE;

void printUsage() {
    std::cout << "Hybrid IDE - Complete Development Environment\n\n";
    std::cout << "Usage: HybridIDE [options] [file/project]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h              Show this help message\n";
    std::cout << "  --version, -v           Show version information\n";
    std::cout << "  --new-extension <path>  Create new VS Code extension\n";
    std::cout << "  --build-extension <path> Build VS Code extension\n";
    std::cout << "  --package-extension <path> Package VS Code extension to .vsix\n";
    std::cout << "  --ai-chat              Start AI assistant chat mode\n";
    std::cout << "  --generate <desc>      Generate code from description\n";
    std::cout << "  --compile <file>       Compile file\n";
    std::cout << "  --build                Build current project\n";
    std::cout << "  --run                  Run current project\n";
    std::cout << "  --debug <file>         Start debugging\n";
    std::cout << "  --profile <file>       Profile application\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  HybridIDE                           # Launch IDE\n";
    std::cout << "  HybridIDE myproject/               # Open project\n";
    std::cout << "  HybridIDE main.cpp                 # Open file\n";
    std::cout << "  HybridIDE --new-extension my-ext   # Create extension\n";
    std::cout << "  HybridIDE --ai-chat                # Start AI chat\n";
    std::cout << "  HybridIDE --generate \"web server\"  # Generate code\n";
    std::cout << "\n";
}

void printVersion() {
    std::cout << "Hybrid IDE v2.0.0\n";
    std::cout << "Complete AI-Powered Development Environment\n";
    std::cout << "Built with C++17, React, and TypeScript\n";
    std::cout << "Copyright (c) 2025\n";
}

int aiChatMode(IDEApplication& app) {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║     AI Assistant Chat Mode           ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";
    std::cout << "Type 'exit' or 'quit' to end chat\n";
    std::cout << "Type 'help' for available commands\n\n";

    AIAssistant* ai = app.getAIAssistant();
    std::string input;

    while (true) {
        std::cout << "\n👤 You: ";
        std::getline(std::cin, input);

        if (input.empty()) continue;
        if (input == "exit" || input == "quit") break;
        
        if (input == "help") {
            std::cout << "\n🤖 AI: Available commands:\n";
            std::cout << "  - 'generate <description>' - Generate code\n";
            std::cout << "  - 'explain <code>' - Explain code\n";
            std::cout << "  - 'fix <code>' - Fix code errors\n";
            std::cout << "  - 'review' - Review current file\n";
            std::cout << "  - 'clear' - Clear chat history\n";
            std::cout << "  - 'exit' - Exit chat mode\n";
            continue;
        }

        if (input == "clear") {
            ai->clearHistory();
            std::cout << "\n🤖 AI: Chat history cleared!\n";
            continue;
        }

        std::cout << "\n🤖 AI: ";
        std::string response = ai->sendMessage(input);
        std::cout << response << "\n";
    }

    std::cout << "\nExiting AI chat mode.\n";
    return 0;
}

int generateCode(IDEApplication& app, const std::string& description) {
    std::cout << "\n🎨 Generating code for: " << description << "\n\n";
    
    AIAssistant* ai = app.getAIAssistant();
    std::string code = ai->generateCode(description, "cpp");
    
    std::cout << "Generated Code:\n";
    std::cout << "═══════════════\n";
    std::cout << code << "\n";
    std::cout << "═══════════════\n\n";
    
    std::cout << "Save to file? (y/n): ";
    std::string answer;
    std::getline(std::cin, answer);
    
    if (answer == "y" || answer == "Y") {
        std::cout << "Filename: ";
        std::string filename;
        std::getline(std::cin, filename);
        
        EditorTab* editor = app.newFile();
        editor->setContent(code);
        if (editor->saveAs(filename)) {
            std::cout << "✅ Saved to " << filename << "\n";
        } else {
            std::cout << "❌ Failed to save file\n";
        }
    }
    
    return 0;
}

int newExtension(IDEApplication& app, const std::string& path) {
    std::cout << "\n📦 Creating VS Code Extension: " << path << "\n\n";
    
    ExtensionBuilder* builder = app.getExtensionBuilder();
    
    // Get extension details
    ExtensionBuilder::ExtensionConfig config;
    
    std::cout << "Extension name (lowercase, no spaces): ";
    std::getline(std::cin, config.name);
    
    std::cout << "Display name: ";
    std::getline(std::cin, config.displayName);
    
    std::cout << "Description: ";
    std::getline(std::cin, config.description);
    
    std::cout << "Publisher: ";
    std::getline(std::cin, config.publisher);
    
    config.version = "0.0.1";
    config.categories = {"Other"};
    
    std::cout << "\n🔨 Creating extension structure...\n";
    
    if (builder->createExtension(path, config)) {
        std::cout << "✅ Extension created successfully!\n\n";
        std::cout << "Next steps:\n";
        std::cout << "  1. cd " << path << "\n";
        std::cout << "  2. code .\n";
        std::cout << "  3. Press F5 to debug\n\n";
        std::cout << "Or use:\n";
        std::cout << "  HybridIDE --build-extension " << path << "\n";
        return 0;
    } else {
        std::cout << "❌ Failed to create extension\n";
        return 1;
    }
}

int buildExtension(IDEApplication& app, const std::string& path) {
    std::cout << "\n🔨 Building VS Code Extension: " << path << "\n\n";
    
    ExtensionBuilder* builder = app.getExtensionBuilder();
    ExtensionBuilder::BuildOptions options;
    
    if (builder->build(path, options)) {
        std::cout << "✅ Extension built successfully!\n";
        std::cout << "\nTo package:\n";
        std::cout << "  HybridIDE --package-extension " << path << "\n";
        return 0;
    } else {
        std::cout << "❌ Build failed\n";
        return 1;
    }
}

int packageExtension(IDEApplication& app, const std::string& path) {
    std::cout << "\n📦 Packaging VS Code Extension: " << path << "\n\n";
    
    ExtensionBuilder* builder = app.getExtensionBuilder();
    
    if (builder->package(path)) {
        std::cout << "✅ Extension packaged successfully!\n";
        std::cout << "\nTo install:\n";
        std::cout << "  code --install-extension " << path << "/*.vsix\n";
        return 0;
    } else {
        std::cout << "❌ Packaging failed\n";
        return 1;
    }
}

int main(int argc, char* argv[]) {
    IDEApplication app;

    // Parse command line arguments
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.empty()) {
        // Launch full IDE
        if (!app.initialize()) {
            std::cerr << "Failed to initialize IDE\n";
            return 1;
        }
        return app.run();
    }

    // Handle command line options
    std::string cmd = args[0];

    if (cmd == "--help" || cmd == "-h") {
        printUsage();
        return 0;
    }

    if (cmd == "--version" || cmd == "-v") {
        printVersion();
        return 0;
    }

    if (cmd == "--ai-chat") {
        if (!app.initialize()) {
            std::cerr << "Failed to initialize AI assistant\n";
            return 1;
        }
        return aiChatMode(app);
    }

    if (cmd == "--generate" && args.size() > 1) {
        if (!app.initialize()) {
            std::cerr << "Failed to initialize AI assistant\n";
            return 1;
        }
        std::string description = args[1];
        return generateCode(app, description);
    }

    if (cmd == "--new-extension" && args.size() > 1) {
        if (!app.initialize()) {
            std::cerr << "Failed to initialize extension builder\n";
            return 1;
        }
        return newExtension(app, args[1]);
    }

    if (cmd == "--build-extension" && args.size() > 1) {
        if (!app.initialize()) {
            std::cerr << "Failed to initialize extension builder\n";
            return 1;
        }
        return buildExtension(app, args[1]);
    }

    if (cmd == "--package-extension" && args.size() > 1) {
        if (!app.initialize()) {
            std::cerr << "Failed to initialize extension builder\n";
            return 1;
        }
        return packageExtension(app, args[1]);
    }

    if (cmd == "--compile" && args.size() > 1) {
        if (!app.initialize()) {
            std::cerr << "Failed to initialize compiler\n";
            return 1;
        }
        EditorTab* editor = app.openFile(args[1]);
        if (editor && app.build()) {
            std::cout << "✅ Compilation successful\n";
            return 0;
        }
        std::cout << "❌ Compilation failed\n";
        return 1;
    }

    // If it's a file or directory, open it
    if (!app.initialize()) {
        std::cerr << "Failed to initialize IDE\n";
        return 1;
    }

    struct stat info;
    if (stat(cmd.c_str(), &info) == 0) {
        if (S_ISDIR(info.st_mode)) {
            app.openProject(cmd);
        } else {
            app.openFile(cmd);
        }
    }

    return app.run();
}
