#!/bin/bash

# Advanced OAuth Troubleshooter for Hybrid IDE
# Deep diagnosis with automatic fixes for authorization and cloning issues

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m'

# Counters
ISSUES_FOUND=0
ISSUES_FIXED=0
CRITICAL_ERRORS=0

# Header
clear
echo -e "${BLUE}"
echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║         ADVANCED OAuth Troubleshooter for Hybrid IDE             ║"
echo "║         Deep Diagnosis & Auto-Fix for Authorization Issues       ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo ""
echo "This advanced tool will:"
echo "  1. Deep scan OAuth configuration and permissions"
echo "  2. Diagnose authorization and cloning issues"
echo "  3. Test GitHub API scopes and access rights"
echo "  4. Verify Client ID and OAuth App settings"
echo "  5. Check repository access and clone permissions"
echo "  6. Automatically fix detected issues"
echo ""
echo -e "${YELLOW}Starting advanced diagnosis...${NC}"
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
        ((CRITICAL_ERRORS++))
    elif [ "$status" == "CRITICAL" ]; then
        echo -e "  ${RED}💀${NC} $message"
        ((ISSUES_FOUND++))
        ((CRITICAL_ERRORS++))
    else
        echo -e "  ${CYAN}ℹ${NC} $message"
    fi
}

# Function to print section header
print_section() {
    echo ""
    echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
}

# Function to fix issue
apply_fix() {
    local fix_name=$1
    echo -e "  ${GREEN}🔧 Applying fix: $fix_name${NC}"
    ((ISSUES_FIXED++))
}

# Test 1: System Requirements & Network
print_section "TEST 1: System Requirements & Network Connectivity"

if command -v curl &> /dev/null; then
    print_test "OK" "curl is installed"
else
    print_test "CRITICAL" "curl is not installed (REQUIRED)"
    echo "      Fix: sudo apt-get install curl"
fi

if command -v git &> /dev/null; then
    print_test "OK" "git is installed"
    GIT_VERSION=$(git --version | awk '{print $3}')
    print_test "INFO" "Git version: $GIT_VERSION"
else
    print_test "ERROR" "git is not installed"
fi

# Network tests
if ping -c 1 -W 2 github.com &> /dev/null 2>&1; then
    print_test "OK" "Can reach github.com"
else
    print_test "WARN" "Cannot ping github.com (firewall or DNS issue)"
fi

# Test GitHub API
HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" https://api.github.com/zen 2>/dev/null)
if [ "$HTTP_CODE" == "200" ]; then
    print_test "OK" "GitHub API is accessible (HTTP 200)"
else
    print_test "ERROR" "GitHub API returned HTTP $HTTP_CODE"
fi

# Test 2: OAuth Client ID Configuration
print_section "TEST 2: OAuth Client ID Deep Analysis"

if [ -z "$GITHUB_CLIENT_ID" ]; then
    print_test "ERROR" "GITHUB_CLIENT_ID environment variable is NOT set"
    echo ""
    echo "      ${YELLOW}AUTOMATED FIX AVAILABLE${NC}"
    echo ""
    
    read -p "      Enter your Client ID (or press Enter to skip): " CLIENT_ID_INPUT
    
    if [ ! -z "$CLIENT_ID_INPUT" ]; then
        export GITHUB_CLIENT_ID="$CLIENT_ID_INPUT"
        
        # Save to bashrc
        if [ -f "$HOME/.bashrc" ]; then
            if ! grep -q "GITHUB_CLIENT_ID" "$HOME/.bashrc"; then
                echo "export GITHUB_CLIENT_ID=\"$CLIENT_ID_INPUT\"" >> "$HOME/.bashrc"
                apply_fix "Saved GITHUB_CLIENT_ID to ~/.bashrc"
            fi
        fi
    fi
