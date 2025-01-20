cmake_minimum_required(VERSION 3.20)
project(FreesoundDownloader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for dependency management
include(FetchContent)

# Fetch nlohmann/json for JSON parsing
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Fetch cpr for HTTP requests
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
# Configure CPR to use built-in libcurl
set(CPR_USE_SYSTEM_CURL OFF)
set(CPR_BUILD_TESTS OFF)
set(CPR_BUILD_TESTS_SSL OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_MakeAvailable(cpr)

# Fetch libcurl if not already available
if(NOT TARGET CURL::libcurl)
    include(FetchContent)
    FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-7_86_0
    )
    set(BUILD_CURL_EXE OFF CACHE BOOL "Build curl executable")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
    set(CURL_STATICLIB ON CACHE BOOL "Build static library")
    FetchContent_MakeAvailable(curl)
endif()

# Fetch doctest for unit testing
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

# Add library
add_library(FreesoundDownloader STATIC 
    src/freesound_downloader.cpp
    include/freesound_downloader.h
)

# Include directories for the library
target_include_directories(FreesoundDownloader PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies to the library
target_link_libraries(FreesoundDownloader 
    PUBLIC 
    nlohmann_json::nlohmann_json
    cpr::cpr
)

# Enable testing
enable_testing()

# Add test executable
add_executable(test_downloader 
    tests/test_downloader.cpp
)

# Include directories for the test executable
target_include_directories(test_downloader 
    PRIVATE 
    ${doctest_SOURCE_DIR} 
    ${CMAKE_SOURCE_DIR}/include
)

# Link dependencies to the test executable
target_link_libraries(test_downloader 
    PRIVATE 
    doctest::doctest
    FreesoundDownloader
)

# Add the test
add_test(
    NAME test_downloader
    COMMAND test_downloader
)
