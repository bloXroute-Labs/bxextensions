cmake_minimum_required(VERSION 3.0)
project(task_pool_executor)
add_subdirectory(lib)
set(SRC_DIR interface/bxextensions)
add_library(
    ${PROJECT_NAME} 
    SHARED 
    ${SRC_DIR}/${PROJECT_NAME}.cpp
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


add_library(
    test_module
    SHARED 
    ${SRC_DIR}/test_module.cpp
)
target_link_libraries(
    test_module
    PRIVATE pybind11::module 
)

set_target_properties(
    test_module PROPERTIES 
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)
