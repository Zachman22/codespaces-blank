#ifndef WEBVIEW_HOST_H
#define WEBVIEW_HOST_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>
#include <functional>
#include <map>

class WebViewHost {
public:
    WebViewHost();
    ~WebViewHost();
    
#ifdef _WIN32
    bool initialize(HWND hwnd, const std::wstring& initialURL);
#else
    bool initialize(void* hwnd, const std::wstring& initialURL);
#endif
    void navigate(const std::wstring& url);
    void postMessage(const std::string& message);
    void registerMessageHandler(const std::string& messageType,
                                std::function<void(const std::string&)> handler);
    void executeScript(const std::wstring& script);
    
    void* getWebView() { return webView; }
    void* getController() { return webViewController; }
    
private:
    void* webViewEnvironment;
    void* webViewController;
    void* webView;
    
    std::map<std::string, std::function<void(const std::string&)>> messageHandlers;
    
    void setupMessageHandler();
    std::string parseMessageType(const std::string& message);
    std::string parseMessageData(const std::string& message);
};

#endif // WEBVIEW_HOST_H
