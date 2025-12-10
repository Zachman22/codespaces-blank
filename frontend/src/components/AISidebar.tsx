import React, { useState } from 'react';
import { Sparkles, Send, X } from 'lucide-react';

interface AISidebarProps {
  isVisible: boolean;
  onClose: () => void;
}

const AISidebar: React.FC<AISidebarProps> = ({ isVisible, onClose }) => {
  const [messages, setMessages] = useState<Array<{ role: 'user' | 'assistant', content: string }>>([
    {
      role: 'assistant',
      content: 'Hello! I\'m your AI coding assistant. I can help you with:\n\n• Code generation\n• Bug fixing\n• Code explanation\n• Refactoring suggestions\n• Best practices\n\nWhat would you like help with?'
    }
  ]);
  const [input, setInput] = useState('');

  const handleSend = () => {
    if (!input.trim()) return;

    const newMessages = [
      ...messages,
      { role: 'user' as const, content: input }
    ];
    setMessages(newMessages);
    setInput('');

    // Simulate AI response
    setTimeout(() => {
      setMessages([
        ...newMessages,
        {
          role: 'assistant',
          content: 'I understand you need help with that. In a production environment, this would connect to an AI model (like GPT-4, Claude, or a local LLM) to provide intelligent coding assistance.'
        }
      ]);
    }, 1000);
  };

  if (!isVisible) return null;

  return (
    <div className="ai-sidebar">
      <div className="ai-sidebar-header">
        <div className="ai-sidebar-title">
          <Sparkles size={18} />
          <span>AI Assistant</span>
        </div>
        <button className="panel-button" onClick={onClose}>
          <X size={16} />
        </button>
      </div>
      
      <div className="ai-sidebar-content">
        <div className="messages">
          {messages.map((msg, index) => (
            <div key={index} className={`message message-${msg.role}`}>
              <div className="message-content">
                {msg.content}
              </div>
            </div>
          ))}
        </div>
      </div>
      
      <div className="ai-sidebar-input">
        <input
          type="text"
          placeholder="Ask me anything..."
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyPress={(e) => e.key === 'Enter' && handleSend()}
        />
        <button onClick={handleSend} disabled={!input.trim()}>
          <Send size={18} />
        </button>
      </div>
    </div>
  );
};

export default AISidebar;
