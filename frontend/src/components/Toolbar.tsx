import React from 'react';
import { Play, Square, Settings, Hammer, Bug } from 'lucide-react';

interface ToolbarProps {
  onBuild: () => void;
  onRun: () => void;
  onDebug: () => void;
  isBuilding: boolean;
  isRunning: boolean;
}

const Toolbar: React.FC<ToolbarProps> = ({ onBuild, onRun, onDebug, isBuilding, isRunning }) => {
  return (
    <div className="toolbar">
      <div className="toolbar-left">
        <button
          className="toolbar-button primary"
          onClick={onBuild}
          disabled={isBuilding}
          title="Build Project (Ctrl+B)"
        >
          <Hammer size={18} />
          <span>{isBuilding ? 'Building...' : 'Build'}</span>
        </button>
        
        <button
          className="toolbar-button success"
          onClick={onRun}
          disabled={isRunning || isBuilding}
          title="Run (Ctrl+F5)"
        >
          {isRunning ? <Square size={18} /> : <Play size={18} />}
          <span>{isRunning ? 'Stop' : 'Run'}</span>
        </button>
        
        <button
          className="toolbar-button"
          onClick={onDebug}
          disabled={isBuilding || isRunning}
          title="Debug (F5)"
        >
          <Bug size={18} />
          <span>Debug</span>
        </button>
      </div>
      
      <div className="toolbar-right">
        <button className="toolbar-button icon-only" title="Settings">
          <Settings size={18} />
        </button>
      </div>
    </div>
  );
};

export default Toolbar;
