cmake_minimum_required(VERSION 3.20)

project(Flow_Field)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_SOURCE_DIR}/external/SDL3"
    "${CMAKE_SOURCE_DIR}/external/SDL3_image"
)


find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
add_executable(Flow_Field src/flow_field.cc src/main.cc src/arrow.cc)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
target_include_directories(Flow_Field PRIVATE include)
target_link_libraries(Flow_Field PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
