import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
    console.log('Hybrid IDE extension is now active!');

    // Create status bar item
    const statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 100);
    statusBarItem.text = "$(robot) Hybrid IDE";
    statusBarItem.command = 'hybrid-ide.openChatbot';
    statusBarItem.show();
    context.subscriptions.push(statusBarItem);

    // Register Open Chatbot Command
    context.subscriptions.push(
        vscode.commands.registerCommand('hybrid-ide.openChatbot', () => {
            const panel = vscode.window.createWebviewPanel(
                'hybridIdeChatbot',
                '🤖 Hybrid IDE AI Chatbot',
                vscode.ViewColumn.Beside,
                {
                    enableScripts: true,
                    retainContextWhenHidden: true
                }
            );

            panel.webview.html = getChatbotHTML();

            // Handle messages from the webview
            panel.webview.onDidReceiveMessage(
                async message => {
                    switch (message.command) {
                        case 'sendMessage':
                            const response = await handleAIMessage(message.text);
                            panel.webview.postMessage({
                                command: 'aiResponse',
                                text: response,
                                timestamp: new Date().toISOString()
                            });
                            break;
                    }
                },
                undefined,
                context.subscriptions
            );
        })
    );

    // Register Build Executable Command
    context.subscriptions.push(
        vscode.commands.registerCommand('hybrid-ide.buildExecutable', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showErrorMessage('No active file to build');
                return;
            }

            const language = detectLanguage(editor.document);
            if (!language) {
                vscode.window.showErrorMessage('Unsupported file type for building');
                return;
            }

            const outputName = await vscode.window.showInputBox({
                prompt: 'Enter executable name',
                value: getFileNameWithoutExt(editor.document.fileName)
            });

            if (!outputName) return;

            await vscode.window.withProgress({
                location: vscode.ProgressLocation.Notification,
                title: `Building ${outputName}...`,
                cancellable: false
            }, async (progress) => {
                progress.report({ increment: 30, message: 'Compiling...' });
                
                const terminal = vscode.window.createTerminal('Build');
                terminal.show();
                
                const buildCommand = getBuildCommand(language, editor.document.fileName, outputName);
                terminal.sendText(buildCommand);
                
                progress.report({ increment: 100 });
                
                setTimeout(() => {
                    vscode.window.showInformationMessage(
                        `✅ Build command executed: ${outputName}`,
                        'Open Terminal'
                    ).then(action => {
                        if (action === 'Open Terminal') {
                            terminal.show();
                        }
                    });
                }, 2000);
            });
        })
    );

    // Register Generate GUI Command
    context.subscriptions.push(
        vscode.commands.registerCommand('hybrid-ide.generateGUI', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showErrorMessage('No active file');
                return;
            }

            const framework = await vscode.window.showQuickPick([
                { label: 'HTML/CSS/JS', description: 'Simple web-based GUI' },
                { label: 'React', description: 'Modern React components' },
                { label: 'GTK', description: 'Native Linux GUI' },
                { label: 'Qt', description: 'Cross-platform native' }
            ], {
                placeHolder: 'Select GUI framework'
            });

            if (!framework) return;

            await vscode.window.withProgress({
                location: vscode.ProgressLocation.Notification,
                title: 'Generating GUI...',
                cancellable: false
            }, async (progress) => {
                progress.report({ increment: 20, message: 'Analyzing code...' });
                
                const functions = analyzeFunctions(editor.document.getText());
                
                progress.report({ increment: 50, message: 'Creating GUI files...' });
                
                const guiCode = generateGUICode(functions, framework.label);
                const guiFilePath = await createGUIFile(guiCode, framework.label);
                
                progress.report({ increment: 100 });
                
                const doc = await vscode.workspace.openTextDocument(guiFilePath);
                await vscode.window.showTextDocument(doc, vscode.ViewColumn.Beside);
                
                vscode.window.showInformationMessage(
                    `✅ GUI generated with ${framework.label}!`
                );
            });
        })
    );

    // Register Generate Code Command
    context.subscriptions.push(
        vscode.commands.registerCommand('hybrid-ide.generateCode', async () => {
            const prompt = await vscode.window.showInputBox({
                prompt: 'Describe the code you want to generate',
                placeHolder: 'e.g., Create a REST API endpoint'
            });

            if (!prompt) return;

            const editor = vscode.window.activeTextEditor;
            if (editor) {
                await vscode.window.withProgress({
                    location: vscode.ProgressLocation.Notification,
                    title: 'Generating code...',
                    cancellable: false
                }, async () => {
                    const code = await generateCodeFromPrompt(prompt, editor.document.languageId);
                    editor.edit(editBuilder => {
                        editBuilder.insert(editor.selection.active, code);
                    });
                });
            }
        })
    );
}

// Helper Functions

function detectLanguage(document: vscode.TextDocument): string | null {
    const extMap: {[key: string]: string} = {
        'cpp': 'cpp',
        'c': 'c',
        'python': 'python',
        'go': 'go',
        'rust': 'rust',
        'javascript': 'javascript',
        'typescript': 'typescript',
        'java': 'java',
        'csharp': 'csharp'
    };
    return extMap[document.languageId] || null;
}

