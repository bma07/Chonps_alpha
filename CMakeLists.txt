cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)

# Chonps 
project(Chonps)

# Architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCHITECTURE_TYPE x64)
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
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/GLFW)

# glad
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/glad)

# Imgui
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/imgui)

# zlib
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/zlib)

# libpng
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/libpng)

# freetype
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/freetype)

# msdfgen
add_subdirectory(${CMAKE_SOURCE_DIR}/Chonps/Dependencies/msdfgen)

# ------------------------------------------------------- /

set(GRAPHICS_SOURCES
	# Graphics
	Chonps/src/Graphics/BatchRenderer.cpp
	Chonps/src/Graphics/BatchRenderer.h
	Chonps/src/Graphics/Camera.cpp
	Chonps/src/Graphics/Camera.h
	Chonps/src/Graphics/Cubemap.cpp
	Chonps/src/Graphics/Cubemap.h
	Chonps/src/Graphics/DrawList.cpp
	Chonps/src/Graphics/DrawList.h
	Chonps/src/Graphics/Font.cpp
	Chonps/src/Graphics/Font.h
	Chonps/src/Graphics/FrameBuffer.cpp
	Chonps/src/Graphics/FrameBuffer.h
	Chonps/src/Graphics/Graphics.h
	Chonps/src/Graphics/IndexBuffer.cpp
	Chonps/src/Graphics/IndexBuffer.h
	Chonps/src/Graphics/Lighting.cpp
	Chonps/src/Graphics/Lighting.h
	Chonps/src/Graphics/Mesh.cpp
	Chonps/src/Graphics/Mesh.h
	Chonps/src/Graphics/Model.cpp
	Chonps/src/Graphics/Model.h
	Chonps/src/Graphics/OrthographicCamera.cpp
	Chonps/src/Graphics/OrthographicCamera.h
	Chonps/src/Graphics/Pipeline.cpp
	Chonps/src/Graphics/Pipeline.h
	Chonps/src/Graphics/Renderer.cpp
	Chonps/src/Graphics/Renderer.h
	Chonps/src/Graphics/Renderer2D.cpp
	Chonps/src/Graphics/Renderer2D.h
	Chonps/src/Graphics/RendererAPI.cpp
	Chonps/src/Graphics/RendererAPI.h
	Chonps/src/Graphics/RendererBackends.cpp
	Chonps/src/Graphics/RendererBackends.h
	Chonps/src/Graphics/RendererCommand.cpp
	Chonps/src/Graphics/RendererCommand.h
	Chonps/src/Graphics/Shader.cpp
	Chonps/src/Graphics/Shader.h
	Chonps/src/Graphics/Texture.cpp
	Chonps/src/Graphics/Texture.h
	Chonps/src/Graphics/UniformBuffer.cpp
	Chonps/src/Graphics/UniformBuffer.h
	Chonps/src/Graphics/VertexArray.cpp
	Chonps/src/Graphics/VertexArray.h
	Chonps/src/Graphics/VertexBuffer.cpp
	Chonps/src/Graphics/VertexBuffer.h
)

set(MODEL_LOADER_SOURCES
	# OBJ
	Chonps/src/ModelLoaders/OBJ/OBJModelLoader.cpp
	Chonps/src/ModelLoaders/OBJ/OBJModelLoader.h
	# gltf
	Chonps/src/ModelLoaders/gltf/gltfModelLoader.cpp
	Chonps/src/ModelLoaders/gltf/gltfModelLoader.h
)

