#include "../include/auto_coder_chatbot.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace HybridIDE {

AutoCoderChatbot::AutoCoderChatbot() 
    : isInitialized_(false)
    , conversationId_(0) {
    // Set default output directory
#ifdef _WIN32
    outputDirectory_ = "G:\\Zachary's Playroom Powerd by AI";
#else
    // On Linux, use home directory or current directory
    const char* home = getenv("HOME");
    if (home) {
        outputDirectory_ = std::string(home) + "/ZacharysPlayroom";
    } else {
        outputDirectory_ = "./ZacharysPlayroom";
    }
#endif
}

AutoCoderChatbot::~AutoCoderChatbot() {
}

bool AutoCoderChatbot::initialize() {
    if (isInitialized_) {
        return true;
    }
    
    // Create output directory structure
    if (!createDirectoryRecursive(outputDirectory_)) {
        std::cerr << "Failed to create output directory: " << outputDirectory_ << std::endl;
        return false;
    }
    
    // Create subdirectories
    createDirectoryRecursive(outputDirectory_ + "/conversations");
    createDirectoryRecursive(outputDirectory_ + "/generated_code");
    createDirectoryRecursive(outputDirectory_ + "/projects");
    createDirectoryRecursive(outputDirectory_ + "/exports");
    
    // Initialize conversation log
    conversationLog_.clear();
    conversationId_ = static_cast<int>(std::time(nullptr));
    
    isInitialized_ = true;
    
    logMessage("AutoCoder Chatbot initialized successfully");
    logMessage("Output directory: " + outputDirectory_);
    
    return true;
}

bool AutoCoderChatbot::createDirectoryRecursive(const std::string& path) {
#ifdef _WIN32
    // Windows: Create directory with all parent directories
    std::string normalizedPath = path;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '/', '\\');
    
    // Split path and create each directory
    std::string currentPath;
    std::istringstream iss(normalizedPath);
    std::string segment;
    
    while (std::getline(iss, segment, '\\')) {
        if (segment.empty()) continue;
        
        if (currentPath.empty() && segment.find(':') != std::string::npos) {
            currentPath = segment;
        } else {
            currentPath += (currentPath.empty() ? "" : "\\") + segment;
        }
        
        // Try to create directory
        if (GetFileAttributesA(currentPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            if (!CreateDirectoryA(currentPath.c_str(), NULL)) {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS) {
                    return false;
                }
            }
        }
    }
    return true;
#else
    // Linux/Unix: Create directory recursively
    std::string normalizedPath = path;
    std::string currentPath;
    std::istringstream iss(normalizedPath);
    std::string segment;
    
    while (std::getline(iss, segment, '/')) {
        if (segment.empty()) continue;
        currentPath += "/" + segment;
        
        struct stat st;
        if (stat(currentPath.c_str(), &st) != 0) {
            if (mkdir(currentPath.c_str(), 0755) != 0) {
                return false;
            }
        }
    }
    return true;
#endif
}

std::string AutoCoderChatbot::chat(const std::string& userMessage) {
    if (!isInitialized_) {
        initialize();
    }
    
    // Log user message
    ChatMessage msg;
    msg.role = "user";
    msg.content = userMessage;
    msg.timestamp = std::time(nullptr);
    conversationLog_.push_back(msg);
    
    // Process the message and generate response
    std::string response = processMessage(userMessage);
    
    // Log assistant response
    ChatMessage responseMsg;
    responseMsg.role = "assistant";
    responseMsg.content = response;
    responseMsg.timestamp = std::time(nullptr);
    conversationLog_.push_back(responseMsg);
    
    // Auto-save conversation
    saveConversation();
    
    return response;
}

