#!/bin/bash

# OAuth Troubleshooter for Hybrid IDE
# Diagnoses and fixes GitHub authentication issues automatically

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Counters
ISSUES_FOUND=0
ISSUES_FIXED=0

# Header
clear
echo -e "${BLUE}"
echo "╔═══════════════════════════════════════════════════════════════╗"
echo "║         OAuth Troubleshooter for Hybrid IDE                  ║"
echo "║         Automatic Diagnosis & Fix Tool                       ║"
echo "╚═══════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo ""
echo "This tool will:"
echo "  1. Check your OAuth configuration"
echo "  2. Diagnose authentication issues"
echo "  3. Suggest or apply fixes automatically"
echo ""
echo -e "${YELLOW}Starting diagnosis...${NC}"
echo ""
sleep 1

# Function to print test result
print_test() {
    local status=$1
    local message=$2
    
    if [ "$status" == "OK" ]; then
        echo -e "  ${GREEN}✓${NC} $message"
    elif [ "$status" == "WARN" ]; then
        echo -e "  ${YELLOW}⚠${NC} $message"
        ((ISSUES_FOUND++))
    elif [ "$status" == "ERROR" ]; then
        echo -e "  ${RED}✗${NC} $message"
        ((ISSUES_FOUND++))
    else
        echo -e "  ${CYAN}ℹ${NC} $message"
    fi
}

# Function to print section header
print_section() {
    echo ""
    echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
}

# Test 1: Check if curl is available
print_section "TEST 1: System Requirements"
if command -v curl &> /dev/null; then
    print_test "OK" "curl is installed"
else
    print_test "ERROR" "curl is not installed (required for GitHub API calls)"
    echo "      Fix: sudo apt-get install curl (or brew install curl on Mac)"
fi

# Test 2: Check internet connectivity
if ping -c 1 github.com &> /dev/null 2>&1; then
    print_test "OK" "Internet connection is working"
else
    print_test "ERROR" "Cannot reach github.com"
    echo "      Fix: Check your internet connection"
fi

# Test 3: Check if GitHub is accessible
print_section "TEST 2: GitHub API Access"
HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" https://api.github.com/zen)
if [ "$HTTP_CODE" == "200" ]; then
    print_test "OK" "GitHub API is accessible"
else
    print_test "ERROR" "GitHub API returned HTTP $HTTP_CODE"
    echo "      Check: https://www.githubstatus.com/"
fi

# Test 4: Check environment variable
print_section "TEST 3: OAuth Configuration"
if [ -z "$GITHUB_CLIENT_ID" ]; then
    print_test "WARN" "GITHUB_CLIENT_ID environment variable is not set"
    echo "      This means you're using the demo/placeholder Client ID"
    echo "      Result: GitHub will ask for username/password instead of device flow"
    
    read -p "      Would you like to set it now? (Y/n): " setup_env
    if [[ ! $setup_env =~ ^[Nn]$ ]]; then
        echo ""
        echo "      Create your OAuth App at: https://github.com/settings/applications/new"
        echo ""
        read -p "      Enter your Client ID (starts with Iv1.): " CLIENT_ID
        
        if [[ $CLIENT_ID =~ ^Iv1\. ]]; then
            export GITHUB_CLIENT_ID="$CLIENT_ID"
            
            # Add to shell profile
            SHELL_RC=""
            if [ -f "$HOME/.bashrc" ]; then
                SHELL_RC="$HOME/.bashrc"
            elif [ -f "$HOME/.zshrc" ]; then
                SHELL_RC="$HOME/.zshrc"
            fi
            
            if [ ! -z "$SHELL_RC" ]; then
                if ! grep -q "GITHUB_CLIENT_ID" "$SHELL_RC"; then
                    echo "export GITHUB_CLIENT_ID=\"$CLIENT_ID\"" >> "$SHELL_RC"
                    print_test "OK" "GITHUB_CLIENT_ID saved to $SHELL_RC"
                    ((ISSUES_FIXED++))
                fi
            fi
        else
            print_test "ERROR" "Invalid Client ID format (must start with Iv1.)"
        fi
    fi
else
    print_test "OK" "GITHUB_CLIENT_ID is set: ${GITHUB_CLIENT_ID:0:10}..."
    
    # Validate Client ID format
    if [[ $GITHUB_CLIENT_ID =~ ^Iv1\. ]]; then
        print_test "OK" "Client ID format is valid"
    else
        print_test "WARN" "Client ID format looks incorrect (should start with Iv1.)"
    fi
fi

