cmake_minimum_required(VERSION 3.0)
project(sodium)
get_filename_component(CONFIGURE configure 
                       ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SODIUM_IMPORT_PATH ${CMAKE_BINARY_DIR})

add_custom_target(
    ${PROJECT_NAME}_ct
    ALL
    COMMAND autoreconf -i
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CONFIGURE}
            --disable-shared 
            --enable-static 
            --disable-debug
            --disable-dependency-tracking
            --with-pic
            --prefix ${SODIUM_IMPORT_PATH}
    COMMAND make
    COMMAND make check
    COMMAND make install
)

set(LIB_SODIUM ${SODIUM_IMPORT_PATH}/lib/libsodium.a)
message(${LIB_SODIUM})

add_library(${PROJECT_NAME} STATIC IMPORTED GLOBAL)

set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION ${LIB_SODIUM})

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_ct)
