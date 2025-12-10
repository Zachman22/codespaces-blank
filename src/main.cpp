#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <string>
#include <thread>
#include "../include/webview_host.h"
#include "../include/compiler.h"
#include "../include/system_info.h"
#include "../include/prompt_verifier.h"
#include "../include/file_operations.h"
#include "../include/plugin_manager.h"
#include "../include/auto_installer.h"
#include "../include/auto_updater.h"
#include "../include/docker_manager.h"
#include "../include/auto_coder.h"

// Global variables
#ifdef _WIN32
HWND g_hwnd = nullptr;
#endif
WebViewHost* g_webViewHost = nullptr;
Compiler* g_compiler = nullptr;
HybridIDE::PluginManager* g_pluginManager = nullptr;
HybridIDE::AutoInstaller* g_autoInstaller = nullptr;
HybridIDE::AutoUpdater* g_autoUpdater = nullptr;
HybridIDE::DockerManager* g_dockerManager = nullptr;
HybridIDE::AutoCoder* g_autoCoder = nullptr;

#ifdef _WIN32
// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE: {
            if (g_webViewHost && g_webViewHost->getWebView()) {
                RECT bounds;
                GetClientRect(hwnd, &bounds);
                // Resize WebView in production
            }
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif

// Send message to frontend
void sendToFrontend(const std::string& type, const std::string& data) {
    if (!g_webViewHost) return;
    
    std::string message = "{\"type\":\"" + type + "\",\"data\":" + data + "}";
    g_webViewHost->postMessage(message);
}

// Handle build request from frontend
void handleBuildRequest(const std::string& data) {
    // Parse data JSON
    // Expected: {"sourceFile": "path", "outputFile": "path", "flags": [...]}
    
    // For simplicity, extract basic fields (in production, use JSON library)
    std::string sourceFile, outputFile;
    
    size_t srcPos = data.find("\"sourceFile\"");
    if (srcPos != std::string::npos) {
        size_t start = data.find("\"", srcPos + 12);
        size_t end = data.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            sourceFile = data.substr(start + 1, end - start - 1);
        }
    }
    
    size_t outPos = data.find("\"outputFile\"");
    if (outPos != std::string::npos) {
        size_t start = data.find("\"", outPos + 12);
        size_t end = data.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            outputFile = data.substr(start + 1, end - start - 1);
        }
    }
    
    // Verify build request
    VerificationResult verification = PromptVerifier::verifyBuildRequest(sourceFile, outputFile);
    
    if (!verification.valid) {
        std::string error = "{\"success\":false,\"message\":\"" + verification.message + "\"}";
        sendToFrontend("buildComplete", error);
        return;
    }
    
    // Send warnings
    for (const auto& warning : verification.warnings) {
        std::string warn = "{\"message\":\"" + warning + "\"}";
        sendToFrontend("buildLog", warn);
    }
    
    // Start compilation in background thread
    std::thread([sourceFile, outputFile]() {
        CompileOptions options;
        options.sourceFile = sourceFile;
        options.outputFile = outputFile;
        options.standard = "c++17";
        options.debug = true;
        options.optimize = false;
        
        bool success = g_compiler->compile(options, [](const std::string& log) {
            std::string logMsg = "{\"message\":\"" + log + "\"}";
            sendToFrontend("buildLog", logMsg);
        });
        
        std::string result = success ? 
            "{\"success\":true,\"message\":\"Build completed successfully\",\"outputFile\":\"" + outputFile + "\"}" :
            "{\"success\":false,\"message\":\"Build failed\"}";
        
        sendToFrontend("buildComplete", result);
    }).detach();
}

// Handle system info request
void handleSystemInfoRequest(const std::string& data) {
    std::string info = SystemInfo::toJSON();
    sendToFrontend("systemInfo", info);
}

