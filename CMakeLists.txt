cmake_minimum_required(VERSION 3.4)
cmake_policy(SET CMP0048 NEW)

mark_as_advanced(MOBILE_VERSION)
set(MOBILE_VERSION 1.0.0)

project(
  mobilecc
  VERSION ${MOBILE_VERSION}
  LANGUAGES CXX)

add_compile_options(-std=c++11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(
  ${mobilecc_SOURCE_DIR}/src
)

mark_as_advanced(mobilecc_srcs)
aux_source_directory(${mobilecc_SOURCE_DIR}/src mobilecc_srcs)
aux_source_directory(${mobilecc_SOURCE_DIR}/src/ATChannel mobilecc_srcs)
aux_source_directory(${mobilecc_SOURCE_DIR}/src/Dail mobilecc_srcs)

add_library(mobilecc-static STATIC ${mobilecc_srcs})
target_link_libraries(mobilecc-static
                      pthread
                      dl)

add_executable(example ${mobilecc_SOURCE_DIR}/example/example.cc)
target_link_libraries(example
                      mobilecc-static
                      pthread
                      dl)
