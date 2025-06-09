cmake_minimum_required(VERSION 4.0)
project(EasyASM)

set(PROJECT_NAME EasyASM)

set(CMAKE_CXX_STANDARD 23)

if (NOT MSVC)
    error("This project requires MSVC compiler.")
else ()
# use multi-threaded static runtime, this could be necessary for some libraries
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

file(GLOB_RECURSE MODULE_FILES "src/*.ixx" "src/*.cppm")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.cxx")
file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.hpp")

add_executable(${PROJECT_NAME} ${MODULE_FILES} ${SOURCE_FILES} ${HEADER_FILES})

target_sources(
        ${PROJECT_NAME}
        PRIVATE
        FILE_SET cxx_modules
        TYPE CXX_MODULES
        FILES
        ${MODULE_FILES}
        ${SOURCE_FILES})

add_subdirectory(vendor/LuaJIT)

SET(SOL2_LUA_VERSION "luajit-2.1.0")
add_subdirectory(vendor/sol2)

add_subdirectory(vendor/yaml-cpp)

target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
        SOL_EXCEPTIONS_SAFE_PROPAGATION
)

add_subdirectory(vendor/args)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        libluajit
        sol2::sol2
        yaml-cpp
        taywee::args
)