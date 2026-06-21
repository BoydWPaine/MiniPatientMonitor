cmake_minimum_required(VERSION 3.20)

project(MiniPatientMonitor VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MPM_BUILD_DEVICE "Build device TCP server executable" ON)
option(MPM_BUILD_HOST "Build host TCP client executable" ON)

set(MPM_TCP_PORT 5000 CACHE STRING "TCP port for device server / host client")
set(MPM_TCP_HOST "127.0.0.1" CACHE STRING "TCP bind/connect address")

add_subdirectory(common)

if(MPM_BUILD_DEVICE)
    add_subdirectory(device)
endif()

if(MPM_BUILD_HOST)
    add_subdirectory(host)
endif()