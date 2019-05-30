cmake_minimum_required(VERSION 3.0)
project(task_pool_executor)

set(${PROJECT_NAME}_SRC_DIR  "src")
file(GLOB_RECURSE ${PROJECT_NAME}_SRC_FILES "${${PROJECT_NAME}_SRC_DIR}/*.cpp")
file(GLOB_RECURSE ${PROJECT_NAME}_HDR_FILES "${${PROJECT_NAME}_SRC_DIR}/*.h")

include_directories(${PROJECT_SOURCE_DIR})
add_library(
    ${PROJECT_NAME} 
    SHARED 
    ${${PROJECT_NAME}_SRC_FILES}
    ${${PROJECT_NAME}_HDR_FILES}
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE pybind11::module 
    PUBLIC tpe 
    PUBLIC utils
)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES 
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

install(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
)
