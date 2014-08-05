cmake_minimum_required(VERSION 2.8)
project(jbms-array_view)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

## Required for tests
find_package(Boost COMPONENTS unit_test_framework REQUIRED)


set (${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set (${PROJECT_NAME}_INCLUDE_DIRS
  "${${PROJECT_NAME}_INCLUDE_DIR}")


#### Build tests
enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1y -ggdb -Wall -Werror")

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

set(test_name test_array_view)
add_executable("${test_name}" test/test_array_view.cpp)
target_link_libraries("${test_name}" ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
set_property(TARGET "${test_name}" PROPERTY COMPILE_DEFINITIONS BOOST_TEST_MAIN BOOST_TEST_DYN_LINK)
add_test("${test_name}" "${test_name}")

#### Install headers
set(header_install_dir "include/${PROJECT_NAME}")
install(DIRECTORY "${${PROJECT_NAME}_INCLUDE_DIR}" DESTINATION "${header_install_dir}")

#### Generate CMake modules for importing this package
include(jbms-WriteConfig)
write_config_file(
  TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in"
  PATH_VARS ${PROJECT_NAME}_INCLUDE_DIR
  REL_PATH "${header_install_dir}" )