std::string AutoCoderChatbot::processMessage(const std::string& message) {
    std::string lowerMsg = message;
    std::transform(lowerMsg.begin(), lowerMsg.end(), lowerMsg.begin(), ::tolower);
    
    // Command detection
    if (lowerMsg.find("generate") != std::string::npos || 
        lowerMsg.find("create") != std::string::npos ||
        lowerMsg.find("write") != std::string::npos) {
        return handleCodeGenerationRequest(message);
    }
    else if (lowerMsg.find("fix") != std::string::npos || 
             lowerMsg.find("debug") != std::string::npos ||
             lowerMsg.find("error") != std::string::npos) {
        return handleDebugRequest(message);
    }
    else if (lowerMsg.find("explain") != std::string::npos ||
             lowerMsg.find("what is") != std::string::npos ||
             lowerMsg.find("how does") != std::string::npos) {
        return handleExplanationRequest(message);
    }
    else if (lowerMsg.find("optimize") != std::string::npos ||
             lowerMsg.find("improve") != std::string::npos ||
             lowerMsg.find("refactor") != std::string::npos) {
        return handleOptimizationRequest(message);
    }
    else if (lowerMsg.find("test") != std::string::npos) {
        return handleTestGenerationRequest(message);
    }
    else if (lowerMsg.find("project") != std::string::npos) {
        return handleProjectRequest(message);
    }
    else {
        return handleGeneralQuery(message);
    }
}

std::string AutoCoderChatbot::handleCodeGenerationRequest(const std::string& request) {
    logMessage("Handling code generation request: " + request);
    
    // Detect programming language
    Language lang = detectLanguage(request);
    std::string langName = getLanguageName(lang);
    
    // Generate code using AutoCoder
    AutoCoder coder;
    std::string generatedCode = coder.generateCode(request, lang);
    
    // Save generated code to file
    std::string filename = generateFilename("generated", getFileExtension(lang));
    std::string filepath = outputDirectory_ + "/generated_code/" + filename;
    
    if (saveCodeToFile(filepath, generatedCode)) {
        std::string response = "I've generated " + langName + " code for your request.\n\n";
        response += "```" + langName + "\n" + generatedCode + "\n```\n\n";
        response += "Code saved to: " + filepath + "\n\n";
        response += "Would you like me to:\n";
        response += "• Generate tests for this code?\n";
        response += "• Explain how it works?\n";
        response += "• Create a full project around this?\n";
        response += "• Optimize the implementation?";
        
        return response;
    }
    
    return "Generated code:\n\n```" + langName + "\n" + generatedCode + "\n```";
}

std::string AutoCoderChatbot::handleDebugRequest(const std::string& request) {
    logMessage("Handling debug request: " + request);
    
    std::string response = "I'll help you debug the code. To provide the best assistance, please:\n\n";
    response += "1. **Share the error message** - What error are you seeing?\n";
    response += "2. **Provide the code** - What code is causing the issue?\n";
    response += "3. **Explain the expected behavior** - What should it do?\n\n";
    response += "I can help with:\n";
    response += "• Syntax errors and compilation issues\n";
    response += "• Runtime errors and exceptions\n";
    response += "• Logic errors and unexpected behavior\n";
    response += "• Performance bottlenecks\n";
    response += "• Memory leaks and resource management\n\n";
    response += "Just paste your code and error message, and I'll analyze it!";
    
    return response;
}

std::string AutoCoderChatbot::handleExplanationRequest(const std::string& request) {
    logMessage("Handling explanation request: " + request);
    
    if (request.find("algorithm") != std::string::npos) {
        return explainAlgorithm(request);
    }
    else if (request.find("pattern") != std::string::npos) {
        return explainDesignPattern(request);
    }
    else {
        std::string response = "I can explain:\n\n";
        response += "• **Algorithms** - Sorting, searching, graph algorithms, etc.\n";
        response += "• **Data Structures** - Arrays, trees, graphs, hash tables, etc.\n";
        response += "• **Design Patterns** - Singleton, Factory, Observer, etc.\n";
        response += "• **Concepts** - OOP, functional programming, concurrency, etc.\n";
        response += "• **Best Practices** - Code organization, testing, documentation\n\n";
        response += "What would you like me to explain in detail?";
        return response;
    }
}