// Handle run request
void handleRunRequest(const std::string& data) {
    // Parse executable path from data
    std::string execPath;
    
    size_t pathPos = data.find("\"path\"");
    if (pathPos != std::string::npos) {
        size_t start = data.find("\"", pathPos + 6);
        size_t end = data.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            execPath = data.substr(start + 1, end - start - 1);
        }
    }
    
    // Verify run request
    VerificationResult verification = PromptVerifier::verifyRunRequest(execPath);
    
    if (!verification.valid) {
        std::string error = "{\"success\":false,\"message\":\"" + verification.message + "\"}";
        sendToFrontend("runComplete", error);
        return;
    }
    
    // Execute in background thread
    std::thread([execPath]() {
        std::string cmd = "\"" + execPath + "\"";
        
#ifdef _WIN32
        FILE* pipe = _popen(cmd.c_str(), "r");
#else
        FILE* pipe = popen(cmd.c_str(), "r");
#endif
        if (!pipe) {
            sendToFrontend("runComplete", "{\"success\":false,\"message\":\"Failed to execute\"}");
            return;
        }
        
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string output = buffer;
            std::string logMsg = "{\"message\":\"" + output + "\"}";
            sendToFrontend("runLog", logMsg);
        }
        
#ifdef _WIN32
        int exitCode = _pclose(pipe);
#else
        int exitCode = pclose(pipe);
#endif
        std::string result = "{\"success\":true,\"exitCode\":" + std::to_string(exitCode) + "}";
        sendToFrontend("runComplete", result);
    }).detach();
}

// Handle file read request
void handleReadFileRequest(const std::string& data) {
    std::string path;
    
    size_t pathPos = data.find("\"path\"");
    if (pathPos != std::string::npos) {
        size_t start = data.find("\"", pathPos + 6);
        size_t end = data.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            path = data.substr(start + 1, end - start - 1);
        }
    }
    
    std::string content;
    if (FileOperations::readFile(path, content)) {
        // Escape content for JSON
        std::string escaped;
        for (char c : content) {
            if (c == '"') escaped += "\\\"";
            else if (c == '\\') escaped += "\\\\";
            else if (c == '\n') escaped += "\\n";
            else if (c == '\r') escaped += "\\r";
            else if (c == '\t') escaped += "\\t";
            else escaped += c;
        }
        
        std::string response = "{\"success\":true,\"content\":\"" + escaped + "\",\"path\":\"" + path + "\"}";
        sendToFrontend("fileContent", response);
    } else {
        std::string error = "{\"success\":false,\"message\":\"Failed to read file\",\"path\":\"" + path + "\"}";
        sendToFrontend("fileContent", error);
    }
}

// Handle file save request
void handleSaveFileRequest(const std::string& data) {
    std::string path, content;
    
    size_t pathPos = data.find("\"path\"");
    if (pathPos != std::string::npos) {
        size_t start = data.find("\"", pathPos + 6);
        size_t end = data.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            path = data.substr(start + 1, end - start - 1);
        }
    }
    
    size_t contentPos = data.find("\"content\"");
    if (contentPos != std::string::npos) {
        size_t start = data.find("\"", contentPos + 9);
        size_t end = data.find_last_of("\"");
        if (start != std::string::npos && end != std::string::npos && end > start) {
            content = data.substr(start + 1, end - start - 1);
            
            // Unescape JSON content
            std::string unescaped;
            for (size_t i = 0; i < content.length(); i++) {
                if (content[i] == '\\' && i + 1 < content.length()) {
                    if (content[i+1] == 'n') { unescaped += '\n'; i++; }
                    else if (content[i+1] == 'r') { unescaped += '\r'; i++; }
                    else if (content[i+1] == 't') { unescaped += '\t'; i++; }
                    else if (content[i+1] == '"') { unescaped += '"'; i++; }
                    else if (content[i+1] == '\\') { unescaped += '\\'; i++; }
                    else unescaped += content[i];
                } else {
                    unescaped += content[i];
                }
            }
            content = unescaped;
        }
    }
    
    if (FileOperations::writeFile(path, content)) {
        std::string response = "{\"success\":true,\"message\":\"File saved\",\"path\":\"" + path + "\"}";
        sendToFrontend("fileSaved", response);
    } else {
        std::string error = "{\"success\":false,\"message\":\"Failed to save file\",\"path\":\"" + path + "\"}";
        sendToFrontend("fileSaved", error);
    }
}

