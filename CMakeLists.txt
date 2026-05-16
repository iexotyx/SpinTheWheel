cmake_minimum_required(VERSION 3.20)
project(WheelApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics Window System)

add_executable(WheelApp src/main.cpp "src/Segment.h" "src/CSVHandler.cpp" "src/CSVHandler.h")

target_link_libraries(WheelApp PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)