#!/bin/bash

# OAuth AutoFixer - Automatically fixes OAuth authentication issues
# No questions asked - just fixes everything automatically

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

clear
echo -e "${BLUE}"
echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║                    OAuth AutoFixer for Hybrid IDE                ║"
echo "║                  Automatic Fix - No User Input                   ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo ""
echo "This tool will automatically:"
echo "  1. Detect OAuth issues"
echo "  2. Apply fixes without asking"
echo "  3. Clean up invalid tokens"
echo "  4. Configure environment"
echo "  5. Set up git credentials"
echo "  6. Run authentication"
echo ""
echo -e "${YELLOW}Starting automatic fixes...${NC}"
echo ""
sleep 1

FIXES_APPLIED=0
ISSUES_FOUND=0

# Function to log fixes
log_fix() {
    echo -e "  ${GREEN}✓${NC} $1"
    ((FIXES_APPLIED++))
}

log_issue() {
    echo -e "  ${RED}✗${NC} $1"
    ((ISSUES_FOUND++))
}

log_info() {
    echo -e "  ${CYAN}ℹ${NC} $1"
}

# Step 1: Install required packages
echo -e "${CYAN}[1/10] Checking system requirements...${NC}"
if ! command -v curl &> /dev/null; then
    log_info "Installing curl..."
    sudo apt-get update -qq && sudo apt-get install -y curl &> /dev/null
    log_fix "curl installed"
else
    log_fix "curl already installed"
fi

if ! command -v git &> /dev/null; then
    log_info "Installing git..."
    sudo apt-get install -y git &> /dev/null
    log_fix "git installed"
else
    log_fix "git already installed"
fi

