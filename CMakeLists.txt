cmake_minimum_required(VERSION 3.15)

# Set vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(RaceCar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(RaceCar src/main.cpp src/Background/Background.cpp src/Track/track.cpp src/BezierCurve/BezierCurve.cpp src/BezierShape/BezierShape.cpp src/Car/Car.cpp)

# Find and link SFML
find_package(SFML COMPONENTS Graphics Window System REQUIRED CONFIG)
target_link_libraries(RaceCar PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System
)

# Include directories
target_include_directories(RaceCar PRIVATE include)

# Copy SFML DLLs to output directory (for Windows)
if(WIN32)
    add_custom_command(TARGET RaceCar POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::Graphics>
        $<TARGET_FILE:SFML::Window>
        $<TARGET_FILE:SFML::System>
        $<TARGET_FILE_DIR:RaceCar>
    )
endif() 