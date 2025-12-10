import React, { useState, useEffect } from 'react';
import { sendMessage, onMessage } from '../services/bridge';

interface Plugin {
  name: string;
  version: string;
  author: string;
  description: string;
  enabled: boolean;
}

export const PluginPanel: React.FC = () => {
  const [plugins, setPlugins] = useState<Plugin[]>([]);
  const [generating, setGenerating] = useState(false);
  const [pluginCode, setPluginCode] = useState('');
  const [pluginName, setPluginName] = useState('');
  const [message, setMessage] = useState('');

  useEffect(() => {
    // Request plugin list on mount
    sendMessage('listPlugins', {});

    // Listen for plugin list
    const unsubList = onMessage('pluginList', (data: any) => {
      if (data.success) {
        setPlugins(data.plugins);
      }
    });

    // Listen for plugin generation complete
    const unsubGen = onMessage('pluginGenerated', (data: any) => {
      setGenerating(false);
      setMessage(data.message);
      if (data.success) {
        setPluginCode('');
        setPluginName('');
        // Refresh plugin list
        sendMessage('listPlugins', {});
      }
    });

    return () => {
      unsubList();
      unsubGen();
    };
  }, []);

  const handleGeneratePlugin = () => {
    if (!pluginName || !pluginCode) {
      setMessage('Please provide plugin name and code');
      return;
    }

    setGenerating(true);
    setMessage('Generating plugin...');
    sendMessage('generatePlugin', {
      name: pluginName,
      code: pluginCode
    });
  };

  return (
    <div style={styles.container}>
      <h2 style={styles.title}>Plugin Manager</h2>
      
      {message && (
        <div style={{
          ...styles.message,
          backgroundColor: message.includes('fail') ? '#ff4444' : '#44ff44'
        }}>
          {message}
        </div>
      )}

      <div style={styles.section}>
        <h3>Generate New Plugin from AI Code</h3>
        <input
          type="text"
          placeholder="Plugin Name"
          value={pluginName}
          onChange={(e) => setPluginName(e.target.value)}
          style={styles.input}
        />
        <textarea
          placeholder="Paste AI-generated C++ plugin code here..."
          value={pluginCode}
          onChange={(e) => setPluginCode(e.target.value)}
          style={styles.textarea}
        />
        <button
          onClick={handleGeneratePlugin}
          disabled={generating}
          style={{
            ...styles.button,
            opacity: generating ? 0.5 : 1
          }}
        >
          {generating ? 'Generating...' : 'Generate & Load Plugin'}
        </button>
      </div>

      <div style={styles.section}>
        <h3>Loaded Plugins ({plugins.length})</h3>
        <div style={styles.pluginList}>
          {plugins.map((plugin, index) => (
            <div key={index} style={styles.pluginCard}>
              <div style={styles.pluginHeader}>
                <strong>{plugin.name}</strong>
                <span style={{
                  ...styles.badge,
                  backgroundColor: plugin.enabled ? '#4CAF50' : '#9E9E9E'
                }}>
                  {plugin.enabled ? 'Enabled' : 'Disabled'}
                </span>
              </div>
              <div style={styles.pluginInfo}>
                <p>Version: {plugin.version}</p>
                <p>Author: {plugin.author}</p>
                <p>{plugin.description}</p>
              </div>
            </div>
          ))}
          {plugins.length === 0 && (
            <p style={styles.emptyText}>No plugins loaded</p>
          )}
        </div>
      </div>
    </div>
  );
};

const styles: { [key: string]: React.CSSProperties } = {
  container: {
    padding: '20px',
    height: '100%',
    overflowY: 'auto',
    backgroundColor: '#1e1e1e',
    color: '#ffffff'
  },
  title: {
    marginTop: 0,
    marginBottom: '20px',
    fontSize: '24px',
    fontWeight: 'bold'
  },
  section: {
    marginBottom: '30px',
    padding: '15px',
    backgroundColor: '#252526',
    borderRadius: '5px'
  },
  message: {
    padding: '10px',
    marginBottom: '15px',
    borderRadius: '5px',
    color: '#000'
  },
  input: {
    width: '100%',
    padding: '10px',
    marginBottom: '10px',
    backgroundColor: '#3c3c3c',
    border: '1px solid #555',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '14px'
  },
  textarea: {
    width: '100%',
    height: '200px',
    padding: '10px',
    marginBottom: '10px',
    backgroundColor: '#3c3c3c',
    border: '1px solid #555',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '13px',
    fontFamily: 'Consolas, monospace',
    resize: 'vertical'
  },
  button: {
    padding: '10px 20px',
    backgroundColor: '#007acc',
    border: 'none',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '14px',
    cursor: 'pointer',
    fontWeight: 'bold'
  },
  pluginList: {
    display: 'flex',
    flexDirection: 'column',
    gap: '10px'
  },
  pluginCard: {
    padding: '15px',
    backgroundColor: '#3c3c3c',
    borderRadius: '5px',
    border: '1px solid #555'
  },
  pluginHeader: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: '10px'
  },
  pluginInfo: {
    fontSize: '13px',
    color: '#cccccc'
  },
  badge: {
    padding: '4px 8px',
    borderRadius: '3px',
    fontSize: '12px',
    fontWeight: 'bold',
    color: '#ffffff'
  },
  emptyText: {
    textAlign: 'center',
    color: '#888',
    padding: '20px'
  }
};

export default PluginPanel;
