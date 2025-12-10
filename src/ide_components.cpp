#include "ide_application.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace HybridIDE;

// ============================================================================
// IDEWindow Implementation
// ============================================================================

IDEWindow::IDEWindow(const WindowConfig& config) : config(config), window(nullptr), running(false) {
    std::cout << "Creating IDE Window: " << config.title << std::endl;
}

IDEWindow::~IDEWindow() {
    close();
}

void IDEWindow::show() {
    std::cout << "Showing IDE Window" << std::endl;
    running = true;
}

void IDEWindow::hide() {
    std::cout << "Hiding IDE Window" << std::endl;
}

void IDEWindow::close() {
    running = false;
    std::cout << "Closing IDE Window" << std::endl;
}

void IDEWindow::setTitle(const std::string& title) {
    config.title = title;
}

void IDEWindow::setTheme(const std::string& theme) {
    config.theme = theme;
}

void IDEWindow::addPanel(const std::string& name, void* panel) {
    panels[name] = panel;
}

void IDEWindow::removePanel(const std::string& name) {
    panels.erase(name);
}

void IDEWindow::showPanel(const std::string& name) {
    std::cout << "Showing panel: " << name << std::endl;
}

void IDEWindow::hidePanel(const std::string& name) {
    std::cout << "Hiding panel: " << name << std::endl;
}

void IDEWindow::run() {
    std::cout << "\nHybrid IDE is running. Press Ctrl+C to exit.\n" << std::endl;
    std::cout << "Interactive mode - type commands:\n";
    std::cout << "  help     - Show available commands\n";
    std::cout << "  new      - Create new file\n";
    std::cout << "  open     - Open file\n";
    std::cout << "  build    - Build project\n";
    std::cout << "  ai       - Talk to AI assistant\n";
    std::cout << "  ext      - Create VS Code extension\n";
    std::cout << "  exit     - Exit IDE\n\n";
    
    running = true;
    // Simple command loop for demonstration
    // In a real implementation, this would integrate with the GUI framework
}

// ============================================================================
// EditorTab Implementation
// ============================================================================

EditorTab::EditorTab(const std::string& filePath) 
    : filePath(filePath), modified(false), currentLine(0), currentColumn(0) {
}

EditorTab::~EditorTab() {
    if (modified) {
        std::cout << "Warning: File " << filePath << " has unsaved changes" << std::endl;
    }
}

bool EditorTab::open(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    this->filePath = filePath;
    modified = false;
    
    return true;
}

bool EditorTab::save() {
    if (filePath.empty()) {
        return false;
    }

    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    file << content;
    modified = false;
    return true;
}

bool EditorTab::saveAs(const std::string& filePath) {
    this->filePath = filePath;
    return save();
}

bool EditorTab::close() {
    if (modified) {
        return save();
    }
    return true;
}

std::string EditorTab::getContent() const {
    return content;
}

void EditorTab::setContent(const std::string& content) {
    this->content = content;
    modified = true;
}

void EditorTab::insertText(int line, int col, const std::string& text) {
    // Simple implementation
    content += text;
    modified = true;
}

void EditorTab::deleteText(int startLine, int startCol, int endLine, int endCol) {
    modified = true;
}

std::string EditorTab::getSelectedText() const {
    return "";
}

void EditorTab::selectAll() {
}

void EditorTab::selectRange(int startLine, int startCol, int endLine, int endCol) {
}

std::vector<int> EditorTab::find(const std::string& text, bool caseSensitive) {
    std::vector<int> positions;
    return positions;
}

int EditorTab::replaceAll(const std::string& find, const std::string& replace) {
    return 0;
}

bool EditorTab::isModified() const {
    return modified;
}

std::string EditorTab::getFilePath() const {
    return filePath;
}

std::string EditorTab::getLanguage() const {
    if (filePath.empty()) return "plaintext";
    
    size_t pos = filePath.rfind('.');
    if (pos == std::string::npos) return "plaintext";
    
    std::string ext = filePath.substr(pos + 1);
    if (ext == "cpp" || ext == "cc" || ext == "cxx") return "cpp";
    if (ext == "h" || ext == "hpp") return "cpp";
    if (ext == "js") return "javascript";
    if (ext == "ts") return "typescript";
    if (ext == "py") return "python";
    if (ext == "java") return "java";
    
    return "plaintext";
}

int EditorTab::getCurrentLine() const {
    return currentLine;
}

int EditorTab::getCurrentColumn() const {
    return currentColumn;
}

