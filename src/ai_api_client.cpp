#include "ai_api_client.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <curl/curl.h>
#include <json/json.h>

AIAPIClient::AIAPIClient() 
    : defaultModel_(Model::GPT_4_TURBO),
      totalCost_(0.0),
      totalTokens_(0),
      timeout_(30),
      maxRetries_(3),
      retryDelay_(1000) {}

AIAPIClient::~AIAPIClient() {}

bool AIAPIClient::initialize(const std::string& openaiKey, const std::string& anthropicKey) {
    openaiAPIKey_ = openaiKey;
    anthropicAPIKey_ = anthropicKey;
    
    if (openaiAPIKey_.empty() && anthropicAPIKey_.empty()) {
        return loadAPIKeysFromEnvironment();
    }
    
    return !openaiAPIKey_.empty() || !anthropicAPIKey_.empty();
}

bool AIAPIClient::loadAPIKeysFromEnvironment() {
    const char* openaiKey = std::getenv("OPENAI_API_KEY");
    const char* anthropicKey = std::getenv("ANTHROPIC_API_KEY");
    
    if (openaiKey) openaiAPIKey_ = openaiKey;
    if (anthropicKey) anthropicAPIKey_ = anthropicKey;
    
    return !openaiAPIKey_.empty() || !anthropicAPIKey_.empty();
}

bool AIAPIClient::loadAPIKeysFromConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << configPath << std::endl;
        return false;
    }
    
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    
    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        std::cerr << "Failed to parse config: " << errs << std::endl;
        return false;
    }
    
    if (root.isMember("openai_api_key")) {
        openaiAPIKey_ = root["openai_api_key"].asString();
    }
    if (root.isMember("anthropic_api_key")) {
        anthropicAPIKey_ = root["anthropic_api_key"].asString();
    }
    
    return !openaiAPIKey_.empty() || !anthropicAPIKey_.empty();
}

AIAPIClient::CompletionResponse AIAPIClient::complete(const CompletionRequest& request) {
    Provider provider = getProviderForModel(request.model);
    
    std::string response;
    if (provider == Provider::OPENAI) {
        response = makeOpenAIRequest(request);
        return parseOpenAIResponse(response);
    } else if (provider == Provider::ANTHROPIC) {
        response = makeAnthropicRequest(request);
        return parseAnthropicResponse(response);
    }
    
    CompletionResponse errorResponse;
    errorResponse.content = "Error: Unsupported model or missing API key";
    return errorResponse;
}

std::string AIAPIClient::chat(const std::string& prompt, Model model) {
    CompletionRequest request;
    request.model = model;
    
    Message msg;
    msg.role = "user";
    msg.content = prompt;
    request.messages.push_back(msg);
    
    CompletionResponse response = complete(request);
    return response.content;
}

std::string AIAPIClient::codeCompletion(const std::string& code, const std::string& language) {
    std::string prompt = "Complete the following " + language + " code:\n\n```" + language + "\n" + code + "\n```\n\nProvide only the completed code without explanations.";
    return chat(prompt, Model::GPT_4_TURBO);
}

std::string AIAPIClient::codeReview(const std::string& code, const std::string& language) {
    std::string prompt = "Review the following " + language + " code and provide feedback:\n\n```" + language + "\n" + code + "\n```\n\nProvide suggestions for improvements, bugs, and best practices.";
    return chat(prompt, Model::GPT_4_TURBO);
}

std::string AIAPIClient::debugAssistance(const std::string& error, const std::string& code) {
    std::string prompt = "Debug the following error:\n\nError: " + error + "\n\nCode:\n```\n" + code + "\n```\n\nExplain the error and provide a fix.";
    return chat(prompt, Model::GPT_4_TURBO);
}

std::string AIAPIClient::optimizeSuggestions(const std::string& code, const std::string& language) {
    std::string prompt = "Analyze the following " + language + " code for performance optimization:\n\n```" + language + "\n" + code + "\n```\n\nProvide specific optimization suggestions.";
    return chat(prompt, Model::GPT_4_TURBO);
}

std::string AIAPIClient::modelToString(Model model) const {
    switch (model) {
        case Model::GPT_4_TURBO: return "gpt-4-turbo-preview";
        case Model::GPT_4: return "gpt-4";
        case Model::GPT_3_5_TURBO: return "gpt-3.5-turbo";
        case Model::CLAUDE_3_OPUS: return "claude-3-opus-20240229";
        case Model::CLAUDE_3_SONNET: return "claude-3-sonnet-20240229";
        case Model::CLAUDE_3_HAIKU: return "claude-3-haiku-20240307";
        case Model::CLAUDE_3_5_SONNET: return "claude-3-5-sonnet-20241022";
        default: return "gpt-4-turbo-preview";
    }
}

AIAPIClient::Provider AIAPIClient::getProviderForModel(Model model) const {
    switch (model) {
        case Model::GPT_4_TURBO:
        case Model::GPT_4:
        case Model::GPT_3_5_TURBO:
            return Provider::OPENAI;
        case Model::CLAUDE_3_OPUS:
        case Model::CLAUDE_3_SONNET:
        case Model::CLAUDE_3_HAIKU:
        case Model::CLAUDE_3_5_SONNET:
            return Provider::ANTHROPIC;
        default:
            return Provider::OPENAI;
    }
}