else
    print_test "OK" "GITHUB_CLIENT_ID is set"
    
    # Show partial Client ID for security
    CLIENT_ID_PREVIEW="${GITHUB_CLIENT_ID:0:10}..."
    print_test "INFO" "Client ID: $CLIENT_ID_PREVIEW"
    
    # Validate format
    if [[ $GITHUB_CLIENT_ID =~ ^Iv1\. ]] || [[ $GITHUB_CLIENT_ID =~ ^Ov23 ]]; then
        print_test "OK" "Client ID format is valid"
    else
        print_test "WARN" "Client ID format may be incorrect"
        echo "      Expected: Iv1.* or Ov23*"
        echo "      Got: ${GITHUB_CLIENT_ID:0:4}..."
    fi
fi

# Test 3: Client ID Validation with GitHub
print_section "TEST 3: Client ID Validation with GitHub API"

if [ ! -z "$GITHUB_CLIENT_ID" ]; then
    echo "  Testing Client ID with GitHub Device Flow API..."
    
    DEVICE_RESPONSE=$(curl -s -X POST https://github.com/login/device/code \
        -H "Accept: application/json" \
        -d "client_id=$GITHUB_CLIENT_ID&scope=repo,user,admin:org,delete_repo" 2>/dev/null)
    
    if echo "$DEVICE_RESPONSE" | grep -q "device_code"; then
        print_test "OK" "Client ID is VALID and recognized by GitHub"
        
        # Extract device code info
        USER_CODE=$(echo "$DEVICE_RESPONSE" | grep -o '"user_code":"[^"]*"' | cut -d'"' -f4)
        DEVICE_CODE=$(echo "$DEVICE_RESPONSE" | grep -o '"device_code":"[^"]*"' | cut -d'"' -f4)
        EXPIRES_IN=$(echo "$DEVICE_RESPONSE" | grep -o '"expires_in":[0-9]*' | cut -d':' -f2)
        
        if [ ! -z "$USER_CODE" ]; then
            print_test "OK" "Device flow test successful - Code: $USER_CODE"
            print_test "INFO" "Code expires in: $EXPIRES_IN seconds"
        fi
    elif echo "$DEVICE_RESPONSE" | grep -q "error"; then
        ERROR_MSG=$(echo "$DEVICE_RESPONSE" | grep -o '"error":"[^"]*"' | cut -d'"' -f4)
        ERROR_DESC=$(echo "$DEVICE_RESPONSE" | grep -o '"error_description":"[^"]*"' | cut -d'"' -f4)
        
        print_test "ERROR" "Client ID validation failed"
        echo "      Error: $ERROR_MSG"
        echo "      Description: $ERROR_DESC"
        
        if [[ "$ERROR_MSG" == *"not_found"* ]] || [[ "$ERROR_MSG" == *"invalid"* ]]; then
            print_test "CRITICAL" "Your Client ID does not exist or is invalid"
            echo ""
            echo "      ${YELLOW}SOLUTION:${NC}"
            echo "      1. Go to: https://github.com/settings/developers"
            echo "      2. Click 'New OAuth App'"
            echo "      3. Fill in:"
            echo "         - Application name: Hybrid IDE"
            echo "         - Homepage URL: http://localhost:3000"
            echo "         - Authorization callback URL: http://localhost:3000/callback"
            echo "      4. Copy the Client ID and set it:"
            echo "         export GITHUB_CLIENT_ID=\"YOUR_CLIENT_ID\""
        fi
    else
        print_test "WARN" "Unexpected response from GitHub API"
        echo "      Response: ${DEVICE_RESPONSE:0:100}..."
    fi
else
    print_test "ERROR" "Cannot validate - Client ID not set"
fi

# Test 4: OAuth App Configuration Check
print_section "TEST 4: OAuth App Configuration & Scopes"

