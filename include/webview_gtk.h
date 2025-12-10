#ifndef WEBVIEW_GTK_H
#define WEBVIEW_GTK_H

#ifdef __linux__

#include <string>
#include <functional>

// Forward declarations for GTK/WebKit
typedef struct _GtkWidget GtkWidget;
typedef struct _GtkWindow GtkWindow;
typedef struct _WebKitWebView WebKitWebView;

/**
 * Linux GTK WebView Implementation
 * Uses WebKitGTK for native Linux GUI
 */
class WebViewGTK {
public:
    WebViewGTK();
    ~WebViewGTK();

    // Window management
    bool create(int width, int height, const std::string& title);
    void show();
    void hide();
    void close();
    void setTitle(const std::string& title);
    void setSize(int width, int height);
    void setFullscreen(bool fullscreen);

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

    // Event handlers
    void setLoadFinishedCallback(std::function<void()> callback);
    void setLoadFailedCallback(std::function<void(const std::string&)> callback);
    void setTitleChangedCallback(std::function<void(const std::string&)> callback);

    // Run event loop
    void run();
    void terminate();

private:
    GtkWidget* window_;
    WebKitWebView* webview_;
    bool running_;
    
    std::function<void()> loadFinishedCallback_;
    std::function<void(const std::string&)> loadFailedCallback_;
    std::function<void(const std::string&)> titleChangedCallback_;
    
    std::map<std::string, std::function<void(const std::string&)>> messageHandlers_;

    // GTK callbacks
    static void onLoadChanged(WebKitWebView* webview, int loadEvent, void* userData);
    static void onLoadFailed(WebKitWebView* webview, int loadEvent, const char* failingUri, void* error, void* userData);
    static void onTitleChanged(WebKitWebView* webview, void* param, void* userData);
    static bool onScriptMessage(WebKitWebView* webview, void* message, void* userData);
    
    void initializeGTK();
    void setupWebKit();
};

#endif // __linux__

#endif // WEBVIEW_GTK_H