function getFileNameWithoutExt(filePath: string): string {
    const parts = filePath.split(/[\\/]/);
    const filename = parts[parts.length - 1];
    return filename.replace(/\.[^/.]+$/, '');
}

function getBuildCommand(language: string, inputFile: string, outputName: string): string {
    switch (language) {
        case 'cpp':
            return `g++ -std=c++17 -O3 -o ${outputName} "${inputFile}"`;
        case 'c':
            return `gcc -std=c11 -O3 -o ${outputName} "${inputFile}"`;
        case 'python':
            return `pyinstaller --onefile --name ${outputName} "${inputFile}"`;
        case 'go':
            return `go build -o ${outputName} "${inputFile}"`;
        case 'rust':
            return `rustc -O -o ${outputName} "${inputFile}"`;
        case 'java':
            return `javac "${inputFile}" && jar cfe ${outputName}.jar Main *.class`;
        default:
            return `echo "Build not configured for ${language}"`;
    }
}

function analyzeFunctions(code: string): Array<{name: string, params: string[]}> {
    const functions: Array<{name: string, params: string[]}> = [];
    
    // C/C++ functions
    const cppPattern = /(?:[\w:]+\s+)?([\w]+)\s*\((.*?)\)\s*{/g;
    let match;
    while ((match = cppPattern.exec(code)) !== null) {
        const name = match[1];
        if (!['if', 'while', 'for', 'switch'].includes(name)) {
            functions.push({
                name,
                params: match[2].split(',').map(p => p.trim()).filter(p => p)
            });
        }
    }
    
    // Python functions
    const pythonPattern = /def\s+([\w]+)\s*\((.*?)\):/g;
    while ((match = pythonPattern.exec(code)) !== null) {
        functions.push({
            name: match[1],
            params: match[2].split(',').map(p => p.trim().split(':')[0]).filter(p => p)
        });
    }
    
    return functions;
}

function generateGUICode(functions: Array<{name: string, params: string[]}>, framework: string): string {
    if (framework === 'HTML/CSS/JS') {
        return `<!DOCTYPE html>
<html>
<head>
    <title>Generated GUI</title>
    <style>
        body { font-family: Arial, sans-serif; max-width: 800px; margin: 50px auto; padding: 20px; }
        .function { background: #f5f5f5; padding: 20px; margin: 20px 0; border-radius: 8px; }
        input { padding: 10px; margin: 5px 0; width: 100%; border: 1px solid #ddd; border-radius: 4px; }
        button { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }
        button:hover { background: #0056b3; }
        .result { margin-top: 10px; padding: 10px; background: #e9ecef; border-radius: 4px; }
    </style>
</head>
<body>
    <h1>🚀 Generated GUI Interface</h1>
    ${functions.map(func => `
    <div class="function">
        <h2>${func.name}()</h2>
        ${func.params.map(param => `
        <input type="text" id="${func.name}_${param}" placeholder="${param}">
        `).join('')}
        <button onclick="call_${func.name}()">Execute</button>
        <div id="result_${func.name}" class="result" style="display:none;"></div>
    </div>
    `).join('')}
    
    <script>
        ${functions.map(func => `
        function call_${func.name}() {
            const params = ${JSON.stringify(func.params)}.map(p => document.getElementById('${func.name}_' + p).value);
            const result = document.getElementById('result_${func.name}');
            result.style.display = 'block';
            result.innerHTML = 'Called ${func.name} with: ' + JSON.stringify(params);
        }
        `).join('')}
    </script>
</body>
</html>`;
    }
    
    return `<!-- GUI for ${framework} -->\n<!-- Functions: ${functions.map(f => f.name).join(', ')} -->`;
}

async function createGUIFile(content: string, framework: string): Promise<string> {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) {
        throw new Error('No workspace folder');
    }
    
    const ext = framework === 'HTML/CSS/JS' ? '.html' : '.txt';
    const uri = vscode.Uri.joinPath(workspaceFolder.uri, `generated-gui${ext}`);
    
    await vscode.workspace.fs.writeFile(uri, Buffer.from(content, 'utf8'));
    
    return uri.fsPath;
}

async function handleAIMessage(userMessage: string): Promise<string> {
    // Placeholder - integrate with OpenAI/Anthropic API
    const responses = [
        `I can help you with that! Based on your message: "${userMessage}", here's what I suggest:\n\n` +
        `1. First, identify the key requirements\n` +
        `2. Break down the problem into smaller steps\n` +
        `3. Implement each component systematically\n\n` +
        `Would you like me to generate code for this?`,
        
        `Great question! For "${userMessage}", you should consider:\n\n` +
        `• Using proper error handling\n` +
        `• Following best practices for your language\n• Adding comprehensive tests\n\n` +
        `Let me know if you need specific code examples!`,
        
        `I understand you want to work on: "${userMessage}". ` +
        `I can help generate code, build executables, or create GUIs. ` +
        `Use the commands:\n• Build Executable\n• Generate GUI\n• Generate Code`
    ];
    
    return responses[Math.floor(Math.random() * responses.length)];
}