std::string AutoCoderChatbot::handleOptimizationRequest(const std::string& request) {
    logMessage("Handling optimization request: " + request);
    
    std::string response = "I can help optimize your code for:\n\n";
    response += "• **Performance** - Speed improvements, algorithm optimization\n";
    response += "• **Memory** - Reduce memory usage, fix memory leaks\n";
    response += "• **Readability** - Cleaner code structure, better naming\n";
    response += "• **Maintainability** - Modular design, documentation\n";
    response += "• **Best Practices** - Follow language idioms and standards\n\n";
    response += "Please share your code, and I'll analyze it and suggest improvements!";
    
    return response;
}

std::string AutoCoderChatbot::handleTestGenerationRequest(const std::string& request) {
    logMessage("Handling test generation request: " + request);
    
    std::string response = "I'll generate comprehensive tests for your code.\n\n";
    response += "I can create:\n";
    response += "• **Unit Tests** - Test individual functions/methods\n";
    response += "• **Integration Tests** - Test component interactions\n";
    response += "• **Edge Cases** - Boundary conditions and error handling\n";
    response += "• **Performance Tests** - Benchmarking and profiling\n\n";
    response += "Please provide the code you'd like me to test, and I'll generate a complete test suite!";
    
    return response;
}

std::string AutoCoderChatbot::handleProjectRequest(const std::string& request) {
    logMessage("Handling project request: " + request);
    
    // Create project structure
    std::string projectName = extractProjectName(request);
    std::string projectPath = outputDirectory_ + "/projects/" + projectName;
    
    createDirectoryRecursive(projectPath);
    createDirectoryRecursive(projectPath + "/src");
    createDirectoryRecursive(projectPath + "/include");
    createDirectoryRecursive(projectPath + "/tests");
    createDirectoryRecursive(projectPath + "/docs");
    createDirectoryRecursive(projectPath + "/build");
    
    // Generate project files
    generateProjectFiles(projectPath, projectName, request);
    
    std::string response = "I've created a full project structure for: **" + projectName + "**\n\n";
    response += "Project location: " + projectPath + "\n\n";
    response += "Structure:\n";
    response += "```\n";
    response += projectName + "/\n";
    response += "├── src/           # Source code files\n";
    response += "├── include/       # Header files\n";
    response += "├── tests/         # Test files\n";
    response += "├── docs/          # Documentation\n";
    response += "├── build/         # Build output\n";
    response += "├── CMakeLists.txt # Build configuration\n";
    response += "└── README.md      # Project documentation\n";
    response += "```\n\n";
    response += "Next steps:\n";
    response += "1. Review the generated files\n";
    response += "2. Build the project: `cmake -B build && cmake --build build`\n";
    response += "3. Run tests: `cd build && ctest`\n\n";
    response += "What would you like to add to the project?";
    
    return response;
}

std::string AutoCoderChatbot::handleGeneralQuery(const std::string& query) {
    logMessage("Handling general query: " + query);
    
    std::string response = "Hello! I'm your AI Auto Coder assistant. I can help you with:\n\n";
    response += "🔧 **Code Generation**\n";
    response += "   • \"Generate a REST API in Python\"\n";
    response += "   • \"Create a sorting algorithm in C++\"\n";
    response += "   • \"Write a React component for authentication\"\n\n";
    response += "🐛 **Debugging & Fixing**\n";
    response += "   • \"Fix this error: [paste error]\"\n";
    response += "   • \"Debug my function that...\"\n";
    response += "   • \"Why isn't this working?\"\n\n";
    response += "📚 **Learning & Explanations**\n";
    response += "   • \"Explain how binary search works\"\n";
    response += "   • \"What is the Singleton pattern?\"\n";
    response += "   • \"How do I use async/await?\"\n\n";
    response += "🚀 **Optimization**\n";
    response += "   • \"Optimize this code for performance\"\n";
    response += "   • \"Improve memory usage\"\n";
    response += "   • \"Refactor this function\"\n\n";
    response += "🧪 **Testing**\n";
    response += "   • \"Generate tests for this function\"\n";
    response += "   • \"Create unit tests\"\n";
    response += "   • \"Add edge case tests\"\n\n";
    response += "📦 **Full Projects**\n";
    response += "   • \"Create a TODO app project\"\n";
    response += "   • \"Build a chat application\"\n";
    response += "   • \"Generate a web scraper\"\n\n";
    response += "All generated code is automatically saved to:\n";
    response += "📁 " + outputDirectory_ + "\n\n";
    response += "What would you like to create today?";
    
    return response;
}

