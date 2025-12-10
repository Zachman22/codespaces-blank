#ifndef GITHUB_AUTH_H
#define GITHUB_AUTH_H

#include <string>
#include <functional>

/**
 * GitHub Authentication - OAuth Device Flow
 * Allows users to authenticate via GitHub when cloning the repository
 */
class GitHubAuth {
public:
    struct UserInfo {
        std::string login;
        std::string id;
        std::string name;
        std::string email;
        std::string avatarUrl;
        std::string createdAt;
    };

    GitHubAuth();
    ~GitHubAuth();

    // Authentication
    bool startDeviceFlow();
    bool startNumericPinFlow(); // NEW: Numeric PIN authentication
    bool isAuthenticated() const;
    std::string getAccessToken() const;
    UserInfo getUserInfo() const;
    
    // Token management
    bool loadSavedToken();
    void logout();
    
    // Callbacks
    void setAuthSuccessCallback(std::function<void(const UserInfo&)> callback);
    void setAuthFailureCallback(std::function<void(const std::string&)> callback);

private:
    bool authenticated_;
    std::string accessToken_;
    UserInfo currentUser_;
    std::string tokenPath_;
    std::string userDataPath_;
    int port_;
    
    std::function<void(const UserInfo&)> authSuccessCallback_;
    std::function<void(const std::string&)> authFailureCallback_;
    
    // OAuth Device Flow steps
    bool requestDeviceCode(const std::string& clientId, const std::string& scope,
                           std::string& deviceCode, std::string& userCode,
                           std::string& verificationUri, int& expiresIn, int& interval);
    bool pollForToken(const std::string& clientId, const std::string& deviceCode);
    bool fetchUserInfo();
    bool verifyToken();
    
    // Storage
    void saveToken();
    void saveUserData();
    void loadUserData();
    
    // Helpers
    void openBrowser(const std::string& url);
    std::string makeGitHubAPIRequest(const std::string& endpoint, const std::string& method = "GET");
};

#endif // GITHUB_AUTH_H
