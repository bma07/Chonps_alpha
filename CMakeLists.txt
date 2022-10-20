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
    add_definitions(
        -DCHONPS_PLATFORM_WINDOWS
    )
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

# glad
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/glad)

# Imgui
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/imgui)

# ------------------------------------------------------- /

set(INCLUDE_HEADERS
    Chonps/include/Chonps/Chonps.h    
)

set(GRAPHICS_SOURCES
    Chonps/src/Graphics/Camera.cpp
    Chonps/src/Graphics/Camera.h
    Chonps/src/Graphics/EBO.cpp
    Chonps/src/Graphics/EBO.h
    Chonps/src/Graphics/FBO.cpp
    Chonps/src/Graphics/FBO.h
    Chonps/src/Graphics/File.cpp
    Chonps/src/Graphics/File.h
    Chonps/src/Graphics/Graphics.h
    Chonps/src/Graphics/Lighting.cpp
    Chonps/src/Graphics/Lighting.h
    Chonps/src/Graphics/Mesh.cpp
    Chonps/src/Graphics/Mesh.h
    Chonps/src/Graphics/Model.cpp
    Chonps/src/Graphics/Model.h
    Chonps/src/Graphics/OrthographicCamera.cpp
    Chonps/src/Graphics/OrthographicCamera.h
    Chonps/src/Graphics/Renderer.cpp
    Chonps/src/Graphics/Renderer.h
    Chonps/src/Graphics/RendererAPI.cpp
    Chonps/src/Graphics/RendererAPI.h
    Chonps/src/Graphics/Shader.cpp
    Chonps/src/Graphics/Shader.h
    Chonps/src/Graphics/Texture.cpp
    Chonps/src/Graphics/Texture.h
    Chonps/src/Graphics/VAO.cpp
    Chonps/src/Graphics/VAO.h
    Chonps/src/Graphics/VBO.cpp
    Chonps/src/Graphics/VBO.h
)

set(MODEL_LOADER_SOURCES
    # OBJ
    Chonps/src/ModelLoaders/OBJ/OBJModelLoader.cpp
    Chonps/src/ModelLoaders/OBJ/OBJModelLoader.h
)

set(PLATFORM_SOURCES
    # OpenGL
    Chonps/src/Platform/OpenGL/OpenGLContext.cpp
    Chonps/src/Platform/OpenGL/OpenGLContext.h
    Chonps/src/Platform/OpenGL/OpenGLEBO.cpp
    Chonps/src/Platform/OpenGL/OpenGLEBO.h
    Chonps/src/Platform/OpenGL/OpenGLFBO.cpp
    Chonps/src/Platform/OpenGL/OpenGLFBO.h
    Chonps/src/Platform/OpenGL/OpenGLRendererAPI.cpp
    Chonps/src/Platform/OpenGL/OpenGLRendererAPI.h
    Chonps/src/Platform/OpenGL/OpenGLShader.cpp
    Chonps/src/Platform/OpenGL/OpenGLShader.h
    Chonps/src/Platform/OpenGL/OpenGLTexture.cpp
    Chonps/src/Platform/OpenGL/OpenGLTexture.h
    Chonps/src/Platform/OpenGL/OpenGLVAO.cpp
    Chonps/src/Platform/OpenGL/OpenGLVAO.h
    Chonps/src/Platform/OpenGL/OpenGLVBO.cpp
    Chonps/src/Platform/OpenGL/OpenGLVBO.h

    # GLFW
    Chonps/src/Platform/WindowAPI/glfwAPI/glfwInput.cpp
    Chonps/src/Platform/WindowAPI/glfwAPI/glfwInput.h
    Chonps/src/Platform/WindowAPI/glfwAPI/glfwWindowAPI.h
    Chonps/src/Platform/WindowAPI/glfwAPI/glfwWindowAPI.cpp
)

set(SOURCES
    ${INCLUDE_HEADERS}
    # stb_image
    Chonps/Dependencies/stb_image/stb_image.cpp
    Chonps/Dependencies/stb_image/stb_image.h

    # Events
    Chonps/src/Events/ApplicationEvents.h
    Chonps/src/Events/Event.h
    Chonps/src/Events/KeyEvents.h
    Chonps/src/Events/MouseEvents.h
    Chonps/src/Events/WindowEvents.h
    
    # Imgui
    Chonps/src/Imgui/ImguiBuild.cpp
    Chonps/src/Imgui/ImguiEditor.h
    Chonps/src/Imgui/ImguiEditor.cpp
    Chonps/src/Imgui/ImguiWindow.h
    Chonps/src/Imgui/ImguiWindow.cpp
    
    # Core
    Chonps/src/Application.cpp
    Chonps/src/Application.h
    Chonps/src/cepch.cpp
    Chonps/src/cepch.h
    Chonps/src/Chonpslib.h
    Chonps/src/Core.h
    Chonps/src/DataTypes.h
    Chonps/src/Input.cpp
    Chonps/src/Input.h
    Chonps/src/KeyCodes.h
    Chonps/src/Layer.h
    Chonps/src/LayerStack.h
    Chonps/src/LayerStack.cpp
    Chonps/src/Log.cpp
    Chonps/src/Log.h
    Chonps/src/MouseButtonCodes.h
    Chonps/src/Window.cpp
    Chonps/src/Window.h
    ${PLATFORM_SOURCES}
    ${GRAPHICS_SOURCES}
    ${MODEL_LOADER_SOURCES}
)

add_library(Chonps STATIC ${SOURCES})

target_include_directories(Chonps 
    PUBLIC
    ${PROJECT_SOURCE_DIR}/Chonps/src
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/spdlog/include
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/GLFW/include
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/glad/include
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/imgui/include
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/glm/include
        ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/stb_image
)


# Build definitions
target_compile_definitions(Chonps PRIVATE
    $<$<CONFIG:Debug>:CHONPS_DEBUG>
    $<$<CONFIG:Release>:CHONPS_RELEASE>
)

target_precompile_headers(Chonps PRIVATE Chonps/src/cepch.h)

# GLFW
target_link_libraries(Chonps 
    PRIVATE 
        GLFW
        glad
        opengl32
    PUBLIC
        Imgui
)

# Sandbox
set(SOURCES
    Sandbox/src/Sandbox.cpp
)

add_executable(Sandbox ${SOURCES})

target_include_directories(Sandbox 
        PUBLIC
            ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/spdlog/include
            ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/imgui/include
            ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/glm/include
)

target_link_libraries(Sandbox
    PRIVATE
        Chonps
    PUBLIC
        Imgui
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)    
endif()