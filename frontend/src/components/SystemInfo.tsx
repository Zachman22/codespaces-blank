import React, { useEffect, useState } from 'react';
import { Monitor, Cpu, HardDrive, Info } from 'lucide-react';
import bridge from '../services/bridge';

interface SystemInfoData {
  os: string;
  architecture: string;
  cpu: string;
  cores: number;
  ram: string;
}

const SystemInfo: React.FC = () => {
  const [info, setInfo] = useState<SystemInfoData | null>(null);
  const [isExpanded, setIsExpanded] = useState(false);

  useEffect(() => {
    // Request system info from C++ backend
    bridge.getSystemInfo();

    // Listen for response
    const handleSystemInfo = (data: SystemInfoData) => {
      setInfo(data);
    };

    bridge.on('systemInfo', handleSystemInfo);

    return () => {
      bridge.off('systemInfo', handleSystemInfo);
    };
  }, []);

  if (!info) {
    return (
      <div className="system-info loading">
        <Info size={14} />
        <span>Loading system info...</span>
      </div>
    );
  }

  return (
    <div className="system-info">
      <button 
        className="system-info-toggle"
        onClick={() => setIsExpanded(!isExpanded)}
        title="System Information"
      >
        <Monitor size={14} />
        <span>{info.os} • {info.architecture}</span>
      </button>
      
      {isExpanded && (
        <div className="system-info-details">
          <div className="info-item">
            <Monitor size={14} />
            <span className="label">OS:</span>
            <span className="value">{info.os}</span>
          </div>
          <div className="info-item">
            <Cpu size={14} />
            <span className="label">CPU:</span>
            <span className="value">{info.cpu}</span>
          </div>
          <div className="info-item">
            <Cpu size={14} />
            <span className="label">Cores:</span>
            <span className="value">{info.cores}</span>
          </div>
          <div className="info-item">
            <HardDrive size={14} />
            <span className="label">RAM:</span>
            <span className="value">{info.ram}</span>
          </div>
          <div className="info-item">
            <Info size={14} />
            <span className="label">Arch:</span>
            <span className="value">{info.architecture}</span>
          </div>
        </div>
      )}
    </div>
  );
};

export default SystemInfo;