# Test 5: Check if Client ID is valid
print_section "TEST 4: Client ID Validation"
if [ ! -z "$GITHUB_CLIENT_ID" ]; then
    echo "  Testing Client ID with GitHub API..."
    
    RESPONSE=$(curl -s -X POST https://github.com/login/device/code \
        -H "Accept: application/json" \
        -d "client_id=$GITHUB_CLIENT_ID&scope=repo")
    
    if echo "$RESPONSE" | grep -q "device_code"; then
        print_test "OK" "Client ID is valid and recognized by GitHub"
        
        # Extract and display device code info
        USER_CODE=$(echo "$RESPONSE" | grep -o '"user_code":"[^"]*"' | cut -d'"' -f4)
        if [ ! -z "$USER_CODE" ]; then
            print_test "INFO" "Test device code generated: $USER_CODE"
        fi
    elif echo "$RESPONSE" | grep -q "bad_verification_code"; then
        print_test "ERROR" "Client ID is not recognized by GitHub"
        echo "      This Client ID doesn't exist or is invalid"
        echo "      Create a new one at: https://github.com/settings/applications/new"
    else
        print_test "WARN" "Unable to validate Client ID (network issue or rate limit)"
    fi
else
    print_test "WARN" "Skipping (no Client ID configured)"
fi