Language AutoCoderChatbot::detectLanguage(const std::string& request) {
    std::string lower = request;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower.find("python") != std::string::npos || lower.find(".py") != std::string::npos) return Language::PYTHON;
    if (lower.find("javascript") != std::string::npos || lower.find(".js") != std::string::npos) return Language::JAVASCRIPT;
    if (lower.find("typescript") != std::string::npos || lower.find(".ts") != std::string::npos) return Language::TYPESCRIPT;
    if (lower.find("c++") != std::string::npos || lower.find("cpp") != std::string::npos) return Language::CPP;
    if (lower.find("c#") != std::string::npos || lower.find("csharp") != std::string::npos) return Language::CSHARP;
    if (lower.find("rust") != std::string::npos || lower.find(".rs") != std::string::npos) return Language::RUST;
    if (lower.find("lua") != std::string::npos || lower.find(".lua") != std::string::npos) return Language::LUA;
    if (lower.find("sql") != std::string::npos) return Language::SQL;
    if (lower.find("bash") != std::string::npos || lower.find("shell") != std::string::npos) return Language::BASH;
    if (lower.find("powershell") != std::string::npos || lower.find(".ps1") != std::string::npos) return Language::POWERSHELL;
    if (lower.find("html") != std::string::npos) return Language::HTML;
    if (lower.find("css") != std::string::npos) return Language::CSS;
    
    return Language::CPP; // Default to C++
}

std::string AutoCoderChatbot::getLanguageName(Language lang) {
    switch (lang) {
        case Language::CPP: return "C++";
        case Language::PYTHON: return "Python";
        case Language::JAVASCRIPT: return "JavaScript";
        case Language::TYPESCRIPT: return "TypeScript";
        case Language::CSHARP: return "C#";
        case Language::RUST: return "Rust";
        case Language::LUA: return "Lua";
        case Language::SQL: return "SQL";
        case Language::BASH: return "Bash";
        case Language::POWERSHELL: return "PowerShell";
        case Language::HTML: return "HTML";
        case Language::CSS: return "CSS";
        case Language::CMAKE: return "CMake";
        case Language::DOCKERFILE: return "Dockerfile";
        default: return "Unknown";
    }
}

std::string AutoCoderChatbot::getFileExtension(Language lang) {
    switch (lang) {
        case Language::CPP: return "cpp";
        case Language::PYTHON: return "py";
        case Language::JAVASCRIPT: return "js";
        case Language::TYPESCRIPT: return "ts";
        case Language::CSHARP: return "cs";
        case Language::RUST: return "rs";
        case Language::LUA: return "lua";
        case Language::SQL: return "sql";
        case Language::BASH: return "sh";
        case Language::POWERSHELL: return "ps1";
        case Language::HTML: return "html";
        case Language::CSS: return "css";
        case Language::CMAKE: return "txt";
        case Language::DOCKERFILE: return "dockerfile";
        default: return "txt";
    }
}

std::string AutoCoderChatbot::generateFilename(const std::string& prefix, const std::string& extension) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm);
    
    return prefix + "_" + std::string(buffer) + "." + extension;
}

