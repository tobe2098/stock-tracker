cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(StockTracker VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOUIC ON) # Automatically process .ui files (for Qt Designer forms)
set(CMAKE_AUTOMOC ON) # Automatically process MOC (Meta-Object Compiler) files
set(CMAKE_AUTORCC ON) # Automatically process .qrc files (Qt Resource files)

find_package(Qt6 COMPONENTS Widgets Charts Network REQUIRED)
# find_package(Qt6 COMPONENTS Charts REQUIRED)
# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Set source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/stock.cpp
    src/stockdatafetcher.cpp
    )
# Set heaeder files
set(HEADERS
    src/mainwindow.hpp
    src/stock.hpp
    src/stockdatafetcher.hpp
    )

# Add executable
add_executable(stock-tracker ${SOURCES} ${HEADERS})

# Include directory
target_include_directories(stock-tracker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


# Link libraries
target_link_libraries(stock-tracker PRIVATE 
    Qt6::Widgets
    Qt6::Charts
    Qt6::Network
)
# Add this line
add_compile_definitions(_UCRT)
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
