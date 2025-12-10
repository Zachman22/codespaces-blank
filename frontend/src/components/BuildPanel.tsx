import React, { useEffect, useRef } from 'react';
import { Terminal, X } from 'lucide-react';

interface BuildPanelProps {
  logs: string[];
  isVisible: boolean;
  onClose: () => void;
  onClear: () => void;
}

const BuildPanel: React.FC<BuildPanelProps> = ({ logs, isVisible, onClose, onClear }) => {
  const logsEndRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (logsEndRef.current) {
      logsEndRef.current.scrollIntoView({ behavior: 'smooth' });
    }
  }, [logs]);

  if (!isVisible) return null;

  const getLogClass = (log: string): string => {
    if (log.includes('[ERROR]') || log.includes('error:')) return 'log-error';
    if (log.includes('[WARNING]') || log.includes('warning:')) return 'log-warning';
    if (log.includes('[SUCCESS]')) return 'log-success';
    if (log.includes('[INFO]')) return 'log-info';
    if (log.includes('[CMD]')) return 'log-command';
    return 'log-default';
  };

  return (
    <div className="build-panel">
      <div className="build-panel-header">
        <div className="build-panel-title">
          <Terminal size={16} />
          <span>Build Output</span>
        </div>
        <div className="build-panel-actions">
          <button className="panel-button" onClick={onClear} title="Clear Output">
            Clear
          </button>
          <button className="panel-button" onClick={onClose} title="Close Panel">
            <X size={16} />
          </button>
        </div>
      </div>
      <div className="build-panel-content">
        {logs.length === 0 ? (
          <div className="no-logs">No build output yet. Click "Build" to compile your project.</div>
        ) : (
          <div className="logs">
            {logs.map((log, index) => (
              <div key={index} className={`log-line ${getLogClass(log)}`}>
                {log}
              </div>
            ))}
            <div ref={logsEndRef} />
          </div>
        )}
      </div>
    </div>
  );
};

export default BuildPanel;
