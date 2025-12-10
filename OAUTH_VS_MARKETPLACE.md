# Creating Your Own GitHub OAuth App (Free & Easy!)

## You DON'T Need a Marketplace App

GitHub Marketplace apps are for **public distribution**. For the Hybrid IDE, you just need a **personal OAuth App** which is:

✅ **100% Free**  
✅ **No approval needed**  
✅ **Takes 2 minutes**  
✅ **Works immediately**  
✅ **Full OAuth 2.0 support**  

---

## Quick Setup (2 Minutes)

### Step 1: Create Your OAuth App

1. **Go directly to:**
   ```
   https://github.com/settings/applications/new
   ```

2. **Or navigate:**
   - GitHub → Your profile picture (top right)
   - Settings → Developer settings → OAuth Apps → New OAuth App

3. **Fill in the form:**

   | Field | Value |
   |-------|-------|
   | **Application name** | `Hybrid IDE` or `My Hybrid IDE` |
   | **Homepage URL** | `https://github.com/Zachman22/codespaces-blank` |
   | **Application description** | `Advanced IDE with AI features for local development` |
   | **Authorization callback URL** | `http://localhost:8888/callback` |

4. **Click "Register application"**

### Step 2: Get Your Client ID

After registration, you'll immediately see:

```
Client ID: Iv1.abc123def456789
```

**Copy this!** This is your personal OAuth App Client ID.

### Step 3: Configure Hybrid IDE

**Option A: Use the setup script (easiest)**
```bash
cd /workspaces/codespaces-blank/hybrid-ide
./setup_oauth.sh
# Paste your Client ID when prompted
```

**Option B: Set environment variable manually**
```bash
# Linux/Mac
export GITHUB_CLIENT_ID="Iv1.abc123def456789"
echo 'export GITHUB_CLIENT_ID="Iv1.abc123def456789"' >> ~/.bashrc

# Windows PowerShell
$env:GITHUB_CLIENT_ID="Iv1.abc123def456789"
[Environment]::SetEnvironmentVariable("GITHUB_CLIENT_ID", "Iv1.abc123def456789", "User")
```

### Step 4: Test It!

```bash
cd build/bin
./SetupAuth
```

**You should see:**
- ✅ Browser opens to https://github.com/login/device
- ✅ "Enter the code displayed on your device"
- ✅ Enter your code
- ✅ "Authorize Hybrid IDE" button
- ✅ Success!

---

## Why Personal OAuth App vs Marketplace?

| Feature | Personal OAuth App | Marketplace App |
|---------|-------------------|-----------------|
| **Cost** | FREE | FREE (but requires approval) |
| **Setup Time** | 2 minutes | Days/weeks for approval |
| **Approval Needed** | NO | YES (GitHub review process) |
| **Use Case** | Personal use, team use | Public distribution |
| **Installation** | Automatic | Users install from Marketplace |
| **Permissions** | You control | Must be approved by GitHub |
| **Best For** | **Hybrid IDE (your use)** | Public SaaS apps |

---

## Your OAuth App vs Marketplace Apps

**What you saw on Marketplace:**
- Those are **published apps** for public use
- Example: Heroku, CircleCI, CodeClimate
- They went through GitHub's approval process
- Users install them from Marketplace

