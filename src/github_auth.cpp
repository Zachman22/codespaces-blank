#include "github_auth.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

GitHubAuth::GitHubAuth() : authenticated_(false), port_(8888) {
    // Set up auth directory
#ifdef _WIN32
    char* appdata = getenv("APPDATA");
    std::string authDir = appdata ? std::string(appdata) + "\\HybridIDE\\auth" : ".\\auth";
#else
    char* home = getenv("HOME");
    std::string authDir = home ? std::string(home) + "/.hybridide/auth" : "./auth";
#endif
    fs::create_directories(authDir);
    tokenPath_ = authDir + "/github_token";
    userDataPath_ = authDir + "/user_data.json";
}

GitHubAuth::~GitHubAuth() {
    // Cleanup
}

bool GitHubAuth::isAuthenticated() const {
    return authenticated_;
}

std::string GitHubAuth::getAccessToken() const {
    return accessToken_;
}

GitHubAuth::UserInfo GitHubAuth::getUserInfo() const {
    return currentUser_;
}

bool GitHubAuth::loadSavedToken() {
    std::ifstream tokenFile(tokenPath_);
    if (!tokenFile.is_open()) {
        return false;
    }
    
    std::getline(tokenFile, accessToken_);
    tokenFile.close();
    
    if (!accessToken_.empty()) {
        // Verify token is still valid
        if (verifyToken()) {
            authenticated_ = true;
            loadUserData();
            return true;
        }
    }
    
    return false;
}

bool GitHubAuth::startDeviceFlow() {
    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          GitHub Authentication - Device Flow                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    // Generate device code request
    std::string clientId = "Iv1.b507a08c87ecfe98"; // GitHub OAuth App Client ID
    std::string scope = "repo,user,read:org";
    
    std::cout << "🔐 Initiating GitHub Device Authentication...\n\n";
    
    // Step 1: Request device code
    std::string deviceCode, userCode, verificationUri;
    int expiresIn, interval;
    
    if (!requestDeviceCode(clientId, scope, deviceCode, userCode, verificationUri, expiresIn, interval)) {
        std::cerr << "❌ Failed to request device code\n";
        return false;
    }
    
    // Step 2: Display verification instructions
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  Please visit this URL to authenticate:\n\n";
    std::cout << "  🌐 " << verificationUri << "\n\n";
    std::cout << "  And enter this code:\n\n";
    std::cout << "  📋 " << userCode << "\n";
    std::cout << "════════════════════════════════════════════════════════════════\n\n";
    
    // Try to open browser automatically
    openBrowser(verificationUri);
    
    std::cout << "⏳ Waiting for authentication";
    std::cout.flush();
    
    // Step 3: Poll for access token
    int attempts = expiresIn / interval;
    for (int i = 0; i < attempts; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(interval));
        std::cout << ".";
        std::cout.flush();
        
        if (pollForToken(clientId, deviceCode)) {
            std::cout << "\n\n✅ Authentication successful!\n\n";
            
            // Fetch user info
            if (fetchUserInfo()) {
                std::cout << "👤 Signed in as: " << currentUser_.login << "\n";
                std::cout << "📧 Email: " << currentUser_.email << "\n";
                std::cout << "🏢 Name: " << currentUser_.name << "\n\n";
                
                // Save token
                saveToken();
                saveUserData();
                
                authenticated_ = true;
                return true;
            }
        }
    }
    
    std::cout << "\n\n❌ Authentication timeout. Please try again.\n";
    return false;
}

bool GitHubAuth::requestDeviceCode(const std::string& clientId, const std::string& scope,
                                   std::string& deviceCode, std::string& userCode,
                                   std::string& verificationUri, int& expiresIn, int& interval) {
    // Simulate API response for demonstration
    // In production, this would make an actual HTTP POST to https://github.com/login/device/code
    
    deviceCode = "3584d83530557fdd1f46af8289938c8ef79f9dc5";
    userCode = "WDJB-MJHT";
    verificationUri = "https://github.com/login/device";
    expiresIn = 900; // 15 minutes
    interval = 5; // Poll every 5 seconds
    
    return true;
}

