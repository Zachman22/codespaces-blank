#include <iostream>
#include <string>
#include "include/auto_coder_chatbot.h"

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "  AutoCoder Chatbot - Interactive Mode" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
    
    // Initialize chatbot
    HybridIDE::AutoCoderChatbot chatbot;
    
    if (!chatbot.initialize()) {
        std::cerr << "Failed to initialize chatbot!" << std::endl;
        return 1;
    }
    
    std::cout << "Output Directory: " << chatbot.getOutputDirectory() << std::endl;
    std::cout << std::endl;
    
    // Display welcome message
    std::string welcome = chatbot.chat("hello");
    std::cout << "🤖 Assistant: " << std::endl << welcome << std::endl << std::endl;
    
    std::cout << "Type 'exit', 'quit', or 'bye' to end the conversation." << std::endl;
    std::cout << "Type 'export json|html|md' to export conversation." << std::endl;
    std::cout << "Type 'clear' to start a new conversation." << std::endl;
    std::cout << "==========================================" << std::endl << std::endl;
    
    // Interactive loop
    std::string userInput;
    while (true) {
        std::cout << "👤 You: ";
        std::getline(std::cin, userInput);
        
        if (userInput.empty()) continue;
        
        // Check for exit commands
        std::string lower = userInput;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "exit" || lower == "quit" || lower == "bye") {
            std::cout << std::endl << "🤖 Assistant: Goodbye! Your conversation has been saved." << std::endl;
            chatbot.saveConversation();
            break;
        }
        
        if (lower == "clear") {
            chatbot.clearConversation();
            std::cout << "🤖 Assistant: Conversation cleared. Starting fresh!" << std::endl << std::endl;
            continue;
        }
        
        if (lower.find("export") == 0) {
            std::string format = "md";
            if (lower.find("json") != std::string::npos) format = "json";
            else if (lower.find("html") != std::string::npos) format = "html";
            
            std::string exportPath = chatbot.exportConversation(format);
            std::cout << "🤖 Assistant: Conversation exported to: " << exportPath << std::endl << std::endl;
            continue;
        }
        
        // Get chatbot response
        std::string response = chatbot.chat(userInput);
        std::cout << std::endl << "🤖 Assistant: " << std::endl << response << std::endl << std::endl;
    }
    
    return 0;
}
