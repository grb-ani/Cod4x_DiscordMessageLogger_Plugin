cmake_minimum_required(VERSION 3.10)
project(Cod4x_DiscordMessageLogger VERSION 2.0.0)

# Force 32-bit build for CODX plugin
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -Wall -O1 -fvisibility=hidden -mtune=core2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -Wall -O1 -fvisibility=hidden -mtune=core2")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32 -g -shared")

# 32-bit pthread paths
set(PTHREAD_INCLUDE_DIR "/usr/include/i386-linux-gnu")
set(PTHREAD_LIBRARY "/lib/i386-linux-gnu/libpthread.so.0")
include_directories(${PTHREAD_INCLUDE_DIR})
set(PTHREAD_LIBRARIES ${PTHREAD_LIBRARY})

# Generate version header
configure_file(version.h.in version.h)

# Include plugin-specific directories
include_directories(libs includes)

# Build plugin shared library
add_library(${PROJECT_NAME} SHARED
    src/Plugin.cpp
    src/Queue.cpp
)

# Include generated version.h
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

# Link 32-bit pthread
target_link_libraries(${PROJECT_NAME} PRIVATE ${PTHREAD_LIBRARIES})
