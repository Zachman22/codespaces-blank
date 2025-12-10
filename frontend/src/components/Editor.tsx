import React from 'react';
import Editor from '@monaco-editor/react';

interface MonacoEditorProps {
  value: string;
  onChange: (value: string | undefined) => void;
  language: string;
  path: string;
}

const MonacoEditor: React.FC<MonacoEditorProps> = ({ value, onChange, language, path }) => {
  const handleEditorChange = (value: string | undefined) => {
    onChange(value);
  };

  return (
    <div className="editor-container">
      <div className="editor-header">
        <span className="editor-path">{path || 'Untitled'}</span>
      </div>
      <Editor
        height="100%"
        defaultLanguage={language}
        language={language}
        value={value}
        onChange={handleEditorChange}
        theme="vs-dark"
        options={{
          fontSize: 14,
          minimap: { enabled: true },
          scrollBeyondLastLine: false,
          automaticLayout: true,
          tabSize: 4,
          wordWrap: 'on',
          lineNumbers: 'on',
          renderWhitespace: 'selection',
          bracketPairColorization: {
            enabled: true
          }
        }}
      />
    </div>
  );
};

export default MonacoEditor;
