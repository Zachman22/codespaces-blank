# GitHub Authentication Fix for Windows

## Problem
You're seeing this error:
```
remote: Invalid username or token. Password authentication is not supported for Git operations.
fatal: Authentication failed for 'https://github.com/yourusername/hybrid-ide.git/'
```

GitHub stopped accepting passwords in August 2021. You need to use a **Personal Access Token (PAT)** instead.

---

## Solution: Create and Use a Personal Access Token

### Step 1: Create a Personal Access Token on GitHub

1. **Go to GitHub Settings:**
   - Visit: https://github.com/settings/tokens
   - Or navigate: GitHub → Click your profile picture → Settings → Developer settings → Personal access tokens → Tokens (classic)

2. **Generate New Token:**
   - Click **"Generate new token"** → **"Generate new token (classic)"**
   
3. **Configure the Token:**
   - **Note:** "Hybrid IDE Git Access" (or any description)
   - **Expiration:** Choose duration (90 days recommended, or "No expiration" for convenience)
   - **Select scopes:** Check these boxes:
     - ✅ **repo** (Full control of private repositories)
     - ✅ **workflow** (Update GitHub Action workflows)
     - ✅ **write:packages** (Upload packages to GitHub Package Registry)
     - ✅ **delete:packages** (Delete packages from GitHub Package Registry)
     - ✅ **admin:org** (if working with organizations)
     - ✅ **user** (Update user data)

4. **Generate and Copy:**
   - Click **"Generate token"** at the bottom
   - **IMPORTANT:** Copy the token immediately! Format: `ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx`
   - You won't be able to see it again!

---

### Step 2: Use Token in Git (Windows PowerShell)

**Option A: Use Token as Password (Quick Fix)**

When Git prompts for credentials:
```powershell
Username for 'https://github.com': zachman22
Password for 'https://zachman22@github.com': [PASTE YOUR TOKEN HERE]
```

**Option B: Update Remote URL with Token Embedded**

```powershell
# Navigate to your project
cd C:\Users\zachg\Downloads\hybrid-ide

# Update the remote URL with your token
git remote set-url origin https://zachman22:YOUR_TOKEN_HERE@github.com/Zachman22/codespaces-blank.git

# Or use this format
git remote set-url origin https://YOUR_TOKEN_HERE@github.com/Zachman22/codespaces-blank.git

# Verify the change
git remote -v
```

**Option C: Use Git Credential Manager (Recommended for Windows)**

```powershell
# Configure Git to use Credential Manager
git config --global credential.helper wincred

# Or use the newer Git Credential Manager Core
git config --global credential.helper manager-core

# Next time you push, enter:
# Username: zachman22
# Password: [YOUR TOKEN]
# Windows will store it securely
```

---

### Step 3: Push to GitHub

```powershell
# Navigate to your project
cd C:\Users\zachg\Downloads\hybrid-ide

# Check status
git status

# Add all files
git add .

# Commit changes
git commit -m "Complete Hybrid IDE Version 2.0 - All features"

# Push to GitHub (will prompt for credentials if not cached)
git push origin main
```

---

## Alternative: Use SSH Instead of HTTPS

If you prefer SSH authentication (more secure, no token needed):

### 1. Generate SSH Key (if you don't have one)

```powershell
# Generate SSH key
ssh-keygen -t ed25519 -C "your_email@example.com"

# Press Enter to accept default location
# Enter a passphrase (optional but recommended)

# Start SSH agent
ssh-agent

# Add your key
ssh-add ~/.ssh/id_ed25519
```

### 2. Add SSH Key to GitHub

```powershell
# Copy your public key
Get-Content ~/.ssh/id_ed25519.pub | Set-Clipboard
```

Then:
1. Go to: https://github.com/settings/keys
2. Click **"New SSH key"**
3. Title: "Windows PC"
4. Paste the key
5. Click **"Add SSH key"**

### 3. Change Remote to SSH

```powershell
cd C:\Users\zachg\Downloads\hybrid-ide

# Change remote from HTTPS to SSH
git remote set-url origin git@github.com:Zachman22/codespaces-blank.git

# Verify
git remote -v

# Test connection
ssh -T git@github.com
# Should see: "Hi zachman22! You've successfully authenticated..."

# Now push
git push origin main
```

---

## Quick Reference Commands

**Check current remote:**
```powershell
git remote -v
```

**Update remote URL:**
```powershell
# HTTPS with token
git remote set-url origin https://YOUR_TOKEN@github.com/Zachman22/codespaces-blank.git

# SSH
git remote set-url origin git@github.com:Zachman22/codespaces-blank.git
```

**Clear cached credentials:**
```powershell
git config --global --unset credential.helper
```

**Store credentials permanently (after entering token once):**
```powershell
git config --global credential.helper store
```

---

## Security Best Practices

1. ✅ **Never share your Personal Access Token** - treat it like a password
2. ✅ **Set expiration dates** on tokens (90 days recommended)
3. ✅ **Use minimal scopes** - only check permissions you need
4. ✅ **Revoke old tokens** - go to https://github.com/settings/tokens and delete unused ones
5. ✅ **Use SSH keys for long-term** - more secure than HTTPS tokens
6. ✅ **Don't commit tokens to code** - keep them in environment variables

---

## Troubleshooting

**Error: "Could not resolve host"**
```powershell
# Check internet connection
ping github.com
```

**Error: "Repository not found"**
```powershell
# Verify repository URL
git remote -v

# Update to correct URL
git remote set-url origin https://github.com/Zachman22/codespaces-blank.git
```

**Error: "Permission denied (publickey)"** (SSH)
```powershell
# Test SSH connection
ssh -T git@github.com

# If fails, add key again
ssh-add ~/.ssh/id_ed25519
```

---

## Need Help?

If you're still having issues:

1. **Verify your GitHub username:** https://github.com/Zachman22
2. **Check repository exists:** https://github.com/Zachman22/codespaces-blank
3. **Verify you have push access** to the repository
4. **Check firewall/antivirus** isn't blocking Git
5. **Try using GitHub Desktop** as an alternative GUI

---

**Recommended:** Use **Option B** (embed token in URL) or **Option C** (Git Credential Manager) for the easiest Windows experience.

For maximum security, use **SSH authentication** instead of HTTPS.
