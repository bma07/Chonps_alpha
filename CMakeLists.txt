cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

# Chonps 
project(Chonps)

# Architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE_TYPE x86_64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCHITECTURE_TYPE x86)
endif()

# Platform
if (WIN32)
    add_definitions(-DCHONPS_PLATFORM_WINDOWS)
    set(PLATFORM_TYPE windows)

elseif(APPLE)
    add_definitions(-DCHONPS_PLATFORM_MACOS)
    set(PLATFORM_TYPE macos)

elseif(UNIX AND NOT APPLE)
    add_definitions(-DCHONPS_PLATFORM_LINUX)
    set(PLATFORM_TYPE linux)

else()
    add_definitions(-DCHONPS_PLATFORM_UNKNOWN)
endif()

# Output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PLATFORM_TYPE}-${ARCHITECTURE_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PLATFORM_TYPE}-${ARCHITECTURE_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PLATFORM_TYPE}-${ARCHITECTURE_TYPE})


# Subdirectories ---------------------------------------- /

# GLFW
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/GLFW)

# ------------------------------------------------------- /

set(INCLUDE_HEADERS
    Chonps/include/Chonps/Chonps.h    
)

set(SOURCES
    Chonps/src/Core.h
    Chonps/src/Log.cpp
    Chonps/src/Log.h
    ${INCLUDE_HEADERS}
)

add_library(Chonps STATIC ${SOURCES})

target_include_directories(Chonps 
    PUBLIC
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/spdlog/include
        ${PROJECT_SOURCE_DIR}/Chonps/src
        ${PROJECT_SOURCE_DIR}/Chonps/include
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/GLFW/include
)


# Build definitions
target_compile_definitions(Chonps PRIVATE
    $<$<CONFIG:Debug>:CHONPS_DEBUG>
    $<$<CONFIG:Release>:CHONPS_RELEASE>
)

# target_precompile_headers(Pulsar PRIVATE Pulsar/src/pch.h)

# GLFW
target_link_libraries(Chonps 
    PRIVATE 
        GLFW
        opengl32
)

# Sandbox
set(SOURCES
    Sandbox/src/Sandbox.cpp
)

add_executable(Sandbox ${SOURCES})

target_include_directories(Sandbox 
        PUBLIC
            ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/spdlog/include
        PRIVATE
            ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/GLFW/include
)

target_link_libraries(Sandbox
    PRIVATE
        Chonps
        GLFW
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)    
endif()