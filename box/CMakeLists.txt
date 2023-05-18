cmake_minimum_required(VERSION 3.26)

set ( CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 递归删除文件名中的特殊字符
add_executable(rsfd ${CMAKE_CURRENT_SOURCE_DIR}/rsfd.c)

# 剩余工作时间提示
add_executable(sina ${CMAKE_CURRENT_SOURCE_DIR}/offduty.c)

