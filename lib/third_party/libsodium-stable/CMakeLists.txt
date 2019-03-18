cmake_minimum_required(VERSION 3.0)
project(sodium)
find_file(
          SODIUM_CONFIGURE_FILE configure
          ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
if(
	${SODIUM_CONFIGURE_FILE} STREQUAL
	SODIUM_CONFIGURE_FILE-NOTFOUND   
)
   message("configure file is missing in dir ${CMAKE_CURRENT_SOURCE_DIR}, running autoreconf")
   execute_process(
   		COMMAND autoreconf -i
   		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   )
else()
	message("configure file found in ${SODIUM_CONFIGURE_FILE}")
endif()

get_filename_component(CONFIGURE configure 
                       ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SODIUM_IMPORT_PATH ${CMAKE_BINARY_DIR})

add_custom_target(
    ${PROJECT_NAME}_ct
    ALL
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
