import React, { useState, useEffect } from 'react';
import FileExplorer from './components/FileExplorer';
import Editor from './components/Editor';
import Toolbar from './components/Toolbar';
import BuildPanel from './components/BuildPanel';
import AISidebar from './components/AISidebar';
import SystemInfo from './components/SystemInfo';
import SplashScreen from './components/SplashScreen';
import PluginPanel from './components/PluginPanel';
import UpdatePanel from './components/UpdatePanel';
import AutoInstallPanel from './components/AutoInstallPanel';
import DockerPanel from './components/DockerPanel';
import AutoCoderPanel from './components/AutoCoderPanel';
import bridge from './services/bridge';
import './App.css';

interface Tab {
  id: string;
  path: string;
  content: string;
  language: string;
  modified: boolean;
}

function App() {
  const [showSplash, setShowSplash] = useState(true);
  const [tabs, setTabs] = useState<Tab[]>([]);
  const [activeTab, setActiveTab] = useState<string | null>(null);
  const [buildLogs, setBuildLogs] = useState<string[]>([]);
  const [showBuildPanel, setShowBuildPanel] = useState(false);
  const [showAISidebar, setShowAISidebar] = useState(false);
  const [showPluginPanel, setShowPluginPanel] = useState(false);
  const [showUpdatePanel, setShowUpdatePanel] = useState(false);
  const [showAutoInstallPanel, setShowAutoInstallPanel] = useState(false);
  const [showDockerPanel, setShowDockerPanel] = useState(false);
  const [showAutoCoderPanel, setShowAutoCoderPanel] = useState(false);
  const [isBuilding, setIsBuilding] = useState(false);
  const [isRunning, setIsRunning] = useState(false);

  useEffect(() => {
    // Register bridge message handlers
    bridge.on('buildLog', (data: { message: string }) => {
      setBuildLogs((prev: string[]) => [...prev, data.message]);
      setShowBuildPanel(true);
    });

    bridge.on('buildComplete', (data: { success: boolean; message: string }) => {
      setIsBuilding(false);
      setBuildLogs((prev: string[]) => [...prev, `\n${data.message}\n`]);
    });

    bridge.on('runLog', (data: { message: string }) => {
      setBuildLogs((prev: string[]) => [...prev, data.message]);
    });

    bridge.on('runComplete', (data: { success: boolean; exitCode?: number }) => {
      setIsRunning(false);
      setBuildLogs((prev: string[]) => [...prev, `\nProcess exited with code ${data.exitCode || 0}\n`]);
    });

    bridge.on('fileSaved', (data: { success: boolean; message: string }) => {
      if (data.success) {
        setBuildLogs((prev: string[]) => [...prev, `[INFO] ${data.message}\n`]);
      }
    });

    // Keyboard shortcuts
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.ctrlKey && e.key === 's') {
        e.preventDefault();
        handleSave();
      } else if (e.key === 'F5') {
        e.preventDefault();
        handleBuild();
      } else if (e.key === 'F6') {
        e.preventDefault();
        handleRun();
      }
    };

    window.addEventListener('keydown', handleKeyDown);

    return () => {
      window.removeEventListener('keydown', handleKeyDown);
    };
  }, [activeTab, tabs]);

  const handleFileSelect = (path: string, content: string) => {
    const existingTab = tabs.find((tab: Tab) => tab.path === path);
    
    if (existingTab) {
      setActiveTab(existingTab.id);
    } else {
      const newTab: Tab = {
        id: Math.random().toString(36).substr(2, 9),
        path,
        content,
        language: getLanguageFromPath(path),
        modified: false
      };
      setTabs([...tabs, newTab]);
      setActiveTab(newTab.id);
    }
  };

  const getLanguageFromPath = (path: string): string => {
    const ext = path.split('.').pop()?.toLowerCase();
    const langMap: { [key: string]: string } = {
      'cpp': 'cpp',
      'cc': 'cpp',
      'cxx': 'cpp',
      'c': 'c',
      'h': 'cpp',
      'hpp': 'cpp',
      'js': 'javascript',
      'ts': 'typescript',
      'py': 'python',
      'java': 'java',
      'json': 'json',
      'md': 'markdown',
      'txt': 'plaintext'
    };
    return langMap[ext || ''] || 'plaintext';
  };

  const handleEditorChange = (value: string | undefined) => {
    if (!activeTab || value === undefined) return;
    
    setTabs(tabs.map((tab: Tab) => 
      tab.id === activeTab 
        ? { ...tab, content: value, modified: true }
        : tab
    ));
  };

  const handleCloseTab = (tabId: string) => {
    const newTabs = tabs.filter((tab: Tab) => tab.id !== tabId);
    setTabs(newTabs);
    
    if (activeTab === tabId) {
      setActiveTab(newTabs.length > 0 ? newTabs[newTabs.length - 1].id : null);
    }
  };

  const handleSave = () => {
    const currentTab = tabs.find((tab: Tab) => tab.id === activeTab);
    if (!currentTab) return;

    // Save file through bridge
    bridge.saveFile(currentTab.path, currentTab.content);
    
    // Mark as not modified
    setTabs(tabs.map((tab: Tab) => 
      tab.id === activeTab 
        ? { ...tab, modified: false }
        : tab
    ));
  };

  const handleBuild = () => {
    const currentTab = tabs.find((tab: Tab) => tab.id === activeTab);
    if (!currentTab) {
      alert('Please open a source file first');
      return;
    }

    // Save file first if modified
    if (currentTab.modified) {
      bridge.saveFile(currentTab.path, currentTab.content);
      setTabs(tabs.map((tab: Tab) => 
        tab.id === activeTab 
          ? { ...tab, modified: false }
          : tab
      ));
    }

    const sourceFile = currentTab.path;
    const outputFile = sourceFile.replace(/\.(cpp|c|cc|cxx)$/, '.exe');

    setIsBuilding(true);
    setBuildLogs([]);
    setShowBuildPanel(true);
    
    bridge.build(sourceFile, outputFile);
  };

  const handleRun = () => {
    const currentTab = tabs.find((tab: Tab) => tab.id === activeTab);
    if (!currentTab) {
      alert('Please open a source file first');
      return;
    }

    const execPath = currentTab.path.replace(/\.(cpp|c|cc|cxx)$/, '.exe');
    
    setIsRunning(true);
    setBuildLogs([]);
    setShowBuildPanel(true);
    
    bridge.run(execPath);
  };

  const handleDebug = () => {
    alert('Debug functionality will be implemented with GDB integration');
  };

  const currentTab = tabs.find((tab: Tab) => tab.id === activeTab);

  if (showSplash) {
    return <SplashScreen onComplete={() => setShowSplash(false)} />;
  }

  return (
    <div className="app">
      <Toolbar
        onBuild={handleBuild}
        onRun={handleRun}
        onDebug={handleDebug}
        isBuilding={isBuilding}
        isRunning={isRunning}
      />
      
      <div className="main-container">
        <FileExplorer onFileSelect={handleFileSelect} />
        
        <div className="editor-area">
          <div className="tabs">
            {tabs.map((tab: Tab) => (
              <div
                key={tab.id}
                className={`tab ${tab.id === activeTab ? 'active' : ''}`}
                onClick={() => setActiveTab(tab.id)}
              >
                <span className="tab-name">
                  {tab.path.split('/').pop()}
                  {tab.modified && ' •'}
                </span>
                <button
                  className="tab-close"
                  onClick={(e: React.MouseEvent) => {
                    e.stopPropagation();
                    handleCloseTab(tab.id);
                  }}
                >
                  ×
                </button>
              </div>
            ))}
          </div>
          
          <div className="editor-wrapper">
            {currentTab ? (
              <Editor
                value={currentTab.content}
                onChange={handleEditorChange}
                language={currentTab.language}
                path={currentTab.path}
              />
            ) : (
              <div className="no-file-open">
                <h2>No File Open</h2>
                <p>Select a file from the explorer to start editing</p>
              </div>
            )}
          </div>
          
          <BuildPanel
            logs={buildLogs}
            isVisible={showBuildPanel}
            onClose={() => setShowBuildPanel(false)}
            onClear={() => setBuildLogs([])}
          />
        </div>
        
        <AISidebar
          isVisible={showAISidebar}
          onClose={() => setShowAISidebar(false)}
        />
      </div>
      
      <div className="status-bar">
        <SystemInfo />
        <button
          className="status-button"
          onClick={() => setShowAISidebar(!showAISidebar)}
        >
          AI Assistant
        </button>
        <button
          className="status-button"
          onClick={() => setShowPluginPanel(!showPluginPanel)}
        >
          Plugins
        </button>
        <button
          className="status-button"
          onClick={() => setShowAutoInstallPanel(!showAutoInstallPanel)}
        >
          Auto Install
        </button>
        <button
          className="status-button"
          onClick={() => setShowUpdatePanel(!showUpdatePanel)}
        >
          Updates
        </button>
        <button
          className="status-button"
          onClick={() => setShowDockerPanel(!showDockerPanel)}
        >
          Docker
        </button>
        <button
          className="status-button"
          onClick={() => setShowAutoCoderPanel(!showAutoCoderPanel)}
        >
          Auto Coder
        </button>
      </div>
      
      {showPluginPanel && (
        <div className="modal-overlay" onClick={() => setShowPluginPanel(false)}>
          <div className="modal-content" onClick={(e) => e.stopPropagation()}>
            <PluginPanel />
            <button className="modal-close" onClick={() => setShowPluginPanel(false)}>×</button>
          </div>
        </div>
      )}
      
      {showAutoInstallPanel && (
        <div className="modal-overlay" onClick={() => setShowAutoInstallPanel(false)}>
          <div className="modal-content" onClick={(e) => e.stopPropagation()}>
            <AutoInstallPanel />
            <button className="modal-close" onClick={() => setShowAutoInstallPanel(false)}>×</button>
          </div>
        </div>
      )}
      
      {showUpdatePanel && (
        <div className="modal-overlay" onClick={() => setShowUpdatePanel(false)}>
          <div className="modal-content" onClick={(e) => e.stopPropagation()}>
            <UpdatePanel />
            <button className="modal-close" onClick={() => setShowUpdatePanel(false)}>×</button>
          </div>
        </div>
      )}
      
      {showDockerPanel && (
        <div className="modal-overlay" onClick={() => setShowDockerPanel(false)}>
          <div className="modal-content" onClick={(e) => e.stopPropagation()}>
            <DockerPanel />
            <button className="modal-close" onClick={() => setShowDockerPanel(false)}>×</button>
          </div>
        </div>
      )}
      
      {showAutoCoderPanel && (
        <div className="modal-overlay" onClick={() => setShowAutoCoderPanel(false)}>
          <div className="modal-content" onClick={(e) => e.stopPropagation()}>
            <AutoCoderPanel />
            <button className="modal-close" onClick={() => setShowAutoCoderPanel(false)}>×</button>
          </div>
        </div>
      )}
    </div>
  );
}

export default App;
