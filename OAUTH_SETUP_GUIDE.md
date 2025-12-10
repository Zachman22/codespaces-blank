# GitHub OAuth Setup Guide - Fix "Username/Password" Prompt Issue

## Problem

When clicking "Sign in with code" or "Sign in with browser", GitHub asks for username and password instead of showing the device authorization page.

## Root Cause

The authentication is using a **placeholder Client ID** that doesn't correspond to a real GitHub OAuth App. GitHub's Device Flow requires a properly registered OAuth Application.

---

## Solution: Create Your Own GitHub OAuth App

### Step 1: Create a GitHub OAuth App (5 minutes)

1. **Go to GitHub Developer Settings:**
   - Visit: https://github.com/settings/developers
   - Or: GitHub → Settings → Developer settings → OAuth Apps

2. **Click "New OAuth App":**
   - Click the **"New OAuth App"** button

3. **Fill in the Application Details:**
   ```
   Application name: Hybrid IDE
   Homepage URL: https://github.com/Zachman22/codespaces-blank
   Application description: Advanced IDE with AI-powered features
   Authorization callback URL: http://localhost:8888/callback
   ```

4. **Register the Application:**
   - Click **"Register application"**

5. **Copy Your Client ID:**
   - After registration, you'll see your **Client ID**
   - It looks like: `Iv1.a1b2c3d4e5f6g7h8`
   - **IMPORTANT:** Copy this ID!

