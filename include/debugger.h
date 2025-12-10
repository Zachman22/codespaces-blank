#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

/**
 * Integrated Debugger - GDB/LLDB wrapper with modern interface
 */
class Debugger {
public:
    enum class DebuggerType {
        GDB,
        LLDB,
        AUTO
    };

    enum class BreakpointType {
        LINE,
        FUNCTION,
        CONDITIONAL,
        WATCHPOINT
    };

    struct Breakpoint {
        int id;
        BreakpointType type;
        std::string location;  // file:line or function name
        std::string condition;
        bool enabled;
        int hitCount;
    };

    struct StackFrame {
        int level;
        std::string function;
        std::string file;
        int line;
        std::string address;
        std::map<std::string, std::string> locals;
    };

    struct Variable {
        std::string name;
        std::string type;
        std::string value;
        std::vector<Variable> children;
    };

    Debugger();
    ~Debugger();

    // Initialize debugger
    bool initialize(DebuggerType type = DebuggerType::AUTO);
    bool loadExecutable(const std::string& exePath, const std::vector<std::string>& args = {});

    // Execution control
    bool start();
    bool run();
    bool continue_();
    bool stepOver();
    bool stepInto();
    bool stepOut();
    bool pause();
    bool stop();
    bool restart();

    // Breakpoints
    int setBreakpoint(const std::string& file, int line);
    int setBreakpointAtFunction(const std::string& function);
    int setConditionalBreakpoint(const std::string& location, const std::string& condition);
    bool removeBreakpoint(int id);
    bool enableBreakpoint(int id);
    bool disableBreakpoint(int id);
    std::vector<Breakpoint> listBreakpoints() const;

    // Watchpoints
    int setWatchpoint(const std::string& variable);
    bool removeWatchpoint(int id);

    // Stack and variables
    std::vector<StackFrame> getStackTrace() const;
    std::map<std::string, std::string> getLocalVariables() const;
    Variable getVariable(const std::string& name) const;
    bool setVariable(const std::string& name, const std::string& value);

    // Evaluation
    std::string evaluateExpression(const std::string& expression) const;

    // Output and events
    void setOutputCallback(std::function<void(const std::string&)> callback);
    void setBreakpointHitCallback(std::function<void(const Breakpoint&)> callback);
    void setExceptionCallback(std::function<void(const std::string&)> callback);

    // Status
    bool isRunning() const;
    bool isPaused() const;
    std::string getCurrentFile() const;
    int getCurrentLine() const;

private:
    DebuggerType type_;
    std::string debuggerPath_;
    std::string executablePath_;
    std::vector<std::string> executableArgs_;
    
    bool running_;
    bool paused_;
    std::string currentFile_;
    int currentLine_;
    
    std::map<int, Breakpoint> breakpoints_;
    int nextBreakpointId_;
    
    std::function<void(const std::string&)> outputCallback_;
    std::function<void(const Breakpoint&)> breakpointHitCallback_;
    std::function<void(const std::string&)> exceptionCallback_;

    // Communication with debugger
    std::string sendCommand(const std::string& command);
    void parseOutput(const std::string& output);
    
    // GDB-specific
    std::string sendGDBCommand(const std::string& command);
    void parseGDBOutput(const std::string& output);
    
    // LLDB-specific
    std::string sendLLDBCommand(const std::string& command);
    void parseLLDBOutput(const std::string& output);
};

#endif // DEBUGGER_H
