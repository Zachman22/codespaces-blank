#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace HybridIDE {

struct PluginManifest {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::vector<std::string> dependencies;
    std::vector<std::string> permissions;
    std::string entryPoint;
};

struct Plugin {
    PluginManifest manifest;
    std::string path;
    bool enabled;
    void* handle;
};

class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    // Plugin lifecycle
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    
    // Plugin discovery
    std::vector<Plugin> scanPluginsDirectory(const std::string& directory);
    std::vector<Plugin> getLoadedPlugins() const;
    Plugin* getPlugin(const std::string& pluginName);
    
    // Auto-generation from AI code
    bool generatePluginFromCode(const std::string& code, const std::string& pluginName);
    bool createPluginManifest(const std::string& pluginName, const PluginManifest& manifest);
    bool compilePlugin(const std::string& pluginPath);
    
    // Hot reload
    bool reloadPlugin(const std::string& pluginName);
    void watchPluginChanges(const std::string& pluginPath);

private:
    std::map<std::string, Plugin> plugins_;
    std::string pluginsDirectory_;
    
    bool validatePluginManifest(const PluginManifest& manifest);
    bool checkDependencies(const Plugin& plugin);
    PluginManifest parseManifest(const std::string& manifestPath);
    bool executePluginEntry(Plugin& plugin);
};

} // namespace HybridIDE

#endif // PLUGIN_MANAGER_H