bool GitHubAuth::pollForToken(const std::string& clientId, const std::string& deviceCode) {
    // Simulate polling for access token
    // In production, this would make HTTP POST to https://github.com/login/oauth/access_token
    
    // For demonstration, simulate successful auth after a few attempts
    static int pollCount = 0;
    pollCount++;
    
    if (pollCount >= 3) {
        // Simulate receiving access token
        accessToken_ = "gho_demo_token_" + std::to_string(std::time(nullptr));
        pollCount = 0;
        return true;
    }
    
    return false;
}

bool GitHubAuth::fetchUserInfo() {
    // Simulate fetching user info from GitHub API
    // In production, this would make HTTP GET to https://api.github.com/user
    
    currentUser_.login = "Zachman22";
    currentUser_.id = "user_id_123456";
    currentUser_.name = "Zachary";
    currentUser_.email = "user@example.com";
    currentUser_.avatarUrl = "https://github.com/Zachman22.png";
    currentUser_.createdAt = "2020-01-01T00:00:00Z";
    
    return true;
}

bool GitHubAuth::verifyToken() {
    // Verify token is still valid by making a test API call
    // In production, this would make HTTP GET to https://api.github.com/user
    return !accessToken_.empty();
}

void GitHubAuth::saveToken() {
    std::ofstream tokenFile(tokenPath_);
    if (tokenFile.is_open()) {
        tokenFile << accessToken_;
        tokenFile.close();
        
        // Set file permissions to owner-only (Unix-like systems)
#ifndef _WIN32
        fs::permissions(tokenPath_, 
                       fs::perms::owner_read | fs::perms::owner_write,
                       fs::perm_options::replace);
#endif
    }
}

void GitHubAuth::saveUserData() {
    std::ofstream userFile(userDataPath_);
    if (userFile.is_open()) {
        // Simple JSON-like format
        userFile << "{\n";
        userFile << "  \"login\": \"" << currentUser_.login << "\",\n";
        userFile << "  \"id\": \"" << currentUser_.id << "\",\n";
        userFile << "  \"name\": \"" << currentUser_.name << "\",\n";
        userFile << "  \"email\": \"" << currentUser_.email << "\",\n";
        userFile << "  \"avatar_url\": \"" << currentUser_.avatarUrl << "\",\n";
        userFile << "  \"created_at\": \"" << currentUser_.createdAt << "\"\n";
        userFile << "}\n";
        userFile.close();
    }
}

void GitHubAuth::loadUserData() {
    std::ifstream userFile(userDataPath_);
    if (!userFile.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(userFile, line)) {
        // Simple parsing for demonstration
        if (line.find("\"login\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                currentUser_.login = line.substr(start, end - start);
            }
        }
        // Parse other fields similarly...
    }
    
    userFile.close();
}

void GitHubAuth::logout() {
    authenticated_ = false;
    accessToken_.clear();
    currentUser_ = UserInfo();
    
    // Remove saved token
    if (fs::exists(tokenPath_)) {
        fs::remove(tokenPath_);
    }
    if (fs::exists(userDataPath_)) {
        fs::remove(userDataPath_);
    }
    
    std::cout << "✅ Logged out successfully\n";
}

void GitHubAuth::openBrowser(const std::string& url) {
    std::cout << "🌐 Attempting to open browser...\n";
    
#ifdef _WIN32
    std::string command = "start " + url;
#elif __APPLE__
    std::string command = "open " + url;
#else
    std::string command = "xdg-open " + url + " 2>/dev/null || sensible-browser " + url + " 2>/dev/null &";
#endif
    
    int result = system(command.c_str());
    if (result != 0) {
        std::cout << "⚠️  Could not open browser automatically. Please visit the URL manually.\n";
    }
}

std::string GitHubAuth::makeGitHubAPIRequest(const std::string& endpoint, const std::string& method) {
    // Placeholder for actual HTTP request
    // In production, use libcurl or similar HTTP library
    std::string url = "https://api.github.com" + endpoint;
    
    // Add authorization header: Authorization: Bearer {token}
    std::string auth = "Bearer " + accessToken_;
    
    return "";
}
