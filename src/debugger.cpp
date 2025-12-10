#include "debugger.h"
#include <iostream>

Debugger::Debugger() : running(false), executable("") {}

Debugger::~Debugger() {
    if (running) {
        stop();
    }
}

bool Debugger::initialize() {
    std::cout << "Debugger initialized" << std::endl;
    return true;
}

bool Debugger::loadExecutable(const std::string& path) {
    executable = path;
    std::cout << "Loaded executable: " << path << std::endl;
    return true;
}

bool Debugger::start() {
    running = true;
    std::cout << "Debugger started" << std::endl;
    return true;
}

bool Debugger::run() {
    std::cout << "Running program" << std::endl;
    return true;
}

bool Debugger::continue_() {
    std::cout << "Continuing execution" << std::endl;
    return true;
}

bool Debugger::stepOver() {
    std::cout << "Step over" << std::endl;
    return true;
}

bool Debugger::stepInto() {
    std::cout << "Step into" << std::endl;
    return true;
}

bool Debugger::stepOut() {
    std::cout << "Step out" << std::endl;
    return true;
}

bool Debugger::stop() {
    running = false;
    std::cout << "Debugger stopped" << std::endl;
    return true;
}

bool Debugger::setBreakpoint(const std::string& file, int line) {
    Breakpoint bp;
    bp.file = file;
    bp.line = line;
    bp.enabled = true;
    breakpoints.push_back(bp);
    std::cout << "Breakpoint set at " << file << ":" << line << std::endl;
    return true;
}

bool Debugger::removeBreakpoint(int id) {
    std::cout << "Breakpoint removed" << std::endl;
    return true;
}

std::vector<Debugger::StackFrame> Debugger::getStackTrace() const {
    return std::vector<StackFrame>();
}

std::vector<Debugger::Variable> Debugger::getLocalVariables() const {
    return std::vector<Variable>();
}

std::string Debugger::evaluateExpression(const std::string& expr) {
    return "Evaluation result";
}

bool Debugger::isRunning() const {
    return running;
}

void Debugger::setOutputCallback(std::function<void(const std::string&)> callback) {
    outputCallback = callback;
}
