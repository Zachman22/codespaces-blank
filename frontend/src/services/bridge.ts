// WebView2 Bridge Service
// Handles communication between React frontend and C++ backend

interface Message {
  type: string;
  data: any;
}

type MessageHandler = (data: any) => void;

class Bridge {
  private handlers: Map<string, MessageHandler[]> = new Map();
  private messageQueue: Message[] = [];
  private isReady = false;

  constructor() {
    this.initialize();
  }

  private initialize() {
    // Check if we're running in WebView2
    if (typeof window !== 'undefined' && (window as any).chrome?.webview) {
      this.isReady = true;
      this.setupMessageListener();
      this.flushQueue();
    } else {
      // Fallback for development/testing
      console.warn('Not running in WebView2 environment. Using mock bridge.');
      this.setupMockBridge();
    }
  }

  private setupMessageListener() {
    const webview = (window as any).chrome.webview;
    
    webview.addEventListener('message', (event: any) => {
      try {
        const message: Message = JSON.parse(event.data);
        this.handleMessage(message);
      } catch (error) {
        console.error('Failed to parse message from C++ backend:', error);
      }
    });
  }

  private setupMockBridge() {
    // Mock implementation for development
    this.isReady = true;
    
    // Simulate system info response
    setTimeout(() => {
      this.handleMessage({
        type: 'systemInfo',
        data: {
          os: 'Windows 11 (Build 22000)',
          architecture: 'x64',
          cpu: 'Intel Core i7-10700K',
          cores: 8,
          ram: '16384 MB'
        }
      });
    }, 500);
  }

  private handleMessage(message: Message) {
    const handlers = this.handlers.get(message.type);
    if (handlers) {
      handlers.forEach(handler => handler(message.data));
    }
  }

  private flushQueue() {
    while (this.messageQueue.length > 0) {
      const message = this.messageQueue.shift();
      if (message) {
        this.sendImmediate(message);
      }
    }
  }

  private sendImmediate(message: Message) {
    if ((window as any).chrome?.webview) {
      (window as any).chrome.webview.postMessage(JSON.stringify(message));
    } else {
      console.log('[Mock Bridge] Sending message:', message);
      
      // Mock responses for development
      if (message.type === 'getSystemInfo') {
        setTimeout(() => {
          this.handleMessage({
            type: 'systemInfo',
            data: {
              os: 'Windows 11 (Build 22000)',
              architecture: 'x64',
              cpu: 'Intel Core i7-10700K',
              cores: 8,
              ram: '16384 MB'
            }
          });
        }, 100);
      }
      
      if (message.type === 'build') {
        setTimeout(() => {
          this.handleMessage({
            type: 'buildLog',
            data: { message: '[INFO] Starting compilation...\n' }
          });
        }, 100);
        
        setTimeout(() => {
          this.handleMessage({
            type: 'buildLog',
            data: { message: '[CMD] g++ test.cpp -o test.exe -std=c++17\n' }
          });
        }, 300);
        
        setTimeout(() => {
          this.handleMessage({
            type: 'buildLog',
            data: { message: '[SUCCESS] Compilation completed successfully\n' }
          });
        }, 800);
        
        setTimeout(() => {
          this.handleMessage({
            type: 'buildComplete',
            data: { success: true, message: 'Build completed successfully', outputFile: 'test.exe' }
          });
        }, 1000);
      }
    }
  }

  // Public API

  public send(type: string, data: any = {}) {
    const message: Message = { type, data };
    
    if (this.isReady) {
      this.sendImmediate(message);
    } else {
      this.messageQueue.push(message);
    }
  }

  public on(messageType: string, handler: MessageHandler): () => void {
    if (!this.handlers.has(messageType)) {
      this.handlers.set(messageType, []);
    }
    this.handlers.get(messageType)!.push(handler);
    
    // Return unsubscribe function
    return () => this.off(messageType, handler);
  }

  public off(messageType: string, handler: MessageHandler) {
    const handlers = this.handlers.get(messageType);
    if (handlers) {
      const index = handlers.indexOf(handler);
      if (index > -1) {
        handlers.splice(index, 1);
      }
    }
  }

  // Convenience methods

  public build(sourceFile: string, outputFile: string, flags: string[] = []) {
    this.send('build', { sourceFile, outputFile, flags });
  }

  public run(path: string) {
    this.send('run', { path });
  }

  public getSystemInfo() {
    this.send('getSystemInfo');
  }

  public openFile(path: string) {
    this.send('openFile', { path });
  }

  public saveFile(path: string, content: string) {
    this.send('saveFile', { path, content });
  }

  public listDirectory(path: string) {
    this.send('listDirectory', { path });
  }

  public createDirectory(path: string) {
    this.send('createDirectory', { path });
  }

  public deleteFile(path: string) {
    this.send('deleteFile', { path });
  }
}

// Export singleton instance
const bridgeInstance = new Bridge();

export const bridge = bridgeInstance;
export default bridge;

// Export convenience functions for direct use
export const sendMessage = (type: string, data?: any): void => bridgeInstance.send(type, data);
export const onMessage = (type: string, handler: MessageHandler): (() => void) => bridgeInstance.on(type, handler);
export const offMessage = (type: string, handler: MessageHandler): void => bridgeInstance.off(type, handler);
