#!/bin/bash

# Feature List Checker - Validates project completion status
# Checks what's done, what's missing, what needs debugging

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m'

# Counters
FEATURES_COMPLETE=0
FEATURES_INCOMPLETE=0
FEATURES_NEEDS_DEBUG=0
FEATURES_MISSING=0
TOTAL_FEATURES=0

clear
echo -e "${BLUE}"
echo "╔═══════════════════════════════════════════════════════════════════╗"
echo "║              Feature List Checker - Project Status               ║"
echo "║           Completion, Debugging & Missing Features               ║"
echo "╚═══════════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo ""
echo "Checking project completion status..."
echo ""
sleep 1

# Status functions
check_complete() {
    echo -e "  ${GREEN}✓ COMPLETE${NC} $1"
    ((FEATURES_COMPLETE++))
    ((TOTAL_FEATURES++))
}

check_incomplete() {
    echo -e "  ${YELLOW}⚠ INCOMPLETE${NC} $1"
    echo -e "    ${YELLOW}→${NC} $2"
    ((FEATURES_INCOMPLETE++))
    ((TOTAL_FEATURES++))
}

check_needs_debug() {
    echo -e "  ${RED}⚠ NEEDS DEBUG${NC} $1"
    echo -e "    ${RED}→${NC} $2"
    ((FEATURES_NEEDS_DEBUG++))
    ((TOTAL_FEATURES++))
}

check_missing() {
    echo -e "  ${RED}✗ MISSING${NC} $1"
    echo -e "    ${RED}→${NC} $2"
    ((FEATURES_MISSING++))
    ((TOTAL_FEATURES++))
}

check_info() {
    echo -e "  ${CYAN}ℹ INFO${NC} $1"
}

