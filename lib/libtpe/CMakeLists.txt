cmake_minimum_required(VERSION 3.0)
project(tpe)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -export-dynamic")
file(GLOB_RECURSE ${PROJECT_NAME}_SRC_FILES "tpe/*.cpp")
file(GLOB_RECURSE ${PROJECT_NAME}_HDR_FILES "tpe/*.h")
add_library(
    ${PROJECT_NAME} 
    SHARED
    ${${PROJECT_NAME}_HDR_FILES} 
    ${${PROJECT_NAME}_SRC_FILES} 
)

target_link_libraries(${PROJECT_NAME} PUBLIC utils)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

