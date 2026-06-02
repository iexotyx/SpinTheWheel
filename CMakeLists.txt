cmake_minimum_required(VERSION 3.20)
project(WheelApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics Window System Main)

add_executable(WheelApp WIN32
    src/main.cpp  
    
    resources.rc   
    
    "src/elements/Wheel.h" "src/EditorState.h" "src/util/ColourUtils.h"  "src/util/Button.h"     "src/util/Scrollbar.h" "src/inputs/EventRouter.h"    "src/Layouts.h" "src/Shortcuts.h" "src/AppStates.h" "src/Persistence/FileBrowser.h" "src/Persistence/WheelFile.h" "src/Persistence/TreeFile.h" "src/elements/DecisionTree.h" "src/persistence/AppSettings.h" "src/rendering/FileBrowserRenderer.h" "src/rendering/CreateWheelRenderer.h" "src/rendering/WheelRenderer.h" "src/rendering/MenuRenderer.h" "src/TextEditor.h" "src/AppContext.h" "src/inputs/EditorInput.h" "src/inputs/FileBrowserInput.h" "src/inputs/GenericInput.h" "src/logic/EditorUpdate.h" "src/logic/Spin.h" "src/util/Table.h")

target_link_libraries(WheelApp PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Main
)