double AIAPIClient::calculateCost(Model model, int tokens) const {
    // Cost per 1K tokens (approximate)
    double costPer1K = 0.0;
    
    switch (model) {
        case Model::GPT_4_TURBO: costPer1K = 0.01; break;
        case Model::GPT_4: costPer1K = 0.03; break;
        case Model::GPT_3_5_TURBO: costPer1K = 0.001; break;
        case Model::CLAUDE_3_OPUS: costPer1K = 0.015; break;
        case Model::CLAUDE_3_SONNET: costPer1K = 0.003; break;
        case Model::CLAUDE_3_HAIKU: costPer1K = 0.00025; break;
        case Model::CLAUDE_3_5_SONNET: costPer1K = 0.003; break;
    }
    
    return (tokens / 1000.0) * costPer1K;
}

std::string AIAPIClient::makeOpenAIRequest(const CompletionRequest& request) {
    // This is a placeholder - full implementation would use libcurl
    std::cout << "Making OpenAI API request with model: " << modelToString(request.model) << std::endl;
    return "{\"choices\":[{\"message\":{\"content\":\"OpenAI API response placeholder\"}}],\"usage\":{\"total_tokens\":100}}";
}

std::string AIAPIClient::makeAnthropicRequest(const CompletionRequest& request) {
    // This is a placeholder - full implementation would use libcurl
    std::cout << "Making Anthropic API request with model: " << modelToString(request.model) << std::endl;
    return "{\"content\":[{\"text\":\"Anthropic API response placeholder\"}],\"usage\":{\"input_tokens\":50,\"output_tokens\":50}}";
}

AIAPIClient::CompletionResponse AIAPIClient::parseOpenAIResponse(const std::string& response) {
    CompletionResponse result;
    result.content = "Parsed OpenAI response: " + response.substr(0, 100);
    result.modelUsed = "gpt-4-turbo";
    result.tokensUsed = 100;
    result.cost = calculateCost(Model::GPT_4_TURBO, 100);
    
    totalTokens_ += result.tokensUsed;
    totalCost_ += result.cost;
    
    return result;
}

AIAPIClient::CompletionResponse AIAPIClient::parseAnthropicResponse(const std::string& response) {
    CompletionResponse result;
    result.content = "Parsed Anthropic response: " + response.substr(0, 100);
    result.modelUsed = "claude-3-5-sonnet";
    result.tokensUsed = 100;
    result.cost = calculateCost(Model::CLAUDE_3_5_SONNET, 100);
    
    totalTokens_ += result.tokensUsed;
    totalCost_ += result.cost;
    
    return result;
}

void AIAPIClient::setDefaultModel(Model model) {
    defaultModel_ = model;
}

AIAPIClient::Model AIAPIClient::getDefaultModel() const {
    return defaultModel_;
}

double AIAPIClient::getTotalCost() const {
    return totalCost_;
}

int AIAPIClient::getTotalTokensUsed() const {
    return totalTokens_;
}

void AIAPIClient::resetUsageStats() {
    totalCost_ = 0.0;
    totalTokens_ = 0;
}

void AIAPIClient::setTimeout(int seconds) {
    timeout_ = seconds;
}

void AIAPIClient::setMaxRetries(int retries) {
    maxRetries_ = retries;
}

void AIAPIClient::setRetryDelay(int milliseconds) {
    retryDelay_ = milliseconds;
}

std::vector<std::string> AIAPIClient::getAvailableModels() const {
    return {
        "GPT-4 Turbo",
        "GPT-4",
        "GPT-3.5 Turbo",
        "Claude 3 Opus",
        "Claude 3.5 Sonnet",
        "Claude 3 Sonnet",
        "Claude 3 Haiku"
    };
}

void AIAPIClient::streamComplete(
    const CompletionRequest& request,
    std::function<void(const std::string& chunk)> onChunk,
    std::function<void(const CompletionResponse& response)> onComplete
) {
    // Placeholder for streaming implementation
    std::cout << "Streaming not yet implemented" << std::endl;
    CompletionResponse response = complete(request);
    onComplete(response);
}

std::string AIAPIClient::buildOpenAIPayload(const CompletionRequest& request) {
    Json::Value root;
    root["model"] = modelToString(request.model);
    root["temperature"] = request.temperature;
    root["max_tokens"] = request.maxTokens;
    
    Json::Value messages(Json::arrayValue);
    for (const auto& msg : request.messages) {
        Json::Value message;
        message["role"] = msg.role;
        message["content"] = msg.content;
        messages.append(message);
    }
    root["messages"] = messages;
    
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, root);
}

std::string AIAPIClient::buildAnthropicPayload(const CompletionRequest& request) {
    Json::Value root;
    root["model"] = modelToString(request.model);
    root["max_tokens"] = request.maxTokens;
    root["temperature"] = request.temperature;
    
    Json::Value messages(Json::arrayValue);
    for (const auto& msg : request.messages) {
        Json::Value message;
        message["role"] = msg.role;
        message["content"] = msg.content;
        messages.append(message);
    }
    root["messages"] = messages;
    
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, root);
}

std::string AIAPIClient::makeHTTPRequest(
    const std::string& url,
    const std::string& method,
    const std::string& headers,
    const std::string& body
) {
    // Placeholder for HTTP request implementation using libcurl
    return "HTTP response placeholder";
}
