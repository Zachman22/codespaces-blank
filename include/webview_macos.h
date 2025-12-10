#ifndef WEBVIEW_MACOS_H
#define WEBVIEW_MACOS_H

#ifdef __APPLE__

#include <string>
#include <functional>
#include <map>

// Forward declarations for Objective-C classes
#ifdef __OBJC__
@class NSWindow;
@class WKWebView;
@class NSString;
#else
typedef void NSWindow;
typedef void WKWebView;
typedef void NSString;
#endif

/**
 * macOS WebKit WebView Implementation
 * Uses WKWebView for native macOS GUI
 */
class WebViewMacOS {
public:
    WebViewMacOS();
    ~WebViewMacOS();

    // Window management
    bool create(int width, int height, const std::string& title);
    void show();
    void hide();
    void close();
    void setTitle(const std::string& title);
    void setSize(int width, int height);
    void setFullscreen(bool fullscreen);
    void setMinimumSize(int width, int height);

    // Content loading
    void loadURL(const std::string& url);
    void loadHTML(const std::string& html);
    void loadFile(const std::string& filepath);

    // JavaScript interaction
    void executeJavaScript(const std::string& script);
    void executeJavaScript(const std::string& script, std::function<void(const std::string&)> callback);
    
    // Message passing (C++ <-> JavaScript)
    void addMessageHandler(const std::string& name, std::function<void(const std::string&)> handler);
    void postMessage(const std::string& name, const std::string& data);

    // Navigation
    void goBack();
    void goForward();
    void reload();
    void stopLoading();

    // Properties
    bool canGoBack() const;
    bool canGoForward() const;
    bool isLoading() const;
    std::string getCurrentURL() const;
    std::string getTitle() const;

    // Developer tools
    void openDevTools();
    void closeDevTools();
    void setDeveloperExtrasEnabled(bool enabled);

    // Event handlers
    void setLoadFinishedCallback(std::function<void()> callback);
    void setLoadFailedCallback(std::function<void(const std::string&)> callback);
    void setTitleChangedCallback(std::function<void(const std::string&)> callback);

    // Run event loop
    void run();
    void terminate();

    // macOS-specific features
    void setTitlebarAppearsTransparent(bool transparent);
    void setTrafficLightInset(int x, int y);
    void enableVibrancy(bool enable);

private:
    NSWindow* window_;
    WKWebView* webview_;
    bool running_;
    
    std::function<void()> loadFinishedCallback_;
    std::function<void(const std::string&)> loadFailedCallback_;
    std::function<void(const std::string&)> titleChangedCallback_;
    
    std::map<std::string, std::function<void(const std::string&)>> messageHandlers_;

    void initializeCocoa();
    void setupWebKit();
    void setupMessageHandlers();
    
    // Objective-C bridge methods
    void* createNSWindow(int width, int height, const std::string& title);
    void* createWKWebView();
    void configureWebView(void* webview);
};

#endif // __APPLE__

#endif // WEBVIEW_MACOS_H
