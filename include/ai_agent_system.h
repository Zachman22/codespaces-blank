#ifndef AI_AGENT_SYSTEM_H
#define AI_AGENT_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace HybridIDE {

// Container Genome Analyzer
struct ContainerGenome {
    std::string id;
    std::map<std::string, std::string> dna; // Configuration DNA
    std::vector<std::string> traits; // Behavioral traits
    std::vector<std::string> mutations; // Configuration changes
    double healthScore;
    std::string ancestry; // Base image lineage
};

// Container Mutation Engine
struct Mutation {
    std::string type; // config, env, volume, port
    std::string target;
    std::string oldValue;
    std::string newValue;
    std::string reason;
    double successProbability;
};

// AI Domain Model
struct BehaviorModel {
    std::string containerId;
    std::map<std::string, double> normalMetrics; // CPU, RAM, etc.
    std::vector<std::string> commonErrors;
    std::vector<std::string> recoveryPatterns;
    double stabilityScore;
};

// Failure Prediction
struct FailurePrediction {
    std::string containerId;
    std::string predictedFailureType;
    double probability;
    int timeToFailure; // seconds
    std::vector<std::string> indicators;
    std::vector<std::string> preventionSteps;
};

class AIAgentSystem {
public:
    AIAgentSystem();
    ~AIAgentSystem();

    // Container Genome Analyzer
    ContainerGenome analyzeGenome(const std::string& containerId);
    std::vector<std::string> identifyTraits(const std::string& containerId);
    std::string traceAncestry(const std::string& imageName);
    double calculateHealthScore(const ContainerGenome& genome);
    std::vector<Mutation> suggestImprovements(const ContainerGenome& genome);
    
    // Container Mutation Engine
    Mutation generateMutation(const std::string& containerId, const std::string& goal);
    bool applyMutation(const std::string& containerId, const Mutation& mutation);
    bool validateMutation(const Mutation& mutation);
    std::vector<Mutation> getMutationHistory(const std::string& containerId);
    bool rollbackMutation(const std::string& containerId, const Mutation& mutation);
    
    // AI Domain Model for Behaviors
    BehaviorModel learnBehavior(const std::string& containerId, int observationPeriod = 3600);
    bool updateBehaviorModel(const std::string& containerId);
    std::vector<std::string> detectAnomalies(const std::string& containerId);
    std::vector<std::string> predictBehavior(const std::string& containerId, int timeframe = 3600);
    double compareToNormalBehavior(const std::string& containerId);
    
    // Failure Prediction
    FailurePrediction predictFailure(const std::string& containerId);
    std::vector<FailurePrediction> scanAllContainersForFailures();
    bool canPreventFailure(const FailurePrediction& prediction);
    bool preventFailure(const std::string& containerId, const FailurePrediction& prediction);
    std::vector<std::string> getFailureHistory(const std::string& containerId);
    
    // Self-Healing Engine
    struct HealingAction {
        std::string type; // restart, rebuild, repair, migrate
        std::string target;
        std::vector<std::string> steps;
        double successRate;
    };
    
    bool enableSelfHealing(const std::string& containerId);
    bool disableSelfHealing(const std::string& containerId);
    HealingAction diagnoseAndHeal(const std::string& containerId);
    bool executeHealingAction(const HealingAction& action);
    std::vector<HealingAction> getHealingHistory(const std::string& containerId);
    
    // Multi-Agent Coordination
    enum class AgentRole {
        MONITOR,
        ANALYZER,
        PREDICTOR,
        HEALER,
        OPTIMIZER,
        SECURITY_GUARD
    };
    
    struct Agent {
        AgentRole role;
        std::string id;
        bool active;
        int tasksCompleted;
        double successRate;
    };
    
    Agent createAgent(AgentRole role);
    bool activateAgent(const std::string& agentId);
    bool deactivateAgent(const std::string& agentId);
    std::vector<Agent> getAllAgents();
    std::string routeTask(const std::string& task);
    
    // Learning & Adaptation
    struct KnowledgeBase {
        std::map<std::string, std::vector<std::string>> solutions; // problem -> solutions
        std::map<std::string, double> successRates; // solution -> success rate
        std::vector<std::string> bestPractices;
    };
    
    bool learnFromSuccess(const std::string& problem, const std::string& solution);
    bool learnFromFailure(const std::string& problem, const std::string& failedSolution);
    std::vector<std::string> queryKnowledge(const std::string& problem);
    double getSolutionConfidence(const std::string& solution);
    bool shareKnowledge(const std::string& agentId);
    
    // Optimization Engine
    struct OptimizationResult {
        std::string containerId;
        std::vector<std::string> recommendations;
        double expectedImprovement;
        std::map<std::string, std::string> newSettings;
    };
    
    OptimizationResult optimizeContainer(const std::string& containerId);
    bool applyOptimizations(const OptimizationResult& result);
    double measurePerformanceGain(const std::string& containerId);
    std::vector<std::string> suggestResourceAllocation();
    
    // Autonomous Decision Making
    struct Decision {
        std::string context;
        std::vector<std::string> options;
        std::string chosen;
        double confidence;
        std::string reasoning;
    };
    
    Decision makeDecision(const std::string& context, const std::vector<std::string>& options);
    bool executeDecision(const Decision& decision);
    std::vector<Decision> getDecisionHistory();
    double evaluateDecision(const Decision& decision);
    
    // Callbacks
    void setAlertCallback(std::function<void(const std::string&, const std::string&)> callback);
    void setHealingCallback(std::function<void(const std::string&, const HealingAction&)> callback);

private:
    std::map<std::string, ContainerGenome> genomes_;
    std::map<std::string, BehaviorModel> behaviorModels_;
    std::map<std::string, std::vector<Mutation>> mutationHistory_;
    std::vector<Agent> agents_;
    KnowledgeBase knowledge_;
    
    std::function<void(const std::string&, const std::string&)> alertCallback_;
    std::function<void(const std::string&, const HealingAction&)> healingCallback_;
    
    double calculateSimilarity(const BehaviorModel& model1, const BehaviorModel& model2);
    std::vector<std::string> extractPatterns(const std::vector<std::string>& data);
    bool isAnomalous(double value, double mean, double stddev);
};

} // namespace HybridIDE

#endif // AI_AGENT_SYSTEM_H
