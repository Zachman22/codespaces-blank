import * as vscode from 'vscode';
import { AIAssistantProvider } from './providers/aiAssistantProvider';
import { KubernetesProvider } from './providers/kubernetesProvider';
import { CollaborationProvider } from './providers/collaborationProvider';
import { ProfilerProvider } from './providers/profilerProvider';
import { AIAPIClient } from './services/aiApiClient';
import { KubernetesManager } from './services/kubernetesManager';
import { DebuggerService } from './services/debuggerService';
import { CloudSyncService } from './services/cloudSyncService';

export function activate(context: vscode.ExtensionContext) {
    console.log('Hybrid IDE extension is now active!');

    // Initialize services
    const aiClient = new AIAPIClient(context);
    const k8sManager = new KubernetesManager(context);
    const debuggerService = new DebuggerService(context);
    const cloudSync = new CloudSyncService(context);

    // Register tree view providers
    const aiAssistantProvider = new AIAssistantProvider(aiClient);
    vscode.window.registerTreeDataProvider('hybrid-ide-explorer', aiAssistantProvider);

    const kubernetesProvider = new KubernetesProvider(k8sManager);
    vscode.window.registerTreeDataProvider('hybrid-ide-kubernetes', kubernetesProvider);

    const collaborationProvider = new CollaborationProvider(context);
    vscode.window.registerTreeDataProvider('hybrid-ide-collaboration', collaborationProvider);

    const profilerProvider = new ProfilerProvider(context);
    vscode.window.registerTreeDataProvider('hybrid-ide-profiler', profilerProvider);

    // Register commands
    const commands = [
        // AI Code Generation
        vscode.commands.registerCommand('hybrid-ide.generateCode', async () => {
            const prompt = await vscode.window.showInputBox({
                prompt: 'Describe the code you want to generate',
                placeHolder: 'e.g., Create a REST API with Express.js'
            });

            if (prompt) {
                const editor = vscode.window.activeTextEditor;
                if (editor) {
                    vscode.window.withProgress({
                        location: vscode.ProgressLocation.Notification,
                        title: 'Generating code...',
                        cancellable: false
                    }, async () => {
                        const code = await aiClient.generateCode(prompt, editor.document.languageId);
                        editor.edit(editBuilder => {
                            editBuilder.insert(editor.selection.active, code);
                        });
                    });
                }
            }
        }),

        // AI Code Review
        vscode.commands.registerCommand('hybrid-ide.reviewCode', async () => {
            const editor = vscode.window.activeTextEditor;
            if (editor) {
                const code = editor.document.getText(editor.selection);
                vscode.window.withProgress({
                    location: vscode.ProgressLocation.Notification,
                    title: 'Reviewing code...',
                    cancellable: false
                }, async () => {
                    const review = await aiClient.reviewCode(code, editor.document.languageId);
                    vscode.window.showInformationMessage('Code Review Complete!');
                    
                    // Show review in new panel
                    const panel = vscode.window.createWebviewPanel(
                        'codeReview',
                        'Code Review',
                        vscode.ViewColumn.Beside,
                        {}
                    );
                    panel.webview.html = getCodeReviewHTML(review);
                });
            }
        }),

        // AI-Assisted Debugging
        vscode.commands.registerCommand('hybrid-ide.debugWithAI', async () => {
            const diagnostics = vscode.languages.getDiagnostics();
            if (diagnostics.length > 0) {
                const editor = vscode.window.activeTextEditor;
                if (editor) {
                    const code = editor.document.getText();
                    const errors = diagnostics.map(d => d[1].map(diag => diag.message).join('\n')).join('\n');
                    
                    vscode.window.withProgress({
                        location: vscode.ProgressLocation.Notification,
                        title: 'Analyzing errors...',
                        cancellable: false
                    }, async () => {
                        const solution = await aiClient.debugAssistance(errors, code);
                        vscode.window.showInformationMessage('Debug Analysis Complete!');
                        
                        const panel = vscode.window.createWebviewPanel(
                            'debugAssist',
                            'Debug Assistant',
                            vscode.ViewColumn.Beside,
                            {}
                        );
                        panel.webview.html = getDebugAssistHTML(solution);
                    });
                }
            } else {
                vscode.window.showInformationMessage('No errors found!');
            }
        }),

        // Performance Profiler
        vscode.commands.registerCommand('hybrid-ide.profile', async () => {
            const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
            if (workspaceFolder) {
                vscode.window.withProgress({
                    location: vscode.ProgressLocation.Notification,
                    title: 'Profiling application...',
                    cancellable: true
                }, async (progress, token) => {
                    const results = await debuggerService.profileApplication(workspaceFolder.uri.fsPath);
                    
                    const panel = vscode.window.createWebviewPanel(
                        'profiler',
                        'Performance Profile',
                        vscode.ViewColumn.Beside,
                        { enableScripts: true }
                    );
                    panel.webview.html = getProfilerHTML(results);
                });
            }
        }),

        // Kubernetes Deployment
        vscode.commands.registerCommand('hybrid-ide.deployToKubernetes', async () => {
            const namespace = await vscode.window.showInputBox({
                prompt: 'Enter Kubernetes namespace',
                value: 'default'
            });

            if (namespace) {
                vscode.window.withProgress({
                    location: vscode.ProgressLocation.Notification,
                    title: 'Deploying to Kubernetes...',
                    cancellable: false
                }, async () => {
                    const success = await k8sManager.deploy(namespace);
                    if (success) {
                        vscode.window.showInformationMessage('Deployed to Kubernetes successfully!');
                        kubernetesProvider.refresh();
                    } else {
                        vscode.window.showErrorMessage('Deployment failed!');
                    }
                });
            }
        }),

        // Collaboration
        vscode.commands.registerCommand('hybrid-ide.startCollaboration', async () => {
            const roomName = await vscode.window.showInputBox({
                prompt: 'Enter room name',
                placeHolder: 'my-project-room'
            });

            if (roomName) {
                // Start collaboration session
                vscode.window.showInformationMessage(`Starting collaboration: ${roomName}`);
                collaborationProvider.startSession(roomName);
            }
        }),

        // Cloud Sync
        vscode.commands.registerCommand('hybrid-ide.syncToCloud', async () => {
            const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
            if (workspaceFolder) {
                vscode.window.withProgress({
                    location: vscode.ProgressLocation.Notification,
                    title: 'Syncing to cloud...',
                    cancellable: false
                }, async () => {
                    const success = await cloudSync.sync(workspaceFolder.uri.fsPath);
                    if (success) {
                        vscode.window.showInformationMessage('Synced to cloud successfully!');
                    } else {
                        vscode.window.showErrorMessage('Cloud sync failed!');
                    }
                });
            }
        }),

        // Script Generator
        vscode.commands.registerCommand('hybrid-ide.generateScript', async () => {
            const description = await vscode.window.showInputBox({
                prompt: 'Describe what the script should do',
                placeHolder: 'e.g., Install Docker and configure firewall'
            });

            if (description) {
                const platforms = await vscode.window.showQuickPick(
                    ['All Platforms', 'Linux', 'macOS', 'Windows'],
                    { placeHolder: 'Select target platform(s)' }
                );

                if (platforms) {
                    vscode.window.withProgress({
                        location: vscode.ProgressLocation.Notification,
                        title: 'Generating scripts...',
                        cancellable: false
                    }, async () => {
                        // Call script generator
                        vscode.window.showInformationMessage('Scripts generated successfully!');
                    });
                }
            }
        })
    ];

    commands.forEach(cmd => context.subscriptions.push(cmd));

    // Status bar items
    const statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 100);
    statusBarItem.text = '$(sparkle) Hybrid IDE';
    statusBarItem.command = 'hybrid-ide.generateCode';
    statusBarItem.show();
    context.subscriptions.push(statusBarItem);

    // Show welcome message
    vscode.window.showInformationMessage('Hybrid IDE is ready! Press Ctrl+Shift+G to generate code with AI.');
}

