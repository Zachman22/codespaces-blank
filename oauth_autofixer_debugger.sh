#!/bin/bash

# OAuth AutoFixer Debugger - Shows detailed debugging output during automatic fixes
# Verbose mode with step-by-step logging

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
GRAY='\033[0;90m'
NC='\033[0m'

# Enable debug mode
DEBUG=1
VERBOSE=1

# Counters
FIXES_APPLIED=0
ISSUES_FOUND=0
API_CALLS=0
FILE_OPERATIONS=0

# Debug log file
LOG_FILE="/tmp/oauth_autofixer_debug_$(date +%Y%m%d_%H%M%S).log"

# Header
clear
echo -e "${BLUE}"
echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║            OAuth AutoFixer DEBUGGER for Hybrid IDE               ║"
echo "║              Verbose Mode with Full Diagnostics                  ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo ""
echo "Debug log: $LOG_FILE"
echo ""
echo "This debugger will:"
echo "  • Show all API calls and responses"
echo "  • Log every file operation"
echo "  • Display environment variables"
echo "  • Track token validation steps"
echo "  • Show curl commands executed"
echo "  • Log timing for each step"
echo "  • Save complete debug log"
echo ""
echo -e "${YELLOW}Starting debug mode...${NC}"
echo ""
sleep 1

# Initialize log
echo "OAuth AutoFixer Debug Log" > "$LOG_FILE"
echo "Started: $(date)" >> "$LOG_FILE"
echo "User: $(whoami)" >> "$LOG_FILE"
echo "PWD: $(pwd)" >> "$LOG_FILE"
echo "═══════════════════════════════════════════════════════════════════" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

# Debug function
debug() {
    local level=$1
    shift
    local message="$@"
    local timestamp=$(date +"%H:%M:%S.%3N")
    
    echo -e "${GRAY}[$timestamp] [DEBUG:$level] $message${NC}"
    echo "[$timestamp] [DEBUG:$level] $message" >> "$LOG_FILE"
}

# Log function
log_fix() {
    echo -e "  ${GREEN}✓${NC} $1"
    echo "[FIX] $1" >> "$LOG_FILE"
    ((FIXES_APPLIED++))
}

log_issue() {
    echo -e "  ${RED}✗${NC} $1"
    echo "[ISSUE] $1" >> "$LOG_FILE"
    ((ISSUES_FOUND++))
}

log_info() {
    echo -e "  ${CYAN}ℹ${NC} $1"
    echo "[INFO] $1" >> "$LOG_FILE"
}

log_warn() {
    echo -e "  ${YELLOW}⚠${NC} $1"
    echo "[WARN] $1" >> "$LOG_FILE"
}

# API call wrapper
api_call() {
    local method=$1
    local url=$2
    shift 2
    local extra_args="$@"
    
    ((API_CALLS++))
    debug "API" "Calling: $method $url"
    debug "API" "Args: $extra_args"
    
    local start_time=$(date +%s%N)
    local response=$(curl -s $extra_args "$url")
    local end_time=$(date +%s%N)
    local duration=$(( (end_time - start_time) / 1000000 ))
    
    debug "API" "Response time: ${duration}ms"
    debug "API" "Response: ${response:0:200}..."
    
    echo "$response"
}

# File operation wrapper
file_op() {
    local operation=$1
    local file=$2
    
    ((FILE_OPERATIONS++))
    debug "FILE" "Operation: $operation on $file"
    
    case $operation in
        "read")
            if [ -f "$file" ]; then
                debug "FILE" "File exists: $file ($(stat -c%s "$file" 2>/dev/null || stat -f%z "$file" 2>/dev/null) bytes)"
                cat "$file"
            else
                debug "FILE" "File does not exist: $file"
            fi
            ;;
        "write")
            debug "FILE" "Writing to: $file"
            ;;
        "delete")
            debug "FILE" "Deleting: $file"
            rm -f "$file" 2>/dev/null
            ;;
        "chmod")
            local perms=$3
            debug "FILE" "Setting permissions $perms on $file"
            chmod $perms "$file" 2>/dev/null
            ;;
    esac
}

