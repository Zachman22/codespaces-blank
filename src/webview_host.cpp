#include "../include/webview_host.h"
#include <iostream>

#ifdef _WIN32
#include <shlwapi.h>
#include <shlobj.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#endif

WebViewHost::WebViewHost() : webViewEnvironment(nullptr), webViewController(nullptr), webView(nullptr) {
}

WebViewHost::~WebViewHost() {
    // Cleanup would happen here in production
}

#ifdef _WIN32
bool WebViewHost::initialize(HWND hwnd, const std::wstring& initialURL) {
#else
bool WebViewHost::initialize(void* hwnd, const std::wstring& initialURL) {
#endif
    // Simplified WebView initialization
    // In production Windows build, this would use WebView2 SDK
    // For now, this provides the interface structure
    
    std::wcout << L"Initializing WebView with URL: " << initialURL << std::endl;
    
    // Store initialization state
    webView = (void*)1; // Non-null to indicate initialized
    webViewController = (void*)1;
    webViewEnvironment = (void*)1;
    
    setupMessageHandler();
    
    return true;
}

void WebViewHost::setupMessageHandler() {
    if (!webView) return;
    
    // Message handler setup
    // In production Windows build, this would register WebView2 message callbacks
    std::cout << "WebView message handler ready" << std::endl;
}

std::string WebViewHost::parseMessageType(const std::string& message) {
    // Expected format: {"type":"...", "data":{...}}
    size_t typeStart = message.find("\"type\"");
    if (typeStart == std::string::npos) return "";
    
    size_t valueStart = message.find(":", typeStart);
    if (valueStart == std::string::npos) return "";
    
    valueStart = message.find("\"", valueStart);
    if (valueStart == std::string::npos) return "";
    
    size_t valueEnd = message.find("\"", valueStart + 1);
    if (valueEnd == std::string::npos) return "";
    
    return message.substr(valueStart + 1, valueEnd - valueStart - 1);
}

std::string WebViewHost::parseMessageData(const std::string& message) {
    // Expected format: {"type":"...", "data":{...}}
    size_t dataStart = message.find("\"data\"");
    if (dataStart == std::string::npos) return "{}";
    
    size_t valueStart = message.find(":", dataStart);
    if (valueStart == std::string::npos) return "{}";
    
    // Find the opening brace or quote
    valueStart = message.find_first_of("{[\"", valueStart);
    if (valueStart == std::string::npos) return "{}";
    
    // Simple extraction - find the matching closing brace
    int braceCount = 0;
    bool inString = false;
    size_t i = valueStart;
    
    for (; i < message.length(); i++) {
        char c = message[i];
        
        if (c == '\"' && (i == 0 || message[i-1] != '\\')) {
            inString = !inString;
        }
        
        if (!inString) {
            if (c == '{' || c == '[') braceCount++;
            if (c == '}' || c == ']') {
                braceCount--;
                if (braceCount == 0) {
                    return message.substr(valueStart, i - valueStart + 1);
                }
            }
        }
    }
    
    return "{}";
}

void WebViewHost::navigate(const std::wstring& url) {
    if (webView) {
        std::wcout << L"Navigate to: " << url << std::endl;
        // In production: webView->Navigate(url.c_str());
    }
}

void WebViewHost::postMessage(const std::string& message) {
    if (webView) {
        std::cout << "Posting message to frontend: " << message << std::endl;
        // In production: webView->PostWebMessageAsString(wMessage.c_str());
    }
}

void WebViewHost::registerMessageHandler(const std::string& messageType,
                                         std::function<void(const std::string&)> handler) {
    messageHandlers[messageType] = handler;
    std::cout << "Registered handler for: " << messageType << std::endl;
}

void WebViewHost::executeScript(const std::wstring& script) {
    if (webView) {
        std::wcout << L"Execute script: " << script << std::endl;
        // In production: webView->ExecuteScript(script.c_str(), nullptr);
    }
}
