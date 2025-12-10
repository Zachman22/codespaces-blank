#ifndef AUTO_CODER_H
#define AUTO_CODER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace HybridIDE {

enum class Language {
    CPP,
    PYTHON,
    JAVASCRIPT,
    TYPESCRIPT,
    CSHARP,
    RUST,
    LUA,
    SQL,
    BASH,
    POWERSHELL,
    HTML,
    CSS,
    CMAKE,
    DOCKERFILE
};

enum class CodeQuality {
    DRAFT,
    WORKING,
    PRODUCTION,
    TESTED
};

struct CodeModule {
    std::string name;
    std::string path;
    Language language;
    std::string content;
    std::vector<std::string> dependencies;
    std::vector<std::string> exports;
    CodeQuality quality;
    bool hasTests;
};

struct CodeError {
    std::string file;
    int line;
    std::string message;
    std::string severity; // error, warning, info
    std::string fix;
};

struct ProjectArchitecture {
    std::string umlClassDiagram;
    std::string umlSequenceDiagram;
    std::string systemArchitectureDiagram;
    std::string flowchart;
    std::string dependencyGraph;
    std::string componentTree;
    std::string apiEndpointMap;
    std::string uiNavigationMap;
    std::string folderStructure;
};

class AutoCoder {
public:
    AutoCoder();
    ~AutoCoder();

    // A - Auto Coder Engine
    std::string generateCode(const std::string& naturalLanguagePrompt, Language targetLanguage);
    std::vector<CodeModule> generateProject(const std::string& projectDescription);
    bool validateSyntax(const std::string& code, Language language);
    std::string formatCode(const std::string& code, Language language);
    bool ensureProductionReady(CodeModule& module);
    
    // B - Auto Coder Updater
    bool updateModule(const std::string& moduleName, const std::string& changes);
    bool updateAllAffectedModules(const std::string& change);
    bool updateUI(const std::string& componentName);
    bool updateDatabase(const std::string& schemaChange);
    bool updateBuildScripts();
    bool maintainConsistency();
    
    // C - Auto Coder Fixer
    std::vector<CodeError> detectErrors(const std::string& code, Language language);
    bool fixError(CodeError& error);
    bool fixAllErrors(const std::string& filePath);
    bool fixImports(const std::string& filePath);
    bool fixTypes(const std::string& filePath);
    bool fixLogicLoops(const std::string& filePath);
    bool fixMissingIncludes(const std::string& filePath);
    bool recursiveFixUntilZeroErrors(const std::string& projectPath);
    
    // D - Auto Coder Completer
    std::string completeUnfinishedCode(const std::string& partialCode, Language language);
    std::string fillMissingFunctions(const std::string& code, Language language);
    std::vector<std::string> detectMissingFiles(const std::string& projectPath);
    bool generateTests(const std::string& modulePath);
    bool completeProjectRecursively(const std::string& projectPath);
    
    // E - Auto Code Saver
    void enableAutoSave(bool enable);
    void setAutoSaveInterval(int seconds);
    bool saveSnapshot(const std::string& version);
    std::string generateDiff(const std::string& oldVersion, const std::string& newVersion);
    
    // F - Code Undoer / Rollback
    bool undo();
    bool rollbackToSnapshot(const std::string& version);
    bool rollbackToLastStable();
    bool rollbackModule(const std::string& moduleName);
    std::vector<std::string> listSnapshots();
    
    // G - Code Tester & Sandbox Runner
    struct TestResult {
        bool success;
        std::string output;
        std::vector<std::string> errors;
        double executionTime;
    };
    
    TestResult runInSandbox(const std::string& code, Language language);
    bool userApprove(const TestResult& result);
    bool makeAutomaticTweaks(const std::string& feedback);
    
    // H - Auto Programmer (Natural Language → Code)
    struct ProjectSpec {
        std::string description;
        std::vector<std::string> features;
        std::string targetPlatform;
        std::vector<Language> languages;
    };
    
    bool generateFullProject(const ProjectSpec& spec);
    bool generateFolders(const std::string& structure);
    bool generateUI(const std::string& uiSpec);
    bool generateBackend(const std::string& apiSpec);
    bool generateDatabase(const std::string& schemaSpec);
    bool generateBuildSystem(const std::string& projectType);
    
    // I - Architecture Generator (Base-44 System)
    ProjectArchitecture generateArchitecture(const std::string& projectDescription);
    std::string generateUMLClass(const std::vector<CodeModule>& modules);
    std::string generateUMLSequence(const std::string& workflow);
    std::string generateSystemDiagram(const std::string& architecture);
    std::string generateFlowchart(const std::string& algorithm);
    std::string generateDependencyGraph(const std::vector<CodeModule>& modules);
    
    // J - Auto Deployment System
    bool buildExecutable(const std::string& projectPath, const std::string& outputPath);
    bool buildInstaller(const std::string& projectPath);
    bool buildDockerContainer(const std::string& projectPath);
    bool packageRelease(const std::string& version);
    bool generateSignedBinary(const std::string& binaryPath);
    bool createZipArtifact(const std::string& projectPath);
    
    // K - Recursion Engine
    struct BuildStatus {
        int totalFeatures;
        int completedFeatures;
        int totalErrors;
        int fixedErrors;
        bool buildSuccess;
        double completionPercent;
    };
    
    bool runUntilComplete(const std::string& projectPath);
    BuildStatus getBuildStatus();
    bool runRecursivePass();
    bool verifyAllFeaturesComplete();
    bool verifyAllModulesUpdated();
    bool verifyZeroErrors();
    bool verifyBuildSuccess();
    
    // L - Add-on Store / Add-on Maker
    struct Addon {
        std::string name;
        std::string description;
        std::vector<std::string> features;
        std::vector<std::string> dependencies;
        std::string code;
    };
    
    std::vector<Addon> suggestAddons(const std::string& request);
    bool buildAddon(const Addon& addon);
    bool integrateAddon(const Addon& addon, const std::string& projectPath);
    
    // M - Multi-Persona Coder Agents
    enum class AgentType {
        DEBUGGER,
        ARCHITECT,
        UI_DESIGNER,
        DOCKER_SPECIALIST,
        README_WRITER,
        SECURITY_AUDITOR
    };
    
    struct Agent {
        AgentType type;
        std::string name;
        std::function<std::string(const std::string&)> analyze;
    };
    
    std::string runAgent(AgentType type, const std::string& context);
    std::vector<std::string> debugWithAgent(const std::string& code);
    ProjectArchitecture architectWithAgent(const std::string& requirements);
    std::string designUIWithAgent(const std::string& specification);
    std::string generateDockerWithAgent(const std::string& projectType);
    std::string writeREADMEWithAgent(const std::string& projectPath);
    std::vector<std::string> auditSecurityWithAgent(const std::string& code);
    
    // Callbacks
    void setProgressCallback(std::function<void(const std::string&, int)> callback);
    void setCompletionCallback(std::function<void(bool)> callback);

private:
    std::vector<CodeModule> modules_;
    std::vector<Agent> agents_;
    std::map<std::string, std::string> snapshots_;
    BuildStatus buildStatus_;
    
    bool autoSaveEnabled_;
    int autoSaveInterval_;
    
    std::function<void(const std::string&, int)> progressCallback_;
    std::function<void(bool)> completionCallback_;
    
    std::string analyzeCode(const std::string& code, Language language);
    std::string generateBoilerplate(Language language);
    bool compileAndTest(const std::string& code, Language language);
    std::vector<std::string> extractDependencies(const std::string& code, Language language);
};

} // namespace HybridIDE

#endif // AUTO_CODER_H
