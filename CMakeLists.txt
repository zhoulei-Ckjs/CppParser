cmake_minimum_required(VERSION 3.10)
project(FunctionParser)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(third_party/json)


IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fexceptions")
    # 参数解析
    # -frtti 启动 RTTI
    # -fexceptions 启动 exception

    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR /EHsc /bigobj")
    # 参数解析
    # /GR 启动 RTTI
    # /EHsc 启动 exception
    # /bigobj 大对象

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
ELSE ()
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF ()


# 寻找 LLVM 和 Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(
        ${LLVM_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}/include
)
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src SRC)
add_executable(${PROJECT_NAME} main.cpp ${SRC})


IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MSVC_RUNTIME_LIBRARY MultiThreaded
    )
ENDIF ()

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
        nlohmann_json::nlohmann_json
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