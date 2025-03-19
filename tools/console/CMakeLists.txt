set(TARGET_NAME jcl_console)

add_executable(${TARGET_NAME} main.cpp consolegame.cpp consolegame.h)

target_link_libraries(${TARGET_NAME} jcl)