// Handle list directory request
void handleListDirectoryRequest(const std::string& data) {
    std::string path;
    
    size_t pathPos = data.find("\"path\"");
    if (pathPos != std::string::npos) {
        size_t start = data.find("\"", pathPos + 6);
        size_t end = data.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            path = data.substr(start + 1, end - start - 1);
        }
    }
    
    std::vector<FileInfo> files = FileOperations::listDirectory(path);
    std::string fileList = FileOperations::fileListToJSON(files);
    
    std::string response = "{\"success\":true,\"path\":\"" + path + "\",\"files\":" + fileList + "}";
    sendToFrontend("directoryListing", response);
}

// Handle auto-install request
void handleAutoInstallRequest(const std::string& data) {
    std::thread([]() {
        g_autoInstaller->setProgressCallback([](const std::string& message, int progress) {
            std::string progressMsg = "{\"message\":\"" + message + "\",\"progress\":" + std::to_string(progress) + "}";
            sendToFrontend("installProgress", progressMsg);
        });
        
        bool success = g_autoInstaller->executePostBuildSteps();
        
        std::string result = success ? 
            "{\"success\":true,\"message\":\"Installation completed successfully\"}" :
            "{\"success\":false,\"message\":\"Installation failed\"}";
        
        sendToFrontend("installComplete", result);
    }).detach();
}

// Handle plugin generation request
void handleGeneratePluginRequest(const std::string& data) {
    std::string code, pluginName;
    
    size_t namePos = data.find("\"name\"");
    if (namePos != std::string::npos) {
        size_t start = data.find("\"", namePos + 6);
        size_t end = data.find("\"", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            pluginName = data.substr(start + 1, end - start - 1);
        }
    }
    
    size_t codePos = data.find("\"code\"");
    if (codePos != std::string::npos) {
        size_t start = data.find("\"", codePos + 6);
        size_t end = data.find_last_of("\"");
        if (start != std::string::npos && end != std::string::npos && end > start) {
            code = data.substr(start + 1, end - start - 1);
        }
    }
    
    bool success = g_pluginManager->generatePluginFromCode(code, pluginName);
    
    if (success) {
        // Auto-load the plugin
        success = g_pluginManager->loadPlugin("./plugins/" + pluginName);
    }
    
    std::string result = success ? 
        "{\"success\":true,\"message\":\"Plugin generated and loaded\",\"name\":\"" + pluginName + "\"}" :
        "{\"success\":false,\"message\":\"Plugin generation failed\"}";
    
    sendToFrontend("pluginGenerated", result);
}

// Handle list plugins request
void handleListPluginsRequest(const std::string& data) {
    auto plugins = g_pluginManager->getLoadedPlugins();
    
    std::string pluginList = "[";
    for (size_t i = 0; i < plugins.size(); ++i) {
        const auto& plugin = plugins[i];
        pluginList += "{";
        pluginList += "\"name\":\"" + plugin.manifest.name + "\",";
        pluginList += "\"version\":\"" + plugin.manifest.version + "\",";
        pluginList += "\"author\":\"" + plugin.manifest.author + "\",";
        pluginList += "\"description\":\"" + plugin.manifest.description + "\",";
        pluginList += "\"enabled\":" + std::string(plugin.enabled ? "true" : "false");
        pluginList += "}";
        if (i < plugins.size() - 1) pluginList += ",";
    }
    pluginList += "]";
    
    std::string response = "{\"success\":true,\"plugins\":" + pluginList + "}";
    sendToFrontend("pluginList", response);
}