# Environment check
echo -e "${CYAN}[DEBUG] Environment Variables${NC}"
debug "ENV" "GITHUB_CLIENT_ID=${GITHUB_CLIENT_ID:-NOT SET}"
debug "ENV" "GITHUB_TOKEN=${GITHUB_TOKEN:-NOT SET}"
debug "ENV" "HOME=$HOME"
debug "ENV" "USER=$(whoami)"
debug "ENV" "SHELL=$SHELL"
debug "ENV" "PATH=${PATH:0:100}..."
echo ""

# Step 1: System Requirements
echo -e "${CYAN}[1/10] Checking system requirements...${NC}"
debug "SYS" "Checking for curl..."
if command -v curl &> /dev/null; then
    CURL_VERSION=$(curl --version | head -1)
    log_fix "curl installed: $CURL_VERSION"
    debug "SYS" "curl location: $(which curl)"
else
    log_issue "curl not installed"
    debug "SYS" "Attempting to install curl..."
    sudo apt-get update -qq && sudo apt-get install -y curl &> /dev/null
    if [ $? -eq 0 ]; then
        log_fix "curl installed successfully"
    else
        log_issue "Failed to install curl"
    fi
fi

debug "SYS" "Checking for git..."
if command -v git &> /dev/null; then
    GIT_VERSION=$(git --version)
    log_fix "git installed: $GIT_VERSION"
    debug "SYS" "git location: $(which git)"
else
    log_issue "git not installed"
fi

# Step 2: Clean up old tokens
echo ""
echo -e "${CYAN}[2/10] Cleaning up old tokens...${NC}"
TOKEN_DIR="$HOME/.hybridide/auth"
TOKEN_FILE="$TOKEN_DIR/github_token"

debug "TOKEN" "Checking token directory: $TOKEN_DIR"
if [ -d "$TOKEN_DIR" ]; then
    debug "TOKEN" "Directory exists"
    ls -la "$TOKEN_DIR" 2>/dev/null | while read line; do
        debug "TOKEN" "  $line"
    done
fi