void EditorTab::format() {
    std::cout << "Formatting code..." << std::endl;
}

void EditorTab::commentSelection() {
}

void EditorTab::uncommentSelection() {
}

void EditorTab::indent() {
}

void EditorTab::outdent() {
}

// ============================================================================
// ProjectExplorer Implementation
// ============================================================================

ProjectExplorer::ProjectExplorer() {
}

ProjectExplorer::~ProjectExplorer() {
}

bool ProjectExplorer::openProject(const std::string& projectPath) {
    this->projectPath = projectPath;
    buildFileTree();
    std::cout << "Opened project: " << projectPath << std::endl;
    return true;
}

bool ProjectExplorer::closeProject() {
    projectPath.clear();
    return true;
}

std::string ProjectExplorer::getCurrentProject() const {
    return projectPath;
}

ProjectExplorer::FileNode ProjectExplorer::getFileTree() const {
    return rootNode;
}

std::vector<std::string> ProjectExplorer::getFiles(const std::string& extension) const {
    std::vector<std::string> files;
    return files;
}

std::vector<std::string> ProjectExplorer::getDirectories() const {
    std::vector<std::string> dirs;
    return dirs;
}

bool ProjectExplorer::createFile(const std::string& path) {
    std::ofstream file(path);
    return file.is_open();
}

bool ProjectExplorer::createDirectory(const std::string& path) {
    return true;
}

bool ProjectExplorer::deleteFile(const std::string& path) {
    return std::remove(path.c_str()) == 0;
}

bool ProjectExplorer::renameFile(const std::string& oldPath, const std::string& newPath) {
    return std::rename(oldPath.c_str(), newPath.c_str()) == 0;
}

std::vector<std::string> ProjectExplorer::searchFiles(const std::string& pattern) const {
    std::vector<std::string> results;
    return results;
}

void ProjectExplorer::buildFileTree() {
    rootNode.name = projectPath;
    rootNode.path = projectPath;
    rootNode.isDirectory = true;
}

// ============================================================================
// Terminal Implementation
// ============================================================================

Terminal::Terminal() : process(nullptr), running(false), exitCode(0) {
}

Terminal::~Terminal() {
    if (running) {
        kill();
    }
}

void Terminal::execute(const std::string& command) {
    std::cout << "Executing: " << command << std::endl;
    running = true;
    exitCode = system(command.c_str());
    running = false;
}

void Terminal::sendInput(const std::string& input) {
    std::cout << input << std::endl;
}

void Terminal::clear() {
    output.clear();
}

void Terminal::kill() {
    running = false;
}

std::string Terminal::getOutput() const {
    return output;
}

void Terminal::setOutputCallback(std::function<void(const std::string&)> callback) {
    outputCallback = callback;
}

bool Terminal::isRunning() const {
    return running;
}

int Terminal::getExitCode() const {
    return exitCode;
}

// ============================================================================
// AIAssistant Implementation
// ============================================================================

AIAssistant::AIAssistant() {
    client = std::make_unique<AIAPIClient>();
}

AIAssistant::~AIAssistant() {
}

std::string AIAssistant::sendMessage(const std::string& message) {
    history.push_back(message);
    
    // Placeholder response - in production, this would call the actual API
    return "AI response: I understand your message. In production, this would use the configured AI API.";
}

std::vector<std::string> AIAssistant::getConversationHistory() const {
    return history;
}

void AIAssistant::clearHistory() {
    history.clear();
}

std::string AIAssistant::generateCode(const std::string& description, const std::string& language) {
    return "// Generated code for: " + description + "\n// Language: " + language;
}

std::string AIAssistant::refactorCode(const std::string& code, const std::string& instructions) {
    return code + "\n// Refactored based on: " + instructions;
}

std::string AIAssistant::explainCode(const std::string& code) {
    return "Code explanation: This code performs various operations.";
}

std::string AIAssistant::fixCode(const std::string& code, const std::string& error) {
    return code + "\n// Fixed error: " + error;
}

AIAssistant::CodeReview AIAssistant::reviewCode(const std::string& code) {
    CodeReview review;
    review.score = 85;
    review.issues.push_back("Consider adding error handling");
    review.suggestions.push_back("Use const references where possible");
    review.improvements.push_back("Add unit tests");
    return review;
}

void AIAssistant::setAPIKey(const std::string& key) {
    client->initialize();
}

void AIAssistant::setModel(const std::string& model) {
}
