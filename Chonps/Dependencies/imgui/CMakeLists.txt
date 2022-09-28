set(IMGUI_SOURCES
    include/imgui/imconfig.h
    include/imgui/imgui.h
    include/imgui/imgui.cpp
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_impl_glfw.cpp
    include/imgui/imgui_impl_glfw.h
    include/imgui/imgui_impl_opengl3.h
    include/imgui/imgui_impl_opengl3.cpp
    include/imgui/imgui_impl_opengl3_loader.h
    include/imgui/imgui_internal.h
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imstb_rectpack.h
    include/imgui/imstb_textedit.h
    include/imgui/imstb_truetype.h
)

add_library(Imgui STATIC ${IMGUI_SOURCES})

target_include_directories(Imgui 
    PRIVATE ${CMAKE_SOURCE_DIR}/Chonps/Dependencies/GLFW/include
)