# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.20)

# The toolchain file setup should ideally be in a CMakePresets.json file,
# but we'll keep it here for simplicity as shown in the video.
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

# Define the project name, version, and language
project(MyRaylibProject VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # Turn off C++20 modules

# Add the executable target
add_executable(MyRaylibApp)

# Find all source and header files recursively from the project root.
# This will find src/main.cpp, fetcher/fetcher.h, fetcher/fetcher.cpp, and ui.h
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Add the collected source files to the executable target
target_sources(MyRaylibApp PRIVATE ${PROJECT_SOURCES})

# Add include directories. Adding the project root allows you to use
# #include "fetcher/fetcher.h" and #include "ui.h" directly.
target_include_directories(MyRaylibApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Add Vcpkg Dependencies ---
# Find the packages declared in vcpkg.json
find_package(raylib CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Link the libraries to your executable
target_link_libraries(MyRaylibApp PRIVATE
    raylib
    cpr::cpr
    nlohmann_json::nlohmann_json
)

# --- Compiler-Specific Options ---
# Set configuration properties for MSVC (Visual Studio)
if(MSVC)
    target_compile_options(MyRaylibApp PRIVATE
        /W4 # Set warning level to 4
        /WX # Treat warnings as errors
        /MP # Enable multi-threaded build
        /Zi # Enable program database for edit and continue
    )
endif()


# --- Visual Studio Helper Settings ---
# Set the default startup project in the Visual Studio solution
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyRaylibApp)

# This command organizes the files in the Solution Explorer to match the folder structure on disk.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})