set(PLATFORM_SOURCES
	# OpenGL
	Chonps/src/Platform/OpenGL/OpenGLContext.cpp
	Chonps/src/Platform/OpenGL/OpenGLContext.h
	Chonps/src/Platform/OpenGL/OpenGLCubemap.cpp
	Chonps/src/Platform/OpenGL/OpenGLCubemap.h
	Chonps/src/Platform/OpenGL/OpenGLFrameBuffer.cpp
	Chonps/src/Platform/OpenGL/OpenGLFrameBuffer.h
	Chonps/src/Platform/OpenGL/OpenGLIndexBuffer.cpp
	Chonps/src/Platform/OpenGL/OpenGLIndexBuffer.h
	Chonps/src/Platform/OpenGL/OpenGLRendererAPI.cpp
	Chonps/src/Platform/OpenGL/OpenGLRendererAPI.h
	Chonps/src/Platform/OpenGL/OpenGLShader.cpp
	Chonps/src/Platform/OpenGL/OpenGLShader.h
	Chonps/src/Platform/OpenGL/OpenGLTexture.cpp
	Chonps/src/Platform/OpenGL/OpenGLTexture.h
	Chonps/src/Platform/OpenGL/OpenGLUniformBuffer.cpp
	Chonps/src/Platform/OpenGL/OpenGLUniformBuffer.h
	Chonps/src/Platform/OpenGL/OpenGLVertexArray.cpp
	Chonps/src/Platform/OpenGL/OpenGLVertexArray.h
	Chonps/src/Platform/OpenGL/OpenGLVertexBuffer.cpp
	Chonps/src/Platform/OpenGL/OpenGLVertexBuffer.h

	# Vulkan
	Chonps/src/Platform/Vulkan/VulkanCubemap.cpp
	Chonps/src/Platform/Vulkan/VulkanCubemap.h
	Chonps/src/Platform/Vulkan/VulkanFrameBuffer.cpp
	Chonps/src/Platform/Vulkan/VulkanFrameBuffer.h
	Chonps/src/Platform/Vulkan/VulkanIndexBuffer.cpp
	Chonps/src/Platform/Vulkan/VulkanIndexBuffer.h
	Chonps/src/Platform/Vulkan/VulkanPipeline.cpp
	Chonps/src/Platform/Vulkan/VulkanPipeline.h
	Chonps/src/Platform/Vulkan/VulkanRendererAPI.cpp
	Chonps/src/Platform/Vulkan/VulkanRendererAPI.h
	Chonps/src/Platform/Vulkan/VulkanRendererBackends.h
	Chonps/src/Platform/Vulkan/VulkanShader.cpp
	Chonps/src/Platform/Vulkan/VulkanShader.h
	Chonps/src/Platform/Vulkan/VulkanTexture.cpp
	Chonps/src/Platform/Vulkan/VulkanTexture.h
	Chonps/src/Platform/Vulkan/VulkanUniformBuffer.cpp
	Chonps/src/Platform/Vulkan/VulkanUniformBuffer.h
	Chonps/src/Platform/Vulkan/VulkanVertexArray.cpp
	Chonps/src/Platform/Vulkan/VulkanVertexArray.h
	Chonps/src/Platform/Vulkan/VulkanVertexBuffer.cpp
	Chonps/src/Platform/Vulkan/VulkanVertexBuffer.h

	# GLFW
	Chonps/src/Platform/WindowAPI/glfwAPI/glfwInput.cpp
	Chonps/src/Platform/WindowAPI/glfwAPI/glfwInput.h
	Chonps/src/Platform/WindowAPI/glfwAPI/glfwWindowAPI.h
	Chonps/src/Platform/WindowAPI/glfwAPI/glfwWindowAPI.cpp
)

set(LIB_SOURCES
	# stb_image
	Chonps/Dependencies/stb_image/stb_image.cpp
	Chonps/Dependencies/stb_image/stb_image.h

	# tinyxml2
	Chonps/Dependencies/tinyxml2/include/tinyxml2.cpp
	Chonps/Dependencies/tinyxml2/include/tinyxml2.h

	# Vma
	Chonps/Dependencies/vma/include/vk_mem_alloc.cpp
	Chonps/Dependencies/vma/include/vk_mem_alloc.h
)