if [ -f "$TOKEN_FILE" ]; then
    debug "TOKEN" "Token file found: $TOKEN_FILE"
    TOKEN=$(file_op "read" "$TOKEN_FILE")
    TOKEN_LENGTH=${#TOKEN}
    debug "TOKEN" "Token length: $TOKEN_LENGTH chars"
    debug "TOKEN" "Token prefix: ${TOKEN:0:10}..."
    
    if [ ! -z "$TOKEN" ]; then
        debug "TOKEN" "Testing token validity..."
        TEST_RESPONSE=$(api_call "GET" "https://api.github.com/user" "-H" "Authorization: Bearer $TOKEN")
        
        if echo "$TEST_RESPONSE" | grep -q '"login"'; then
            USERNAME=$(echo "$TEST_RESPONSE" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
            log_info "Existing token is valid for user: $USERNAME"
            debug "TOKEN" "Token is valid - keeping it"
        else
            log_warn "Existing token is invalid"
            debug "TOKEN" "API response indicates invalid token"
            file_op "delete" "$TOKEN_FILE"
            file_op "delete" "$TOKEN_DIR/user_data.json"
            log_fix "Removed invalid token files"
        fi
    else
        debug "TOKEN" "Token file is empty"
        log_info "Token file is empty"
    fi
else
    debug "TOKEN" "No existing token file"
    log_info "No existing token found"
fi

# Step 3: Check and set Client ID
echo ""
echo -e "${CYAN}[3/10] Configuring OAuth Client ID...${NC}"
debug "CLIENT" "Checking GITHUB_CLIENT_ID environment variable"

if [ -z "$GITHUB_CLIENT_ID" ]; then
    debug "CLIENT" "GITHUB_CLIENT_ID not set in environment"
    
    # Try bashrc
    if grep -q "GITHUB_CLIENT_ID" "$HOME/.bashrc" 2>/dev/null; then
        debug "CLIENT" "Found in .bashrc, sourcing..."
        source "$HOME/.bashrc"
        log_fix "Loaded Client ID from .bashrc: ${GITHUB_CLIENT_ID:0:10}..."
    elif [ ! -z "$1" ]; then
        debug "CLIENT" "Using Client ID from argument: ${1:0:10}..."
        export GITHUB_CLIENT_ID="$1"
        echo "export GITHUB_CLIENT_ID=\"$1\"" >> "$HOME/.bashrc"
        log_fix "Client ID set from argument and saved to .bashrc"
    else
        log_issue "No Client ID found"
        echo ""
        echo "  ${RED}ERROR: GITHUB_CLIENT_ID not found${NC}"
        echo "  Usage: $0 YOUR_CLIENT_ID"
        echo "  Or: export GITHUB_CLIENT_ID=\"YOUR_CLIENT_ID\" && $0"
        echo ""
        echo "Debug log saved to: $LOG_FILE"
        exit 1
    fi
else
    log_fix "Client ID already set: ${GITHUB_CLIENT_ID:0:10}..."
    debug "CLIENT" "Full Client ID: $GITHUB_CLIENT_ID"
fi

# Step 4: Validate Client ID
echo ""
echo -e "${CYAN}[4/10] Validating Client ID with GitHub...${NC}"
debug "VALIDATE" "Testing Client ID: ${GITHUB_CLIENT_ID:0:10}..."
debug "VALIDATE" "Requesting device code from GitHub..."

VALIDATION_RESPONSE=$(api_call "POST" "https://github.com/login/device/code" \
    "-H" "Accept: application/json" \
    "-d" "client_id=$GITHUB_CLIENT_ID&scope=repo,user,admin:org,delete_repo")

debug "VALIDATE" "Parsing response..."
if echo "$VALIDATION_RESPONSE" | grep -q "device_code"; then
    log_fix "Client ID is valid and recognized by GitHub"
    
    DEVICE_CODE=$(echo "$VALIDATION_RESPONSE" | grep -o '"device_code":"[^"]*"' | cut -d'"' -f4)
    USER_CODE=$(echo "$VALIDATION_RESPONSE" | grep -o '"user_code":"[^"]*"' | cut -d'"' -f4)
    
    debug "VALIDATE" "Device code: ${DEVICE_CODE:0:20}..."
    debug "VALIDATE" "User code: $USER_CODE"
elif echo "$VALIDATION_RESPONSE" | grep -q "error"; then
    ERROR=$(echo "$VALIDATION_RESPONSE" | grep -o '"error":"[^"]*"' | cut -d'"' -f4)
    ERROR_DESC=$(echo "$VALIDATION_RESPONSE" | grep -o '"error_description":"[^"]*"' | cut -d'"' -f4)
    log_issue "Client ID validation failed: $ERROR"
    debug "VALIDATE" "Error description: $ERROR_DESC"
    echo ""
    echo "Debug log saved to: $LOG_FILE"
    exit 1
else
    log_issue "Unexpected response from GitHub"
    debug "VALIDATE" "Full response: $VALIDATION_RESPONSE"
    exit 1
fi

# Step 5: Configure Git
echo ""
echo -e "${CYAN}[5/10] Configuring Git...${NC}"
debug "GIT" "Checking git configuration..."

GIT_USER=$(git config --global user.name 2>/dev/null)
GIT_EMAIL=$(git config --global user.email 2>/dev/null)

debug "GIT" "Current user.name: ${GIT_USER:-NOT SET}"
debug "GIT" "Current user.email: ${GIT_EMAIL:-NOT SET}"

if [ -z "$GIT_USER" ]; then
    if [ -f "$TOKEN_FILE" ]; then
        debug "GIT" "Fetching username from GitHub..."
        TOKEN=$(cat "$TOKEN_FILE")
        USER_INFO=$(api_call "GET" "https://api.github.com/user" "-H" "Authorization: Bearer $TOKEN")
        
        if echo "$USER_INFO" | grep -q '"login"'; then
            USERNAME=$(echo "$USER_INFO" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
            git config --global user.name "$USERNAME" 2>/dev/null
            log_fix "Git user.name set to: $USERNAME"
            debug "GIT" "Set from GitHub API"
        fi
    else
        git config --global user.name "HybridIDE-User" 2>/dev/null
        log_fix "Git user.name set to default"
        debug "GIT" "Set to default value"
    fi
else
    log_fix "Git user.name already set: $GIT_USER"
fi

if [ -z "$GIT_EMAIL" ]; then
    git config --global user.email "user@hybridide.local" 2>/dev/null
    log_fix "Git user.email set to default"
    debug "GIT" "Set to default value"
else
    log_fix "Git user.email already set: $GIT_EMAIL"
fi

# Step 6: Credential helper
echo ""
echo -e "${CYAN}[6/10] Configuring credential helper...${NC}"
CRED_HELPER=$(git config --global credential.helper 2>/dev/null)
debug "GIT" "Current credential.helper: ${CRED_HELPER:-NOT SET}"

if [ -z "$CRED_HELPER" ]; then
    git config --global credential.helper cache 2>/dev/null
    log_fix "Enabled credential caching (15 min)"
    debug "GIT" "Set to 'cache'"
else
    log_fix "Credential helper already set: $CRED_HELPER"
fi

# Step 7: Fix permissions
echo ""
echo -e "${CYAN}[7/10] Fixing file permissions...${NC}"
debug "PERM" "Checking auth directory..."

if [ -d "$TOKEN_DIR" ]; then
    CURRENT_PERM=$(stat -c "%a" "$TOKEN_DIR" 2>/dev/null || stat -f "%A" "$TOKEN_DIR" 2>/dev/null)
    debug "PERM" "Current directory permissions: $CURRENT_PERM"
    
    file_op "chmod" "$TOKEN_DIR" "700"
    log_fix "Auth directory permissions: 700"
    
    if [ -f "$TOKEN_FILE" ]; then
        TOKEN_PERM=$(stat -c "%a" "$TOKEN_FILE" 2>/dev/null || stat -f "%A" "$TOKEN_FILE" 2>/dev/null)
        debug "PERM" "Current token file permissions: $TOKEN_PERM"
        
        file_op "chmod" "$TOKEN_FILE" "600"
        log_fix "Token file permissions: 600"
    fi
else
    debug "PERM" "Creating auth directory..."
    mkdir -p "$TOKEN_DIR" 2>/dev/null
    chmod 700 "$TOKEN_DIR" 2>/dev/null
    log_fix "Created auth directory with secure permissions"
fi

# Step 8: Executable permissions
echo ""
echo -e "${CYAN}[8/10] Fixing executable permissions...${NC}"
HYBRID_IDE_DIR="/workspaces/codespaces-blank/hybrid-ide"
debug "EXEC" "Checking for executables in: $HYBRID_IDE_DIR/build/bin"

if [ -d "$HYBRID_IDE_DIR/build/bin" ]; then
    ls -la "$HYBRID_IDE_DIR/build/bin" | while read line; do
        debug "EXEC" "  $line"
    done
    
    chmod +x "$HYBRID_IDE_DIR"/build/bin/* 2>/dev/null
    log_fix "All executables are now executable"
elif [ -d "./build/bin" ]; then
    debug "EXEC" "Found in current directory"
    chmod +x ./build/bin/* 2>/dev/null
    log_fix "All executables are now executable"
else
    log_info "No build directory found"
    debug "EXEC" "Run cmake to build executables"
fi

# Step 9: Test authentication
echo ""
echo -e "${CYAN}[9/10] Testing authentication...${NC}"
debug "AUTH" "Checking for existing token..."

if [ -f "$TOKEN_FILE" ]; then
    TOKEN=$(file_op "read" "$TOKEN_FILE")
    debug "AUTH" "Token found, length: ${#TOKEN}"
    
    if [ ! -z "$TOKEN" ]; then
        debug "AUTH" "Testing token with GitHub API..."
        USER_RESPONSE=$(api_call "GET" "https://api.github.com/user" \
            "-H" "Authorization: Bearer $TOKEN" \
            "-H" "Accept: application/vnd.github+json")
        
        if echo "$USER_RESPONSE" | grep -q '"login"'; then
            USERNAME=$(echo "$USER_RESPONSE" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
            USER_ID=$(echo "$USER_RESPONSE" | grep -o '"id":[0-9]*' | cut -d':' -f2 | head -1)
            log_fix "Authenticated as: $USERNAME (ID: $USER_ID)"
            debug "AUTH" "Token is valid"
            
            # Check scopes
            debug "AUTH" "Checking token scopes..."
            SCOPES_RESPONSE=$(curl -s -I -H "Authorization: Bearer $TOKEN" \
                https://api.github.com/user | grep -i "x-oauth-scopes:")
            
            if [ ! -z "$SCOPES_RESPONSE" ]; then
                SCOPES=$(echo "$SCOPES_RESPONSE" | cut -d':' -f2 | xargs)
                debug "AUTH" "Token scopes: $SCOPES"
                
                if echo "$SCOPES" | grep -q "repo"; then
                    log_fix "Token has 'repo' scope"
                else
                    log_issue "Token missing 'repo' scope"
                    debug "AUTH" "Need to re-authenticate with correct scopes"
                    file_op "delete" "$TOKEN_FILE"
                fi
            fi
        else
            log_issue "Token is invalid"
            debug "AUTH" "API returned error, removing token"
            file_op "delete" "$TOKEN_FILE"
        fi
    fi
else
    debug "AUTH" "No token file found"
fi

# Step 10: Auto-authenticate if needed
echo ""
echo -e "${CYAN}[10/10] Running authentication if needed...${NC}"

if [ ! -f "$TOKEN_FILE" ]; then
    log_info "No valid token - starting OAuth Device Flow"
    debug "AUTH" "Generating device code..."
    
    AUTH_RESPONSE=$(api_call "POST" "https://github.com/login/device/code" \
        "-H" "Accept: application/json" \
        "-d" "client_id=$GITHUB_CLIENT_ID&scope=repo,user,admin:org,delete_repo")
    
    if echo "$AUTH_RESPONSE" | grep -q "device_code"; then
        USER_CODE=$(echo "$AUTH_RESPONSE" | grep -o '"user_code":"[^"]*"' | cut -d'"' -f4)
        VERIFICATION_URI=$(echo "$AUTH_RESPONSE" | grep -o '"verification_uri":"[^"]*"' | cut -d'"' -f4)
        DEVICE_CODE=$(echo "$AUTH_RESPONSE" | grep -o '"device_code":"[^"]*"' | cut -d'"' -f4)
        INTERVAL=$(echo "$AUTH_RESPONSE" | grep -o '"interval":[0-9]*' | cut -d':' -f2)
        EXPIRES_IN=$(echo "$AUTH_RESPONSE" | grep -o '"expires_in":[0-9]*' | cut -d':' -f2)
        
        debug "AUTH" "User code: $USER_CODE"
        debug "AUTH" "Device code: ${DEVICE_CODE:0:20}..."
        debug "AUTH" "Verification URI: $VERIFICATION_URI"
        debug "AUTH" "Polling interval: ${INTERVAL}s"
        debug "AUTH" "Expires in: ${EXPIRES_IN}s"
        
        echo ""
        echo "  ╔═══════════════════════════════════════════════════════════════╗"
        echo "  ║  AUTHORIZATION CODE: $USER_CODE"
        echo "  ║  Visit: $VERIFICATION_URI"
        echo "  ╚═══════════════════════════════════════════════════════════════╝"
        echo ""
        
        # Try to open browser
        debug "BROWSER" "Attempting to open browser..."
        if command -v brave-browser &> /dev/null; then
            brave-browser "$VERIFICATION_URI" 2>/dev/null &
            log_fix "Brave browser opened"
            debug "BROWSER" "Used: brave-browser"
        elif command -v brave &> /dev/null; then
            brave "$VERIFICATION_URI" 2>/dev/null &
            log_fix "Brave browser opened"
            debug "BROWSER" "Used: brave"
        elif command -v xdg-open &> /dev/null; then
            xdg-open "$VERIFICATION_URI" 2>/dev/null &
            log_fix "Browser opened"
            debug "BROWSER" "Used: xdg-open"
        elif command -v open &> /dev/null; then
            open "$VERIFICATION_URI" 2>/dev/null &
            log_fix "Browser opened"
            debug "BROWSER" "Used: open"
        else
            log_info "Please open URL manually"
            debug "BROWSER" "No browser command found"
        fi
        
        echo ""
        echo -e "  ${YELLOW}Waiting for authorization...${NC}"
        echo ""
        
        # Poll for token
        MAX_ATTEMPTS=24
        ATTEMPT=0
        
        while [ $ATTEMPT -lt $MAX_ATTEMPTS ]; do
            sleep ${INTERVAL:-5}
            ((ATTEMPT++))
            
            debug "POLL" "Attempt $ATTEMPT/$MAX_ATTEMPTS"
            
            TOKEN_RESPONSE=$(api_call "POST" "https://github.com/login/oauth/access_token" \
                "-H" "Accept: application/json" \
                "-d" "client_id=$GITHUB_CLIENT_ID&device_code=$DEVICE_CODE&grant_type=urn:ietf:params:oauth:grant-type:device_code")
            
            debug "POLL" "Response: ${TOKEN_RESPONSE:0:100}..."
            
            if echo "$TOKEN_RESPONSE" | grep -q "access_token"; then
                ACCESS_TOKEN=$(echo "$TOKEN_RESPONSE" | grep -o '"access_token":"[^"]*"' | cut -d'"' -f4)
                
                if [ ! -z "$ACCESS_TOKEN" ]; then
                    debug "AUTH" "Access token received!"
                    debug "AUTH" "Token length: ${#ACCESS_TOKEN}"
                    debug "AUTH" "Token prefix: ${ACCESS_TOKEN:0:10}..."
                    
                    # Save token
                    mkdir -p "$TOKEN_DIR"
                    echo "$ACCESS_TOKEN" > "$TOKEN_FILE"
                    file_op "chmod" "$TOKEN_FILE" "600"
                    
                    log_fix "✓ Authorization successful!"
                    log_fix "Access token saved securely"
                    debug "AUTH" "Token saved to: $TOKEN_FILE"
                    
                    # Get user info
                    USER_INFO=$(api_call "GET" "https://api.github.com/user" \
                        "-H" "Authorization: Bearer $ACCESS_TOKEN" \
                        "-H" "Accept: application/vnd.github+json")
                    
                    if echo "$USER_INFO" | grep -q '"login"'; then
                        USERNAME=$(echo "$USER_INFO" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
                        echo "$USER_INFO" > "$TOKEN_DIR/user_data.json"
                        log_fix "Authenticated as: $USERNAME"
                        debug "AUTH" "User data saved"
                    fi
                    
                    break
                fi
            elif echo "$TOKEN_RESPONSE" | grep -q "authorization_pending"; then
                echo "  ⏳ Waiting... (attempt $ATTEMPT/$MAX_ATTEMPTS)"
                debug "POLL" "Status: authorization_pending"
            elif echo "$TOKEN_RESPONSE" | grep -q "slow_down"; then
                echo "  ⏳ Slowing down..."
                debug "POLL" "Status: slow_down - adding delay"
                sleep 5
            elif echo "$TOKEN_RESPONSE" | grep -q "expired_token"; then
                log_issue "Device code expired"
                debug "POLL" "Status: expired_token"
                break
            elif echo "$TOKEN_RESPONSE" | grep -q "access_denied"; then
                log_issue "Authorization denied"
                debug "POLL" "Status: access_denied"
                break
            fi
        done
        
        if [ $ATTEMPT -eq $MAX_ATTEMPTS ]; then
            log_issue "Timeout waiting for authorization"
            debug "POLL" "Reached max attempts"
        fi
    else
        log_issue "Failed to generate device code"
        debug "AUTH" "Error response: $AUTH_RESPONSE"
    fi
else
    log_fix "Already authenticated"
    debug "AUTH" "Token exists and is valid"
fi

# Summary
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}DEBUG SESSION COMPLETE${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo ""
echo "Statistics:"
echo "  • Fixes applied: $FIXES_APPLIED"
echo "  • Issues found: $ISSUES_FOUND"
echo "  • API calls made: $API_CALLS"
echo "  • File operations: $FILE_OPERATIONS"
echo ""
echo "Debug log saved to: $LOG_FILE"
echo ""

if [ $ISSUES_FOUND -eq 0 ]; then
    echo -e "${GREEN}✓ No issues found - everything working!${NC}"
else
    echo -e "${YELLOW}⚠ Found $ISSUES_FOUND issue(s) - check log for details${NC}"
fi

echo ""
echo -e "${BLUE}═══════════════════════════════════════════════════════════════════${NC}"
echo ""

# Save final stats to log
echo "" >> "$LOG_FILE"
echo "═══════════════════════════════════════════════════════════════════" >> "$LOG_FILE"
echo "Debug Session Summary" >> "$LOG_FILE"
echo "═══════════════════════════════════════════════════════════════════" >> "$LOG_FILE"
echo "Fixes applied: $FIXES_APPLIED" >> "$LOG_FILE"
echo "Issues found: $ISSUES_FOUND" >> "$LOG_FILE"
echo "API calls: $API_CALLS" >> "$LOG_FILE"
echo "File operations: $FILE_OPERATIONS" >> "$LOG_FILE"
echo "Completed: $(date)" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

exit 0
