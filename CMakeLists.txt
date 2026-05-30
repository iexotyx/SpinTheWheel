cmake_minimum_required(VERSION 3.20)
project(WheelApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics Window System Main)

add_executable(WheelApp WIN32
    src/main.cpp  
    
    resources.rc   
    
    "src/Wheel.h" "src/EditorState.h" "src/util/ColourUtils.h"  "src/util/Button.h"     "src/util/Scrollbar.h" "src/Input.h"    "src/Renderer.h" "src/Shortcuts.h" "src/AppStates.h")

target_link_libraries(WheelApp PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Main
)