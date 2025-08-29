
cmake_minimum_required(VERSION 3.10)

# Read project name from config file
file(READ "${CMAKE_SOURCE_DIR}/project_config.txt" CONFIG_CONTENT)
string(REGEX MATCH "PROJECT_NAME=([A-Za-z0-9_\-]+)" _ ${CONFIG_CONTENT})
string(REGEX REPLACE "PROJECT_NAME=" "" PROJECT_NAME "${_}")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)


if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/libomp/lib -lomp")
endif()

# Find and link OpenMP
# find_package(OpenMP REQUIRED)
# if(OpenMP_CXX_FOUND)
# 	target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
# endif()
