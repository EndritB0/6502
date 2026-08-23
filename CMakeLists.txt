cmake_minimum_required(VERSION 3.25)
project(6502 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "Available build types" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(ProjectWarnings INTERFACE)

if(MSVC)
    add_compile_options(/MP)
    target_compile_options(ProjectWarnings INTERFACE /W4 /WX /permissive- /Zc:__cplusplus)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(ProjectWarnings INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

include(projects.cmake)
