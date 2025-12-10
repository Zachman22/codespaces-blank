# Hybrid IDE - Post-Clone Setup

Thank you for cloning the Hybrid IDE repository!

## 🔐 Required: GitHub Authentication

To verify your installation and unlock all features, please authenticate with GitHub:

### Quick Setup (1 minute)

1. **Build the setup tool:**
   ```bash
   cd hybrid-ide/build
   cmake ..
   make SetupAuth
   ```

2. **Run authentication:**
   ```bash
   ./bin/SetupAuth
   ```

3. **Follow the prompts:**
   - Click the link displayed
   - Enter the verification code shown
   - Authorize Hybrid IDE in your browser
   - Done! ✅

### What This Does

**Verifies:**
- You have a GitHub account
- You've legally cloned the repository
- You agree to the license terms

**Enables:**
- ✅ Cloud sync features
- ✅ Private repository access
- ✅ Collaboration tools
- ✅ AI-powered code generation
- ✅ Automatic updates

**Security:**
- 🔒 Your token is stored locally only
- 🔒 Never shared with third parties
- 🔒 Can be revoked anytime at github.com/settings/tokens
- 🔒 Uses GitHub's official OAuth Device Flow

### Alternative: Skip Authentication

You can use Hybrid IDE without authentication, but some features will be limited:

```bash
# Build and run without authentication
make HybridIDE
./bin/HybridIDE
```

**Limited features:**
- ❌ No cloud sync
- ❌ No collaboration
- ❌ No private repo access
- ✅ Local development works
- ✅ Compiler and debugger work
- ✅ Most IDE features available

---

## 🚀 After Authentication

Once authenticated, you can:

```bash
# Launch full IDE
./bin/HybridIDE_Complete

# Use AI code assistant
./bin/AutoCoderChatbot

# Generate scripts
./bin/ScriptGeneratorChatbot

# Check authentication status
./bin/SetupAuth
```

---

## ❓ FAQ

**Q: Why do I need to authenticate?**
A: To verify you're a legitimate user and enable cloud features.

**Q: What data is collected?**
A: Only your GitHub username and email. No code is ever uploaded without your explicit permission.

**Q: Can I revoke access?**
A: Yes! Visit https://github.com/settings/applications and revoke "Hybrid IDE"

**Q: Is this required?**
A: No, but highly recommended for full functionality.

**Q: Is my token secure?**
A: Yes! It's stored with file permissions set to owner-only (chmod 600).

---

## 📞 Support

Having issues? 
- Check: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
- Issues: https://github.com/Zachman22/codespaces-blank/issues
- Docs: See README.md

**Happy coding! 🎉**
