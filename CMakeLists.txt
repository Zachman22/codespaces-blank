cmake_minimum_required(VERSION 3.15)
project(HybridIDE VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source files
set(SOURCES
    src/main.cpp
    src/webview_host.cpp
    src/compiler.cpp
    src/system_info.cpp
    src/prompt_verifier.cpp
    src/code_signer.cpp
    src/file_operations.cpp
    src/plugin_manager.cpp
    src/auto_installer.cpp
    src/auto_updater.cpp
    src/docker_manager.cpp
    src/auto_coder.cpp
    src/ai_agent_system.cpp
    src/auto_coder_chatbot.cpp
)

# Header files
set(HEADERS
    include/webview_host.h
    include/compiler.h
    include/system_info.h
    include/prompt_verifier.h
    include/code_signer.h
    include/file_operations.h
    include/plugin_manager.h
    include/auto_installer.h
    include/auto_updater.h
    include/docker_manager.h
    include/auto_coder.h
    include/ai_agent_system.h
    include/auto_coder_chatbot.h
    include/script_generator.h
    include/script_verifier.h
)

# Create executable
add_executable(HybridIDE ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(HybridIDE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler flags
if(MSVC)
    target_compile_options(HybridIDE PRIVATE /W4)
    target_compile_definitions(HybridIDE PRIVATE UNICODE _UNICODE)
else()
    target_compile_options(HybridIDE PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Windows-specific settings
if(WIN32)
    # Link Windows libraries
    target_link_libraries(HybridIDE PRIVATE
        ole32
        oleaut32
        uuid
        shell32
        user32
        shlwapi
        crypt32
    )
    
    # Try to find OpenSSL
    find_package(OpenSSL QUIET)
    if(OPENSSL_FOUND)
        target_link_libraries(HybridIDE PRIVATE OpenSSL::SSL OpenSSL::Crypto)
        message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
    else()
        message(WARNING "OpenSSL not found - code signing may use fallback methods")
        # Try to link directly if available
        target_link_libraries(HybridIDE PRIVATE ssl crypto)
    endif()
    
    # Set subsystem to Windows (GUI app)
    if(MSVC)
        set_target_properties(HybridIDE PROPERTIES
            WIN32_EXECUTABLE ON
            LINK_FLAGS "/SUBSYSTEM:WINDOWS"
        )
    else()
        # MinGW
        set_target_properties(HybridIDE PROPERTIES
            WIN32_EXECUTABLE ON
            LINK_FLAGS "-mwindows"
        )
        target_link_options(HybridIDE PRIVATE -static-libgcc -static-libstdc++)
    endif()
else()
    # Linux/Unix
    # Try to find OpenSSL
    find_package(OpenSSL REQUIRED)
    if(OPENSSL_FOUND)
        target_link_libraries(HybridIDE PRIVATE OpenSSL::SSL OpenSSL::Crypto)
        message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
    endif()
endif()

# Installation rules
install(TARGETS HybridIDE
    RUNTIME DESTINATION bin
)

# Install frontend dist
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/frontend/dist/
    DESTINATION bin/frontend
    PATTERN ".*" EXCLUDE
)

# Install examples
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/
    DESTINATION examples
    FILES_MATCHING PATTERN "*.cpp" PATTERN "*.h"
)

# Install documentation
install(FILES
    README.md
    QUICKSTART.md
    CODE_SIGNING.md
    LICENSE
    DESTINATION .
)

# CPack configuration for installer
set(CPACK_PACKAGE_NAME "HybridIDE")
set(CPACK_PACKAGE_VENDOR "Zachary's Playroom")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern C++ IDE with React UI")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "HybridIDE")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# Windows installer
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "Hybrid IDE")
    set(CPACK_NSIS_PACKAGE_NAME "Hybrid IDE")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Hybrid IDE.lnk' '$INSTDIR\\\\bin\\\\HybridIDE.exe'"
    )
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$SMPROGRAMS\\\\$START_MENU_FOLDER\\\\Hybrid IDE.lnk'"
    )
endif()

include(CPack)

# Build standalone chatbot executable
add_executable(AutoCoderChatbot 
    chatbot_main.cpp
    src/auto_coder_chatbot.cpp
    src/auto_coder.cpp
    src/prompt_verifier.cpp
    src/compiler.cpp
    src/code_signer.cpp
)
target_include_directories(AutoCoderChatbot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(AutoCoderChatbot PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
if(MSVC)
    target_compile_options(AutoCoderChatbot PRIVATE /W4)
else()
    target_compile_options(AutoCoderChatbot PRIVATE -Wall -Wextra)
endif()

# Print configuration
# Build standalone script generator chatbot executable
add_executable(ScriptGeneratorChatbot 
    script_generator_main.cpp
    src/script_generator.cpp
    src/script_verifier.cpp
    src/code_signer.cpp
)
target_include_directories(ScriptGeneratorChatbot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(ScriptGeneratorChatbot PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
if(MSVC)
    target_compile_options(ScriptGeneratorChatbot PRIVATE /W4)
else()
    target_compile_options(ScriptGeneratorChatbot PRIVATE -Wall -Wextra)
endif()

# Print configuration
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  Hybrid IDE Configuration")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Targets: HybridIDE, AutoCoderChatbot, ScriptGeneratorChatbot")
message(STATUS "========================================")
message(STATUS "")

