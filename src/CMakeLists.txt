set(TARGET_NAME jcl)
set(BUILD_TYPE STATIC)

set(HDR_FILES
    jcl_bitboard.h
    jcl_board.h
    jcl_board8x8.h
    jcl_evaluation.h
    jcl_fastboard8x8.h
    jcl_fen.h
    jcl_move.h
    jcl_movelist.h
    jcl_perft.h
    jcl_timer.h
    jcl_types.h
    jcl_util.h
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
    jcl_bitboard.cpp
    jcl_board.cpp
    jcl_board8x8.cpp
    jcl_evaluation.cpp
    jcl_fastboard8x8.cpp
    jcl_fen.cpp
    jcl_move.cpp
    jcl_movelist.cpp
    jcl_perft.cpp
    jcl_timer.cpp
    jcl_util.cpp
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