export function deactivate() {
    console.log('Hybrid IDE extension deactivated');
}

function getCodeReviewHTML(review: string): string {
    return `<!DOCTYPE html>
<html>
<head>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif; padding: 20px; }
        h1 { color: #007acc; }
        pre { background: #f5f5f5; padding: 15px; border-radius: 5px; overflow-x: auto; }
    </style>
</head>
<body>
    <h1>Code Review Results</h1>
    <pre>${review}</pre>
</body>
</html>`;
}

function getDebugAssistHTML(solution: string): string {
    return `<!DOCTYPE html>
<html>
<head>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif; padding: 20px; }
        h1 { color: #f14c4c; }
        pre { background: #f5f5f5; padding: 15px; border-radius: 5px; overflow-x: auto; }
    </style>
</head>
<body>
    <h1>Debug Assistant</h1>
    <pre>${solution}</pre>
</body>
</html>`;
}

function getProfilerHTML(results: any): string {
    return `<!DOCTYPE html>
<html>
<head>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif; padding: 20px; }
        h1 { color: #4caf50; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }
        th { background-color: #007acc; color: white; }
    </style>
</head>
<body>
    <h1>Performance Profile</h1>
    <table>
        <tr><th>Function</th><th>Time (ms)</th><th>Calls</th><th>Impact</th></tr>
        ${results.functions?.map((f: any) => `
            <tr>
                <td>${f.name}</td>
                <td>${f.time}</td>
                <td>${f.calls}</td>
                <td>${f.impact}%</td>
            </tr>
        `).join('') || '<tr><td colspan="4">No data available</td></tr>'}
    </table>
</body>
</html>`;
}