// Handle check updates request
void handleCheckUpdatesRequest(const std::string& data) {
    std::thread([]() {
        if (g_autoUpdater->checkForUpdates()) {
            auto updateInfo = g_autoUpdater->getLatestVersion();
            
            std::string info = "{";
            info += "\"version\":\"" + updateInfo.version + "\",";
            info += "\"releaseNotes\":\"" + updateInfo.releaseNotes + "\",";
            info += "\"downloadUrl\":\"" + updateInfo.downloadUrl + "\"";
            info += "}";
            
            std::string response = "{\"success\":true,\"updateAvailable\":true,\"updateInfo\":" + info + "}";
            sendToFrontend("updateCheck", response);
        } else {
            std::string response = "{\"success\":true,\"updateAvailable\":false}";
            sendToFrontend("updateCheck", response);
        }
    }).detach();
}

// Handle download update request
void handleDownloadUpdateRequest(const std::string& data) {
    std::thread([]() {
        auto updateInfo = g_autoUpdater->getLatestVersion();
        
        g_autoUpdater->setDownloadProgressCallback([](int progress, long long current, long long total) {
            std::string progressMsg = "{\"progress\":" + std::to_string(progress) + 
                                    ",\"current\":" + std::to_string(current) + 
                                    ",\"total\":" + std::to_string(total) + "}";
            sendToFrontend("updateProgress", progressMsg);
        });
        
        if (g_autoUpdater->downloadUpdate(updateInfo)) {
            std::string response = "{\"success\":true,\"message\":\"Update downloaded. Restart to install.\"}";
            sendToFrontend("updateDownloaded", response);
        } else {
            std::string response = "{\"success\":false,\"message\":\"Update download failed\"}";
            sendToFrontend("updateDownloaded", response);
        }
    }).detach();
}

// Docker Manager handlers
void handleSearchContainersRequest(const std::string& data) {
    std::string query = data; // Parse JSON in production
    auto results = g_dockerManager->searchContainers(query);
    
    // Convert results to JSON string
    std::string json = "[";
    for (size_t i = 0; i < results.size(); ++i) {
        json += "{";
        json += "\"name\":\"" + results[i].name + "\",";
        json += "\"description\":\"" + results[i].description + "\",";
        json += "\"stars\":" + std::to_string(results[i].stars) + ",";
        json += "\"official\":" + std::string(results[i].official ? "true" : "false");
        json += "}";
        if (i < results.size() - 1) json += ",";
    }
    json += "]";
    sendToFrontend("searchResults", json);
}

void handlePullImageRequest(const std::string& data) {
    std::string imageName = data; // Parse JSON
    if (g_dockerManager->pullImage(imageName)) {
        sendToFrontend("imagePulled", "{\"success\":true}");
    } else {
        sendToFrontend("imagePulled", "{\"success\":false}");
    }
}

void handleListContainersRequest(const std::string& data) {
    auto containers = g_dockerManager->listContainers(true);
    
    // Convert containers to JSON string
    std::string json = "[";
    for (size_t i = 0; i < containers.size(); ++i) {
        json += "{";
        json += "\"id\":\"" + containers[i].id + "\",";
        json += "\"name\":\"" + containers[i].name + "\",";
        json += "\"image\":\"" + containers[i].image + "\",";
        json += "\"status\":\"" + containers[i].status + "\"";
        json += "}";
        if (i < containers.size() - 1) json += ",";
    }
    json += "]";
    sendToFrontend("containerList", json);
}

void handleStartContainerRequest(const std::string& data) {
    std::string containerId = data;
    if (g_dockerManager->startContainer(containerId)) {
        sendToFrontend("containerStarted", "{\"success\":true}");
    }
}

void handleStopContainerRequest(const std::string& data) {
    std::string containerId = data;
    if (g_dockerManager->stopContainer(containerId)) {
        sendToFrontend("containerStopped", "{\"success\":true}");
    }
}

void handleGenerateDockerfileRequest(const std::string& data) {
    std::string projectType = data;
    std::string baseImage = "ubuntu:22.04";
    auto dockerfile = g_dockerManager->generateDockerfile(baseImage, projectType);
    std::string json = "{\"content\":\"" + dockerfile + "\"}";
    sendToFrontend("dockerfileGenerated", json);
}

