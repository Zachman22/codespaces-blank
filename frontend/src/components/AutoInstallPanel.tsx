import React, { useState, useEffect } from 'react';
import { sendMessage, onMessage } from '../services/bridge';

export const AutoInstallPanel: React.FC = () => {
  const [installing, setInstalling] = useState(false);
  const [progress, setProgress] = useState(0);
  const [message, setMessage] = useState('');
  const [logs, setLogs] = useState<string[]>([]);

  useEffect(() => {
    // Listen for installation progress
    const unsubProgress = onMessage('installProgress', (data: any) => {
      setProgress(data.progress);
      setMessage(data.message);
      setLogs(prev => [...prev, data.message]);
    });

    // Listen for installation complete
    const unsubComplete = onMessage('installComplete', (data: any) => {
      setInstalling(false);
      setMessage(data.message);
      setLogs(prev => [...prev, data.message]);
    });

    return () => {
      unsubProgress();
      unsubComplete();
    };
  }, []);

  const handleAutoInstall = () => {
    setInstalling(true);
    setProgress(0);
    setLogs([]);
    setMessage('Starting auto-installation...');
    sendMessage('autoInstall', {});
  };

  return (
    <div style={styles.container}>
      <h2 style={styles.title}>Auto Installer</h2>

      <div style={styles.section}>
        <h3>Post-Build Auto Configuration</h3>
        <p style={styles.description}>
          Automatically install all dependencies, configure environment, and set up the application after building.
        </p>

        <div style={styles.features}>
          <div style={styles.feature}>✓ Install system dependencies</div>
          <div style={styles.feature}>✓ Setup Node.js runtime</div>
          <div style={styles.feature}>✓ Install npm packages</div>
          <div style={styles.feature}>✓ Configure environment paths</div>
          <div style={styles.feature}>✓ Create necessary directories</div>
          <div style={styles.feature}>✓ Generate config files</div>
        </div>

        <button
          onClick={handleAutoInstall}
          disabled={installing}
          style={{
            ...styles.button,
            opacity: installing ? 0.5 : 1
          }}
        >
          {installing ? 'Installing...' : 'Run Auto Install'}
        </button>
      </div>

      {installing && (
        <div style={styles.section}>
          <h3>Installation Progress</h3>
          <div style={styles.progressContainer}>
            <div style={styles.progressBar}>
              <div style={{
                ...styles.progressFill,
                width: `${progress}%`
              }}></div>
            </div>
            <span style={styles.progressText}>{progress}%</span>
          </div>
          <p style={styles.currentTask}>{message}</p>
        </div>
      )}

      {logs.length > 0 && (
        <div style={styles.section}>
          <h3>Installation Log</h3>
          <div style={styles.logContainer}>
            {logs.map((log, index) => (
              <div key={index} style={styles.logEntry}>
                [{new Date().toLocaleTimeString()}] {log}
              </div>
            ))}
          </div>
        </div>
      )}
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
  description: {
    fontSize: '14px',
    color: '#cccccc',
    marginBottom: '15px'
  },
  features: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
    gap: '10px',
    marginBottom: '20px'
  },
  feature: {
    padding: '10px',
    backgroundColor: '#3c3c3c',
    borderRadius: '3px',
    fontSize: '13px',
    color: '#4CAF50'
  },
  button: {
    padding: '12px 24px',
    backgroundColor: '#007acc',
    border: 'none',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '16px',
    cursor: 'pointer',
    fontWeight: 'bold'
  },
  progressContainer: {
    marginBottom: '15px'
  },
  progressBar: {
    width: '100%',
    height: '25px',
    backgroundColor: '#555',
    borderRadius: '12px',
    overflow: 'hidden',
    marginBottom: '8px'
  },
  progressFill: {
    height: '100%',
    backgroundColor: '#4CAF50',
    transition: 'width 0.3s ease',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center'
  },
  progressText: {
    fontSize: '14px',
    fontWeight: 'bold',
    color: '#4CAF50'
  },
  currentTask: {
    fontSize: '13px',
    color: '#cccccc',
    fontStyle: 'italic'
  },
  logContainer: {
    maxHeight: '300px',
    overflowY: 'auto',
    backgroundColor: '#1e1e1e',
    padding: '10px',
    borderRadius: '3px',
    border: '1px solid #555'
  },
  logEntry: {
    fontSize: '12px',
    fontFamily: 'Consolas, monospace',
    color: '#cccccc',
    marginBottom: '5px',
    padding: '2px 0'
  }
};

export default AutoInstallPanel;
