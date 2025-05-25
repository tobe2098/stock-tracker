cmake_minimum_required(VERSION 3.14)
project(StockTracker VERSION 0.0.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch the json dependency
include(FetchContent)
FetchContent_Declare(json DOWNLOAD_EXTRACT_TIMESTAMP true URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)
FetchContent_Declare(rmutexpp GIT_REPOSITORY https://github.com/tobe2098/rmutexpp.git GIT_TAG        main)
FetchContent_MakeAvailable(rmutexpp)
# If the project was installed more seriously, and it was inside the PATH variable, we could fetch libcurl this way, but if not, the .dll linking at runtime requires adjustments.
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
# set(CURL_STATICLIB ON CACHE BOOL "Build libcurl as a static library" FORCE)
# Disable any features you don't need to reduce dependencies
# set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
# FetchContent_Declare(CURL
# 	DOWNLOAD_EXTRACT_TIMESTAMP true
#     URL https://curl.se/download/curl-8.5.0.tar.gz
# )
# FetchContent_MakeAvailable(CURL)

# We require the curl package
find_package(CURL REQUIRED)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Set source files
set(SOURCES
    src/main.cpp
)
# Set heaeder files
set(HEADERS
)

# Add executable
add_executable(stock-tracker ${SOURCES} ${HEADERS})

# Include directory
target_include_directories(stock-tracker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


# Link libraries
target_link_libraries(stock-tracker PRIVATE 
    CURL::libcurl
    nlohmann_json::nlohmann_json
    rmutexpp::rmutexpp_core
)

# Compile options
target_compile_options(stock-tracker PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Install executable
install(TARGETS stock-tracker
    RUNTIME DESTINATION bin
    # LIBRARY DESTINATION lib
    # ARCHIVE DESTINATION lib
)

# Generate compile_commands.json for tools like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
