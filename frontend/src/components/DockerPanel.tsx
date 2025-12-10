import React, { useState, useEffect } from 'react';
import { sendMessage, onMessage } from '../services/bridge';

interface ContainerInfo {
  id: string;
  name: string;
  image: string;
  status: string;
  healthy: boolean;
}

export const DockerPanel: React.FC = () => {
  const [containers, setContainers] = useState<ContainerInfo[]>([]);
  const [searchQuery, setSearchQuery] = useState('');
  const [searchResults, setSearchResults] = useState<any[]>([]);
  const [logs, setLogs] = useState<string[]>([]);
  const [selectedContainer, setSelectedContainer] = useState<string | null>(null);

  useEffect(() => {
    // Request container list
    sendMessage('listContainers', {});

    const unsubList = onMessage('containerList', (data: any) => {
      if (data.success) {
        setContainers(data.containers);
      }
    });

    const unsubSearch = onMessage('containerSearchResults', (data: any) => {
      if (data.success) {
        setSearchResults(data.results);
      }
    });

    const unsubLogs = onMessage('containerLogs', (data: any) => {
      if (data.success) {
        setLogs(data.logs);
      }
    });

    return () => {
      unsubList();
      unsubSearch();
      unsubLogs();
    };
  }, []);

  const handleSearch = () => {
    sendMessage('searchContainers', { query: searchQuery });
  };

  const handleStartContainer = (id: string) => {
    sendMessage('startContainer', { id });
  };

  const handleStopContainer = (id: string) => {
    sendMessage('stopContainer', { id });
  };

  const handleRestartContainer = (id: string) => {
    sendMessage('restartContainer', { id });
  };

  const handleRemoveContainer = (id: string) => {
    if (confirm('Are you sure you want to remove this container?')) {
      sendMessage('removeContainer', { id, force: true });
    }
  };

  const handlePullImage = (imageName: string) => {
    sendMessage('pullImage', { imageName, tag: 'latest' });
  };

  const handleGenerateDockerfile = (appType: string) => {
    sendMessage('generateDockerfile', { baseImage: 'alpine:latest', appType });
  };

  const handleGenerateDockerCompose = () => {
    sendMessage('generateDockerCompose', { services: ['web', 'db', 'redis'] });
  };

  const handleViewLogs = (id: string) => {
    setSelectedContainer(id);
    sendMessage('getContainerLogs', { id, lines: 100 });
  };

  const handleCheckHealth = () => {
    sendMessage('checkDockerHealth', {});
  };

  const handleCleanImages = () => {
    if (confirm('Remove all unused Docker images?')) {
      sendMessage('cleanDockerImages', {});
    }
  };

  return (
    <div style={styles.container}>
      <h2 style={styles.title}>🐳 Docker Manager</h2>

      {/* Search Section */}
      <div style={styles.section}>
        <h3>Search Docker Hub</h3>
        <div style={styles.searchBar}>
          <input
            type="text"
            placeholder="Search for containers..."
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
            onKeyPress={(e) => e.key === 'Enter' && handleSearch()}
            style={styles.input}
          />
          <button onClick={handleSearch} style={styles.button}>
            Search
          </button>
        </div>
        
        {searchResults.length > 0 && (
          <div style={styles.searchResults}>
            {searchResults.map((result, index) => (
              <div key={index} style={styles.resultCard}>
                <strong>{result.name}</strong>
                {result.official && <span style={styles.badge}>Official</span>}
                <button 
                  onClick={() => handlePullImage(result.name)}
                  style={styles.smallButton}
                >
                  Pull
                </button>
              </div>
            ))}
          </div>
        )}
      </div>

      {/* Container List */}
      <div style={styles.section}>
        <h3>Running Containers ({containers.length})</h3>
        <div style={styles.containerList}>
          {containers.map((container) => (
            <div key={container.id} style={styles.containerCard}>
              <div style={styles.containerHeader}>
                <strong>{container.name}</strong>
                <span style={{
                  ...styles.statusBadge,
                  backgroundColor: container.status.includes('Up') ? '#4CAF50' : '#f44336'
                }}>
                  {container.status}
                </span>
              </div>
              <div style={styles.containerInfo}>
                <p>Image: {container.image}</p>
                <p>ID: {container.id.substring(0, 12)}</p>
              </div>
              <div style={styles.containerActions}>
                <button onClick={() => handleStartContainer(container.id)} style={styles.actionButton}>
                  ▶ Start
                </button>
                <button onClick={() => handleStopContainer(container.id)} style={styles.actionButton}>
                  ⏸ Stop
                </button>
                <button onClick={() => handleRestartContainer(container.id)} style={styles.actionButton}>
                  🔄 Restart
                </button>
                <button onClick={() => handleViewLogs(container.id)} style={styles.actionButton}>
                  📄 Logs
                </button>
                <button onClick={() => handleRemoveContainer(container.id)} style={{...styles.actionButton, backgroundColor: '#f44336'}}>
                  🗑 Remove
                </button>
              </div>
            </div>
          ))}
          {containers.length === 0 && (
            <p style={styles.emptyText}>No containers running</p>
          )}
        </div>
      </div>

      {/* Auto-Builder */}
      <div style={styles.section}>
        <h3>Auto-Builder</h3>
        <div style={styles.builderGrid}>
          <button onClick={() => handleGenerateDockerfile('node')} style={styles.button}>
            Generate Node.js Dockerfile
          </button>
          <button onClick={() => handleGenerateDockerfile('python')} style={styles.button}>
            Generate Python Dockerfile
          </button>
          <button onClick={() => handleGenerateDockerfile('cpp')} style={styles.button}>
            Generate C++ Dockerfile
          </button>
          <button onClick={handleGenerateDockerCompose} style={styles.button}>
            Generate docker-compose.yml
          </button>
        </div>
      </div>

      {/* Docker Doctor */}
      <div style={styles.section}>
        <h3>Docker Environment Doctor</h3>
        <div style={styles.doctorGrid}>
          <button onClick={handleCheckHealth} style={styles.button}>
            🏥 Check Health
          </button>
          <button onClick={handleCleanImages} style={styles.button}>
            🧹 Clean Images
          </button>
        </div>
      </div>

      {/* Logs Viewer */}
      {selectedContainer && (
        <div style={styles.section}>
          <h3>Container Logs</h3>
          <div style={styles.logsContainer}>
            {logs.map((log, index) => (
              <div key={index} style={styles.logLine}>
                {log}
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
    fontSize: '28px',
    fontWeight: 'bold'
  },
  section: {
    marginBottom: '30px',
    padding: '15px',
    backgroundColor: '#252526',
    borderRadius: '5px'
  },
  searchBar: {
    display: 'flex',
    gap: '10px',
    marginBottom: '15px'
  },
  input: {
    flex: 1,
    padding: '10px',
    backgroundColor: '#3c3c3c',
    border: '1px solid #555',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '14px'
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
  searchResults: {
    display: 'flex',
    flexDirection: 'column',
    gap: '10px'
  },
  resultCard: {
    padding: '10px',
    backgroundColor: '#3c3c3c',
    borderRadius: '5px',
    display: 'flex',
    alignItems: 'center',
    gap: '10px'
  },
  badge: {
    padding: '4px 8px',
    borderRadius: '3px',
    fontSize: '12px',
    fontWeight: 'bold',
    backgroundColor: '#4CAF50',
    color: '#ffffff'
  },
  smallButton: {
    padding: '5px 10px',
    backgroundColor: '#007acc',
    border: 'none',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '12px',
    cursor: 'pointer',
    marginLeft: 'auto'
  },
  containerList: {
    display: 'flex',
    flexDirection: 'column',
    gap: '10px'
  },
  containerCard: {
    padding: '15px',
    backgroundColor: '#3c3c3c',
    borderRadius: '5px',
    border: '1px solid #555'
  },
  containerHeader: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: '10px'
  },
  statusBadge: {
    padding: '4px 8px',
    borderRadius: '3px',
    fontSize: '12px',
    fontWeight: 'bold',
    color: '#ffffff'
  },
  containerInfo: {
    fontSize: '13px',
    color: '#cccccc',
    marginBottom: '10px'
  },
  containerActions: {
    display: 'flex',
    gap: '8px',
    flexWrap: 'wrap'
  },
  actionButton: {
    padding: '6px 12px',
    backgroundColor: '#007acc',
    border: 'none',
    borderRadius: '3px',
    color: '#ffffff',
    fontSize: '12px',
    cursor: 'pointer'
  },
  builderGrid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
    gap: '10px'
  },
  doctorGrid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
    gap: '10px'
  },
  logsContainer: {
    maxHeight: '400px',
    overflowY: 'auto',
    backgroundColor: '#1e1e1e',
    padding: '10px',
    borderRadius: '3px',
    border: '1px solid #555'
  },
  logLine: {
    fontSize: '12px',
    fontFamily: 'Consolas, monospace',
    color: '#cccccc',
    marginBottom: '5px'
  },
  emptyText: {
    textAlign: 'center',
    color: '#888',
    padding: '20px'
  }
};

export default DockerPanel;
