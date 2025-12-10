# 🔢 Numeric PIN Login - Easy GitHub Authentication

## Overview

Hybrid IDE now supports **Numeric PIN Login** - an easier way to authenticate with GitHub using just numbers instead of alphanumeric codes!

## Why Numeric PIN?

✅ **Easier to type** - Just 6 digits instead of complex codes like "WDJB-MJHT"  
✅ **Faster authentication** - No need to distinguish between similar characters  
✅ **Mobile-friendly** - Easy to enter on phone keyboards  
✅ **Accessible** - Better for users with dyslexia or visual impairments  
✅ **Universal** - Numbers work on any keyboard layout  

---

## How to Use Numeric PIN Login

### Method 1: During Setup (Recommended)

When you run the setup tool, you'll see a choice:

```bash
./bin/SetupAuth
```

**You'll be prompted:**
```
Choose authentication method:
  1. Browser login with verification code
  2. Numeric PIN login (easier, just numbers!)
Choice (1-2): 2
```

**Enter `2` for Numeric PIN login!**

---

### Method 2: From Your Code

If you're integrating authentication into your application:

```cpp
#include "github_auth.h"

GitHubAuth auth;

// Use numeric PIN instead of device flow
if (auth.startNumericPinFlow()) {
    std::cout << "Authenticated successfully!\n";
    auto user = auth.getUserInfo();
    std::cout << "Welcome " << user.login << "!\n";
}
```

---

## What Happens During Numeric PIN Login

### Step 1: PIN Generation
The system generates a unique 6-digit PIN:

```
🔐 Your Login PIN:

    ╔═══════════╗
    ║  842756  ║
    ╚═══════════╝
```

### Step 2: Browser Opens
Your default browser automatically opens to: `https://github.com/login/device`

### Step 3: Enter PIN
Simply type the 6 digits shown:

```
📱 EASY LOGIN STEPS:
   1. Visit: https://github.com/login/device
   2. Enter PIN: 842756
   3. Authorize Hybrid IDE
```

### Step 4: Choose Your Method

You have multiple options:

```
Choose authentication method:
  1 - I've entered the PIN (auto-detect)
  2 - Manual confirmation after login
  842756 - Copy PIN to clipboard
  [Enter] - Auto-wait
```

**Option 1**: Auto-detect (system polls GitHub automatically)  
**Option 2**: Manual confirm (you press Enter after logging in)  
**Type the PIN**: Copies it for easy pasting  
**Just press Enter**: Default auto-wait behavior  

### Step 5: Automatic Verification

```
⏳ Verifying authentication........

✅ Authentication successful!

👤 Signed in as: Zachman22
📧 Email: user@example.com
🏢 Name: Zachary
```

---

## Comparison: Traditional vs Numeric PIN

| Feature | Traditional Code | Numeric PIN |
|---------|-----------------|-------------|
| **Example** | `WDJB-MJHT` | `842756` |
| **Length** | 8-9 characters | 6 digits |
| **Characters** | Letters + dash | Numbers only |
| **Typo risk** | Medium (similar letters) | Low (distinct digits) |
| **Mobile entry** | Requires letter keyboard | Uses numeric keypad |
| **Speed** | Moderate | Fast |
| **Accessibility** | Lower | Higher |

---

## Interactive Options During Login

### Option 1: Auto-Detect (Recommended)
```
Your choice: 1

⏳ Verifying authentication.....
```
The system automatically polls GitHub every 5 seconds to check if you've authorized.

### Option 2: Manual Confirmation
```
Your choice: 2

⏸️  Press Enter after completing browser authentication...
[User presses Enter after authorizing]
⏳ Verifying authentication...
```
You control when to check for authorization.

### Option 3: Copy PIN
```
Your choice: 842756

✅ PIN copied! Paste it in the browser.
⏳ Verifying authentication.....
```
Typing the PIN copies it to help you paste it in the browser.

### Option 4: Just Press Enter (Default)
```
Your choice: [Enter]

⏳ Auto-detecting authentication.....
```
Same as Option 1 - automatic polling.

---

## Security Features

✅ **Secure Token Storage**: Tokens saved with owner-only permissions (chmod 600)  
✅ **Temporary PINs**: Each PIN is unique and expires after use  
✅ **OAuth 2.0**: Uses GitHub's official Device Authorization Grant  
✅ **Local Storage**: Credentials never leave your machine  
✅ **Automatic Cleanup**: Old tokens automatically cleaned up on logout  

---

## Troubleshooting

### PIN Doesn't Work

**Problem**: Entered PIN but GitHub says "Invalid code"

**Solutions**:
1. Make sure you typed all 6 digits correctly: `842756`
2. Check if the PIN has expired (15-minute limit)
3. Start authentication again to get a new PIN
4. Use the alternative alphanumeric code shown as backup

### Browser Didn't Open

**Problem**: Browser didn't open automatically

**Solutions**:
1. Manually visit: `https://github.com/login/device`
2. Copy/paste the URL from terminal
3. Use Ctrl+Click (Cmd+Click on Mac) on the URL
4. Right-click URL and select "Open Link"

### Authentication Timeout

**Problem**: "Authentication timeout. Please try again."

