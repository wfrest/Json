cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")

project(
  Json
  VERSION 0.1.0
  LANGUAGES C CXX
)

option(JSON_OPT_BUILD_TESTS "Build all tests" ON)
option(JSON_OPT_BUILD_EXAMPLES "Build all examples" ON)

message(STATUS "JSON_OPT_BUILD_TESTS is ${JSON_OPT_BUILD_TESTS}")

# CMake helpers:
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(
  ${PROJECT_NAME}
  STATIC
  ${PROJECT_SOURCE_DIR}/json_parser.c
  ${PROJECT_SOURCE_DIR}/rbtree.c
  ${PROJECT_SOURCE_DIR}/Json.cc
)

if (JSON_OPT_BUILD_TESTS)
  add_subdirectory(test)
endif()

if (JSON_OPT_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()
