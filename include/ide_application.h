#ifndef IDE_APPLICATION_H
#define IDE_APPLICATION_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include "ai_api_client.h"
#include "compiler.h"
#include "debugger.h"
#include "profiler.h"
#include "kubernetes_manager.h"
#include "cloud_sync.h"
#include "collaboration.h"

namespace HybridIDE {

// Forward declarations
class EditorTab;
class ProjectExplorer;
class Terminal;
class OutputPanel;
class AIAssistant;
class ExtensionBuilder;

// IDE Window Manager
class IDEWindow {
public:
    struct WindowConfig {
        std::string title;
        int width;
        int height;
        bool maximized;
        std::string theme;
        
        WindowConfig() : title("Hybrid IDE"), width(1600), height(900), maximized(false), theme("dark") {}
    };

    IDEWindow(const WindowConfig& config);
    ~IDEWindow();

    // Window management
    void show();
    void hide();
    void close();
    void setTitle(const std::string& title);
    void setTheme(const std::string& theme);
    
    // Layout management
    void addPanel(const std::string& name, void* panel);
    void removePanel(const std::string& name);
    void showPanel(const std::string& name);
    void hidePanel(const std::string& name);

    // Event loop
    void run();
    
private:
    void* window;
    WindowConfig config;
    std::map<std::string, void*> panels;
    bool running;
};

// Code Editor Tab
class EditorTab {
public:
    struct EditorConfig {
        std::string language;
        bool showLineNumbers = true;
        bool enableAutoComplete = true;
        bool enableSyntaxHighlight = true;
        int tabSize = 4;
        bool useTabs = false;
    };

    EditorTab(const std::string& filePath = "");
    ~EditorTab();

    // File operations
    bool open(const std::string& filePath);
    bool save();
    bool saveAs(const std::string& filePath);
    bool close();

    // Content operations
    std::string getContent() const;
    void setContent(const std::string& content);
    void insertText(int line, int col, const std::string& text);
    void deleteText(int startLine, int startCol, int endLine, int endCol);

    // Selection
    std::string getSelectedText() const;
    void selectAll();
    void selectRange(int startLine, int startCol, int endLine, int endCol);

    // Search and replace
    std::vector<int> find(const std::string& text, bool caseSensitive = true);
    int replaceAll(const std::string& find, const std::string& replace);

    // Editor state
    bool isModified() const;
    std::string getFilePath() const;
    std::string getLanguage() const;
    int getCurrentLine() const;
    int getCurrentColumn() const;

    // Code actions
    void format();
    void commentSelection();
    void uncommentSelection();
    void indent();
    void outdent();

private:
    std::string filePath;
    std::string content;
    EditorConfig config;
    bool modified;
    int currentLine;
    int currentColumn;
};

// Project Explorer
class ProjectExplorer {
public:
    struct FileNode {
        std::string name;
        std::string path;
        bool isDirectory;
        std::vector<FileNode> children;
    };

    ProjectExplorer();
    ~ProjectExplorer();

    // Project operations
    bool openProject(const std::string& projectPath);
    bool closeProject();
    std::string getCurrentProject() const;

    // File tree operations
    FileNode getFileTree() const;
    std::vector<std::string> getFiles(const std::string& extension = "") const;
    std::vector<std::string> getDirectories() const;

    // File operations
    bool createFile(const std::string& path);
    bool createDirectory(const std::string& path);
    bool deleteFile(const std::string& path);
    bool renameFile(const std::string& oldPath, const std::string& newPath);

    // Search
    std::vector<std::string> searchFiles(const std::string& pattern) const;

private:
    std::string projectPath;
    FileNode rootNode;
    void buildFileTree();
};

// Integrated Terminal
class Terminal {
public:
    Terminal();
    ~Terminal();

    // Terminal operations
    void execute(const std::string& command);
    void sendInput(const std::string& input);
    void clear();
    void kill();

    // Output handling
    std::string getOutput() const;
    void setOutputCallback(std::function<void(const std::string&)> callback);

    // Terminal state
    bool isRunning() const;
    int getExitCode() const;

private:
    void* process;
    std::string output;
    std::function<void(const std::string&)> outputCallback;
    bool running;
    int exitCode;
};

// AI Assistant Integration
class AIAssistant {
public:
    AIAssistant();
    ~AIAssistant();

    // Chat interface
    std::string sendMessage(const std::string& message);
    std::vector<std::string> getConversationHistory() const;
    void clearHistory();

    // Code generation
    std::string generateCode(const std::string& description, const std::string& language);
    std::string refactorCode(const std::string& code, const std::string& instructions);
    std::string explainCode(const std::string& code);
    std::string fixCode(const std::string& code, const std::string& error);