# Test 6: Check auth directory
print_section "TEST 5: Local Authentication Storage"
if [ -d "$HOME/.hybridide/auth" ]; then
    print_test "OK" "Auth directory exists: ~/.hybridide/auth"
    
    # Check for existing token
    if [ -f "$HOME/.hybridide/auth/github_token" ]; then
        print_test "OK" "Saved token file found"
        
        # Check file permissions
        PERMS=$(stat -c "%a" "$HOME/.hybridide/auth/github_token" 2>/dev/null || stat -f "%A" "$HOME/.hybridide/auth/github_token" 2>/dev/null)
        if [ "$PERMS" == "600" ]; then
            print_test "OK" "Token file has correct permissions (600)"
        else
            print_test "WARN" "Token file permissions are $PERMS (should be 600)"
            
            read -p "      Fix permissions? (Y/n): " fix_perms
            if [[ ! $fix_perms =~ ^[Nn]$ ]]; then
                chmod 600 "$HOME/.hybridide/auth/github_token"
                print_test "OK" "Permissions fixed"
                ((ISSUES_FIXED++))
            fi
        fi
        
        # Check if token is valid
        TOKEN=$(cat "$HOME/.hybridide/auth/github_token")
        if [ ! -z "$TOKEN" ] && [[ $TOKEN =~ ^gh[ps]_ ]]; then
            echo "  Testing token validity..."
            
            USER_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" \
                -H "Accept: application/vnd.github+json" \
                https://api.github.com/user)
            
            if echo "$USER_RESPONSE" | grep -q '"login"'; then
                USERNAME=$(echo "$USER_RESPONSE" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
                print_test "OK" "Token is valid (authenticated as: $USERNAME)"
            else
                print_test "ERROR" "Token is invalid or expired"
                echo "      You need to re-authenticate"
                
                read -p "      Delete invalid token? (Y/n): " del_token
                if [[ ! $del_token =~ ^[Nn]$ ]]; then
                    rm "$HOME/.hybridide/auth/github_token"
                    rm "$HOME/.hybridide/auth/user_data.json" 2>/dev/null
                    print_test "OK" "Invalid token removed"
                    ((ISSUES_FIXED++))
                fi
            fi
        fi
    else
        print_test "INFO" "No saved token found (not authenticated yet)"
    fi
else
    print_test "INFO" "Auth directory doesn't exist yet (will be created on first auth)"
fi

# Test 7: Check executables
print_section "TEST 6: Hybrid IDE Installation"
HYBRID_IDE_DIR="/workspaces/codespaces-blank/hybrid-ide"

if [ -d "$HYBRID_IDE_DIR" ]; then
    print_test "OK" "Hybrid IDE directory found"
    
    if [ -f "$HYBRID_IDE_DIR/build/bin/SetupAuth" ]; then
        print_test "OK" "SetupAuth executable exists"
        
        if [ -x "$HYBRID_IDE_DIR/build/bin/SetupAuth" ]; then
            print_test "OK" "SetupAuth is executable"
        else
            print_test "WARN" "SetupAuth is not executable"
            
            read -p "      Fix permissions? (Y/n): " fix_exec
            if [[ ! $fix_exec =~ ^[Nn]$ ]]; then
                chmod +x "$HYBRID_IDE_DIR/build/bin/SetupAuth"
                print_test "OK" "Made executable"
                ((ISSUES_FIXED++))
            fi
        fi
    else
        print_test "ERROR" "SetupAuth not found. Run: cd build && make SetupAuth"
    fi
else
    print_test "WARN" "Hybrid IDE not found at expected location"
    echo "      Looking in current directory..."
    
    if [ -f "./build/bin/SetupAuth" ]; then
        print_test "OK" "Found SetupAuth in current directory"
    else
        print_test "ERROR" "SetupAuth not built. Run: cd build && cmake .. && make SetupAuth"
    fi
fi

# Test 8: Browser detection
print_section "TEST 7: Browser Availability"
BROWSERS=("$BROWSER" "xdg-open" "sensible-browser" "google-chrome" "firefox" "chromium")
BROWSER_FOUND=0

for browser in "${BROWSERS[@]}"; do
    if command -v $browser &> /dev/null; then
        print_test "OK" "Found browser: $browser"
        BROWSER_FOUND=1
        break
    fi
done

if [ $BROWSER_FOUND -eq 0 ]; then
    print_test "WARN" "No browser command found"
    echo "      Authentication will still work, but you'll need to manually open URLs"
fi

# Test 9: Device Flow test
print_section "TEST 8: Device Flow End-to-End Test"
if [ ! -z "$GITHUB_CLIENT_ID" ]; then
    echo "  Would you like to test the complete authentication flow?"
    read -p "  This will open your browser (y/N): " test_flow
    
    if [[ $test_flow =~ ^[Yy]$ ]]; then
        echo ""
        echo "  Requesting device code from GitHub..."
        
        RESPONSE=$(curl -s -X POST https://github.com/login/device/code \
            -H "Accept: application/json" \
            -d "client_id=$GITHUB_CLIENT_ID&scope=repo,user")
        
        if echo "$RESPONSE" | grep -q "device_code"; then
            USER_CODE=$(echo "$RESPONSE" | grep -o '"user_code":"[^"]*"' | cut -d'"' -f4)
            VERIFICATION_URI=$(echo "$RESPONSE" | grep -o '"verification_uri":"[^"]*"' | cut -d'"' -f4)
            
            print_test "OK" "Device code obtained successfully"
            echo ""
            echo "  ╔═══════════════════════════════════════════════════════════════╗"
            echo "  ║  Your verification code: $USER_CODE"
            echo "  ║  Visit: $VERIFICATION_URI"
            echo "  ╚═══════════════════════════════════════════════════════════════╝"
            echo ""
            echo "  Opening browser..."
            
            if command -v xdg-open &> /dev/null; then
                xdg-open "$VERIFICATION_URI" 2>/dev/null
            elif command -v open &> /dev/null; then
                open "$VERIFICATION_URI" 2>/dev/null
            fi
            
            echo ""
            print_test "OK" "If browser opened and shows device authorization page: SUCCESS!"
            print_test "INFO" "If it asked for username/password: Your Client ID is invalid"
        else
            print_test "ERROR" "Failed to obtain device code"
            echo "      Response: $RESPONSE"
        fi
    fi
else
    print_test "WARN" "Skipping (no Client ID configured)"
fi

# Summary
print_section "DIAGNOSIS SUMMARY"
echo ""

if [ $ISSUES_FOUND -eq 0 ]; then
    echo -e "${GREEN}✓ Perfect! No issues found.${NC}"
    echo ""
    echo "Your OAuth setup is working correctly!"
    echo ""
    echo "To authenticate, run:"
    echo "  cd $HYBRID_IDE_DIR/build/bin"
    echo "  ./SetupAuth"
else
    echo -e "${YELLOW}⚠ Found $ISSUES_FOUND potential issue(s)${NC}"
    
    if [ $ISSUES_FIXED -gt 0 ]; then
        echo -e "${GREEN}✓ Fixed $ISSUES_FIXED issue(s) automatically${NC}"
    fi
    
    echo ""
    echo "Remaining actions needed:"
    echo ""
    
    if [ -z "$GITHUB_CLIENT_ID" ]; then
        echo "  1. Create GitHub OAuth App:"
        echo "     https://github.com/settings/applications/new"
        echo ""
        echo "  2. Set environment variable:"
        echo "     export GITHUB_CLIENT_ID=\"Iv1.YOUR_CLIENT_ID\""
        echo "     echo 'export GITHUB_CLIENT_ID=\"Iv1.YOUR_CLIENT_ID\"' >> ~/.bashrc"
        echo ""
    fi
    
    if [ ! -f "$HYBRID_IDE_DIR/build/bin/SetupAuth" ] && [ ! -f "./build/bin/SetupAuth" ]; then
        echo "  3. Build SetupAuth:"
        echo "     cd build && cmake .. && make SetupAuth"
        echo ""
    fi
fi

echo "═══════════════════════════════════════════════════════════════"
echo ""
echo "📖 For detailed help, see:"
echo "   • OAUTH_SETUP_GUIDE.md - Complete OAuth setup"
echo "   • OAUTH_VS_MARKETPLACE.md - OAuth App vs Marketplace"
echo "   • NUMERIC_PIN_LOGIN.md - PIN authentication guide"
echo ""
echo "🔧 Quick fix script:"
echo "   ./setup_oauth.sh"
echo ""
echo "💡 Alternative (fastest):"
echo "   Use Personal Access Token - see OAUTH_SETUP_GUIDE.md Method 3"
echo ""

exit 0