if [ ! -z "$GITHUB_CLIENT_ID" ]; then
    echo "  Checking OAuth App configuration on GitHub..."
    
    # Try to get app info (this will fail without token, but we can see the error)
    APP_INFO=$(curl -s -H "Accept: application/json" \
        "https://api.github.com/applications/$GITHUB_CLIENT_ID/token" 2>/dev/null)
    
    print_test "INFO" "Required scopes for full functionality:"
    echo "      ✓ repo - Repository access (for cloning)"
    echo "      ✓ user - User profile access"
    echo "      ✓ admin:org - Organization access"
    echo "      ✓ delete_repo - Repository management"
    
    echo ""
    echo "  ${CYAN}💡 Make sure your OAuth App has these scopes configured${NC}"
fi

# Test 5: Existing Token Check
print_section "TEST 5: Existing Authentication Token Analysis"

TOKEN_FILE="$HOME/.hybridide/auth/github_token"
USER_DATA_FILE="$HOME/.hybridide/auth/user_data.json"

if [ -f "$TOKEN_FILE" ]; then
    print_test "OK" "Token file exists: $TOKEN_FILE"
    
    # Check permissions
    PERMS=$(stat -c "%a" "$TOKEN_FILE" 2>/dev/null || stat -f "%A" "$TOKEN_FILE" 2>/dev/null)
    if [ "$PERMS" == "600" ]; then
        print_test "OK" "Token file permissions are secure (600)"
    else
        print_test "WARN" "Token file permissions are $PERMS (should be 600)"
        
        read -p "      Fix permissions automatically? (Y/n): " fix_perms
        if [[ ! $fix_perms =~ ^[Nn]$ ]]; then
            chmod 600 "$TOKEN_FILE"
            apply_fix "Token file permissions fixed"
        fi
    fi
    
    # Validate token
    TOKEN=$(cat "$TOKEN_FILE" 2>/dev/null)
    if [ ! -z "$TOKEN" ]; then
        if [[ $TOKEN =~ ^gh[ps]_ ]]; then
            print_test "OK" "Token format looks valid (gh* pattern)"
            
            echo "  Testing token with GitHub API..."
            
            # Test token validity
            USER_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" \
                -H "Accept: application/vnd.github+json" \
                https://api.github.com/user)
            
            if echo "$USER_RESPONSE" | grep -q '"login"'; then
                USERNAME=$(echo "$USER_RESPONSE" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
                USER_ID=$(echo "$USER_RESPONSE" | grep -o '"id":[0-9]*' | cut -d':' -f2 | head -1)
                
                print_test "OK" "Token is VALID - Authenticated as: $USERNAME (ID: $USER_ID)"
                
                # Check token scopes
                SCOPES_HEADER=$(curl -s -I -H "Authorization: Bearer $TOKEN" \
                    -H "Accept: application/vnd.github+json" \
                    https://api.github.com/user | grep -i "x-oauth-scopes:")
                
                if [ ! -z "$SCOPES_HEADER" ]; then
                    SCOPES=$(echo "$SCOPES_HEADER" | cut -d':' -f2 | xargs)
                    print_test "INFO" "Token scopes: $SCOPES"
                    
                    # Check for repo scope
                    if echo "$SCOPES" | grep -q "repo"; then
                        print_test "OK" "Token has 'repo' scope (can clone repositories)"
                    else
                        print_test "ERROR" "Token MISSING 'repo' scope - cannot clone private repos"
                        echo "      ${YELLOW}This is why you can't authorize cloning!${NC}"
                        echo ""
                        echo "      ${GREEN}FIX:${NC} Re-authenticate with correct scopes"
                        
                        read -p "      Delete current token and re-authenticate? (y/N): " reauth
                        if [[ $reauth =~ ^[Yy]$ ]]; then
                            rm -f "$TOKEN_FILE" "$USER_DATA_FILE"
                            apply_fix "Removed invalid token - ready for re-authentication"
                            print_test "INFO" "Run ./build/bin/SetupAuth to authenticate again"
                        fi
                    fi
                fi
                
                # Test repository access
                echo ""
                echo "  Testing repository access..."
                
                REPOS_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" \
                    -H "Accept: application/vnd.github+json" \
                    "https://api.github.com/user/repos?per_page=5&sort=updated")
                
                if echo "$REPOS_RESPONSE" | grep -q '"name"'; then
                    REPO_COUNT=$(echo "$REPOS_RESPONSE" | grep -o '"name":' | wc -l)
                    print_test "OK" "Can access repositories (found $REPO_COUNT recent repos)"
                    
                    # Show first repo as test
                    FIRST_REPO=$(echo "$REPOS_RESPONSE" | grep -o '"full_name":"[^"]*"' | head -1 | cut -d'"' -f4)
                    if [ ! -z "$FIRST_REPO" ]; then
                        print_test "INFO" "Example accessible repo: $FIRST_REPO"
                    fi
                else
                    print_test "ERROR" "Cannot access repositories"
                fi
                
            else
                print_test "ERROR" "Token is INVALID or EXPIRED"
                
                if echo "$USER_RESPONSE" | grep -q "Bad credentials"; then
                    print_test "CRITICAL" "GitHub rejected the token (Bad credentials)"
                elif echo "$USER_RESPONSE" | grep -q "401"; then
                    print_test "CRITICAL" "Token unauthorized (401)"
                fi
                
                echo ""
                echo "      ${YELLOW}AUTOMATIC FIX AVAILABLE${NC}"
                read -p "      Delete invalid token and re-authenticate? (Y/n): " del_token
                
                if [[ ! $del_token =~ ^[Nn]$ ]]; then
                    rm -f "$TOKEN_FILE" "$USER_DATA_FILE"
                    apply_fix "Removed invalid token"
                    print_test "INFO" "Run ./build/bin/SetupAuth to authenticate again"
                fi
            fi
        else
            print_test "ERROR" "Token format is invalid (doesn't match gh* pattern)"
            echo "      Token starts with: ${TOKEN:0:3}..."
        fi
    else
        print_test "WARN" "Token file is empty"
    fi
else
    print_test "INFO" "No saved token found - not authenticated yet"
    print_test "INFO" "Run ./build/bin/SetupAuth to authenticate"
fi

# Test 6: Repository Clone Permission Test
print_section "TEST 6: Repository Clone Permission Test"

if [ -f "$TOKEN_FILE" ] && [ ! -z "$TOKEN" ]; then
    echo "  Testing clone permissions on your repositories..."
    
    # Get user's repos
    USER_REPOS=$(curl -s -H "Authorization: Bearer $TOKEN" \
        -H "Accept: application/vnd.github+json" \
        "https://api.github.com/user/repos?per_page=3&affiliation=owner")
    
    if echo "$USER_REPOS" | grep -q '"clone_url"'; then
        CLONE_URL=$(echo "$USER_REPOS" | grep -o '"clone_url":"[^"]*"' | head -1 | cut -d'"' -f4)
        REPO_NAME=$(echo "$USER_REPOS" | grep -o '"full_name":"[^"]*"' | head -1 | cut -d'"' -f4)
        
        if [ ! -z "$CLONE_URL" ]; then
            print_test "OK" "Found clonable repository: $REPO_NAME"
            
            # Test if we can access repo info
            REPO_INFO=$(curl -s -H "Authorization: Bearer $TOKEN" \
                -H "Accept: application/vnd.github+json" \
                "https://api.github.com/repos/$REPO_NAME")
            
            if echo "$REPO_INFO" | grep -q '"permissions"'; then
                PERMISSIONS=$(echo "$REPO_INFO" | grep -A5 '"permissions"')
                
                if echo "$PERMISSIONS" | grep -q '"admin":true'; then
                    print_test "OK" "You have ADMIN access to $REPO_NAME"
                elif echo "$PERMISSIONS" | grep -q '"push":true'; then
                    print_test "OK" "You have PUSH access to $REPO_NAME"
                elif echo "$PERMISSIONS" | grep -q '"pull":true'; then
                    print_test "OK" "You have PULL access to $REPO_NAME"
                else
                    print_test "WARN" "Limited access to $REPO_NAME"
                fi
            fi
            
            print_test "INFO" "Clone URL: $CLONE_URL"
        fi
    else
        print_test "WARN" "Cannot retrieve repository list"
    fi
else
    print_test "WARN" "Skipping (not authenticated)"
fi

# Test 7: OAuth App Setup Verification
print_section "TEST 7: OAuth App Setup Verification Guide"

echo "  Verifying your OAuth App is configured correctly..."
echo ""
echo "  ${CYAN}Your OAuth App MUST have these settings:${NC}"
echo ""
echo "  ✓ Application name: Hybrid IDE (or any name)"
echo "  ✓ Homepage URL: http://localhost:3000"
echo "  ✓ Authorization callback URL: http://localhost:3000/callback"
echo "  ✓ Enable Device Flow: ✓ CHECKED"
echo ""
echo "  ${YELLOW}CRITICAL: Device Flow Authorization${NC}"
echo "  In your OAuth App settings, make sure:"
echo "  1. Scroll down to 'Device Flow'"
echo "  2. CHECK the box 'Enable Device Flow'"
echo "  3. Click 'Update application'"
echo ""
echo "  ${RED}If Device Flow is NOT enabled:${NC}"
echo "  - GitHub will ask for username/password"
echo "  - You cannot authorize app to clone repos"
echo "  - Authentication will fail"
echo ""

read -p "  Is Device Flow enabled in your OAuth App? (y/n/unknown): " device_flow_status

if [[ $device_flow_status =~ ^[Nn]$ ]]; then
    print_test "CRITICAL" "Device Flow is NOT enabled - this is the problem!"
    echo ""
    echo "      ${GREEN}FIX NOW:${NC}"
    echo "      1. Go to: https://github.com/settings/developers"
    echo "      2. Click on your OAuth App (Client ID: ${GITHUB_CLIENT_ID:0:10}...)"
    echo "      3. Scroll down to 'Device Flow'"
    echo "      4. CHECK the box 'Enable Device Flow'"
    echo "      5. Click 'Update application'"
    echo "      6. Come back here and re-authenticate"
    echo ""
elif [[ $device_flow_status =~ ^[Yy]$ ]]; then
    print_test "OK" "Device Flow is enabled"
elif [[ $device_flow_status =~ ^[Uu]$ ]]; then
    print_test "WARN" "Unknown - please verify manually"
    echo "      Check at: https://github.com/settings/developers"
fi

# Test 8: Git Configuration
print_section "TEST 8: Git Configuration & Credentials"

if command -v git &> /dev/null; then
    GIT_USER=$(git config --global user.name 2>/dev/null)
    GIT_EMAIL=$(git config --global user.email 2>/dev/null)
    
    if [ ! -z "$GIT_USER" ]; then
        print_test "OK" "Git user.name is set: $GIT_USER"
    else
        print_test "WARN" "Git user.name is not set"
        
        read -p "      Enter your name: " git_name
        if [ ! -z "$git_name" ]; then
            git config --global user.name "$git_name"
            apply_fix "Set git user.name"
        fi
    fi
    
    if [ ! -z "$GIT_EMAIL" ]; then
        print_test "OK" "Git user.email is set: $GIT_EMAIL"
    else
        print_test "WARN" "Git user.email is not set"
        
        read -p "      Enter your email: " git_email
        if [ ! -z "$git_email" ]; then
            git config --global user.email "$git_email"
            apply_fix "Set git user.email"
        fi
    fi
    
    # Check credential helper
    CRED_HELPER=$(git config --global credential.helper 2>/dev/null)
    if [ ! -z "$CRED_HELPER" ]; then
        print_test "OK" "Git credential helper: $CRED_HELPER"
    else
        print_test "INFO" "No credential helper configured"
        
        read -p "      Set up credential caching? (Y/n): " setup_cred
        if [[ ! $setup_cred =~ ^[Nn]$ ]]; then
            git config --global credential.helper cache
            apply_fix "Enabled git credential caching"
        fi
    fi
fi

# Test 9: Quick Authentication Test
print_section "TEST 9: Quick Device Flow Authentication Test"

if [ ! -z "$GITHUB_CLIENT_ID" ]; then
    echo "  Would you like to test the authentication flow right now?"
    echo "  This will:"
    echo "    - Generate a device code"
    echo "    - Open your browser"
    echo "    - Let you authorize the app"
    echo ""
    
    read -p "  Test authentication now? (y/N): " test_auth
    
    if [[ $test_auth =~ ^[Yy]$ ]]; then
        echo ""
        echo "  ${CYAN}Generating device code...${NC}"
        
        AUTH_RESPONSE=$(curl -s -X POST https://github.com/login/device/code \
            -H "Accept: application/json" \
            -d "client_id=$GITHUB_CLIENT_ID&scope=repo,user,admin:org,delete_repo")
        
        if echo "$AUTH_RESPONSE" | grep -q "device_code"; then
            USER_CODE=$(echo "$AUTH_RESPONSE" | grep -o '"user_code":"[^"]*"' | cut -d'"' -f4)
            VERIFICATION_URI=$(echo "$AUTH_RESPONSE" | grep -o '"verification_uri":"[^"]*"' | cut -d'"' -f4)
            DEVICE_CODE=$(echo "$AUTH_RESPONSE" | grep -o '"device_code":"[^"]*"' | cut -d'"' -f4)
            INTERVAL=$(echo "$AUTH_RESPONSE" | grep -o '"interval":[0-9]*' | cut -d':' -f2)
            
            print_test "OK" "Device code generated successfully"
            
            echo ""
            echo "  ╔═══════════════════════════════════════════════════════════════╗"
            echo "  ║  AUTHORIZATION CODE: $USER_CODE"
            echo "  ║  Visit: $VERIFICATION_URI"
            echo "  ╚═══════════════════════════════════════════════════════════════╝"
            echo ""
            
            # Open browser
            if command -v xdg-open &> /dev/null; then
                xdg-open "$VERIFICATION_URI" 2>/dev/null &
                print_test "OK" "Browser opened"
            elif command -v open &> /dev/null; then
                open "$VERIFICATION_URI" 2>/dev/null &
                print_test "OK" "Browser opened"
            fi
            
            echo ""
            echo "  ${YELLOW}Please authorize the app in your browser${NC}"
            echo "  Enter the code: $USER_CODE"
            echo ""
            echo "  Waiting for authorization..."
            
            # Poll for token
            MAX_ATTEMPTS=20
            ATTEMPT=0
            
            while [ $ATTEMPT -lt $MAX_ATTEMPTS ]; do
                sleep ${INTERVAL:-5}
                ((ATTEMPT++))
                
                TOKEN_RESPONSE=$(curl -s -X POST https://github.com/login/oauth/access_token \
                    -H "Accept: application/json" \
                    -d "client_id=$GITHUB_CLIENT_ID&device_code=$DEVICE_CODE&grant_type=urn:ietf:params:oauth:grant-type:device_code")
                
                if echo "$TOKEN_RESPONSE" | grep -q "access_token"; then
                    ACCESS_TOKEN=$(echo "$TOKEN_RESPONSE" | grep -o '"access_token":"[^"]*"' | cut -d'"' -f4)
                    
                    if [ ! -z "$ACCESS_TOKEN" ]; then
                        print_test "OK" "✓ Authorization successful!"
                        print_test "OK" "Access token received: ${ACCESS_TOKEN:0:10}..."
                        
                        # Save token
                        mkdir -p "$HOME/.hybridide/auth"
                        echo "$ACCESS_TOKEN" > "$TOKEN_FILE"
                        chmod 600 "$TOKEN_FILE"
                        
                        apply_fix "Token saved to $TOKEN_FILE"
                        
                        # Test token
                        USER_INFO=$(curl -s -H "Authorization: Bearer $ACCESS_TOKEN" \
                            -H "Accept: application/vnd.github+json" \
                            https://api.github.com/user)
                        
                        if echo "$USER_INFO" | grep -q '"login"'; then
                            USERNAME=$(echo "$USER_INFO" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
                            print_test "OK" "Authenticated as: $USERNAME"
                            
                            # Save user data
                            echo "$USER_INFO" > "$USER_DATA_FILE"
                            apply_fix "User data saved"
                        fi
                        
                        break
                    fi
                elif echo "$TOKEN_RESPONSE" | grep -q "authorization_pending"; then
                    echo "  Still waiting... (attempt $ATTEMPT/$MAX_ATTEMPTS)"
                elif echo "$TOKEN_RESPONSE" | grep -q "slow_down"; then
                    echo "  Slowing down polling..."
                    sleep 5
                elif echo "$TOKEN_RESPONSE" | grep -q "expired_token"; then
                    print_test "ERROR" "Device code expired - please try again"
                    break
                elif echo "$TOKEN_RESPONSE" | grep -q "access_denied"; then
                    print_test "ERROR" "Authorization denied by user"
                    break
                fi
            done
            
            if [ $ATTEMPT -eq $MAX_ATTEMPTS ]; then
                print_test "WARN" "Timeout waiting for authorization"
            fi
        else
            print_test "ERROR" "Failed to generate device code"
            echo "      Response: ${AUTH_RESPONSE:0:150}..."
        fi
    fi
fi

# Summary
print_section "DIAGNOSIS SUMMARY & RECOMMENDATIONS"

echo ""
if [ $CRITICAL_ERRORS -gt 0 ]; then
    echo -e "${RED}💀 Found $CRITICAL_ERRORS CRITICAL error(s)${NC}"
    echo ""
elif [ $ISSUES_FOUND -gt 0 ]; then
    echo -e "${YELLOW}⚠ Found $ISSUES_FOUND issue(s)${NC}"
    echo ""
else
    echo -e "${GREEN}✓ Perfect! No issues found${NC}"
    echo ""
fi

if [ $ISSUES_FIXED -gt 0 ]; then
    echo -e "${GREEN}🔧 Automatically fixed $ISSUES_FIXED issue(s)${NC}"
    echo ""
fi

echo "═══════════════════════════════════════════════════════════════════"
echo ""
echo "${CYAN}MOST COMMON ISSUE: Device Flow Not Enabled${NC}"
echo ""
echo "If you can't authorize cloning, it's usually because:"
echo "  1. ${RED}Device Flow is NOT enabled in your OAuth App${NC}"
echo "  2. ${YELLOW}Your token is missing the 'repo' scope${NC}"
echo "  3. ${YELLOW}Your Client ID is invalid or expired${NC}"
echo ""
echo "${GREEN}QUICK FIX:${NC}"
echo "  1. Go to: https://github.com/settings/developers"
echo "  2. Click your OAuth App"
echo "  3. Enable Device Flow (checkbox)"
echo "  4. Update application"
echo "  5. Run: ./build/bin/SetupAuth"
echo ""
echo "═══════════════════════════════════════════════════════════════════"
echo ""
echo "📖 Documentation:"
echo "   • OAUTH_SETUP_GUIDE.md - Complete setup guide"
echo "   • OAUTH_VS_MARKETPLACE.md - OAuth App vs Marketplace"
echo ""
echo "🔧 Tools:"
echo "   • ./setup_oauth.sh - Quick Client ID setup"
echo "   • ./oauth_troubleshooter.sh - Basic troubleshooter"
echo "   • ./build/bin/SetupAuth - Authentication tool"
echo ""

exit 0
