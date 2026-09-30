cmake_minimum_required(VERSION 3.10)
project(Cod4x_DiscordMessageLogger VERSION 2.0.0)

# Normal 64-bit compilation
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# DPP module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find DPP
set(DPP_ROOT_DIR "/usr/local")
find_package(DPP REQUIRED)

# Find pthread
find_package(Threads REQUIRED)

# Build executable FIRST
add_executable(${PROJECT_NAME}
    src/Server.cpp
    src/DiscordBot.cpp
    src/Queue.cpp
    src/Signal.cpp
    src/Thread.cpp
    src/JSON.cpp
)

# Generate version header
configure_file(version.h.in version.h)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes"
    ${DPP_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${DPP_LIBRARIES}
    Threads::Threads
    cjson
)