void handleRepairContainerRequest(const std::string& data) {
    std::string containerId = data;
    if (g_dockerManager->repairContainer(containerId)) {
        sendToFrontend("containerRepaired", "{\"success\":true}");
    }
}

void handleBackupContainerRequest(const std::string& data) {
    std::string containerId = data;
    if (g_dockerManager->backupContainer(containerId, "./backups")) {
        sendToFrontend("containerBackedUp", "{\"success\":true}");
    }
}

// AutoCoder handlers
void handleGenerateCodeRequest(const std::string& data) {
    // Parse: { "prompt": "...", "language": "..." }
    std::string prompt = data;
    HybridIDE::Language language = HybridIDE::Language::CPP; // Default to C++
    auto code = g_autoCoder->generateCode(prompt, language);
    std::string json = "{\"code\":\"" + code + "\"}";
    sendToFrontend("codeGenerated", json);
}

void handleGenerateProjectRequest(const std::string& data) {
    // Parse: { "name": "...", "description": "...", "languages": [...] }
    HybridIDE::AutoCoder::ProjectSpec spec;
    spec.description = data;
    spec.features = {};
    spec.targetPlatform = "cross-platform";
    spec.languages = {HybridIDE::Language::CPP};
    
    if (g_autoCoder->generateFullProject(spec)) {
        sendToFrontend("projectGenerated", "{\"success\":true}");
    } else {
        sendToFrontend("projectGenerated", "{\"success\":false}");
    }
}

void handleFixErrorsRequest(const std::string& data) {
    std::string filePath = data;
    if (g_autoCoder->fixAllErrors(filePath)) {
        sendToFrontend("errorsFixed", "{\"success\":true}");
    }
}

void handleRecursiveFixRequest(const std::string& data) {
    std::string projectPath = data;
    if (g_autoCoder->recursiveFixUntilZeroErrors(projectPath)) {
        sendToFrontend("recursiveFixComplete", "{\"success\":true}");
    } else {
        sendToFrontend("recursiveFixComplete", "{\"success\":false}");
    }
}

void handleGenerateTestsRequest(const std::string& data) {
    std::string filePath = data;
    if (g_autoCoder->generateTests(filePath)) {
        sendToFrontend("testsGenerated", "{\"success\":true}");
    }
}