bool AutoCoderChatbot::saveCodeToFile(const std::string& filepath, const std::string& code) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << code;
    file.close();
    
    logMessage("Code saved to: " + filepath);
    return true;
}

bool AutoCoderChatbot::saveConversation() {
    std::string filename = "conversation_" + std::to_string(conversationId_) + ".txt";
    std::string filepath = outputDirectory_ + "/conversations/" + filename;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== AutoCoder Chatbot Conversation ===" << std::endl;
    file << "Conversation ID: " << conversationId_ << std::endl;
    file << "Started: " << std::ctime(&conversationLog_[0].timestamp);
    file << "==========================================\n" << std::endl;
    
    for (const auto& msg : conversationLog_) {
        file << "[" << msg.role << "] ";
        file << std::ctime(&msg.timestamp);
        file << msg.content << "\n" << std::endl;
    }
    
    file.close();
    return true;
}

std::string AutoCoderChatbot::exportConversation(const std::string& format) {
    std::string filename = "conversation_" + std::to_string(conversationId_) + "." + format;
    std::string filepath = outputDirectory_ + "/exports/" + filename;
    
    if (format == "json") {
        return exportAsJSON(filepath);
    } else if (format == "html") {
        return exportAsHTML(filepath);
    } else if (format == "md") {
        return exportAsMarkdown(filepath);
    }
    
    return "";
}

std::string AutoCoderChatbot::exportAsJSON(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return "";
    
    file << "{\n";
    file << "  \"conversation_id\": " << conversationId_ << ",\n";
    file << "  \"messages\": [\n";
    
    for (size_t i = 0; i < conversationLog_.size(); ++i) {
        const auto& msg = conversationLog_[i];
        file << "    {\n";
        file << "      \"role\": \"" << msg.role << "\",\n";
        file << "      \"timestamp\": " << msg.timestamp << ",\n";
        file << "      \"content\": \"" << escapeJSON(msg.content) << "\"\n";
        file << "    }" << (i < conversationLog_.size() - 1 ? "," : "") << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    file.close();
    
    return filepath;
}

std::string AutoCoderChatbot::exportAsHTML(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return "";
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n";
    file << "<title>AutoCoder Conversation</title>\n";
    file << "<style>\n";
    file << "body { font-family: Arial, sans-serif; max-width: 800px; margin: 20px auto; }\n";
    file << ".message { margin: 10px 0; padding: 10px; border-radius: 5px; }\n";
    file << ".user { background: #e3f2fd; }\n";
    file << ".assistant { background: #f1f8e9; }\n";
    file << "</style>\n</head>\n<body>\n";
    file << "<h1>AutoCoder Conversation</h1>\n";
    
    for (const auto& msg : conversationLog_) {
        file << "<div class='message " << msg.role << "'>\n";
        file << "<strong>" << msg.role << ":</strong><br>\n";
        file << "<pre>" << escapeHTML(msg.content) << "</pre>\n";
        file << "</div>\n";
    }
    
    file << "</body>\n</html>\n";
    file.close();
    
    return filepath;
}

std::string AutoCoderChatbot::exportAsMarkdown(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return "";
    
    file << "# AutoCoder Conversation\n\n";
    file << "**Conversation ID:** " << conversationId_ << "\n\n";
    file << "---\n\n";
    
    for (const auto& msg : conversationLog_) {
        file << "## " << (msg.role == "user" ? "👤 User" : "🤖 Assistant") << "\n\n";
        file << msg.content << "\n\n";
        file << "---\n\n";
    }
    
    file.close();
    return filepath;
}

std::string AutoCoderChatbot::escapeJSON(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        if (c == '"') escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else if (c == '\n') escaped += "\\n";
        else if (c == '\r') escaped += "\\r";
        else if (c == '\t') escaped += "\\t";
        else escaped += c;
    }
    return escaped;
}

