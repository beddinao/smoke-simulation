cmake_minimum_required(VERSION 3.20)
project(smoke C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

add_subdirectory(assets/SDL3)
target_link_libraries(${PROJECT_NAME} SDL3::SDL3)

add_subdirectory(assets/SDL3_image)
target_link_libraries(${PROJECT_NAME} SDL3_image::SDL3_image)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)