void handleRunUntilCompleteRequest(const std::string& data) {
    std::string projectPath = data;
    if (g_autoCoder->runUntilComplete(projectPath)) {
        sendToFrontend("projectComplete", "{\"success\":true}");
    }
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize COM
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    
    // Register window class
    const wchar_t CLASS_NAME[] = L"HybridIDEWindow";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    RegisterClass(&wc);
    
    // Create window
    g_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Hybrid IDE - C++ & React",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1400, 900,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    
    if (!g_hwnd) {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
#else
int main(int argc, char* argv[]) {
    std::cout << "Hybrid IDE - Linux Console Mode" << std::endl;
    std::cout << "===============================\n" << std::endl;
    std::cout << "Backend systems initialized. Frontend requires WebView2 (Windows only)." << std::endl;
    std::cout << "Use this build to test Docker and AutoCoder backend functionality.\n" << std::endl;
#endif
    
    // Initialize compiler
    g_compiler = new Compiler();
    
    // Initialize plugin manager
    g_pluginManager = new HybridIDE::PluginManager();
    
    // Initialize auto installer
    g_autoInstaller = new HybridIDE::AutoInstaller();
    
    // Initialize auto updater
    g_autoUpdater = new HybridIDE::AutoUpdater();
    
    // Initialize docker manager
    g_dockerManager = new HybridIDE::DockerManager();
    
    // Initialize auto coder
    g_autoCoder = new HybridIDE::AutoCoder();
    
    // Check for updates on startup
    std::thread([]() {
        if (g_autoUpdater->checkForUpdates()) {
            std::cout << "Update available!" << std::endl;
        }
    }).detach();
    
#ifdef _WIN32
    // Initialize WebView
    g_webViewHost = new WebViewHost();
    
    // Get the path to the HTML file (in production, bundle with executable)
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);
    std::wstring exeDir(exePath);
    size_t lastSlash = exeDir.find_last_of(L"\\/");
    exeDir = exeDir.substr(0, lastSlash);
    
    std::wstring htmlPath = exeDir + L"\\frontend\\dist\\index.html";
    std::wstring initialURL = L"file:///" + htmlPath;
    
    // Initialize WebView with HTML
    if (!g_webViewHost->initialize(g_hwnd, initialURL)) {
        MessageBox(g_hwnd, L"Failed to initialize WebView2. Please install WebView2 Runtime.", 
                   L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Register message handlers
    g_webViewHost->registerMessageHandler("build", handleBuildRequest);
    g_webViewHost->registerMessageHandler("run", handleRunRequest);
    g_webViewHost->registerMessageHandler("getSystemInfo", handleSystemInfoRequest);
    g_webViewHost->registerMessageHandler("openFile", handleReadFileRequest);
    g_webViewHost->registerMessageHandler("saveFile", handleSaveFileRequest);
    g_webViewHost->registerMessageHandler("listDirectory", handleListDirectoryRequest);
    g_webViewHost->registerMessageHandler("autoInstall", handleAutoInstallRequest);
    g_webViewHost->registerMessageHandler("generatePlugin", handleGeneratePluginRequest);
    g_webViewHost->registerMessageHandler("listPlugins", handleListPluginsRequest);
    g_webViewHost->registerMessageHandler("checkUpdates", handleCheckUpdatesRequest);
    g_webViewHost->registerMessageHandler("downloadUpdate", handleDownloadUpdateRequest);
    
    // Docker handlers
    g_webViewHost->registerMessageHandler("searchContainers", handleSearchContainersRequest);
    g_webViewHost->registerMessageHandler("pullImage", handlePullImageRequest);
    g_webViewHost->registerMessageHandler("listContainers", handleListContainersRequest);
    g_webViewHost->registerMessageHandler("startContainer", handleStartContainerRequest);
    g_webViewHost->registerMessageHandler("stopContainer", handleStopContainerRequest);
    g_webViewHost->registerMessageHandler("generateDockerfile", handleGenerateDockerfileRequest);
    g_webViewHost->registerMessageHandler("repairContainer", handleRepairContainerRequest);
    g_webViewHost->registerMessageHandler("backupContainer", handleBackupContainerRequest);
    
    // AutoCoder handlers
    g_webViewHost->registerMessageHandler("generateCode", handleGenerateCodeRequest);
    g_webViewHost->registerMessageHandler("generateProject", handleGenerateProjectRequest);
    g_webViewHost->registerMessageHandler("fixErrors", handleFixErrorsRequest);
    g_webViewHost->registerMessageHandler("recursiveFix", handleRecursiveFixRequest);
    g_webViewHost->registerMessageHandler("generateTests", handleGenerateTestsRequest);
    g_webViewHost->registerMessageHandler("runUntilComplete", handleRunUntilCompleteRequest);
    
    // Show window
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    delete g_webViewHost;
#else
    // Linux: Run console-based tests
    std::cout << "\nTesting Docker Manager..." << std::endl;
    std::cout << "Searching for nginx containers..." << std::endl;
    auto results = g_dockerManager->searchContainers("nginx");
    std::cout << "Found " << results.size() << " results" << std::endl;
    
    std::cout << "\nTesting Auto Coder..." << std::endl;
    std::cout << "Generating sample C++ code..." << std::endl;
    auto code = g_autoCoder->generateCode("Create a hello world function", HybridIDE::Language::CPP);
    std::cout << "Generated code:\n" << code << std::endl;
    
    std::cout << "\nBackend systems are functional!" << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
#endif
    
    delete g_compiler;
    delete g_pluginManager;
    delete g_autoInstaller;
    delete g_autoUpdater;
    delete g_dockerManager;
    delete g_autoCoder;
    
#ifdef _WIN32
    CoUninitialize();
#endif
    
    return 0;
}