std::string AutoCoderChatbot::escapeHTML(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        if (c == '<') escaped += "&lt;";
        else if (c == '>') escaped += "&gt;";
        else if (c == '&') escaped += "&amp;";
        else if (c == '"') escaped += "&quot;";
        else escaped += c;
    }
    return escaped;
}

void AutoCoderChatbot::logMessage(const std::string& message) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &tm);
    
    std::cout << buffer << " " << message << std::endl;
}

std::string AutoCoderChatbot::extractProjectName(const std::string& request) {
    // Try to extract project name from request
    size_t pos = request.find("project");
    if (pos != std::string::npos) {
        std::string after = request.substr(pos + 7);
        std::istringstream iss(after);
        std::string word;
        while (iss >> word) {
            if (word.length() > 2 && word[0] != '\"') {
                // Clean up the word
                word.erase(std::remove_if(word.begin(), word.end(), 
                    [](char c) { return !std::isalnum(c) && c != '_' && c != '-'; }), word.end());
                if (!word.empty()) {
                    return word;
                }
            }
        }
    }
    
    // Generate default name
    auto t = std::time(nullptr);
    return "project_" + std::to_string(t);
}

void AutoCoderChatbot::generateProjectFiles(const std::string& projectPath, 
                                            const std::string& projectName,
                                            const std::string& description) {
    // Generate README.md
    std::string readmePath = projectPath + "/README.md";
    std::ofstream readme(readmePath);
    readme << "# " << projectName << "\n\n";
    readme << description << "\n\n";
    readme << "## Build Instructions\n\n";
    readme << "```bash\n";
    readme << "cmake -B build\n";
    readme << "cmake --build build\n";
    readme << "```\n\n";
    readme << "## Run\n\n";
    readme << "```bash\n";
    readme << "./build/" << projectName << "\n";
    readme << "```\n";
    readme.close();
    
    // Generate CMakeLists.txt
    std::string cmakePath = projectPath + "/CMakeLists.txt";
    std::ofstream cmake(cmakePath);
    cmake << "cmake_minimum_required(VERSION 3.15)\n";
    cmake << "project(" << projectName << " VERSION 1.0.0)\n\n";
    cmake << "set(CMAKE_CXX_STANDARD 17)\n";
    cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
    cmake << "file(GLOB SOURCES src/*.cpp)\n";
    cmake << "add_executable(" << projectName << " ${SOURCES})\n";
    cmake << "target_include_directories(" << projectName << " PRIVATE include)\n";
    cmake.close();
    
    // Generate main.cpp
    std::string mainPath = projectPath + "/src/main.cpp";
    std::ofstream main(mainPath);
    main << "#include <iostream>\n\n";
    main << "int main() {\n";
    main << "    std::cout << \"" << projectName << " - Generated by AutoCoder\" << std::endl;\n";
    main << "    return 0;\n";
    main << "}\n";
    main.close();
    
    logMessage("Generated project files in: " + projectPath);
}

std::string AutoCoderChatbot::explainAlgorithm(const std::string& request) {
    return "Algorithm explanation feature - analyzing your request...";
}

std::string AutoCoderChatbot::explainDesignPattern(const std::string& request) {
    return "Design pattern explanation feature - analyzing your request...";
}

void AutoCoderChatbot::clearConversation() {
    conversationLog_.clear();
    conversationId_ = static_cast<int>(std::time(nullptr));
    logMessage("Conversation cleared. New conversation ID: " + std::to_string(conversationId_));
}

std::vector<ChatMessage> AutoCoderChatbot::getConversationHistory() const {
    return conversationLog_;
}

void AutoCoderChatbot::setOutputDirectory(const std::string& directory) {
    outputDirectory_ = directory;
    createDirectoryRecursive(outputDirectory_);
    logMessage("Output directory changed to: " + outputDirectory_);
}

std::string AutoCoderChatbot::getOutputDirectory() const {
    return outputDirectory_;
}

} // namespace HybridIDE
