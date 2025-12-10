#!/bin/bash

# GitHub OAuth Quick Setup Script
# This helps you configure your GitHub OAuth Client ID

echo ""
echo "╔═══════════════════════════════════════════════════════════════╗"
echo "║      GitHub OAuth Setup - Fix Authentication Issue            ║"
echo "╚═══════════════════════════════════════════════════════════════╝"
echo ""

# Check if already configured
if [ ! -z "$GITHUB_CLIENT_ID" ]; then
    echo "✅ GITHUB_CLIENT_ID is already set: $GITHUB_CLIENT_ID"
    echo ""
    read -p "Do you want to change it? (y/N): " change
    if [[ ! $change =~ ^[Yy]$ ]]; then
        echo "Keeping current Client ID"
        exit 0
    fi
fi

echo "To fix the username/password prompt, you need a GitHub OAuth App."
echo ""
echo "📋 STEP 1: Create GitHub OAuth App"
echo "   1. Visit: https://github.com/settings/developers"
echo "   2. Click 'New OAuth App'"
echo "   3. Fill in:"
echo "      - Name: Hybrid IDE"
echo "      - Homepage: https://github.com/Zachman22/codespaces-blank"
echo "      - Callback: http://localhost:8888/callback"
echo "   4. Click 'Register application'"
echo "   5. Copy your Client ID (starts with Iv1.)"
echo ""
echo "📋 STEP 2: Enter Your Client ID Below"
echo ""

read -p "Enter your GitHub OAuth Client ID: " CLIENT_ID

# Validate format
if [[ ! $CLIENT_ID =~ ^Iv1\. ]]; then
    echo ""
    echo "❌ Invalid Client ID format!"
    echo "   GitHub OAuth Client IDs start with 'Iv1.'"
    echo "   Example: Iv1.a1b2c3d4e5f6g7h8"
    echo ""
    exit 1
fi

# Set environment variable for current session
export GITHUB_CLIENT_ID="$CLIENT_ID"

# Add to shell profile
SHELL_RC=""
if [ -f "$HOME/.bashrc" ]; then
    SHELL_RC="$HOME/.bashrc"
elif [ -f "$HOME/.zshrc" ]; then
    SHELL_RC="$HOME/.zshrc"
elif [ -f "$HOME/.profile" ]; then
    SHELL_RC="$HOME/.profile"
fi

if [ ! -z "$SHELL_RC" ]; then
    # Check if already exists
    if grep -q "GITHUB_CLIENT_ID" "$SHELL_RC"; then
        echo ""
        echo "⚠️  GITHUB_CLIENT_ID already in $SHELL_RC"
        read -p "Update it? (Y/n): " update
        if [[ ! $update =~ ^[Nn]$ ]]; then
            # Remove old line and add new one
            grep -v "GITHUB_CLIENT_ID" "$SHELL_RC" > "${SHELL_RC}.tmp"
            mv "${SHELL_RC}.tmp" "$SHELL_RC"
            echo "export GITHUB_CLIENT_ID=\"$CLIENT_ID\"" >> "$SHELL_RC"
            echo "✅ Updated $SHELL_RC"
        fi
    else
        echo "export GITHUB_CLIENT_ID=\"$CLIENT_ID\"" >> "$SHELL_RC"
        echo "✅ Added to $SHELL_RC for persistence"
    fi
fi

echo ""
echo "════════════════════════════════════════════════════════════════"
echo "✅ Configuration Complete!"
echo ""
echo "Your Client ID: $CLIENT_ID"
echo ""
echo "📋 Next Steps:"
echo ""
echo "1. Rebuild (if needed):"
echo "   cd build && make SetupAuth"
echo ""
echo "2. Run setup:"
echo "   cd bin && ./SetupAuth"
echo ""
echo "3. When browser opens, you should see:"
echo "   ✅ GitHub device authorization page"
echo "   ✅ 'Enter code' input field"
echo "   ❌ NO username/password prompt!"
echo ""
echo "════════════════════════════════════════════════════════════════"
echo ""
echo "Need help? See: OAUTH_SETUP_GUIDE.md"
echo ""
