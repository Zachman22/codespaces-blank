#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>

/**
 * Performance Profiler - CPU, Memory, and I/O profiling
 */
class Profiler {
public:
    enum class ProfileType {
        CPU,
        MEMORY,
        IO,
        ALL
    };

    struct FunctionProfile {
        std::string name;
        unsigned long long callCount;
        double totalTime;      // milliseconds
        double averageTime;
        double percentage;
        std::string file;
        int line;
    };

    struct MemoryProfile {
        size_t totalAllocated;
        size_t totalFreed;
        size_t currentUsage;
        size_t peakUsage;
        std::map<std::string, size_t> allocationsByType;
    };

    struct IOProfile {
        unsigned long long readOperations;
        unsigned long long writeOperations;
        size_t bytesRead;
        size_t bytesWritten;
        double readTime;
        double writeTime;
    };

    struct HotSpot {
        std::string function;
        std::string file;
        int line;
        double impact;  // percentage
        std::string recommendation;
    };

    Profiler();
    ~Profiler();

    // Start/Stop profiling
    bool startProfiling(const std::string& executable, ProfileType type = ProfileType::ALL);
    bool stopProfiling();
    bool isRunning() const;

    // Attach to running process
    bool attachToProcess(int pid, ProfileType type = ProfileType::ALL);
    bool detach();

    // Results
    std::vector<FunctionProfile> getCPUProfile() const;
    MemoryProfile getMemoryProfile() const;
    IOProfile getIOProfile() const;
    std::vector<HotSpot> getHotSpots(int topN = 10) const;

    // Analysis
    std::vector<std::string> analyzePerformance() const;
    std::vector<std::string> getOptimizationSuggestions() const;

    // Export
    bool exportToJSON(const std::string& filepath) const;
    bool exportToHTML(const std::string& filepath) const;
    bool exportFlameGraph(const std::string& filepath) const;

    // Configuration
    void setSamplingInterval(int milliseconds);
    void setStackDepth(int depth);
    void enableMemoryProfiling(bool enable);
    void enableIOProfiling(bool enable);

private:
    bool running_;
    ProfileType type_;
    std::string executable_;
    int pid_;
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
    
    std::vector<FunctionProfile> cpuProfile_;
    MemoryProfile memoryProfile_;
    IOProfile ioProfile_;
    
    int samplingInterval_;  // milliseconds
    int stackDepth_;
    bool memoryProfilingEnabled_;
    bool ioProfilingEnabled_;

    // Profiling backends
    bool profileWithPerf();
    bool profileWithValgrind();
    bool profileWithGprof();
    
    void parsePerfOutput(const std::string& output);
    void parseValgrindOutput(const std::string& output);
    void parseGprofOutput(const std::string& output);
    
    HotSpot identifyHotSpot(const FunctionProfile& profile) const;
    std::string generateRecommendation(const FunctionProfile& profile) const;
};

#endif // PROFILER_H
