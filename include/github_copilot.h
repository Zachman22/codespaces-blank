#ifndef GITHUB_COPILOT_H
#define GITHUB_COPILOT_H

#include <string>
#include <vector>
#include <functional>

/**
 * GitHub Copilot Integration - AI pair programmer
 */
class GitHubCopilot {
public:
    struct Suggestion {
        std::string code;
        std::string displayText;
        int confidence;  // 0-100
        std::string language;
        int startLine;
        int startColumn;
    };

    struct CompletionContext {
        std::string filePath;
        std::string language;
        std::string currentLine;
        std::string beforeCursor;
        std::string afterCursor;
        std::vector<std::string> recentFiles;
        int cursorLine;
        int cursorColumn;
    };

    GitHubCopilot();
    ~GitHubCopilot();

    // Initialization
    bool initialize(const std::string& authToken);
    bool isInitialized() const;
    
    // Code completion
    std::vector<Suggestion> getCompletions(const CompletionContext& context);
    std::string getInlineCompletion(const CompletionContext& context);
    
    // Code generation
    std::string generateFunction(const std::string& description, const std::string& language);
    std::string generateTests(const std::string& code, const std::string& language);
    std::string explainCode(const std::string& code);
    
    // Refactoring
    std::string refactorCode(const std::string& code, const std::string& instruction);
    std::string fixBugs(const std::string& code, const std::string& error);
    std::string optimizeCode(const std::string& code);
    
    // Documentation
    std::string generateDocstring(const std::string& code, const std::string& language);
    std::string generateComments(const std::string& code);
    
    // Chat
    std::string askQuestion(const std::string& question, const std::string& context = "");
    
    // Settings
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void setAutoComplete(bool autoComplete);
    void setSuggestionDelay(int milliseconds);
    
    // Callbacks
    void setSuggestionCallback(std::function<void(const std::vector<Suggestion>&)> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);

private:
    bool initialized_;
    bool enabled_;
    bool autoComplete_;
    int suggestionDelay_;
    std::string authToken_;
    std::string endpoint_;
    
    std::function<void(const std::vector<Suggestion>&)> suggestionCallback_;
    std::function<void(const std::string&)> errorCallback_;
    
    // API communication
    std::string makeAPIRequest(const std::string& endpoint, const std::string& payload);
    std::vector<Suggestion> parseSuggestions(const std::string& response);
    
    // Context analysis
    std::string buildPrompt(const CompletionContext& context);
    std::string detectLanguage(const std::string& filePath);
};

#endif // GITHUB_COPILOT_H
