#ifndef COLLABORATION_H
#define COLLABORATION_H

#include <string>
#include <vector>
#include <map>
#include <functional>

/**
 * Team Collaboration - Real-time collaborative editing and chat
 */
class Collaboration {
public:
    struct User {
        std::string id;
        std::string name;
        std::string email;
        std::string avatar;
        bool online;
        std::string currentFile;
        int cursorLine;
        int cursorColumn;
    };

    struct Message {
        std::string id;
        std::string userId;
        std::string userName;
        std::string content;
        std::string timestamp;
        std::string type;  // "text", "code", "file"
    };

    struct EditOperation {
        std::string userId;
        std::string file;
        std::string action;  // "insert", "delete", "replace"
        int startLine;
        int startColumn;
        int endLine;
        int endColumn;
        std::string content;
        std::string timestamp;
    };

    struct Room {
        std::string id;
        std::string name;
        std::string projectId;
        std::vector<User> members;
        bool isPublic;
    };

    Collaboration();
    ~Collaboration();

    // Connection
    bool connect(const std::string& serverUrl, const std::string& token);
    bool disconnect();
    bool isConnected() const;

    // Room management
    Room createRoom(const std::string& name, const std::string& projectId, bool isPublic = false);
    bool joinRoom(const std::string& roomId);
    bool leaveRoom();
    std::vector<Room> listRooms() const;
    Room getCurrentRoom() const;

    // User management
    User getCurrentUser() const;
    std::vector<User> getActiveUsers() const;
    bool inviteUser(const std::string& email);
    bool removeUser(const std::string& userId);

    // Real-time editing
    bool sendEdit(const EditOperation& edit);
    void setEditReceivedCallback(std::function<void(const EditOperation&)> callback);
    
    // Conflict resolution
    EditOperation resolveConflict(const EditOperation& local, const EditOperation& remote);

    // Chat
    bool sendMessage(const std::string& content);
    bool sendCodeSnippet(const std::string& code, const std::string& language);
    bool sendFile(const std::string& filepath);
    std::vector<Message> getMessages(int limit = 50) const;
    void setMessageReceivedCallback(std::function<void(const Message&)> callback);

    // Presence
    bool updatePresence(const std::string& file, int line, int column);
    void setPresenceUpdateCallback(std::function<void(const User&)> callback);

    // Permissions
    enum class Permission {
        READ,
        WRITE,
        ADMIN
    };
    bool setUserPermission(const std::string& userId, Permission permission);
    Permission getUserPermission(const std::string& userId) const;

    // File locking
    bool lockFile(const std::string& filepath);
    bool unlockFile(const std::string& filepath);
    bool isFileLocked(const std::string& filepath) const;
    std::string getFileLockOwner(const std::string& filepath) const;

private:
    std::string serverUrl_;
    std::string token_;
    bool connected_;
    
    User currentUser_;
    Room currentRoom_;
    std::vector<User> activeUsers_;
    std::vector<Message> messages_;
    std::map<std::string, std::string> fileLocks_;  // filepath -> userId
    
    std::function<void(const EditOperation&)> editReceivedCallback_;
    std::function<void(const Message&)> messageReceivedCallback_;
    std::function<void(const User&)> presenceUpdateCallback_;

    // WebSocket communication
    bool sendWebSocketMessage(const std::string& message);
    void handleWebSocketMessage(const std::string& message);
    
    // Operational Transform for concurrent editing
    EditOperation transformEdit(const EditOperation& edit1, const EditOperation& edit2);
};

#endif // COLLABORATION_H
