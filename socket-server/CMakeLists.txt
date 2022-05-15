cmake_minimum_required(VERSION 3.5)
project(socketServerLib VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS On)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32)
# Details auf https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-160
# GS       - Checks buffer security
# Gs       - Controls stack probes
# GT       - Supports fiber safety for data allocated by using static thread-local storage.
# guard:cf - Adds control flow guard security checks.
# RTC1     - Equivalent to /RTCsu, /RTCs - Enables stack frame run-time error checking
#                                  /RTCu - Reports when a variable is used without having been initialized
# Wall		- Enable all warnings
# W3,W4		- Enable Warning Level 3 or Warning Level 4
# WX		- Treat all Warnings as errors
# $<$<CONFIG:Debug>:/fsanitize=address>)
add_compile_options(/GS /Gs /GT /guard:cf /W3 $<$<CONFIG:Debug>:/RTC1>)
    
add_definitions(-D_WINSOCKAPI_=)
add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DWIN32_LEAN_AND_MEAN)
add_definitions(-DNOMINMAX)

include(InstallRequiredSystemLibraries)
elseif(UNIX)
# Compile options https://gcc.gnu.org/onlinedocs/gcc-9.4.0/gcc/C_002b_002b-Dialect-Options.html
add_compile_options(-Werror -Wall -Wextra -fstack-protector-all -Wno-unused-variables -Wformat-security -Wformat $<$<CONFIG:Debug>:-ggdb> $<$<CONFIG:Release>:-D_FORTIFY_SOURCES=2>)
endif()

find_package(Threads)

set(SERVER_RESPONSE_BUFFER_SIZE 2048)

message(STATUS "Server message buffer size: ${SERVER_RESPONSE_BUFFER_SIZE} B")

configure_file(${CMAKE_CURRENT_LIST_DIR}/include/server_constants.h.in ${CMAKE_CURRENT_LIST_DIR}/include/server_constants.h @ONLY)

set(PROJECT_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

enable_testing()
add_subdirectory(tests)

option(build_examples "" ON)

if(build_examples)

if(UNIX)
add_executable(socket_client_example 
examples/socket_client_example.cpp
src/socket/tcp_socket_linux.cpp)

add_executable(socket_server_example 
examples/socket_server_example.cpp
src/socket/tcp_socket_linux.cpp)
endif()

target_include_directories(socket_client_example PRIVATE include)
target_include_directories(socket_server_example PRIVATE include)

endif()