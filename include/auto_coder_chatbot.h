#ifndef AUTO_CODER_CHATBOT_H
#define AUTO_CODER_CHATBOT_H

#include <string>
#include <vector>
#include <map>
#include "auto_coder.h"

namespace HybridIDE {

struct ChatMessage {
    std::string role;      // "user" or "assistant"
    std::string content;   // Message content
    time_t timestamp;      // When message was sent
};

class AutoCoderChatbot {
public:
    AutoCoderChatbot();
    ~AutoCoderChatbot();
    
    // Initialize chatbot with automatic directory creation
    bool initialize();
    
    // Main chat interface
    std::string chat(const std::string& userMessage);
    
    // Directory management
    void setOutputDirectory(const std::string& directory);
    std::string getOutputDirectory() const;
    bool createDirectoryRecursive(const std::string& path);
    
    // Conversation management
    void clearConversation();
    std::vector<ChatMessage> getConversationHistory() const;
    bool saveConversation();
    std::string exportConversation(const std::string& format); // json, html, md
    
    // Code generation and saving
    bool saveCodeToFile(const std::string& filepath, const std::string& code);
    
private:
    // Message processing
    std::string processMessage(const std::string& message);
    
    // Request handlers
    std::string handleCodeGenerationRequest(const std::string& request);
    std::string handleDebugRequest(const std::string& request);
    std::string handleExplanationRequest(const std::string& request);
    std::string handleOptimizationRequest(const std::string& request);
    std::string handleTestGenerationRequest(const std::string& request);
    std::string handleProjectRequest(const std::string& request);
    std::string handleGeneralQuery(const std::string& query);
    
    // Language detection
    Language detectLanguage(const std::string& request);
    std::string getLanguageName(Language lang);
    std::string getFileExtension(Language lang);
    
    // File and project management
    std::string generateFilename(const std::string& prefix, const std::string& extension);
    std::string extractProjectName(const std::string& request);
    void generateProjectFiles(const std::string& projectPath, 
                             const std::string& projectName,
                             const std::string& description);
    
    // Export utilities
    std::string exportAsJSON(const std::string& filepath);
    std::string exportAsHTML(const std::string& filepath);
    std::string exportAsMarkdown(const std::string& filepath);
    
    // String utilities
    std::string escapeJSON(const std::string& str);
    std::string escapeHTML(const std::string& str);
    
    // Learning and explanation
    std::string explainAlgorithm(const std::string& request);
    std::string explainDesignPattern(const std::string& request);
    
    // Logging
    void logMessage(const std::string& message);
    
private:
    std::string outputDirectory_;
    std::vector<ChatMessage> conversationLog_;
    int conversationId_;
    bool isInitialized_;
};

} // namespace HybridIDE

#endif // AUTO_CODER_CHATBOT_H