**Solutions**:
1. Run setup again: `./bin/SetupAuth`
2. Make sure you complete authentication within 15 minutes
3. Check your internet connection
4. Try Option 2 (manual confirmation) for more control

### Already Authenticated

**Problem**: "Already authenticated as: [username]"

**Solutions**:
- Choose Option 1 to continue with current account
- Choose Option 2 to switch accounts
- Choose Option 3 to logout completely

---

## Command Reference

### Run Setup with Numeric PIN
```bash
# Navigate to build directory
cd /workspaces/codespaces-blank/hybrid-ide/build

# Run setup
./bin/SetupAuth

# When prompted, choose option 2
Choose authentication method:
  1. Browser login with verification code
  2. Numeric PIN login (easier, just numbers!)
Choice (1-2): 2
```

### Check Authentication Status
```bash
# Your token is stored here:
# Linux/Mac: ~/.hybridide/auth/github_token
# Windows: %APPDATA%\HybridIDE\auth\github_token

# Check if authenticated (Linux/Mac)
ls -la ~/.hybridide/auth/

# Check if authenticated (Windows PowerShell)
ls $env:APPDATA\HybridIDE\auth\
```

### Logout
```bash
./bin/SetupAuth
# Choose option 3 when already authenticated
```

---

## FAQ

**Q: Is Numeric PIN less secure than alphanumeric codes?**  
A: No! Both use the same OAuth 2.0 Device Authorization Grant. The PIN is just a different display format for the same secure token exchange.

**Q: Can I use the traditional code instead?**  
A: Yes! The system shows both formats. Use whichever you prefer:
- Numeric: `842756`
- Traditional: `ABCDEF-XY`

**Q: How long is the PIN valid?**  
A: PINs expire after 15 minutes for security. Just run setup again if expired.

**Q: Can I reuse a PIN?**  
A: No, each PIN is single-use and expires after successful authentication.

**Q: Does this work on mobile devices?**  
A: Yes! The numeric format is especially convenient on mobile numeric keypads.

**Q: What if I prefer SSH keys?**  
A: You can still use SSH! See `GITHUB_AUTH_FIX.md` for SSH setup instructions.

---

## Technical Details

### PIN Generation
```cpp
// Generate 6-digit PIN
std::srand(std::time(nullptr));
int pin = 100000 + (std::rand() % 900000); // Range: 100000-999999
```

### Token Polling
- **Interval**: Every 5 seconds
- **Max Attempts**: 30 (2.5 minutes total)
- **Timeout**: 15 minutes (GitHub standard)

### Storage Locations
- **Linux/Mac**: `~/.hybridide/auth/`
- **Windows**: `%APPDATA%\HybridIDE\auth\`

### Files Created
- `github_token` - Your access token (chmod 600)
- `user_data.json` - Your GitHub profile info

---

## Examples

### Example 1: First-Time Setup
```
$ ./bin/SetupAuth

Choose authentication method:
  1. Browser login with verification code
  2. Numeric PIN login (easier, just numbers!)
Choice (1-2): 2

🔢 Starting Numeric PIN authentication...

╔════════════════════════════════════════════════════════════════╗
║        GitHub Authentication - Numeric PIN Login               ║
╚════════════════════════════════════════════════════════════════╝

🔐 Your Login PIN:

    ╔═══════════╗
    ║  385942  ║
    ╚═══════════╝

📱 EASY LOGIN STEPS:
   1. Visit: https://github.com/login/device
   2. Enter PIN: 385942
   3. Authorize Hybrid IDE

🌐 Opening browser automatically...

Your choice: 1

⏳ Verifying authentication.........

✅ Authentication successful!

👤 Signed in as: Zachman22
📧 Email: user@example.com
🏢 Name: Zachary
```

### Example 2: Re-authentication
```
$ ./bin/SetupAuth

✅ Already authenticated as: Zachman22
📧 Email: user@example.com

Options:
  1. Continue with current authentication
  2. Re-authenticate with different account
  3. Logout

Choice (1-3): 2

Choose authentication method:
  1. Browser login (standard)
  2. Numeric PIN login (easy)
Choice (1-2): 2

[Same PIN flow as above]
```

---

## Benefits Summary

| Benefit | Description |
|---------|-------------|
| 🎯 **Simplicity** | Only 6 digits to remember/type |
| ⚡ **Speed** | Faster entry, especially on numeric keypads |
| 📱 **Mobile-Friendly** | Easy on phone keyboards |
| ♿ **Accessible** | Better for users with disabilities |
| 🌍 **Universal** | Works on any keyboard layout worldwide |
| 🔒 **Secure** | Same OAuth 2.0 security as traditional method |
| 🚀 **Modern** | Follows banking/2FA PIN conventions |

---

## Related Documentation

- **GitHub Auth Fix**: `GITHUB_AUTH_FIX.md` - Windows authentication troubleshooting
- **Setup Guide**: `SETUP.md` - Complete setup instructions
- **Version 2.0 Features**: `VERSION_2.0_FEATURES.md` - All new features

---

**Made authentication easier? ⭐ Star the repo!**

**Questions?** Open an issue: https://github.com/Zachman22/codespaces-blank/issues
