#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <vector>
#include <functional>

struct CompileOptions {
    std::string sourceFile;
    std::string outputFile;
    std::vector<std::string> includePaths;
    std::vector<std::string> libraries;
    std::vector<std::string> flags;
    std::string standard = "c++17";
    bool optimize = false;
    bool debug = true;
};

class Compiler {
public:
    Compiler();
    
    bool isAvailable();
    std::string getVersion();
    bool compile(const CompileOptions& options, 
                 std::function<void(const std::string&)> logCallback);
    
private:
    std::string compilerPath;
    bool findCompiler();
    std::string buildCommand(const CompileOptions& options);
    bool executeCompile(const std::string& command,
                       std::function<void(const std::string&)> logCallback);
};

#endif // COMPILER_H
