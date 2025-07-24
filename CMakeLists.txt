cmake_minimum_required(VERSION 3.15)
project(RaceCar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(RaceCar src/main.cpp)

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