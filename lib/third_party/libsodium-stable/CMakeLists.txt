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
   # execute_process() does not fail the CMake configure step on its own --
   # without checking RESULT_VARIABLE, a missing/failing `autoreconf` (e.g.
   # autoconf/automake/libtool not installed, which is the case on a fresh
   # macOS/Homebrew setup that doesn't ship them by default) was silently
   # swallowed here, leaving `configure` never actually created. The failure
   # then only surfaced much later, confusingly, as a plain
   # "/bin/sh: .../configure: No such file or directory" from `make`. Fail
   # loudly here instead, with a message that says what's actually missing.
   execute_process(
   		COMMAND autoreconf -i
   		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   		RESULT_VARIABLE SODIUM_AUTORECONF_RESULT
   )
   if(NOT SODIUM_AUTORECONF_RESULT EQUAL 0)
       message(FATAL_ERROR
           "autoreconf -i failed (exit code ${SODIUM_AUTORECONF_RESULT}) while "
           "generating libsodium's configure script. This usually means "
           "autoconf/automake/libtool aren't installed. On macOS: "
           "`brew install autoconf automake libtool`. On Debian/Ubuntu: "
           "`apt-get install -y automake autoconf libtool libtool-bin`."
       )
   endif()
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
