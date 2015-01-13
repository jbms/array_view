cmake_minimum_required(VERSION 2.8)
project(jbms-array_view)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

## Required for tests
find_package(Boost COMPONENTS unit_test_framework REQUIRED)


## Dependencies
find_package(jbms-utility REQUIRED)


set (${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set (${PROJECT_NAME}_INCLUDE_DIRS
  "${${PROJECT_NAME}_INCLUDE_DIR}"
  ${jbms-utility_INCLUDE_DIRS}
  )


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


set(config_PATH_VARS ${PROJECT_NAME}_INCLUDE_DIR)
set(config_REL_PATH "${header_install_dir}")

### Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SOURCES ${PROJECT_BINARY_DIR}/Doxyfile
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  set (${PROJECT_NAME}_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")
  set (${PROJECT_NAME}_DOXYGEN_TAGFILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.doctag")
  set(html_install_dir "doc/${PROJECT_NAME}/html")
  set(tag_install_file "doc/${PROJECT_NAME}/${PROJECT_NAME}.doctag")
  install(DIRECTORY "${${PROJECT_NAME}_HTML_DIR}" DESTINATION "${html_install_dir}")
  install(FILES "${${PROJECT_NAME}_DOXYGEN_TAGFILE}" DESTINATION "${tag_install_file}")
  set(config_PATH_VARS ${config_PATH_VARS} ${PROJECT_NAME}_HTML_DIR ${PROJECT_NAME}_DOXYGEN_TAGFILE)
  set(config_REL_PATH ${config_REL_PATH} "${html_install_dir}" "${tag_install_file}")

endif(DOXYGEN_FOUND)

#### Generate CMake modules for importing this package
include(jbms-WriteConfig)
write_config_file(
  TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in"
  PATH_VARS ${config_PATH_VARS}
  REL_PATH ${config_REL_PATH})
