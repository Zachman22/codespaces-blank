#include "plugin_manager.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace fs = std::filesystem;

namespace HybridIDE {

PluginManager::PluginManager() {
    pluginsDirectory_ = "./plugins";
    fs::create_directories(pluginsDirectory_);
}

PluginManager::~PluginManager() {
    // Unload all plugins
    for (auto& [name, plugin] : plugins_) {
        if (plugin.handle) {
#ifdef _WIN32
            FreeLibrary((HMODULE)plugin.handle);
#else
            dlclose(plugin.handle);
#endif
        }
    }
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    try {
        // Check if plugin directory exists
        if (!fs::exists(pluginPath)) {
            std::cerr << "Plugin path does not exist: " << pluginPath << std::endl;
            return false;
        }

        // Parse manifest
        std::string manifestPath = pluginPath + "/manifest.json";
        if (!fs::exists(manifestPath)) {
            std::cerr << "Plugin manifest not found: " << manifestPath << std::endl;
            return false;
        }

        PluginManifest manifest = parseManifest(manifestPath);
        
        // Validate manifest
        if (!validatePluginManifest(manifest)) {
            std::cerr << "Invalid plugin manifest for: " << manifest.name << std::endl;
            return false;
        }

        // Check if already loaded
        if (plugins_.find(manifest.name) != plugins_.end()) {
            std::cerr << "Plugin already loaded: " << manifest.name << std::endl;
            return false;
        }

        // Create plugin structure
        Plugin plugin;
        plugin.manifest = manifest;
        plugin.path = pluginPath;
        plugin.enabled = false;
        plugin.handle = nullptr;

        // Check dependencies
        if (!checkDependencies(plugin)) {
            std::cerr << "Missing dependencies for plugin: " << manifest.name << std::endl;
            return false;
        }

        // Load shared library if present
        std::string libPath = pluginPath + "/" + manifest.entryPoint;
        if (fs::exists(libPath)) {
#ifdef _WIN32
            plugin.handle = LoadLibraryA(libPath.c_str());
#else
            plugin.handle = dlopen(libPath.c_str(), RTLD_LAZY);
#endif
            if (!plugin.handle) {
                std::cerr << "Failed to load plugin library: " << libPath << std::endl;
                return false;
            }
        }

        // Execute entry point
        if (!executePluginEntry(plugin)) {
            std::cerr << "Failed to execute plugin entry point: " << manifest.name << std::endl;
            return false;
        }

        plugin.enabled = true;
        plugins_[manifest.name] = plugin;
        
        std::cout << "Plugin loaded successfully: " << manifest.name << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error loading plugin: " << e.what() << std::endl;
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }

    Plugin& plugin = it->second;
    
    if (plugin.handle) {
#ifdef _WIN32
        FreeLibrary((HMODULE)plugin.handle);
#else
        dlclose(plugin.handle);
#endif
    }

    plugins_.erase(it);
    std::cout << "Plugin unloaded: " << pluginName << std::endl;
    return true;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    it->second.enabled = true;
    return true;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    it->second.enabled = false;
    return true;
}

std::vector<Plugin> PluginManager::scanPluginsDirectory(const std::string& directory) {
    std::vector<Plugin> found;
    
    if (!fs::exists(directory)) {
        return found;
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_directory()) {
            std::string manifestPath = entry.path().string() + "/manifest.json";
            if (fs::exists(manifestPath)) {
                Plugin plugin;
                plugin.manifest = parseManifest(manifestPath);
                plugin.path = entry.path().string();
                plugin.enabled = false;
                plugin.handle = nullptr;
                found.push_back(plugin);
            }
        }
    }

    return found;
}

std::vector<Plugin> PluginManager::getLoadedPlugins() const {
    std::vector<Plugin> result;
    for (const auto& [name, plugin] : plugins_) {
        result.push_back(plugin);
    }
    return result;
}

Plugin* PluginManager::getPlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it != plugins_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool PluginManager::generatePluginFromCode(const std::string& code, const std::string& pluginName) {
    try {
        // Create plugin directory
        std::string pluginDir = pluginsDirectory_ + "/" + pluginName;
        fs::create_directories(pluginDir);

        // Write source code
        std::string sourceFile = pluginDir + "/plugin.cpp";
        std::ofstream srcOut(sourceFile);
        if (!srcOut) {
            return false;
        }
        srcOut << code;
        srcOut.close();

        // Create default manifest
        PluginManifest manifest;
        manifest.name = pluginName;
        manifest.version = "1.0.0";
        manifest.author = "Auto-generated";
        manifest.description = "Auto-generated plugin";
        manifest.entryPoint = "plugin.so";

        if (!createPluginManifest(pluginName, manifest)) {
            return false;
        }

        // Compile plugin
        if (!compilePlugin(pluginDir)) {
            return false;
        }

        std::cout << "Plugin generated successfully: " << pluginName << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error generating plugin: " << e.what() << std::endl;
        return false;
    }
}

