cmake_minimum_required(VERSION 3.30.5)
project(tsanalyze)

set(CMAKE_CXX_STANDARD 20)

# Include nlohmann JSON using FetchContent
include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3  # Use the latest version or a specific version
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG 20250127.1
)
FetchContent_MakeAvailable(absl)

# LLVM_ROOT = /opt/homebrew/opt/llvm
# Find LLVM
find_package(LLVM REQUIRED CONFIG PATHS "${LLVM_ROOT}/lib/cmake/llvm" NO_DEFAULT_PATH)
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Clang
find_package(Clang REQUIRED CONFIG PATHS "${LLVM_ROOT}/lib/cmake/clang" NO_DEFAULT_PATH)
#find_library(LIBCLANG_PATH clang PATHS "${LLVM_ROOT}/lib" NO_DEFAULT_PATH REQUIRED)


# JDK_DIR = /Library/Java/JavaVirtualMachines/jdk-22.jdk/Contents/Home
include_directories(${JDK_DIR}/include)
if (APPLE)
    include_directories(${JDK_DIR}/include/darwin)
endif ()

file(GLOB_RECURSE TSANALYZE_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/tsanalyze/*.h
        ${CMAKE_CURRENT_LIST_DIR}/src/tsanalyze/*.cc
)

file(GLOB_RECURSE TSAGHIDRA_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/ghidralib/*.h
        ${CMAKE_CURRENT_LIST_DIR}/src/ghidralib/*.cc
)

file(GLOB_RECURSE EXEC_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/exec/*.h
        ${CMAKE_CURRENT_LIST_DIR}/src/exec/*.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/exec/*.cpp
)

#set(LLVM_COMPONENTS
#        core
#        support
#        irreader
#)

add_library(tsanalyze ${TSANALYZE_SOURCES})
target_link_libraries(tsanalyze PRIVATE
        clangAST
        clangBasic
        clangFrontend
        clangSerialization
        clangTooling
        clangDriver
        clangParse
        clangSema
        clangAnalysis
        clangEdit
        clangLex
        LLVM
        absl::status
)

add_library(tsaghidra SHARED ${TSAGHIDRA_SOURCES})
target_link_libraries(
        tsaghidra
        PRIVATE
        nlohmann_json::nlohmann_json
        absl::status
        tsanalyze
)


add_executable(exec ${EXEC_SOURCES})
target_link_libraries(exec PRIVATE
        tsanalyze
)


