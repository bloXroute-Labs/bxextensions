cmake_minimum_required(VERSION 3.0)
project(tpe)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
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

## Tests

find_file(
          ${PROJECT_NAME}_TEST_MAIN main.cpp
          PATHS test
)
if(NOT ${${PROJECT_NAME}_TEST_MAIN} STREQUAL ${PROJECT_NAME}_TEST_MAIN-NOTFOUND)

    file(GLOB_RECURSE ${PROJECT_NAME}_TEST_FILES "test/*.cpp")
    add_executable(check_${PROJECT_NAME} ${${PROJECT_NAME}_TEST_FILES})
    target_link_libraries(check_${PROJECT_NAME} gtest_main ${PROJECT_NAME})
    if(NOT "${RUN_TESTS}" STREQUAL "")
    	string(TOLOWER ${RUN_TESTS} RUN_TESTS)
    else()
       set(RUN_TESTS "true")
    endif()
    if(${RUN_TESTS} STREQUAL "true")
        add_custom_command(
            TARGET check_${PROJECT_NAME}
            POST_BUILD
            COMMAND check_${PROJECT_NAME}
        )
    endif()

    if("${INSTALL_TESTS}" STREQUAL "TRUE")
        install(
                TARGETS check_${PROJECT_NAME}
                RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/tests
        )
    endif()


endif()


install(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
)
