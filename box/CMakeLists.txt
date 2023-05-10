cmake_minimum_required(VERSION 3.26)

set ( CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(rsfn)
target_sources(rsfn
PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rsfn.c
)
# target_include_directories(rsfn
# PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
# )