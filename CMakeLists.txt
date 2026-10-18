cmake_minimum_required(VERSION 3.10)

project(OpenCVApp)

set(OpenCV_DIR "C:/OpenCV_Source/opencv/build")
set(SFML_DIR "C:/SMFL_LIBRARY/SFML-3.0.2/lib/cmake/SFML")
set(IMGUI_DIR "C:/Users/santi/Documents/imgui-master")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui-SFML.cpp
    ## Lineas a comentar para que funcione en VSCODE
    ##${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ##${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

find_package(OpenCV REQUIRED)
find_package(SFML 3.0.2 COMPONENTS Graphics Window System REQUIRED)


add_executable(app display.cpp ${IMGUI_SOURCES})

target_include_directories(app PRIVATE 
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
)
## Lineas a comentar para que funcione en VSCODE
#set_target_properties(app PROPERTIES LINK_FLAGS "-s USE_GLFW=3 -s USE_WEBGL2=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1")
#set_target_properties(app PROPERTIES SUFFIX ".html")

target_include_directories(app PRIVATE "C:/SMFL_LIBRARY/SFML-3.0.2/include")
target_link_libraries(app PRIVATE ${OpenCV_LIBS} SFML::Graphics )
target_link_libraries(app PRIVATE 
    ${SFML_LIBRARIES} 
    opengl32
)