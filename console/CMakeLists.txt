cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(jchess CXX)
set(TARGET_NAME jchess_console)

# Add the target executable
add_executable(${TARGET_NAME} main.cpp consolegame.cpp consolegame.h application.cpp application.h)

target_link_libraries(${TARGET_NAME} jchess)