    // Code review
    struct CodeReview {
        std::vector<std::string> issues;
        std::vector<std::string> suggestions;
        std::vector<std::string> improvements;
        int score;
    };
    CodeReview reviewCode(const std::string& code);

    // Configuration
    void setAPIKey(const std::string& key);
    void setModel(const std::string& model);

private:
    std::unique_ptr<AIAPIClient> client;
    std::vector<std::string> history;
};

// VS Code Extension Builder
class ExtensionBuilder {
public:
    struct ExtensionConfig {
        std::string name;
        std::string displayName;
        std::string description;
        std::string version;
        std::string publisher;
        std::vector<std::string> categories;
        std::map<std::string, std::string> commands;
        std::map<std::string, std::string> configuration;
    };

    struct BuildOptions {
        bool minify;
        bool bundle;
        bool sourceMaps;
        std::string outputDir;
        
        BuildOptions() : minify(true), bundle(true), sourceMaps(false), outputDir("./out") {}
    };

    ExtensionBuilder();
    ~ExtensionBuilder();

    // Extension scaffolding
    bool createExtension(const std::string& path, const ExtensionConfig& config);
    bool generateManifest(const std::string& path, const ExtensionConfig& config);
    bool generateActivationCode(const std::string& path);
    bool generatePackageJSON(const std::string& path, const ExtensionConfig& config);

    // Build operations
    bool build(const std::string& extensionPath, const BuildOptions& options);
    bool compile(const std::string& extensionPath);
    bool package(const std::string& extensionPath);
    bool publish(const std::string& extensionPath, const std::string& token);

    // Testing
    bool test(const std::string& extensionPath);
    bool lint(const std::string& extensionPath);

    // Templates
    std::string getCommandTemplate() const;
    std::string getWebviewTemplate() const;
    std::string getLanguageTemplate() const;
    std::string getTreeViewTemplate() const;

private:
    bool installDependencies(const std::string& path);
    bool runVSCE(const std::string& path, const std::string& command);
};

// Main IDE Application
class IDEApplication {
public:
    IDEApplication();
    ~IDEApplication();

    // Application lifecycle
    bool initialize();
    int run();
    void shutdown();

    // Window management
    IDEWindow* getMainWindow();
    
    // Editor management
    EditorTab* newFile();
    EditorTab* openFile(const std::string& filePath);
    bool closeFile(EditorTab* tab);
    std::vector<EditorTab*> getOpenFiles() const;
    EditorTab* getCurrentEditor() const;

    // Project management
    bool openProject(const std::string& projectPath);
    bool closeProject();
    ProjectExplorer* getProjectExplorer();

    // Terminal
    Terminal* createTerminal();
    std::vector<Terminal*> getTerminals() const;

    // Build and run
    bool build();
    bool buildAndRun();
    bool clean();

    // Debugging
    Debugger* getDebugger();
    bool startDebugging();
    bool stopDebugging();

    // Profiling
    Profiler* getProfiler();
    bool profileApplication();

    // AI features
    AIAssistant* getAIAssistant();

    // Extension builder
    ExtensionBuilder* getExtensionBuilder();
    bool createVSCodeExtension(const std::string& path);
    bool buildVSCodeExtension(const std::string& path);

    // Cloud integration
    CloudSync* getCloudSync();
    bool syncToCloud();

    // Collaboration
    Collaboration* getCollaboration();
    bool startCollabSession();

    // Kubernetes
    KubernetesManager* getKubernetesManager();
    bool deployToKubernetes();

    // Settings
    void loadSettings();
    void saveSettings();
    void setSetting(const std::string& key, const std::string& value);
    std::string getSetting(const std::string& key) const;

private:
    std::unique_ptr<IDEWindow> mainWindow;
    std::vector<std::unique_ptr<EditorTab>> editors;
    std::unique_ptr<ProjectExplorer> projectExplorer;
    std::vector<std::unique_ptr<Terminal>> terminals;
    std::unique_ptr<Debugger> debugger;
    std::unique_ptr<Profiler> profiler;
    std::unique_ptr<AIAssistant> aiAssistant;
    std::unique_ptr<ExtensionBuilder> extensionBuilder;
    std::unique_ptr<CloudSync> cloudSync;
    std::unique_ptr<Collaboration> collaboration;
    std::unique_ptr<KubernetesManager> kubernetesManager;
    std::unique_ptr<Compiler> compiler;
    
    std::map<std::string, std::string> settings;
    std::string configPath;
    EditorTab* currentEditor;
    bool initialized;
};

} // namespace HybridIDE

#endif // IDE_APPLICATION_H
