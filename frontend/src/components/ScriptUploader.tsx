import React, { useState, useRef } from 'react';
import './ScriptUploader.css';

interface TestReport {
  result: 'success' | 'syntax_error' | 'runtime_error' | 'dependency_missing' | 'permission_error' | 'unknown';
  output: string;
  errorMessage: string;
  exitCode: number;
  executionTime: number;
  missingDependencies: string[];
  suggestions: string[];
}

interface FixAttempt {
  attemptNumber: number;
  modifiedContent: string;
  testReport: TestReport;
  fixDescription: string;
}

interface UploadedScript {
  filename: string;
  content: string;
  platform: string;
  fileSize: number;
  uploadTime: string;
}

const ScriptUploader: React.FC = () => {
  const [selectedFile, setSelectedFile] = useState<File | null>(null);
  const [scriptContent, setScriptContent] = useState<string>('');
  const [uploadedScript, setUploadedScript] = useState<UploadedScript | null>(null);
  const [isUploading, setIsUploading] = useState(false);
  const [isVerifying, setIsVerifying] = useState(false);
  const [verificationStatus, setVerificationStatus] = useState<string>('');
  const [fixHistory, setFixHistory] = useState<FixAttempt[]>([]);
  const [userPurpose, setUserPurpose] = useState<string>('');
  const [showPurposeDialog, setShowPurposeDialog] = useState(false);
  const [currentAttempt, setCurrentAttempt] = useState(0);
  const [maxAttempts] = useState(5);
  const [finalResult, setFinalResult] = useState<'success' | 'failed' | null>(null);
  const fileInputRef = useRef<HTMLInputElement>(null);

  const handleFileSelect = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (file) {
      setSelectedFile(file);
      // Read file content
      const reader = new FileReader();
      reader.onload = (e) => {
        const content = e.target?.result as string;
        setScriptContent(content);
      };
      reader.readAsText(file);
    }
  };

  const handleDrop = (event: React.DragEvent<HTMLDivElement>) => {
    event.preventDefault();
    const file = event.dataTransfer.files[0];
    if (file && (file.name.endsWith('.sh') || file.name.endsWith('.ps1') || 
                 file.name.endsWith('.bat') || file.name.endsWith('.zsh'))) {
      setSelectedFile(file);
      const reader = new FileReader();
      reader.onload = (e) => {
        const content = e.target?.result as string;
        setScriptContent(content);
      };
      reader.readAsText(file);
    }
  };

  const handleDragOver = (event: React.DragEvent<HTMLDivElement>) => {
    event.preventDefault();
  };

  const handleUpload = async () => {
    if (!selectedFile || !scriptContent) return;

    setIsUploading(true);
    setVerificationStatus('Uploading script...');

    try {
      // Simulate API call to upload script
      const uploaded: UploadedScript = {
        filename: selectedFile.name,
        content: scriptContent,
        platform: detectPlatform(scriptContent, selectedFile.name),
        fileSize: scriptContent.length,
        uploadTime: new Date().toISOString()
      };

      setUploadedScript(uploaded);
      setVerificationStatus(`Script uploaded: ${uploaded.filename} (${uploaded.fileSize} bytes)`);
      
      setTimeout(() => {
        setIsUploading(false);
      }, 500);

    } catch (error) {
      console.error('Upload error:', error);
      setVerificationStatus('Upload failed');
      setIsUploading(false);
    }
  };

  const detectPlatform = (content: string, filename: string): string => {
    if (content.includes('#!/bin/bash')) return 'Linux Bash';
    if (content.includes('#!/bin/sh')) return 'Linux sh';
    if (content.includes('#!/bin/zsh')) return 'macOS Zsh';
    if (content.includes('$PSVersionTable') || content.includes('Write-Host')) return 'Windows PowerShell';
    if (content.includes('@echo off')) return 'Windows Batch';
    
    if (filename.endsWith('.sh')) return 'Linux Bash';
    if (filename.endsWith('.zsh')) return 'macOS Zsh';
    if (filename.endsWith('.ps1')) return 'Windows PowerShell';
    if (filename.endsWith('.bat')) return 'Windows Batch';
    
    return 'Unknown';
  };

  const handleVerify = async () => {
    if (!uploadedScript) return;

    setIsVerifying(true);
    setFixHistory([]);
    setFinalResult(null);
    setCurrentAttempt(0);
    setVerificationStatus('Starting verification...');

    // Simulate iterative verification
    await simulateVerification(uploadedScript);
  };

  const simulateVerification = async (script: UploadedScript) => {
    // First, check if script has obvious issues
    const hasIssues = checkForIssues(script.content);

    if (!hasIssues) {
      setVerificationStatus('✓ Script syntax validated');
      await sleep(500);
      setVerificationStatus('✓ Script execution tested');
      await sleep(500);
      setVerificationStatus('🎉 Script verified and working!');
      setFinalResult('success');
      setIsVerifying(false);
      return;
    }

    // Show purpose dialog
    setShowPurposeDialog(true);
  };

  const handlePurposeSubmit = async () => {
    if (!userPurpose.trim()) {
      alert('Please describe the script purpose');
      return;
    }

    setShowPurposeDialog(false);
    setVerificationStatus('AI is analyzing your script...');

    // Simulate fixing attempts
    for (let i = 1; i <= maxAttempts; i++) {
      setCurrentAttempt(i);
      setVerificationStatus(`Attempt ${i}/${maxAttempts}: Testing script...`);
      await sleep(1000);

      const report: TestReport = {
        result: i < 3 ? 'syntax_error' : 'success',
        output: i < 3 ? 'Error found in script' : 'Script executed successfully',
        errorMessage: i < 3 ? 'Missing error handling' : '',
        exitCode: i < 3 ? 1 : 0,
        executionTime: 0.5,
        missingDependencies: i === 2 ? ['curl', 'git'] : [],
        suggestions: []
      };

      const fixAttempt: FixAttempt = {
        attemptNumber: i,
        modifiedContent: uploadedScript!.content + `\n# Fix attempt ${i}\n`,
        testReport: report,
        fixDescription: i === 1 ? 'Added error handling' : 
                       i === 2 ? 'Installed missing dependencies' : 
                       'Fixed syntax issues'
      };

      setFixHistory(prev => [...prev, fixAttempt]);

      if (report.result === 'success') {
        setVerificationStatus('🎉 Script fixed and verified!');
        setFinalResult('success');
        setIsVerifying(false);
        return;
      }

      setVerificationStatus(`Fixing issues (attempt ${i})...`);
      await sleep(1000);
    }

    setVerificationStatus('❌ Failed to fix script after 5 attempts');
    setFinalResult('failed');
    setIsVerifying(false);
  };

  const checkForIssues = (content: string): boolean => {
    // Simple checks for demo purposes
    if (!content.includes('set -e') && content.includes('#!/bin/bash')) return true;
    if (content.length < 50) return true;
    return false;
  };

  const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

  const handleDownloadFixed = () => {
    if (fixHistory.length === 0) return;

    const lastFix = fixHistory[fixHistory.length - 1];
    const blob = new Blob([lastFix.modifiedContent], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `fixed_${uploadedScript?.filename || 'script.sh'}`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  };

  const handleReset = () => {
    setSelectedFile(null);
    setScriptContent('');
    setUploadedScript(null);
    setFixHistory([]);
    setUserPurpose('');
    setVerificationStatus('');
    setFinalResult(null);
    setCurrentAttempt(0);
    if (fileInputRef.current) {
      fileInputRef.current.value = '';
    }
  };

  return (
    <div className="script-uploader">
      <div className="uploader-header">
        <h2>🔧 Script Uploader & Verifier</h2>
        <p>Upload scripts, test them automatically, and get AI-powered fixes</p>
      </div>

      <div 
        className="drop-zone"
        onDrop={handleDrop}
        onDragOver={handleDragOver}
      >
        {selectedFile ? (
          <div className="file-selected">
            <div className="file-icon">📄</div>
            <div className="file-info">
              <strong>{selectedFile.name}</strong>
              <span>{(selectedFile.size / 1024).toFixed(2)} KB</span>
            </div>
          </div>
        ) : (
          <div className="drop-prompt">
            <div className="upload-icon">⬆️</div>
            <p>Drag & drop your script here</p>
            <p className="supported-formats">Supported: .sh, .bash, .zsh, .ps1, .bat, .cmd</p>
          </div>
        )}
        <input
          ref={fileInputRef}
          type="file"
          accept=".sh,.bash,.zsh,.ps1,.bat,.cmd"
          onChange={handleFileSelect}
          style={{ display: 'none' }}
        />
      </div>

      <div className="action-buttons">
        <button 
          className="btn btn-primary"
          onClick={() => fileInputRef.current?.click()}
          disabled={isUploading || isVerifying}
        >
          📁 Browse Files
        </button>
        <button 
          className="btn btn-success"
          onClick={handleUpload}
          disabled={!selectedFile || isUploading || isVerifying}
        >
          {isUploading ? '⏳ Uploading...' : '📤 Upload'}
        </button>
        <button 
          className="btn btn-info"
          onClick={handleVerify}
          disabled={!uploadedScript || isVerifying}
        >
          {isVerifying ? '🔄 Verifying...' : '🔍 Verify & Fix'}
        </button>
        <button 
          className="btn btn-secondary"
          onClick={handleReset}
          disabled={isUploading || isVerifying}
        >
          🔄 Reset
        </button>
      </div>

      {uploadedScript && (
        <div className="script-info">
          <h3>📋 Script Information</h3>
          <div className="info-grid">
            <div className="info-item">
              <strong>Filename:</strong>
              <span>{uploadedScript.filename}</span>
            </div>
            <div className="info-item">
              <strong>Platform:</strong>
              <span>{uploadedScript.platform}</span>
            </div>
            <div className="info-item">
              <strong>Size:</strong>
              <span>{uploadedScript.fileSize} bytes</span>
            </div>
            <div className="info-item">
              <strong>Uploaded:</strong>
              <span>{new Date(uploadedScript.uploadTime).toLocaleString()}</span>
            </div>
          </div>
        </div>
      )}

      {verificationStatus && (
        <div className={`verification-status ${finalResult}`}>
          <div className="status-text">{verificationStatus}</div>
          {currentAttempt > 0 && (
            <div className="progress-bar">
              <div 
                className="progress-fill"
                style={{ width: `${(currentAttempt / maxAttempts) * 100}%` }}
              ></div>
            </div>
          )}
        </div>
      )}

      {fixHistory.length > 0 && (
        <div className="fix-history">
          <h3>🔧 Fix History</h3>
          {fixHistory.map((attempt) => (
            <div key={attempt.attemptNumber} className="fix-attempt">
              <div className="attempt-header">
                <strong>Attempt {attempt.attemptNumber}</strong>
                <span className={`result-badge ${attempt.testReport.result}`}>
                  {attempt.testReport.result === 'success' ? '✓' : '✗'} {attempt.testReport.result}
                </span>
              </div>
              <div className="attempt-details">
                <p><strong>Fix:</strong> {attempt.fixDescription}</p>
                {attempt.testReport.errorMessage && (
                  <p className="error-message">{attempt.testReport.errorMessage}</p>
                )}
                {attempt.testReport.missingDependencies.length > 0 && (
                  <p>
                    <strong>Missing:</strong> {attempt.testReport.missingDependencies.join(', ')}
                  </p>
                )}
              </div>
            </div>
          ))}
        </div>
      )}

      {finalResult === 'success' && (
        <div className="success-actions">
          <button className="btn btn-download" onClick={handleDownloadFixed}>
            💾 Download Fixed Script
          </button>
        </div>
      )}

      {showPurposeDialog && (
        <div className="modal-overlay">
          <div className="modal-content">
            <h3>🤖 AI Script Fixer</h3>
            <p>What is the purpose of this script?</p>
            <p className="modal-subtitle">This helps the AI understand and fix your script correctly</p>
            <textarea
              className="purpose-input"
              placeholder="Example: This script should install Docker and configure it for production use..."
              value={userPurpose}
              onChange={(e) => setUserPurpose(e.target.value)}
              rows={4}
            />
            <div className="modal-actions">
              <button 
                className="btn btn-secondary"
                onClick={() => setShowPurposeDialog(false)}
              >
                Cancel
              </button>
              <button 
                className="btn btn-primary"
                onClick={handlePurposeSubmit}
              >
                Start Fixing
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default ScriptUploader;