# Step 2: Clean up old/invalid tokens
echo ""
echo -e "${CYAN}[2/10] Cleaning up old tokens...${NC}"
if [ -f "$HOME/.hybridide/auth/github_token" ]; then
    TOKEN=$(cat "$HOME/.hybridide/auth/github_token" 2>/dev/null)
    if [ ! -z "$TOKEN" ]; then
        # Test if token is valid
        TEST_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" https://api.github.com/user 2>/dev/null)
        if echo "$TEST_RESPONSE" | grep -q '"login"'; then
            log_info "Existing token is valid - keeping it"
        else
            rm -rf ~/.hybridide/auth/* 2>/dev/null
            log_fix "Removed invalid token"
        fi
    fi
else
    log_info "No existing token found"
fi

# Step 3: Check and set Client ID
echo ""
echo -e "${CYAN}[3/10] Configuring OAuth Client ID...${NC}"
if [ -z "$GITHUB_CLIENT_ID" ]; then
    # Try to read from bashrc
    if grep -q "GITHUB_CLIENT_ID" "$HOME/.bashrc" 2>/dev/null; then
        source "$HOME/.bashrc"
        log_fix "Loaded Client ID from .bashrc"
    else
        # Check if user provided it as argument
        if [ ! -z "$1" ]; then
            export GITHUB_CLIENT_ID="$1"
            echo "export GITHUB_CLIENT_ID=\"$1\"" >> "$HOME/.bashrc"
            log_fix "Client ID set from argument: ${1:0:10}..."
        else
            log_issue "No Client ID found - please provide as argument or set GITHUB_CLIENT_ID"
            echo ""
            echo "  Usage: ./oauth_autofixer.sh YOUR_CLIENT_ID"
            echo "  Or: export GITHUB_CLIENT_ID=\"YOUR_CLIENT_ID\" && ./oauth_autofixer.sh"
            exit 1
        fi
    fi
else
    log_fix "Client ID already set: ${GITHUB_CLIENT_ID:0:10}..."
fi

# Step 4: Validate Client ID
echo ""
echo -e "${CYAN}[4/10] Validating Client ID with GitHub...${NC}"
VALIDATION_RESPONSE=$(curl -s -X POST https://github.com/login/device/code \
    -H "Accept: application/json" \
    -d "client_id=$GITHUB_CLIENT_ID&scope=repo,user,admin:org,delete_repo" 2>/dev/null)

if echo "$VALIDATION_RESPONSE" | grep -q "device_code"; then
    log_fix "Client ID is valid and recognized by GitHub"
else
    log_issue "Client ID is invalid or not recognized"
    echo "  Create one at: https://github.com/settings/applications/new"
    echo "  Then run: ./oauth_autofixer.sh YOUR_NEW_CLIENT_ID"
    exit 1
fi

# Step 5: Configure Git
echo ""
echo -e "${CYAN}[5/10] Configuring Git...${NC}"
GIT_USER=$(git config --global user.name 2>/dev/null)
GIT_EMAIL=$(git config --global user.email 2>/dev/null)

if [ -z "$GIT_USER" ]; then
    # Try to get from GitHub
    if [ -f "$HOME/.hybridide/auth/github_token" ]; then
        TOKEN=$(cat "$HOME/.hybridide/auth/github_token")
        USER_INFO=$(curl -s -H "Authorization: Bearer $TOKEN" https://api.github.com/user 2>/dev/null)
        if echo "$USER_INFO" | grep -q '"login"'; then
            USERNAME=$(echo "$USER_INFO" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
            git config --global user.name "$USERNAME" 2>/dev/null
            log_fix "Git user.name set to: $USERNAME"
        fi
    else
        git config --global user.name "HybridIDE-User" 2>/dev/null
        log_fix "Git user.name set to default: HybridIDE-User"
    fi
else
    log_fix "Git user.name already set: $GIT_USER"
fi

if [ -z "$GIT_EMAIL" ]; then
    if [ -f "$HOME/.hybridide/auth/github_token" ]; then
        TOKEN=$(cat "$HOME/.hybridide/auth/github_token")
        USER_INFO=$(curl -s -H "Authorization: Bearer $TOKEN" https://api.github.com/user 2>/dev/null)
        if echo "$USER_INFO" | grep -q '"email"'; then
            EMAIL=$(echo "$USER_INFO" | grep -o '"email":"[^"]*"' | cut -d'"' -f4 | head -1)
            if [ ! -z "$EMAIL" ] && [ "$EMAIL" != "null" ]; then
                git config --global user.email "$EMAIL" 2>/dev/null
                log_fix "Git user.email set to: $EMAIL"
            else
                git config --global user.email "user@hybridide.local" 2>/dev/null
                log_fix "Git user.email set to default"
            fi
        fi
    else
        git config --global user.email "user@hybridide.local" 2>/dev/null
        log_fix "Git user.email set to default"
    fi
else
    log_fix "Git user.email already set: $GIT_EMAIL"
fi

# Step 6: Set up credential helper
echo ""
echo -e "${CYAN}[6/10] Configuring credential helper...${NC}"
CRED_HELPER=$(git config --global credential.helper 2>/dev/null)
if [ -z "$CRED_HELPER" ]; then
    git config --global credential.helper cache 2>/dev/null
    log_fix "Enabled credential caching (15 min)"
else
    log_fix "Credential helper already set: $CRED_HELPER"
fi

# Step 7: Fix file permissions
echo ""
echo -e "${CYAN}[7/10] Fixing file permissions...${NC}"
if [ -d "$HOME/.hybridide/auth" ]; then
    chmod 700 "$HOME/.hybridide/auth" 2>/dev/null
    log_fix "Auth directory permissions: 700"
    
    if [ -f "$HOME/.hybridide/auth/github_token" ]; then
        chmod 600 "$HOME/.hybridide/auth/github_token" 2>/dev/null
        log_fix "Token file permissions: 600"
    fi
else
    mkdir -p "$HOME/.hybridide/auth" 2>/dev/null
    chmod 700 "$HOME/.hybridide/auth" 2>/dev/null
    log_fix "Created auth directory with secure permissions"
fi

# Step 8: Make executables executable
echo ""
echo -e "${CYAN}[8/10] Fixing executable permissions...${NC}"
HYBRID_IDE_DIR="/workspaces/codespaces-blank/hybrid-ide"
if [ -d "$HYBRID_IDE_DIR/build/bin" ]; then
    chmod +x "$HYBRID_IDE_DIR"/build/bin/* 2>/dev/null
    log_fix "All executables are now executable"
elif [ -d "./build/bin" ]; then
    chmod +x ./build/bin/* 2>/dev/null
    log_fix "All executables are now executable"
else
    log_info "No build directory found - run cmake first"
fi

# Step 9: Test authentication
echo ""
echo -e "${CYAN}[9/10] Testing authentication...${NC}"
if [ -f "$HOME/.hybridide/auth/github_token" ]; then
    TOKEN=$(cat "$HOME/.hybridide/auth/github_token" 2>/dev/null)
    if [ ! -z "$TOKEN" ]; then
        USER_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" \
            -H "Accept: application/vnd.github+json" \
            https://api.github.com/user 2>/dev/null)
        
        if echo "$USER_RESPONSE" | grep -q '"login"'; then
            USERNAME=$(echo "$USER_RESPONSE" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
            log_fix "Authenticated as: $USERNAME"
            
            # Check scopes
            SCOPES=$(curl -s -I -H "Authorization: Bearer $TOKEN" \
                https://api.github.com/user 2>/dev/null | grep -i "x-oauth-scopes:" | cut -d':' -f2 | xargs)
            
            if echo "$SCOPES" | grep -q "repo"; then
                log_fix "Token has 'repo' scope - can clone repositories"
            else
                log_issue "Token missing 'repo' scope - need to re-authenticate"
                rm -rf ~/.hybridide/auth/* 2>/dev/null
            fi
        else
            log_issue "Token is invalid - will re-authenticate"
            rm -rf ~/.hybridide/auth/* 2>/dev/null
        fi
    fi
fi

# Step 10: Auto-authenticate if needed
echo ""
echo -e "${CYAN}[10/10] Running authentication if needed...${NC}"
if [ ! -f "$HOME/.hybridide/auth/github_token" ]; then
    log_info "No valid token found - starting OAuth Device Flow..."
    echo ""
    
    # Generate device code
    AUTH_RESPONSE=$(curl -s -X POST https://github.com/login/device/code \
        -H "Accept: application/json" \
        -d "client_id=$GITHUB_CLIENT_ID&scope=repo,user,admin:org,delete_repo" 2>/dev/null)
    
    if echo "$AUTH_RESPONSE" | grep -q "device_code"; then
        USER_CODE=$(echo "$AUTH_RESPONSE" | grep -o '"user_code":"[^"]*"' | cut -d'"' -f4)
        VERIFICATION_URI=$(echo "$AUTH_RESPONSE" | grep -o '"verification_uri":"[^"]*"' | cut -d'"' -f4)
        DEVICE_CODE=$(echo "$AUTH_RESPONSE" | grep -o '"device_code":"[^"]*"' | cut -d'"' -f4)
        INTERVAL=$(echo "$AUTH_RESPONSE" | grep -o '"interval":[0-9]*' | cut -d':' -f2)
        
        echo "  ╔═══════════════════════════════════════════════════════════════╗"
        echo "  ║  AUTHORIZATION CODE: $USER_CODE"
        echo "  ║  Visit: $VERIFICATION_URI"
        echo "  ╚═══════════════════════════════════════════════════════════════╝"
        echo ""
        
        # Try to open browser
        if command -v xdg-open &> /dev/null; then
            xdg-open "$VERIFICATION_URI" 2>/dev/null &
            log_fix "Browser opened automatically"
        elif command -v open &> /dev/null; then
            open "$VERIFICATION_URI" 2>/dev/null &
            log_fix "Browser opened automatically"
        else
            log_info "Please open the URL manually in your browser"
        fi
        
        echo ""
        echo -e "  ${YELLOW}Waiting for you to authorize in browser...${NC}"
        echo "  (This will take 5-60 seconds after you click Authorize)"
        echo ""
        
        # Poll for token
        MAX_ATTEMPTS=24
        ATTEMPT=0
        
        while [ $ATTEMPT -lt $MAX_ATTEMPTS ]; do
            sleep ${INTERVAL:-5}
            ((ATTEMPT++))
            
            TOKEN_RESPONSE=$(curl -s -X POST https://github.com/login/oauth/access_token \
                -H "Accept: application/json" \
                -d "client_id=$GITHUB_CLIENT_ID&device_code=$DEVICE_CODE&grant_type=urn:ietf:params:oauth:grant-type:device_code" 2>/dev/null)
            
            if echo "$TOKEN_RESPONSE" | grep -q "access_token"; then
                ACCESS_TOKEN=$(echo "$TOKEN_RESPONSE" | grep -o '"access_token":"[^"]*"' | cut -d'"' -f4)
                
                if [ ! -z "$ACCESS_TOKEN" ]; then
                    # Save token
                    mkdir -p "$HOME/.hybridide/auth"
                    echo "$ACCESS_TOKEN" > "$HOME/.hybridide/auth/github_token"
                    chmod 600 "$HOME/.hybridide/auth/github_token"
                    
                    log_fix "✓ Authorization successful!"
                    log_fix "Access token saved securely"
                    
                    # Get user info
                    USER_INFO=$(curl -s -H "Authorization: Bearer $ACCESS_TOKEN" \
                        -H "Accept: application/vnd.github+json" \
                        https://api.github.com/user 2>/dev/null)
                    
                    if echo "$USER_INFO" | grep -q '"login"'; then
                        USERNAME=$(echo "$USER_INFO" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
                        echo "$USER_INFO" > "$HOME/.hybridide/auth/user_data.json"
                        log_fix "Authenticated as: $USERNAME"
                    fi
                    
                    break
                fi
            elif echo "$TOKEN_RESPONSE" | grep -q "authorization_pending"; then
                echo "  ⏳ Waiting for authorization... (attempt $ATTEMPT/$MAX_ATTEMPTS)"
            elif echo "$TOKEN_RESPONSE" | grep -q "slow_down"; then
                sleep 5
            elif echo "$TOKEN_RESPONSE" | grep -q "expired_token"; then
                log_issue "Device code expired - please run again"
                exit 1
            elif echo "$TOKEN_RESPONSE" | grep -q "access_denied"; then
                log_issue "Authorization denied by user"
                exit 1
            fi
        done
        
        if [ $ATTEMPT -eq $MAX_ATTEMPTS ]; then
            log_issue "Timeout waiting for authorization"
            echo "  Please run the script again and authorize within 2 minutes"
            exit 1
        fi
    else
        log_issue "Failed to generate device code"
        exit 1
    fi
else
    log_fix "Already authenticated - no action needed"
fi

# Summary
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}AUTOFIX COMPLETE!${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo ""
echo "Fixes applied: $FIXES_APPLIED"
echo "Issues found: $ISSUES_FOUND"
echo ""

if [ $ISSUES_FOUND -eq 0 ]; then
    echo -e "${GREEN}✓ Everything is working perfectly!${NC}"
    echo ""
    echo "You can now:"
    echo "  • Clone repositories"
    echo "  • Use GitHub API"
    echo "  • Access private repos"
    echo "  • Run Hybrid IDE"
    echo ""
else
    echo -e "${YELLOW}⚠ Some issues remain - see messages above${NC}"
    echo ""
fi

# Test clone
if [ -f "$HOME/.hybridide/auth/github_token" ]; then
    echo -e "${CYAN}Testing repository access...${NC}"
    TOKEN=$(cat "$HOME/.hybridide/auth/github_token")
    
    REPOS=$(curl -s -H "Authorization: Bearer $TOKEN" \
        -H "Accept: application/vnd.github+json" \
        "https://api.github.com/user/repos?per_page=1" 2>/dev/null)
    
    if echo "$REPOS" | grep -q '"clone_url"'; then
        echo -e "  ${GREEN}✓ Can access repositories${NC}"
        
        CLONE_URL=$(echo "$REPOS" | grep -o '"clone_url":"[^"]*"' | head -1 | cut -d'"' -f4)
        REPO_NAME=$(echo "$REPOS" | grep -o '"full_name":"[^"]*"' | head -1 | cut -d'"' -f4)
        
        echo "  Example: git clone $CLONE_URL"
    fi
fi

echo ""
echo -e "${BLUE}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}OAuth AutoFixer completed successfully!${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════════${NC}"
echo ""

exit 0
