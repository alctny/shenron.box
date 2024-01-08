cmake_minimum_required(VERSION 3.10)

project(rngst C)

add_executable(rngst-c)
target_sources(rngst-c PRIVATE main.c)