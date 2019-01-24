cmake_minimum_required(VERSION 3.0)
project(utils)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
add_definitions(--std=c++11)
file(GLOB_RECURSE ${PROJECT_NAME}_SRC_FILES "utils/*.cpp")
file(GLOB_RECURSE ${PROJECT_NAME}_HDR_FILES "utils/*.h")

add_library(
    ${PROJECT_NAME} 
    SHARED 
    ${${PROJECT_NAME}_HDR_FILES} 
    ${${PROJECT_NAME}_SRC_FILES}
)

set_target_properties(
    ${PROJECT_NAME}  
    PROPERTIES 
    LINKER_LANGUAGE CXX
)


include_directories(${CMAKE_BINARY_DIR}/include)

target_link_libraries(${PROJECT_NAME} sodium)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
