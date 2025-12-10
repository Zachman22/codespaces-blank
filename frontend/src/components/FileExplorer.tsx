import React, { useState, useEffect } from 'react';
import { FileText, Folder, ChevronRight, ChevronDown } from 'lucide-react';
import bridge from '../services/bridge';

interface FileNode {
  name: string;
  path: string;
  type: 'file' | 'folder';
  children?: FileNode[];
}

interface FileExplorerProps {
  onFileSelect: (path: string, content: string) => void;
}

const FileExplorer: React.FC<FileExplorerProps> = ({ onFileSelect }) => {
  const [files, setFiles] = useState<FileNode[]>([]);
  const [expanded, setExpanded] = useState<Set<string>>(new Set());
  const [rootPath] = useState<string>('.');

  useEffect(() => {
    // Listen for directory listings from backend
    bridge.on('directoryListing', (data: { success: boolean; path: string; files: any[] }) => {
      if (data.success) {
        updateDirectoryContents(data.path, data.files);
      }
    });

    // Listen for file content from backend
    bridge.on('fileContent', (data: { success: boolean; content: string; path: string }) => {
      if (data.success) {
        onFileSelect(data.path, data.content);
      } else {
        alert(`Failed to open file: ${data.path}`);
      }
    });

    // Load initial directory
    loadDirectory(rootPath);
  }, []);

  const loadDirectory = (path: string) => {
    bridge.listDirectory(path);
  };

  const updateDirectoryContents = (path: string, fileList: any[]) => {
    const nodes: FileNode[] = fileList.map((file: any) => ({
      name: file.name,
      path: file.path,
      type: file.type,
      children: file.type === 'folder' ? [] : undefined
    }));

    if (path === rootPath) {
      setFiles(nodes);
      // Auto-expand root
      const newExpanded = new Set(expanded);
      nodes.forEach(node => {
        if (node.type === 'folder') {
          newExpanded.add(node.path);
        }
      });
      setExpanded(newExpanded);
    } else {
      // Update children of expanded folder
      setFiles(prevFiles => updateNodeChildren(prevFiles, path, nodes));
    }
  };

  const updateNodeChildren = (nodes: FileNode[], targetPath: string, children: FileNode[]): FileNode[] => {
    return nodes.map(node => {
      if (node.path === targetPath) {
        return { ...node, children };
      } else if (node.children) {
        return { ...node, children: updateNodeChildren(node.children, targetPath, children) };
      }
      return node;
    });
  };

  const toggleExpand = (path: string) => {
    const newExpanded = new Set(expanded);
    if (newExpanded.has(path)) {
      newExpanded.delete(path);
    } else {
      newExpanded.add(path);
      // Load directory contents if not already loaded
      loadDirectory(path);
    }
    setExpanded(newExpanded);
  };

  const handleFileClick = (node: FileNode) => {
    if (node.type === 'file') {
      // Request file content from C++ backend
      bridge.openFile(node.path);
    } else {
      toggleExpand(node.path);
    }
  };

  const renderNode = (node: FileNode, depth: number = 0): React.ReactNode => {
    const isExpanded = expanded.has(node.path);
    const hasChildren = node.children && node.children.length > 0;

    return (
      <div key={node.path}>
        <div
          className="file-node"
          style={{ paddingLeft: `${depth * 16 + 8}px` }}
          onClick={() => handleFileClick(node)}
        >
          {node.type === 'folder' && (
            <span className="icon">
              {isExpanded ? <ChevronDown size={16} /> : <ChevronRight size={16} />}
            </span>
          )}
          <span className="icon">
            {node.type === 'folder' ? <Folder size={16} /> : <FileText size={16} />}
          </span>
          <span className="name">{node.name}</span>
        </div>
        {node.type === 'folder' && isExpanded && hasChildren && (
          <div className="children">
            {node.children!.map(child => renderNode(child, depth + 1))}
          </div>
        )}
      </div>
    );
  };

  return (
    <div className="file-explorer">
      <div className="file-explorer-header">
        <h3>EXPLORER</h3>
      </div>
      <div className="file-explorer-content">
        {files.map(node => renderNode(node))}
      </div>
    </div>
  );
};

export default FileExplorer;
