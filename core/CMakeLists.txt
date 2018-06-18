cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Set project properties
project(jchess CXX)
set(TARGET_NAME jchess)
set(JCHESS_BUILD_TYPE STATIC)

# Add source files
set(JCHESS_SOURCES
    alphabetasearch.cpp
    bitboard.cpp
    board0x88.cpp
    board8x8.cpp
    boardbase.cpp
    game.cpp
    engine.cpp
    evaluation.cpp
    fen.cpp
    move.cpp
    movelist.cpp
    negamaxsearch.cpp
    perft.cpp
    search.cpp
    timer.cpp
    util.cpp
)

# Add include files
set(JCHESS_HEADERS
    alphabetasearch.h
    bitboard.h
    board.h
    board0x88.h
    board8x8.h
    boardbase.h
    engine.h
    evaluation.h
    fen.h
    game.h
    jchess.h
    move.h
    movelist.h
    negamaxsearch.h
    perft.h
    search.h
    timer.h
    types.h
    util.h
)

# Create target
add_library(${TARGET_NAME} ${JCHESS_BUILD_TYPE} ${JCHESS_SOURCES} ${JCHESS_HEADERS})

# Specify target include directories
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
