import React, { useEffect, useState } from 'react';
import { Code2, Sparkles, Zap, Shield, Cpu } from 'lucide-react';
import '../styles/SplashScreen.css';

interface SplashScreenProps {
  onComplete: () => void;
}

const SplashScreen: React.FC<SplashScreenProps> = ({ onComplete }) => {
  const [progress, setProgress] = useState(0);
  const [loadingText, setLoadingText] = useState('Initializing...');

  useEffect(() => {
    const stages = [
      { progress: 20, text: 'Loading WebView2...' },
      { progress: 40, text: 'Initializing Monaco Editor...' },
      { progress: 60, text: 'Connecting to backend...' },
      { progress: 80, text: 'Loading system information...' },
      { progress: 100, text: 'Ready!' }
    ];

    let stageIndex = 0;
    const interval = setInterval(() => {
      if (stageIndex < stages.length) {
        setProgress(stages[stageIndex].progress);
        setLoadingText(stages[stageIndex].text);
        stageIndex++;
      } else {
        clearInterval(interval);
        setTimeout(onComplete, 500);
      }
    }, 400);

    return () => clearInterval(interval);
  }, [onComplete]);

  return (
    <div className="splash-screen">
      <div className="splash-background">
        <div className="gradient-orb orb-1"></div>
        <div className="gradient-orb orb-2"></div>
        <div className="gradient-orb orb-3"></div>
      </div>

      <div className="splash-content">
        <div className="splash-logo">
          <div className="logo-icon">
            <Code2 size={64} strokeWidth={1.5} />
          </div>
          <h1 className="splash-title">
            Hybrid <span className="highlight">IDE</span>
          </h1>
          <p className="splash-subtitle">
            Professional C++ Development Environment
          </p>
        </div>

        <div className="features-showcase">
          <div className="feature-item">
            <Zap size={20} />
            <span>Lightning Fast</span>
          </div>
          <div className="feature-item">
            <Cpu size={20} />
            <span>Native Performance</span>
          </div>
          <div className="feature-item">
            <Sparkles size={20} />
            <span>AI Powered</span>
          </div>
          <div className="feature-item">
            <Shield size={20} />
            <span>Windows 11</span>
          </div>
        </div>

        <div className="loading-section">
          <div className="loading-bar-container">
            <div 
              className="loading-bar" 
              style={{ width: `${progress}%` }}
            ></div>
          </div>
          <p className="loading-text">{loadingText}</p>
        </div>

        <div className="splash-footer">
          <p>Powered by React + C++ + WebView2</p>
          <p className="version">v1.0.0</p>
        </div>
      </div>
    </div>
  );
};

export default SplashScreen;