6. **Generate a Client Secret (Optional):**
   - Click **"Generate a new client secret"**
   - Copy and save it securely (you won't see it again)
   - Note: For Device Flow, you don't strictly need the secret

---

### Step 2: Update Hybrid IDE with Your Client ID

**Option A: Environment Variable (Recommended)**

```bash
# Linux/Mac
export GITHUB_CLIENT_ID="Iv1.YOUR_CLIENT_ID_HERE"

# Windows PowerShell
$env:GITHUB_CLIENT_ID="Iv1.YOUR_CLIENT_ID_HERE"

# Add to shell profile for persistence
echo 'export GITHUB_CLIENT_ID="Iv1.YOUR_CLIENT_ID_HERE"' >> ~/.bashrc
```

**Option B: Edit Source Code**

Edit the file: `src/github_auth.cpp`

Find line 73:
```cpp
std::string clientId = "Iv1.b507a08c87ecfe98"; // GitHub OAuth App Client ID
```

Replace with your Client ID:
```cpp
std::string clientId = "Iv1.YOUR_ACTUAL_CLIENT_ID"; // Your GitHub OAuth App Client ID
```

Also update line 324 for numeric PIN flow:
```cpp
std::string clientId = "Iv1.YOUR_ACTUAL_CLIENT_ID";
```

**Then rebuild:**
```bash
cd build
make SetupAuth
```

---

### Step 3: Enable Device Flow for Your OAuth App

GitHub OAuth Apps support Device Flow by default, but verify:

1. Go to your OAuth App settings: https://github.com/settings/developers
2. Click on your "Hybrid IDE" app
3. Ensure these settings:
   - ✅ Application name: Hybrid IDE
   - ✅ Authorization callback URL: http://localhost:8888/callback
   - ✅ Device Flow is automatically enabled

---

### Step 4: Test Authentication

```bash
cd /workspaces/codespaces-blank/hybrid-ide/build/bin
./SetupAuth
```

**What Should Happen:**

1. ✅ **Correct Flow:**
   ```
   Choose authentication method:
     1. Browser login with verification code
     2. Numeric PIN login (easier, just numbers!)
   Choice (1-2): 2
   
   🔐 Your Login PIN:
       ╔═══════════╗
       ║  842756  ║
       ╚═══════════╝
   
   📱 EASY LOGIN STEPS:
      1. Visit: https://github.com/login/device
      2. Enter PIN: 842756
      3. Authorize Hybrid IDE
   
   🌐 Opening browser automatically...
   ```

2. ✅ **Browser Opens to:**
   - URL: https://github.com/login/device
   - Shows: "Enter the code displayed on your device"
   - Input field for the code (WDJB-MJHT or 842756)
   - **NO username/password prompt!**

3. ✅ **After Entering Code:**
   - Shows: "Authorize Hybrid IDE"
   - Lists permissions being requested
   - Button: "Authorize [Your Username]"
   - Click authorize → Success!

---

## Troubleshooting

### Issue 1: Still Asks for Username/Password

**Cause:** Using invalid/placeholder Client ID

**Solution:**
1. Verify you created a GitHub OAuth App
2. Check the Client ID is correct (starts with `Iv1.`)
3. Ensure you rebuilt after updating the code
4. Try clearing browser cache/cookies

**Quick Test:**
```bash
# Test if your Client ID works
curl -X POST https://github.com/login/device/code \
  -H "Accept: application/json" \
  -d "client_id=YOUR_CLIENT_ID&scope=repo"

# Should return JSON with device_code and user_code
# If it returns an error, your Client ID is invalid
```

---

### Issue 2: "Application Not Found" Error

**Cause:** Client ID doesn't exist or is typo'd

**Solution:**
1. Double-check your Client ID from GitHub settings
2. Ensure no extra spaces or characters
3. Verify the OAuth App still exists in your GitHub account

---

### Issue 3: "Redirect URI Mismatch"

**Cause:** OAuth App callback URL doesn't match

**Solution:**
1. Go to https://github.com/settings/developers
2. Edit your OAuth App
3. Set callback URL to: `http://localhost:8888/callback`
4. Save changes

---

### Issue 4: Browser Opens but Shows Error Page

**Cause:** Network issue or GitHub is down

**Solution:**
1. Check internet connection
2. Try: https://www.githubstatus.com/
3. Wait a few minutes and try again
4. Use manual code entry

---

### Issue 5: "This application is not configured for Device Flow"

**Cause:** Old GitHub OAuth App type

**Solution:**
1. Device Flow is enabled by default for all OAuth Apps created after 2021
2. If using an old app, create a new one following Step 1
3. Modern GitHub OAuth Apps automatically support Device Flow

---

## Alternative: Use GitHub Personal Access Token (PAT)

If you can't create an OAuth App or prefer simpler authentication:

### Option 1: Classic PAT Method

1. **Generate a Token:**
   - Go to: https://github.com/settings/tokens
   - Click "Generate new token (classic)"
   - Give it a name: "Hybrid IDE"
   - Select scopes:
     - ✅ repo (all)
     - ✅ user (all)
     - ✅ read:org
   - Click "Generate token"
   - **Copy the token!** (starts with `ghp_`)

2. **Use Token Directly:**
   ```bash
   # Linux/Mac
   mkdir -p ~/.hybridide/auth
   echo "ghp_YOUR_TOKEN_HERE" > ~/.hybridide/auth/github_token
   chmod 600 ~/.hybridide/auth/github_token
   
   # Windows PowerShell
   mkdir $env:APPDATA\HybridIDE\auth -Force
   "ghp_YOUR_TOKEN_HERE" | Out-File -FilePath "$env:APPDATA\HybridIDE\auth\github_token" -NoNewline
   ```

3. **Test:**
   ```bash
   ./SetupAuth
   # Should detect existing token and authenticate automatically
   ```

---

### Option 2: Fine-Grained PAT (More Secure)

1. Go to: https://github.com/settings/tokens?type=beta
2. Click "Generate new token"
3. Configure:
   - Name: "Hybrid IDE"
   - Expiration: 90 days (recommended)
   - Repository access: "All repositories" or select specific ones
   - Permissions:
     - Repository: Read and Write
     - Account: Read
     - Organization: Read
4. Generate and copy token
5. Use as shown in Option 1

---

## Complete Working Example

### 1. Create OAuth App
```
Name: Hybrid IDE Dev
URL: https://github.com/yourusername/hybrid-ide
Callback: http://localhost:8888/callback
Result: Client ID = Iv1.abc123def456
```

### 2. Set Environment Variable
```bash
export GITHUB_CLIENT_ID="Iv1.abc123def456"
```

### 3. Update Code (if not using env var)
```cpp
// src/github_auth.cpp line 73
std::string clientId = getenv("GITHUB_CLIENT_ID") 
    ? getenv("GITHUB_CLIENT_ID") 
    : "Iv1.abc123def456";
```

### 4. Rebuild
```bash
cd build
cmake ..
make SetupAuth -j4
```

### 5. Test
```bash
./bin/SetupAuth
# Choose option 2 (Numeric PIN)
# Browser opens → Enter code → Authorize → Success!
```

---

## Quick Fix Script

Save this as `fix_github_auth.sh`:

```bash
#!/bin/bash

echo "🔧 GitHub OAuth Quick Fix"
echo "=========================="
echo ""
echo "Enter your GitHub OAuth App Client ID:"
echo "(Get it from: https://github.com/settings/developers)"
read -p "Client ID (starts with Iv1.): " CLIENT_ID

if [[ ! $CLIENT_ID =~ ^Iv1\. ]]; then
    echo "❌ Invalid Client ID format. Should start with 'Iv1.'"
    exit 1
fi

echo ""
echo "Setting environment variable..."
export GITHUB_CLIENT_ID="$CLIENT_ID"
echo "export GITHUB_CLIENT_ID=\"$CLIENT_ID\"" >> ~/.bashrc

echo ""
echo "Rebuilding Hybrid IDE..."
cd "$(dirname "$0")/build"
make SetupAuth -j4

echo ""
echo "✅ Fix complete! Run: ./bin/SetupAuth"
echo ""
```

Make executable and run:
```bash
chmod +x fix_github_auth.sh
./fix_github_auth.sh
```

---

## Understanding the Authentication Flow

### Device Flow (How It Should Work)

```
1. App requests device code from GitHub
   ├─ POST https://github.com/login/device/code
   ├─ Body: client_id=YOUR_ID&scope=repo,user
   └─ Response: {device_code, user_code, verification_uri}

2. User visits verification_uri in browser
   ├─ URL: https://github.com/login/device
   ├─ Enters user_code (e.g., WDJB-MJHT or 842756)
   └─ GitHub shows "Authorize [App Name]" page

3. User authorizes app
   ├─ Clicks "Authorize" button
   └─ GitHub grants permissions

4. App polls for token
   ├─ POST https://github.com/login/oauth/access_token
   ├─ Body: client_id=YOUR_ID&device_code=...
   └─ Response: {access_token, token_type, scope}

5. Success!
   └─ Token saved locally, user authenticated
```

### What's Going Wrong Now

```
Current Issue:
├─ Using placeholder Client ID: "Iv1.b507a08c87ecfe98"
├─ GitHub doesn't recognize this ID
├─ Falls back to basic auth (username/password)
└─ Device Flow doesn't activate

Fix:
├─ Create real OAuth App on GitHub
├─ Use your actual Client ID
├─ Device Flow works correctly
└─ No username/password needed!
```

---

## Security Notes

✅ **Safe to Share:**
- OAuth App Client ID (public identifier)
- Device codes (temporary, expire in 15 minutes)
- User codes (temporary, single-use)

❌ **Never Share:**
- Personal Access Tokens (starts with `ghp_` or `github_pat_`)
- OAuth Client Secrets (if generated)
- Access tokens (returned after authorization)

---

## Summary

**Problem:** Placeholder Client ID causes username/password prompt  
**Solution:** Create GitHub OAuth App and use your real Client ID  
**Time:** 5 minutes to set up  
**Result:** Browser-based authentication works perfectly!

**Quick Steps:**
1. Create OAuth App: https://github.com/settings/developers
2. Copy Client ID
3. Set `GITHUB_CLIENT_ID` environment variable
4. Run `./SetupAuth`
5. Done! No more password prompts!

---

## Need Help?

- **GitHub OAuth Docs:** https://docs.github.com/en/apps/oauth-apps/building-oauth-apps/authorizing-oauth-apps#device-flow
- **Device Flow Spec:** https://datatracker.ietf.org/doc/html/rfc8628
- **Open an Issue:** https://github.com/Zachman22/codespaces-blank/issues

---

**Made authentication work? ⭐ Star the repo!**
