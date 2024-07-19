cmake_minimum_required(VERSION 3.10)
project(FunctionParser)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

# 寻找 LLVM 和 Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fexceptions")

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(FunctionParser main.cpp)
# 链接 Clang 和 LLVM 库
target_link_libraries(FunctionParser
        clangTooling
        clangBasic
        clangAST
        clangFrontend
        clangRewrite
        clangSerialization
        clangDriver
        clangParse
        clangSema
        clangEdit
        clangLex
        clangAnalysis
        clangASTMatchers
        clangStaticAnalyzerFrontend
        clangStaticAnalyzerCheckers
        clangStaticAnalyzerCore
        LLVMCore
        LLVMSupport
        LLVMBitReader
        LLVMBitWriter
        LLVMOption
        LLVMMC
        LLVMMCParser
)

if(LLVM_ENABLE_RTTI)
    message(STATUS "LLVM was built with RTTI")
else()
    message(STATUS "LLVM was not built with RTTI")
endif()

if(LLVM_ENABLE_EH)
    message(STATUS "LLVM was built with exceptions")
else()
    message(STATUS "LLVM was not built with exceptions")
endif()

target_compile_features(FunctionParser PRIVATE cxx_std_17)