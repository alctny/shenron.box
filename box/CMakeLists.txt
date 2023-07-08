cmake_minimum_required(VERSION 3.26)

set ( CMAKE_C_FLAGS "-D_BSD_SOURCE -D_DEFAULT_SOURCE")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# 递归删除文件名中的特殊字符
add_executable(rsfd ${CMAKE_CURRENT_SOURCE_DIR}/rsfd.c)

# 剩余工作时间提示
add_executable(sina ${CMAKE_CURRENT_SOURCE_DIR}/offduty.c)

# 生成指定范围内的随机顺序数列
add_executable(rseq ${CMAKE_CURRENT_SOURCE_DIR}/random_sequence.c)