async function generateCodeFromPrompt(prompt: string, languageId: string): Promise<string> {
    // Placeholder - integrate with AI API
    const templates: {[key: string]: string} = {
        'python': `# Generated code for: ${prompt}\n\ndef generated_function():\n    # TODO: Implement\n    pass\n`,
        'javascript': `// Generated code for: ${prompt}\n\nfunction generatedFunction() {\n    // TODO: Implement\n}\n`,
        'typescript': `// Generated code for: ${prompt}\n\nfunction generatedFunction(): void {\n    // TODO: Implement\n}\n`,
        'cpp': `// Generated code for: ${prompt}\n\nvoid generatedFunction() {\n    // TODO: Implement\n}\n`,
        'java': `// Generated code for: ${prompt}\n\npublic void generatedFunction() {\n    // TODO: Implement\n}\n`
    };
    
    return templates[languageId] || `// Generated code for: ${prompt}\n`;
}

function getChatbotHTML(): string {
    return `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hybrid IDE Chatbot</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
            background: #1e1e1e;
            color: #d4d4d4;
            height: 100vh;
            display: flex;
            flex-direction: column;
        }
        .header {
            padding: 15px 20px;
            background: #252526;
            border-bottom: 1px solid #3e3e42;
        }
        .header h1 {
            font-size: 16px;
            font-weight: 600;
        }
        .chat-container {
            flex: 1;
            overflow-y: auto;
            padding: 20px;
        }
        .message {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            animation: slideIn 0.3s ease-out;
        }
        @keyframes slideIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }
        .message.user { flex-direction: row-reverse; }
        .avatar {
            width: 32px;
            height: 32px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            font-weight: bold;
            flex-shrink: 0;
        }
        .message.user .avatar {
            background: #0e639c;
            color: white;
        }
        .message.assistant .avatar {
            background: #569cd6;
            color: white;
        }
        .message-content {
            max-width: 70%;
            padding: 12px 16px;
            border-radius: 8px;
            line-height: 1.5;
        }
        .message.user .message-content {
            background: #0e639c;
            color: white;
        }
        .message.assistant .message-content {
            background: #2d2d30;
            border: 1px solid #3e3e42;
        }
        .input-container {
            padding: 15px 20px;
            background: #252526;
            border-top: 1px solid #3e3e42;
            display: flex;
            gap: 10px;
        }
        .input-box {
            flex: 1;
            background: #3c3c3c;
            color: #d4d4d4;
            border: 1px solid #3e3e42;
            border-radius: 4px;
            padding: 10px 12px;
            font-family: inherit;
            font-size: 13px;
            resize: none;
        }
        .send-btn {
            background: #0e639c;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 4px;
            cursor: pointer;
            font-weight: 600;
        }
        .send-btn:hover { background: #1177bb; }
    </style>
</head>
<body>
    <div class="header">
        <h1>🤖 Hybrid IDE AI Assistant</h1>
    </div>
    
    <div class="chat-container" id="chatContainer">
        <div class="message assistant">
            <div class="avatar">AI</div>
            <div class="message-content">
                Hello! I'm your Hybrid IDE AI assistant. I can help you:<br>
                • Generate code in any language<br>
                • Build executables<br>
                • Create GUIs<br>
                • Debug and optimize code<br><br>
                What would you like to work on?
            </div>
        </div>
    </div>
    
    <div class="input-container">
        <textarea class="input-box" id="messageInput" placeholder="Ask me anything..." rows="1"></textarea>
        <button class="send-btn" id="sendBtn" onclick="sendMessage()">Send</button>
    </div>

    <script>
        const vscode = acquireVsCodeApi();
        const chatContainer = document.getElementById('chatContainer');
        const messageInput = document.getElementById('messageInput');
        
        messageInput.addEventListener('keydown', function(e) {
            if (e.key === 'Enter' && !e.shiftKey) {
                e.preventDefault();
                sendMessage();
            }
        });

        function sendMessage() {
            const text = messageInput.value.trim();
            if (!text) return;

            addMessage('user', text);
            messageInput.value = '';

            vscode.postMessage({
                command: 'sendMessage',
                text: text
            });
        }

        function addMessage(role, content) {
            const messageDiv = document.createElement('div');
            messageDiv.className = \`message \${role}\`;
            
            const avatar = document.createElement('div');
            avatar.className = 'avatar';
            avatar.textContent = role === 'user' ? 'U' : 'AI';
            
            const contentDiv = document.createElement('div');
            contentDiv.className = 'message-content';
            contentDiv.innerHTML = content.replace(/\\n/g, '<br>');
            
            messageDiv.appendChild(avatar);
            messageDiv.appendChild(contentDiv);
            chatContainer.appendChild(messageDiv);
            chatContainer.scrollTop = chatContainer.scrollHeight;
        }

        window.addEventListener('message', event => {
            const message = event.data;
            if (message.command === 'aiResponse') {
                addMessage('assistant', message.text);
            }
        });

        messageInput.focus();
    </script>
</body>
</html>`;
}

export function deactivate() {}
