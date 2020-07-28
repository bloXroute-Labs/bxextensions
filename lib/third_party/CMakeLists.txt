cmake_minimum_required(VERSION 3.0)
project(third_party)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
add_subdirectory(googletest)
add_subdirectory(libsodium-stable)
add_subdirectory(jsoncons)
include(secp256k1/ProjectSecp256k1.cmake)


