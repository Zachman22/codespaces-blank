import React, { useState, useEffect } from 'react';
import { sendMessage, onMessage } from '../services/bridge';

interface UpdateInfo {
  version: string;
  releaseNotes: string;
  downloadUrl: string;
}

export const UpdatePanel: React.FC = () => {
  const [checking, setChecking] = useState(false);
  const [downloading, setDownloading] = useState(false);
  const [updateAvailable, setUpdateAvailable] = useState(false);
  const [updateInfo, setUpdateInfo] = useState<UpdateInfo | null>(null);
  const [progress, setProgress] = useState(0);
  const [message, setMessage] = useState('');

  useEffect(() => {
    // Listen for update check results
    const unsubCheck = onMessage('updateCheck', (data: any) => {
      setChecking(false);
      if (data.success) {
        setUpdateAvailable(data.updateAvailable);
        if (data.updateAvailable) {
          setUpdateInfo(data.updateInfo);
          setMessage(`Update ${data.updateInfo.version} is available!`);
        } else {
          setMessage('You are on the latest version');
        }
      }
    });

    // Listen for download progress
    const unsubProgress = onMessage('updateProgress', (data: any) => {
      setProgress(data.progress);
    });

    // Listen for download complete
    const unsubDownload = onMessage('updateDownloaded', (data: any) => {
      setDownloading(false);
      setMessage(data.message);
    });

    return () => {
      unsubCheck();
      unsubProgress();
      unsubDownload();
    };
  }, []);

  const handleCheckUpdates = () => {
    setChecking(true);
    setMessage('Checking for updates...');
    sendMessage('checkUpdates', {});
  };

  const handleDownloadUpdate = () => {
    setDownloading(true);
    setProgress(0);
    setMessage('Downloading update...');
    sendMessage('downloadUpdate', {});
  };

  return (
    <div style={styles.container}>
      <h2 style={styles.title}>Auto Updater</h2>

      {message && (
        <div style={styles.message}>
          {message}
        </div>
      )}

      <div style={styles.section}>
        <h3>Update Status</h3>
        <button
          onClick={handleCheckUpdates}
          disabled={checking}
          style={{
            ...styles.button,
            opacity: checking ? 0.5 : 1
          }}
        >
          {checking ? 'Checking...' : 'Check for Updates'}
        </button>

        {updateAvailable && updateInfo && (
          <div style={styles.updateCard}>
            <h4>Version {updateInfo.version} Available</h4>
            <p style={styles.releaseNotes}>{updateInfo.releaseNotes}</p>
            
            {downloading && (
              <div style={styles.progressContainer}>
                <div style={styles.progressBar}>
                  <div style={{
                    ...styles.progressFill,
                    width: `${progress}%`
                  }}></div>
                </div>
                <span>{progress}%</span>
              </div>
            )}
            
            <button
              onClick={handleDownloadUpdate}
              disabled={downloading}
              style={{
                ...styles.button,
                backgroundColor: '#4CAF50',
                opacity: downloading ? 0.5 : 1
              }}
            >
              {downloading ? 'Downloading...' : 'Download Update'}
            </button>
          </div>
        )}
      </div>

      <div style={styles.section}>
        <h3>Auto-Update Settings</h3>
        <label style={styles.checkbox}>
          <input type="checkbox" defaultChecked />
          <span>Automatically check for updates</span>
        </label>
        <label style={styles.checkbox}>
          <input type="checkbox" />
          <span>Download updates automatically</span>
        </label>
        <label style={styles.checkbox}>
          <input type="checkbox" />
          <span>Include beta versions</span>
        </label>
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
    backgroundColor: '#264f78',
    color: '#ffffff'
  },
  button: {
    padding: '10px 20px',
    backgroundColor: '#007acc',
    border: 'none',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '14px',
    cursor: 'pointer',
    fontWeight: 'bold',
    marginRight: '10px'
  },
  updateCard: {
    marginTop: '20px',
    padding: '15px',
    backgroundColor: '#3c3c3c',
    borderRadius: '5px',
    border: '1px solid #555'
  },
  releaseNotes: {
    fontSize: '13px',
    color: '#cccccc',
    whiteSpace: 'pre-wrap',
    marginBottom: '15px'
  },
  progressContainer: {
    marginBottom: '15px'
  },
  progressBar: {
    width: '100%',
    height: '20px',
    backgroundColor: '#555',
    borderRadius: '10px',
    overflow: 'hidden',
    marginBottom: '5px'
  },
  progressFill: {
    height: '100%',
    backgroundColor: '#4CAF50',
    transition: 'width 0.3s ease'
  },
  checkbox: {
    display: 'flex',
    alignItems: 'center',
    marginBottom: '10px',
    cursor: 'pointer'
  }
};

export default UpdatePanel;