# Frontend Check
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}FRONTEND (React + TypeScript)${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

# Check if frontend build exists
if [ -d "frontend/dist" ]; then
    check_complete "Production build exists (frontend/dist/)"
else
    check_missing "Production build" "Run: cd frontend && npm run build"
fi

# Check package.json
if [ -f "frontend/package.json" ]; then
    check_complete "package.json exists"
    
    # Check if node_modules exists
    if [ -d "frontend/node_modules" ]; then
        check_complete "Dependencies installed (node_modules/)"
    else
        check_incomplete "Dependencies not installed" "Run: cd frontend && npm install"
    fi
else
    check_missing "package.json" "Frontend configuration missing"
fi

# Check TypeScript config
if [ -f "frontend/tsconfig.json" ]; then
    check_complete "TypeScript configuration (tsconfig.json)"
else
    check_incomplete "TypeScript config missing" "Create tsconfig.json"
fi

# Check React components
COMPONENTS=(
    "frontend/src/components/Editor.tsx"
    "frontend/src/components/FileExplorer.tsx"
    "frontend/src/components/Toolbar.tsx"
    "frontend/src/components/AISidebar.tsx"
    "frontend/src/components/AutoCoderPanel.tsx"
    "frontend/src/components/BuildPanel.tsx"
)

COMPONENT_COUNT=0
for component in "${COMPONENTS[@]}"; do
    if [ -f "$component" ]; then
        ((COMPONENT_COUNT++))
    fi
done

if [ $COMPONENT_COUNT -eq ${#COMPONENTS[@]} ]; then
    check_complete "Core React components (${COMPONENT_COUNT}/${#COMPONENTS[@]})"
elif [ $COMPONENT_COUNT -gt 0 ]; then
    check_incomplete "Some components missing (${COMPONENT_COUNT}/${#COMPONENTS[@]})" "Check frontend/src/components/"
else
    check_missing "React components" "Create components in frontend/src/components/"
fi

# Check for TypeScript errors
if [ -f "frontend/tsconfig.json" ]; then
    if command -v tsc &> /dev/null; then
        cd frontend 2>/dev/null && tsc --noEmit &> /tmp/tsc_check.log
        if [ $? -eq 0 ]; then
            check_complete "No TypeScript errors"
        else
            check_needs_debug "TypeScript has errors" "Run: cd frontend && npm run build to see errors"
        fi
        cd - > /dev/null 2>&1
    fi
fi

# Backend Check
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}BACKEND (C++)${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

# Check CMakeLists.txt
if [ -f "CMakeLists.txt" ]; then
    check_complete "CMakeLists.txt exists"
else
    check_missing "CMakeLists.txt" "Create CMake configuration"
fi

# Check build directory
if [ -d "build" ]; then
    check_complete "Build directory exists"
    
    # Check if CMake was run
    if [ -f "build/CMakeCache.txt" ]; then
        check_complete "CMake configured"
    else
        check_incomplete "CMake not configured" "Run: cd build && cmake .."
    fi
else
    check_incomplete "Build directory missing" "Run: mkdir build && cd build && cmake .."
fi

# Check executables
EXECUTABLES=(
    "build/bin/HybridIDE"
    "build/bin/AutoCoderChatbot"
    "build/bin/ScriptGeneratorChatbot"
    "build/bin/SetupAuth"
)

EXEC_COUNT=0
for exec in "${EXECUTABLES[@]}"; do
    if [ -f "$exec" ]; then
        ((EXEC_COUNT++))
        # Check if executable
        if [ -x "$exec" ]; then
            :
        else
            check_needs_debug "$(basename $exec) not executable" "Run: chmod +x $exec"
        fi
    fi
done

if [ $EXEC_COUNT -eq ${#EXECUTABLES[@]} ]; then
    check_complete "All executables built (${EXEC_COUNT}/${#EXECUTABLES[@]})"
elif [ $EXEC_COUNT -gt 0 ]; then
    check_incomplete "Some executables missing (${EXEC_COUNT}/${#EXECUTABLES[@]})" "Run: cd build && make"
else
    check_missing "Executables not built" "Run: cd build && cmake .. && make"
fi

# Check source files
CORE_SYSTEMS=(
    "src/github_auth.cpp"
    "src/compiler.cpp"
    "src/debugger.cpp"
    "src/ai_api_client.cpp"
    "src/file_operations.cpp"
)

SRC_COUNT=0
for src in "${CORE_SYSTEMS[@]}"; do
    if [ -f "$src" ]; then
        ((SRC_COUNT++))
    fi
done

if [ $SRC_COUNT -eq ${#CORE_SYSTEMS[@]} ]; then
    check_complete "Core system source files (${SRC_COUNT}/${#CORE_SYSTEMS[@]})"
elif [ $SRC_COUNT -gt 0 ]; then
    check_incomplete "Some source files missing (${SRC_COUNT}/${#CORE_SYSTEMS[@]})" "Check src/ directory"
else
    check_missing "Source files" "Create C++ implementations in src/"
fi

# Check for compilation errors
if [ -d "build" ] && [ -f "build/CMakeCache.txt" ]; then
    cd build 2>/dev/null && make 2>&1 | grep -i "error:" > /tmp/compile_errors.log
    if [ -s /tmp/compile_errors.log ]; then
        ERROR_COUNT=$(wc -l < /tmp/compile_errors.log)
        check_needs_debug "Compilation errors found ($ERROR_COUNT)" "Run: cd build && make to see errors"
    else
        check_complete "No compilation errors"
    fi
    cd - > /dev/null 2>&1
fi

# Authentication System
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}AUTHENTICATION SYSTEM${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

# Check OAuth implementation
if [ -f "src/github_auth.cpp" ]; then
    # Check for real API calls vs simulated
    if grep -q "curl_easy_perform" "src/github_auth.cpp" 2>/dev/null; then
        check_complete "Real GitHub API integration (curl)"
    elif grep -q "SIMULATED" "src/github_auth.cpp" 2>/dev/null; then
        check_needs_debug "Using simulated API calls" "Replace with real curl-based API calls"
    else
        check_incomplete "API implementation unclear" "Verify github_auth.cpp implementation"
    fi
    
    # Check for environment variable support
    if grep -q "getenv.*GITHUB_CLIENT_ID" "src/github_auth.cpp" 2>/dev/null; then
        check_complete "Environment variable support (GITHUB_CLIENT_ID)"
    else
        check_incomplete "No environment variable support" "Add getenv(\"GITHUB_CLIENT_ID\")"
    fi
else
    check_missing "github_auth.cpp" "Create OAuth authentication implementation"
fi

# Check Client ID configuration
if [ ! -z "$GITHUB_CLIENT_ID" ]; then
    check_complete "GITHUB_CLIENT_ID environment variable set"
else
    check_incomplete "GITHUB_CLIENT_ID not set" "Run: export GITHUB_CLIENT_ID=\"YOUR_CLIENT_ID\""
fi

# Check token storage
if [ -d "$HOME/.hybridide/auth" ]; then
    check_complete "Auth storage directory exists"
    
    if [ -f "$HOME/.hybridide/auth/github_token" ]; then
        # Check token validity
        TOKEN=$(cat "$HOME/.hybridide/auth/github_token" 2>/dev/null)
        if [ ! -z "$TOKEN" ]; then
            # Test with API
            RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" https://api.github.com/user 2>/dev/null)
            if echo "$RESPONSE" | grep -q '"login"'; then
                USERNAME=$(echo "$RESPONSE" | grep -o '"login":"[^"]*"' | cut -d'"' -f4)
                check_complete "Valid token (authenticated as: $USERNAME)"
            else
                check_needs_debug "Token is invalid or expired" "Re-authenticate: ./build/bin/SetupAuth"
            fi
        else
            check_incomplete "Token file is empty" "Run authentication"
        fi
    else
        check_incomplete "No authentication token" "Run: ./build/bin/SetupAuth"
    fi
else
    check_incomplete "Auth directory not created" "Will be created on first auth"
fi

# OAuth Troubleshooting Tools
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}TROUBLESHOOTING TOOLS${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

TOOLS=(
    "oauth_troubleshooter.sh:Basic OAuth diagnostics"
    "advanced_oauth_troubleshooter.sh:Advanced diagnostics with auto-fix"
    "oauth_autofixer.sh:Automatic OAuth fixes"
    "oauth_autofixer_debugger.sh:Verbose debug mode"
    "setup_oauth.sh:Client ID setup"
)

for tool_info in "${TOOLS[@]}"; do
    IFS=':' read -r tool desc <<< "$tool_info"
    if [ -f "$tool" ]; then
        if [ -x "$tool" ]; then
            check_complete "$desc ($tool)"
        else
            check_needs_debug "$tool exists but not executable" "Run: chmod +x $tool"
        fi
    else
        check_missing "$tool" "Create troubleshooting script"
    fi
done

# Documentation
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}DOCUMENTATION${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

DOCS=(
    "README.md:Main documentation"
    "OAUTH_SETUP_GUIDE.md:OAuth setup guide"
    "OAUTH_VS_MARKETPLACE.md:OAuth vs Marketplace explanation"
    "AUTHORIZATION_FIX.md:Authorization troubleshooting"
    "COMPLETE_BUILD_STATUS.md:Complete project status"
)

for doc_info in "${DOCS[@]}"; do
    IFS=':' read -r doc desc <<< "$doc_info"
    if [ -f "$doc" ]; then
        LINE_COUNT=$(wc -l < "$doc")
        if [ $LINE_COUNT -gt 50 ]; then
            check_complete "$desc ($LINE_COUNT lines)"
        else
            check_incomplete "$desc is too short ($LINE_COUNT lines)" "Expand documentation"
        fi
    else
        check_missing "$doc" "Create $desc"
    fi
done

# Git Repository
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}GIT REPOSITORY${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

if [ -d ".git" ]; then
    check_complete "Git repository initialized"
    
    # Check for uncommitted changes
    if git diff --quiet && git diff --cached --quiet 2>/dev/null; then
        check_complete "No uncommitted changes"
    else
        check_incomplete "Uncommitted changes exist" "Run: git add -A && git commit"
    fi
    
    # Check remote
    if git remote -v | grep -q "origin"; then
        REMOTE_URL=$(git remote get-url origin 2>/dev/null)
        check_complete "Remote origin configured: $REMOTE_URL"
        
        # Check if pushed
        LOCAL=$(git rev-parse @ 2>/dev/null)
        REMOTE=$(git rev-parse @{u} 2>/dev/null)
        if [ "$LOCAL" = "$REMOTE" ]; then
            check_complete "All changes pushed to remote"
        else
            check_incomplete "Local commits not pushed" "Run: git push origin main"
        fi
    else
        check_incomplete "No remote repository" "Run: git remote add origin <URL>"
    fi
else
    check_missing "Git repository" "Run: git init"
fi

# Build System
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}BUILD SYSTEM${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

# Check build scripts
BUILD_SCRIPTS=(
    "build.sh:Linux build script"
    "build.bat:Windows build script"
    "cmake_build.sh:CMake build script"
)

for script_info in "${BUILD_SCRIPTS[@]}"; do
    IFS=':' read -r script desc <<< "$script_info"
    if [ -f "$script" ]; then
        if [ -x "$script" ]; then
            check_complete "$desc"
        else
            check_needs_debug "$script not executable" "Run: chmod +x $script"
        fi
    else
        check_incomplete "$desc missing" "Create build automation script"
    fi
done

# Dependencies
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}SYSTEM DEPENDENCIES${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

# Check required tools
REQUIRED_TOOLS=(
    "git:Version control"
    "cmake:Build system"
    "g++:C++ compiler"
    "curl:HTTP client"
    "node:JavaScript runtime"
    "npm:Package manager"
)

for tool_info in "${REQUIRED_TOOLS[@]}"; do
    IFS=':' read -r tool desc <<< "$tool_info"
    if command -v $tool &> /dev/null; then
        VERSION=$($tool --version 2>&1 | head -1)
        check_complete "$desc ($tool) - $VERSION"
    else
        check_missing "$tool" "Install: sudo apt-get install $tool"
    fi
done

# Summary
echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}SUMMARY${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"
echo ""

COMPLETION_PERCENT=$((FEATURES_COMPLETE * 100 / TOTAL_FEATURES))

echo "Total Features Checked: $TOTAL_FEATURES"
echo ""
echo -e "  ${GREEN}✓ Complete:${NC}        $FEATURES_COMPLETE"
echo -e "  ${YELLOW}⚠ Incomplete:${NC}      $FEATURES_INCOMPLETE"
echo -e "  ${RED}⚠ Needs Debug:${NC}     $FEATURES_NEEDS_DEBUG"
echo -e "  ${RED}✗ Missing:${NC}         $FEATURES_MISSING"
echo ""

if [ $COMPLETION_PERCENT -ge 90 ]; then
    echo -e "${GREEN}Project Completion: $COMPLETION_PERCENT% ✓ EXCELLENT${NC}"
elif [ $COMPLETION_PERCENT -ge 75 ]; then
    echo -e "${CYAN}Project Completion: $COMPLETION_PERCENT% - Good Progress${NC}"
elif [ $COMPLETION_PERCENT -ge 50 ]; then
    echo -e "${YELLOW}Project Completion: $COMPLETION_PERCENT% - Needs Work${NC}"
else
    echo -e "${RED}Project Completion: $COMPLETION_PERCENT% - Major Work Needed${NC}"
fi

echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════════${NC}"

# Priority actions
if [ $FEATURES_MISSING -gt 0 ] || [ $FEATURES_NEEDS_DEBUG -gt 0 ] || [ $FEATURES_INCOMPLETE -gt 0 ]; then
    echo ""
    echo -e "${YELLOW}PRIORITY ACTIONS:${NC}"
    echo ""
    
    if [ $FEATURES_MISSING -gt 0 ]; then
        echo -e "  ${RED}1. Fix MISSING features first (critical)${NC}"
    fi
    
    if [ $FEATURES_NEEDS_DEBUG -gt 0 ]; then
        echo -e "  ${RED}2. Debug failing components${NC}"
    fi
    
    if [ $FEATURES_INCOMPLETE -gt 0 ]; then
        echo -e "  ${YELLOW}3. Complete incomplete features${NC}"
    fi
    
    echo ""
fi

echo ""
exit 0
