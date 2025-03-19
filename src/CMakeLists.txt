set(TARGET_NAME jchess)
set(BUILD_TYPE STATIC)

set(HDR_FILES
    jchess_board.h
    jchess_board8x8.h
    jchess_consts.h
    jchess_fen.h
    jchess_move.h
    jchess_movelist.h
    jchess_types.h
    #alphabetasearch.h
    #bitboard.h
    #board.h
    #board0x88.h
    #board8x8.h
    #boardbase.h
    #engine.h
    #evaluation.h
    #fen.h
    #game.h
    #jchess.h
    #move.h
    #movelist.h
    #negamaxsearch.h
    #perft.h
    #plf_nanotimer.h
    #search.h
    #timer.h
    #types.h
    #util.h
)

# Add source files
set(SRC_FILES
    jchess_board.cpp
    jchess_board8x8.cpp
    jchess_fen.cpp
    jchess_move.cpp
    jchess_movelist.cpp
    #alphabetasearch.cpp
    #bitboard.cpp
    #board0x88.cpp
    #board8x8.cpp
    #boardbase.cpp
    #game.cpp
    #engine.cpp
    #evaluation.cpp
    #fen.cpp
    #move.cpp
    #movelist.cpp
    #negamaxsearch.cpp
    #perft.cpp
    #search.cpp
    #timer.cpp
    #util.cpp
)

# Add include files


# Create target
add_library(${TARGET_NAME} ${BUILD_TYPE} ${HDR_FILES} ${SRC_FILES})

# Specify target include directories
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