set(SOURCES
	# Events
	Chonps/src/Core/Events/ApplicationEvents.h
	Chonps/src/Core/Events/Event.h
	Chonps/src/Core/Events/KeyEvents.h
	Chonps/src/Core/Events/MouseEvents.h
	Chonps/src/Core/Events/WindowEvents.h
		
	# ECS
	Chonps/src/Core/ECS/ComponentManager.h
	Chonps/src/Core/ECS/Components.h
	Chonps/src/Core/ECS/Entity.h
	Chonps/src/Core/ECS/Registry.h
	Chonps/src/Core/ECS/SystemManager.h

	# Imgui
	Chonps/src/Imgui/ImguiBuild.cpp
	Chonps/src/Imgui/ImguiEditor.h
	Chonps/src/Imgui/ImguiEditor.cpp
	Chonps/src/Imgui/ImguiWindow.h
	Chonps/src/Imgui/ImguiWindow.cpp
		
	# GUI
	Chonps/src/GUI/ChonpsGuiContext.cpp
	Chonps/src/GUI/ChonpsGuiContext.h
	Chonps/src/GUI/ChonpsGuiShaders.h
	Chonps/src/GUI/GuiWindow.cpp
	Chonps/src/GUI/GuiWindow.h

	# Core
	Chonps/src/Core/Application.cpp
	Chonps/src/Core/Application.h
	Chonps/src/Core/Config.h
	Chonps/src/Core/Core.h
	Chonps/src/Core/DataTypes.h
	Chonps/src/Core/File.cpp
	Chonps/src/Core/File.h
	Chonps/src/Core/Input.cpp
	Chonps/src/Core/Input.h
	Chonps/src/Core/KeyCodes.h
	Chonps/src/Core/Layer.h
	Chonps/src/Core/LayerStack.h
	Chonps/src/Core/LayerStack.cpp
	Chonps/src/Core/Log.cpp
	Chonps/src/Core/Log.h
	Chonps/src/Core/Math.cpp
	Chonps/src/Core/Math.h
	Chonps/src/Core/MouseButtonCodes.h
	Chonps/src/Core/QueueChain.h
	Chonps/src/Core/Timer.h
	Chonps/src/Core/Window.cpp
	Chonps/src/Core/Window.h

	Chonps/src/cepch.cpp
	Chonps/src/cepch.h
	Chonps/src/Chonps.h

	${LIB_SOURCES}
	${PLATFORM_SOURCES}
	${GRAPHICS_SOURCES}
	${MODEL_LOADER_SOURCES}
)

add_library(Chonps STATIC ${SOURCES})

target_include_directories(Chonps 
	PUBLIC
		${PROJECT_SOURCE_DIR}/Chonps/src
		${PROJECT_SOURCE_DIR}/Chonps/Platform
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/spdlog/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/GLFW/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/glad/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/imgui/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/glm/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/stb_image
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/json/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/vulkan/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/vma/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/freetype/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/msdfgen/include
)


# Build definitions
target_compile_definitions(Chonps PRIVATE
	$<$<CONFIG:Debug>:CHONPS_DEBUG>
	$<$<CONFIG:Release>:CHONPS_RELEASE>
)

# PCH
target_precompile_headers(Chonps PRIVATE Chonps/src/cepch.h)

# Find VulkanSDK
set(MIN_VULKAN_VERSION 1.3)
find_package(Vulkan ${MIN_VULKAN_VERSION} REQUIRED COMPONENTS shaderc_combined)

if(Vulkan_FOUND)
    message ("Vulkan found")
else()
    message (FATAL_ERROR "Cannot find Vulkan")
endif()

if(Vulkan_shaderc_combined_FOUND)
    message ("shaderc_combined library found")
else()
    message (FATAL_ERROR "Cannot find shaderc_combined library")
endif()

set(VULKAN_LIB_DIR
	Vulkan::Vulkan
	Vulkan::shaderc_combined
)

target_link_libraries(Chonps 
	PRIVATE 
		freetype
		glfw
		glad
		msdfgen
		${VULKAN_LIB_DIR}
	PUBLIC
		Imgui
)

# ChonpsEditor
set(SOURCES
	ChonpsEditor/src/Main.cpp
	ChonpsEditor/src/ChonpsEditor.cpp
	ChonpsEditor/src/ChonpsEditor.h

	# Panels 
	ChonpsEditor/src/panels/Properties.cpp
	ChonpsEditor/src/panels/Properties.h
	ChonpsEditor/src/panels/Scene.cpp
	ChonpsEditor/src/panels/Scene.h
	ChonpsEditor/src/panels/SceneHierarchy.cpp
	ChonpsEditor/src/panels/SceneHierarchy.h
)

add_executable(ChonpsEditor ${SOURCES})

target_include_directories(ChonpsEditor 
		PUBLIC
			${PROJECT_SOURCE_DIR}/Chonps/Dependencies/spdlog/include
			${PROJECT_SOURCE_DIR}/Chonps/Dependencies/imgui/include
			${PROJECT_SOURCE_DIR}/Chonps/Dependencies/glm/include
			${PROJECT_SOURCE_DIR}/Chonps/src
			${PROJECT_SOURCE_DIR}/ChonpsEditor/src
)

target_link_libraries(ChonpsEditor
	PRIVATE
		Chonps
	PUBLIC
		Imgui
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ChonpsEditor)	
endif()