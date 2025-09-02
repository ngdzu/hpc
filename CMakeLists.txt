cmake_minimum_required(VERSION 3.10)

# Read project name from config file
file(READ "${CMAKE_SOURCE_DIR}/project_config.txt" CONFIG_CONTENT)
string(REGEX MATCH "PROJECT_NAME=([A-Za-z0-9_\-]+)" _ ${CONFIG_CONTENT})
string(REGEX REPLACE "PROJECT_NAME=" "" PROJECT_NAME "${_}")

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)

# Link spdlog from Conan
find_package(spdlog REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json)

# Debug: Print spdlog properties
# get_target_property(SPDLOG_INCLUDE_DIRS spdlog::spdlog INTERFACE_INCLUDE_DIRECTORIES)
# message(STATUS "spdlog include dirs: ${SPDLOG_INCLUDE_DIRS}")

target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}")


if(APPLE)

    # Apply OpenMP to simd_demo specifically
    target_link_libraries(${PROJECT_NAME} PRIVATE "-L/opt/homebrew/opt/libomp/lib" "-lomp")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Xpreprocessor" "-fopenmp" "-O3" "-march=native")
    target_include_directories(${PROJECT_NAME} PRIVATE "/opt/homebrew/opt/libomp/include")
endif()

