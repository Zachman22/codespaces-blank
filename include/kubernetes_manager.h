#ifndef KUBERNETES_MANAGER_H
#define KUBERNETES_MANAGER_H

#include <string>
#include <vector>
#include <map>

/**
 * Kubernetes Manager - Deploy and manage containers in K8s clusters
 */
class KubernetesManager {
public:
    struct Pod {
        std::string name;
        std::string namespace_;
        std::string status;
        std::string node;
        std::vector<std::string> containers;
        std::map<std::string, std::string> labels;
    };

    struct Deployment {
        std::string name;
        std::string namespace_;
        int replicas;
        int readyReplicas;
        std::string image;
        std::map<std::string, std::string> labels;
    };

    struct Service {
        std::string name;
        std::string namespace_;
        std::string type;  // ClusterIP, NodePort, LoadBalancer
        std::string clusterIP;
        std::vector<int> ports;
    };

    KubernetesManager();
    ~KubernetesManager();

    // Initialization
    bool initialize(const std::string& kubeconfigPath = "");
    bool connectToCluster(const std::string& context = "");

    // Cluster operations
    std::string getCurrentContext() const;
    std::vector<std::string> listContexts() const;
    bool switchContext(const std::string& context);
    
    // Namespace operations
    std::vector<std::string> listNamespaces() const;
    bool createNamespace(const std::string& name);
    bool deleteNamespace(const std::string& name);

    // Pod operations
    std::vector<Pod> listPods(const std::string& namespace_ = "default") const;
    Pod getPod(const std::string& name, const std::string& namespace_ = "default") const;
    bool deletePod(const std::string& name, const std::string& namespace_ = "default");
    std::string getPodLogs(const std::string& name, const std::string& namespace_ = "default") const;

    // Deployment operations
    std::vector<Deployment> listDeployments(const std::string& namespace_ = "default") const;
    bool createDeployment(const std::string& yamlPath);
    bool updateDeployment(const std::string& name, const std::string& image, const std::string& namespace_ = "default");
    bool scaleDeployment(const std::string& name, int replicas, const std::string& namespace_ = "default");
    bool deleteDeployment(const std::string& name, const std::string& namespace_ = "default");

    // Service operations
    std::vector<Service> listServices(const std::string& namespace_ = "default") const;
    bool createService(const std::string& yamlPath);
    bool deleteService(const std::string& name, const std::string& namespace_ = "default");

    // Apply YAML
    bool applyYAML(const std::string& yamlPath);
    bool applyYAMLContent(const std::string& yamlContent);

    // Exec into pod
    std::string execInPod(
        const std::string& podName,
        const std::string& command,
        const std::string& namespace_ = "default"
    ) const;

    // Port forwarding
    bool startPortForward(
        const std::string& podName,
        int localPort,
        int remotePort,
        const std::string& namespace_ = "default"
    );
    bool stopPortForward(int localPort);

private:
    std::string kubeconfigPath_;
    std::string currentContext_;
    std::map<int, std::string> portForwards_;  // localPort -> podName

    std::string executeKubectl(const std::string& args) const;
    std::vector<std::string> parseKubectlOutput(const std::string& output) const;
};

#endif // KUBERNETES_MANAGER_H
