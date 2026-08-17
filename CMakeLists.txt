cmake_minimum_required(VERSION 3.24)

project(pwnit
    VERSION 1.0.0
    DESCRIPTION "A C++ binary exploitation toolkit"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

include(FetchContent)

set(LIEF_DOC OFF CACHE BOOL "" FORCE)
set(LIEF_PYTHON_API OFF CACHE BOOL "" FORCE)
set(LIEF_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIEF_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    lief
    URL https://github.com/lief-project/LIEF/archive/refs/tags/1.0.0.tar.gz
)

FetchContent_MakeAvailable(lief)

FetchContent_Declare(
    capstone
    GIT_REPOSITORY https://github.com/capstone-engine/capstone.git
    GIT_TAG 5.0.6
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(capstone)

set(BUILD_TESTING OFF CACHE BOOL "Disable tests" FORCE)
set(BUILD_BENCHMARK OFF CACHE BOOL "Disable benchmarks" FORCE)

FetchContent_Declare(
    inja
    GIT_REPOSITORY https://github.com/pantor/inja.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(inja)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.6.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(cli11)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spdlog)

find_package(CURL REQUIRED)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(pwnit ${SOURCES})

target_include_directories(pwnit
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(pwnit
    PRIVATE
        LIEF::LIEF
        capstone
        CLI11::CLI11
        pantor::inja
        tomlplusplus::tomlplusplus
        nlohmann_json::nlohmann_json
        CURL::libcurl
        spdlog::spdlog
)

target_compile_features(pwnit
    PRIVATE
        cxx_std_23
)

if(MSVC)

    target_compile_options(pwnit PRIVATE
        /W4
        /permissive-
    )

else()

    target_compile_options(pwnit PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
    )

endif()
