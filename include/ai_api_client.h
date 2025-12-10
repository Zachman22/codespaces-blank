#ifndef AI_API_CLIENT_H
#define AI_API_CLIENT_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

/**
 * AI API Client - Unified interface for OpenAI and Anthropic APIs
 * Supports GPT-4, GPT-3.5, Claude 3 (Opus, Sonnet, Haiku)
 */
class AIAPIClient {
public:
    enum class Provider {
        OPENAI,
        ANTHROPIC,
        AUTO  // Auto-select based on task
    };

    enum class Model {
        // OpenAI Models
        GPT_4_TURBO,
        GPT_4,
        GPT_3_5_TURBO,
        
        // Anthropic Models
        CLAUDE_3_OPUS,
        CLAUDE_3_SONNET,
        CLAUDE_3_HAIKU,
        CLAUDE_3_5_SONNET
    };

    struct Message {
        std::string role;      // "user", "assistant", "system"
        std::string content;
        std::map<std::string, std::string> metadata;
    };

    struct CompletionRequest {
        Model model;
        std::vector<Message> messages;
        double temperature = 0.7;
        int maxTokens = 2048;
        std::vector<std::string> stopSequences;
        bool stream = false;
    };

    struct CompletionResponse {
        std::string content;
        std::string modelUsed;
        int tokensUsed;
        double cost;
        std::map<std::string, std::string> metadata;
    };

    AIAPIClient();
    ~AIAPIClient();

    // Initialize with API keys
    bool initialize(const std::string& openaiKey = "", const std::string& anthropicKey = "");

    // Set API keys from environment or config
    bool loadAPIKeysFromEnvironment();
    bool loadAPIKeysFromConfig(const std::string& configPath);

    // Main completion method
    CompletionResponse complete(const CompletionRequest& request);

    // Streaming completion with callback
    void streamComplete(
        const CompletionRequest& request,
        std::function<void(const std::string& chunk)> onChunk,
        std::function<void(const CompletionResponse& response)> onComplete
    );

    // Convenience methods
    std::string chat(const std::string& prompt, Model model = Model::GPT_4_TURBO);
    std::string codeCompletion(const std::string& code, const std::string& language);
    std::string codeReview(const std::string& code, const std::string& language);
    std::string debugAssistance(const std::string& error, const std::string& code);
    std::string optimizeSuggestions(const std::string& code, const std::string& language);

    // Model management
    void setDefaultModel(Model model);
    Model getDefaultModel() const;
    std::vector<std::string> getAvailableModels() const;

    // Cost tracking
    double getTotalCost() const;
    int getTotalTokensUsed() const;
    void resetUsageStats();

    // Configuration
    void setTimeout(int seconds);
    void setMaxRetries(int retries);
    void setRetryDelay(int milliseconds);

private:
    std::string openaiAPIKey_;
    std::string anthropicAPIKey_;
    Model defaultModel_;
    
    double totalCost_;
    int totalTokens_;
    int timeout_;
    int maxRetries_;
    int retryDelay_;

    // HTTP client methods
    std::string makeOpenAIRequest(const CompletionRequest& request);
    std::string makeAnthropicRequest(const CompletionRequest& request);
    std::string makeHTTPRequest(
        const std::string& url,
        const std::string& method,
        const std::string& headers,
        const std::string& body
    );

    // Helper methods
    std::string modelToString(Model model) const;
    Provider getProviderForModel(Model model) const;
    double calculateCost(Model model, int tokens) const;
    std::string buildOpenAIPayload(const CompletionRequest& request);
    std::string buildAnthropicPayload(const CompletionRequest& request);
    CompletionResponse parseOpenAIResponse(const std::string& response);
    CompletionResponse parseAnthropicResponse(const std::string& response);
};

#endif // AI_API_CLIENT_H