**What you need:**
- A **personal OAuth App** (what we're using)
- For your own use or your team
- No approval needed
- Works immediately
- Same OAuth 2.0 functionality

---

## Understanding OAuth App Types

### 1. Personal OAuth App (What You Need) ✅

**Create at:** https://github.com/settings/applications/new

**Features:**
- OAuth 2.0 Device Flow ✅
- Access user data ✅
- Access repos ✅
- Immediate use ✅
- No approval needed ✅

**Perfect for:**
- Desktop applications (Hybrid IDE)
- CLI tools
- Local development
- Personal projects
- Team tools

### 2. GitHub App (Alternative)

**Create at:** https://github.com/settings/apps/new

**Features:**
- More granular permissions
- Better for automation
- Can act as a bot
- More complex setup

**Use if:**
- Building integrations
- Need organization-level access
- Want fine-grained permissions

### 3. Marketplace App (Not Needed)

**Requires:**
- GitHub review
- Terms acceptance
- Public listing

**Only use if:**
- Distributing to public
- Building SaaS product
- Want Marketplace listing

---

## Complete Working Example

Here's exactly what to do:

### 1. Create OAuth App (1 minute)

Visit: https://github.com/settings/applications/new

Fill in:
```
Name: Hybrid IDE Dev
Homepage: https://github.com/Zachman22/codespaces-blank
Description: AI-powered IDE for coding
Callback: http://localhost:8888/callback
```

Click "Register application"

You'll see:
```
✅ Application created successfully!

Client ID: Iv1.a1b2c3d4e5f6g7h8
Client secret: [Generate if needed]
```

### 2. Save Client ID (30 seconds)

```bash
export GITHUB_CLIENT_ID="Iv1.a1b2c3d4e5f6g7h8"
echo 'export GITHUB_CLIENT_ID="Iv1.a1b2c3d4e5f6g7h8"' >> ~/.bashrc
source ~/.bashrc
```

### 3. Test (30 seconds)

```bash
cd /workspaces/codespaces-blank/hybrid-ide/build/bin
./SetupAuth
```

Choose option 2 (Numeric PIN)

You'll see:
```
🔐 Your Login PIN:
    ╔═══════════╗
    ║  842756  ║
    ╚═══════════╝

🌐 Opening browser automatically...
```

Browser opens → Enter code → Authorize → Done! ✅

---

## Troubleshooting

### "I don't see OAuth Apps in settings"

Make sure you're here:
```
https://github.com/settings/developers
```

You should see tabs:
- OAuth Apps
- GitHub Apps
- Personal access tokens

Click "OAuth Apps" → "New OAuth App"

### "What's the difference between OAuth Apps and GitHub Apps?"

**OAuth Apps** (what you need):
- Traditional OAuth 2.0
- User-centric authentication
- Perfect for desktop apps like Hybrid IDE

**GitHub Apps** (more advanced):
- Newer, more granular
- Organization-centric
- Better for automation/bots

**For Hybrid IDE:** Use OAuth Apps ✅

### "Do I need to publish to Marketplace?"

**NO!** Only if you want to:
- Distribute to millions of users
- List in GitHub Marketplace
- Build a commercial product

For personal use or team use: Just create an OAuth App!

### "Can I use the same OAuth App on multiple machines?"

**YES!** Your OAuth App Client ID can be used:
- On all your computers
- By your team members
- In multiple environments
- Unlimited installations

Just share the Client ID with your team:
```bash
export GITHUB_CLIENT_ID="Iv1.YOUR_CLIENT_ID"
```

---

## Security Best Practices

### ✅ Safe to Share:
- OAuth App Client ID (public identifier)
- OAuth App name
- Homepage URL
- Callback URL

### ❌ Never Share:
- Personal Access Tokens (`ghp_*`)
- OAuth Client Secret (if generated)
- Access tokens (after authorization)

### ⚠️ Team Sharing:
- Client ID: Safe to share with team ✅
- Client Secret: Not needed for Device Flow ✅
- Each user authorizes individually ✅

---

## FAQ

**Q: Is this the right way to do OAuth?**  
A: Yes! Personal OAuth Apps are the standard way for desktop/CLI applications.

**Q: Will this work for my team?**  
A: Yes! Share your Client ID with your team. Each person authorizes individually.

**Q: Do I need to pay?**  
A: No! OAuth Apps are completely free, no limits.

**Q: Can GitHub revoke my app?**  
A: Only if you violate GitHub's terms of service. Normal use is fine.

**Q: What permissions should I grant?**  
A: The app requests: `repo`, `user`, `read:org` - these are standard for IDE functionality.

**Q: Can I rename my OAuth App later?**  
A: Yes! Edit at https://github.com/settings/developers

**Q: What if I lose my Client ID?**  
A: Just go to https://github.com/settings/developers and copy it again.

---

## Alternative: Use Personal Access Token

If you don't want to create an OAuth App at all:

### Quick Token Method (1 minute)

1. Visit: https://github.com/settings/tokens
2. Click "Generate new token (classic)"
3. Name: `Hybrid IDE`
4. Scopes: ✅ `repo`, ✅ `user`, ✅ `read:org`
5. Generate and copy token (`ghp_*`)

6. Save it:
```bash
mkdir -p ~/.hybridide/auth
echo "ghp_YOUR_TOKEN" > ~/.hybridide/auth/github_token
chmod 600 ~/.hybridide/auth/github_token
```

7. Done! Run `./SetupAuth`

**Pros:**
- Even faster setup (1 minute)
- No OAuth App needed
- Works immediately

**Cons:**
- Token expires (30-90 days)
- Less secure (token is credential)
- No nice authorization flow

---

## Summary

**What you need:** Personal OAuth App (NOT Marketplace)

**How to create:** https://github.com/settings/applications/new

**Time required:** 2 minutes

**Cost:** FREE

**Approval needed:** NO

**Works immediately:** YES

**Use Marketplace?** Only if distributing publicly (not needed for Hybrid IDE)

---

## Quick Commands Recap

```bash
# 1. Create OAuth App at:
#    https://github.com/settings/applications/new

# 2. Configure Hybrid IDE
export GITHUB_CLIENT_ID="Iv1.YOUR_CLIENT_ID"
echo 'export GITHUB_CLIENT_ID="Iv1.YOUR_CLIENT_ID"' >> ~/.bashrc

# 3. Run setup
cd /workspaces/codespaces-blank/hybrid-ide
./setup_oauth.sh

# 4. Test
cd build/bin
./SetupAuth
```

**That's it!** No Marketplace needed. ✅

---

## More Help

- **OAuth Apps:** https://docs.github.com/en/apps/oauth-apps/building-oauth-apps/creating-an-oauth-app
- **Device Flow:** https://docs.github.com/en/apps/oauth-apps/building-oauth-apps/authorizing-oauth-apps#device-flow
- **Setup Guide:** See `OAUTH_SETUP_GUIDE.md` in this repo

---

**TL;DR:** Create a free personal OAuth App at https://github.com/settings/applications/new - no Marketplace needed! Takes 2 minutes.