bool PluginManager::createPluginManifest(const std::string& pluginName, const PluginManifest& manifest) {
    std::string pluginDir = pluginsDirectory_ + "/" + pluginName;
    std::string manifestPath = pluginDir + "/manifest.json";

    std::ofstream out(manifestPath);
    if (!out) {
        return false;
    }

    // Write JSON manifest
    out << "{\n";
    out << "  \"name\": \"" << manifest.name << "\",\n";
    out << "  \"version\": \"" << manifest.version << "\",\n";
    out << "  \"author\": \"" << manifest.author << "\",\n";
    out << "  \"description\": \"" << manifest.description << "\",\n";
    out << "  \"entryPoint\": \"" << manifest.entryPoint << "\",\n";
    out << "  \"dependencies\": [";
    for (size_t i = 0; i < manifest.dependencies.size(); ++i) {
        out << "\"" << manifest.dependencies[i] << "\"";
        if (i < manifest.dependencies.size() - 1) out << ", ";
    }
    out << "],\n";
    out << "  \"permissions\": [";
    for (size_t i = 0; i < manifest.permissions.size(); ++i) {
        out << "\"" << manifest.permissions[i] << "\"";
        if (i < manifest.permissions.size() - 1) out << ", ";
    }
    out << "]\n";
    out << "}\n";
    
    out.close();
    return true;
}

bool PluginManager::compilePlugin(const std::string& pluginPath) {
    std::string sourceFile = pluginPath + "/plugin.cpp";
    std::string outputFile = pluginPath + "/plugin.so";

#ifdef _WIN32
    outputFile = pluginPath + "/plugin.dll";
    std::string compileCmd = "g++ -shared -fPIC -o " + outputFile + " " + sourceFile + " -std=c++17";
#else
    std::string compileCmd = "g++ -shared -fPIC -o " + outputFile + " " + sourceFile + " -std=c++17";
#endif

    int result = system(compileCmd.c_str());
    return result == 0;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }

    std::string pluginPath = it->second.path;
    
    if (!unloadPlugin(pluginName)) {
        return false;
    }

    return loadPlugin(pluginPath);
}

void PluginManager::watchPluginChanges(const std::string& pluginPath) {
    // TODO: Implement file watching for hot reload
    std::cout << "Watching plugin for changes: " << pluginPath << std::endl;
}

bool PluginManager::validatePluginManifest(const PluginManifest& manifest) {
    return !manifest.name.empty() && 
           !manifest.version.empty() && 
           !manifest.entryPoint.empty();
}

bool PluginManager::checkDependencies(const Plugin& plugin) {
    for (const auto& dep : plugin.manifest.dependencies) {
        if (plugins_.find(dep) == plugins_.end()) {
            std::cerr << "Missing dependency: " << dep << std::endl;
            return false;
        }
    }
    return true;
}

PluginManifest PluginManager::parseManifest(const std::string& manifestPath) {
    PluginManifest manifest;
    
    std::ifstream file(manifestPath);
    if (!file) {
        return manifest;
    }

    // Simple JSON parsing (in production, use a proper JSON library)
    std::string line;
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        
        if (line.find("\"name\":") != std::string::npos) {
            size_t start = line.find(":\"") + 2;
            size_t end = line.find("\"", start);
            manifest.name = line.substr(start, end - start);
        }
        else if (line.find("\"version\":") != std::string::npos) {
            size_t start = line.find(":\"") + 2;
            size_t end = line.find("\"", start);
            manifest.version = line.substr(start, end - start);
        }
        else if (line.find("\"author\":") != std::string::npos) {
            size_t start = line.find(":\"") + 2;
            size_t end = line.find("\"", start);
            manifest.author = line.substr(start, end - start);
        }
        else if (line.find("\"description\":") != std::string::npos) {
            size_t start = line.find(":\"") + 2;
            size_t end = line.find("\"", start);
            manifest.description = line.substr(start, end - start);
        }
        else if (line.find("\"entryPoint\":") != std::string::npos) {
            size_t start = line.find(":\"") + 2;
            size_t end = line.find("\"", start);
            manifest.entryPoint = line.substr(start, end - start);
        }
    }

    file.close();
    return manifest;
}

bool PluginManager::executePluginEntry(Plugin& plugin) {
    if (!plugin.handle) {
        return true; // No shared library, just metadata
    }

    // Try to find and call init function
#ifdef _WIN32
    typedef bool (*InitFunc)();
    InitFunc initFunc = (InitFunc)GetProcAddress((HMODULE)plugin.handle, "plugin_init");
#else
    typedef bool (*InitFunc)();
    InitFunc initFunc = (InitFunc)dlsym(plugin.handle, "plugin_init");
#endif

    if (initFunc) {
        return initFunc();
    }

    return true; // No init function required
}

} // namespace HybridIDE
