/*!
 * \file jcl_board8x8.h
 *
 * This file contains the interface for the Board8x8 object
 */

#ifndef JCL_BOARD8X8_H
#define JCL_BOARD8X8_H

#include <map>

#include "jcl_board.h"
#include "jcl_move.h"
#include "jcl_movelist.h"

namespace jcl
{

/*! \brief Represents a standard 8x8 board representation
 *
 * The Board8x8 class represents the chess board in a standard
 * 8x8 set of squares. This is easiest of the board representations
 * from an intuitive standpoint though it suffers from poor performance
 * due to the numerous checks for moves and attacks that happen off
 * the board. While some optimizations were made for performance
 * most of the code is pretty straightforward and can be a good reference
 * to understand the type of logic involved in a board representation.
 *
 * The performance for this board could be increased by pre-computing
 * possible moves for each square on the board for each attacking
 * direction. This would eliminate a lot of the checks for pieces
 * moving or attacking off the board. Even with that, however,
 * this class will never be able to match the performance of
 * more complex board representations such as bit boards.
 */
class Board8x8
    : public Board
{
public:

  /*!
   * \brief Constructor
   *
   * Constructs a default Board8x8 instance.
   */
  Board8x8();

protected:

  // Overrides
  bool doGenerateMoves(MoveList & moveList) const override;
  bool doGenerateMoves(uint8_t row, uint8_t col, MoveList & moveList) const override;
  PieceType doGetPieceType(uint8_t row, uint8_t col) const override;
  bool doIsCellAttacked(uint8_t row, uint8_t col, Color attackColor) const override;
  bool doMakeMove(const Move * move) override;
  void doReset() override;
  bool doSetPieceType(uint8_t row, uint8_t col, PieceType pieceType) override;
  bool doSetPosition(const Fen & fenData) override;
  bool doUnmakeMove(const Move * move) override;

private:

  /*!
   * \brief Generates the castling moves
   *
   * This function generates the possible castling moves
   * based on the current board position. Castling moves
   * are only dependent of the current state of the board
   * and the castling rights available so they only need
   * to be generated once when all moves are being generated.
   *
   * \param moveList The move list to hold the moves
   */
  void generateCastlingMoves(MoveList & moveList) const;

  /*!
   * \brief Generates a move list
   *
   * This function generates the moves for the piece at the
   * specified row and column and adds them to the move list.
   * Moves are generated is a pseudo-legal fashion. Checks are
   * not made to determine if a paticular move will place the
   * king of the same color is in check. The \ref isCellAttacked
   * function can be used to determine which moves are
   * actually legal.
   *
   * The generateCastling parameter determines whether castling
   * moves will be generated. Since castling moves are not index
   * dependent they could be precomputed before this function is
   * called so this parameter allows castling move generation
   * to be skipped. When all moves for all pieces are being generated
   * this function gets called in a loop so castling moves are
   * skipped in that instance.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   * \param moveList The move list to update
   * \param generateCastling Set to true to include castling moves
   *
   * \return true if the function is successful, false otherwise
   */
  bool generateMoves(uint8_t row,
                     uint8_t col,
                     MoveList & moveList,
                     bool generateCastling) const;

  /*!
   * \brief Generates a move list
   *
   * This function generates the moves for the piece at the
   * specified index and adds them to the move list.
   * Moves are generated is a pseudo-legal fashion. Checks are
   * not made to determine if a paticular move will place the
   * king of the same color is in check. The \ref isCellAttacked
   * function can be used to determine which moves are
   * actually legal.
   *
   * \param index The index of the piece
   * \param moveList The move list to update
   *
   * \return true if the function is successful, false otherwise
   */
  bool generateMoves(uint8_t index,
                     MoveList & moveList) const;

  /*!
   * \brief Generates the pawn moves for an index
   *
   * This function generates all moves for a pawn
   * located at a specified index. Pawns are the only
   * piece that cannot move backwards so its move
   * generation is handled specially. Pawns also have
   * different capture and non-capture move techniques.
   *
   * \param index The index of the pawn
   * \param sideToMove The color of the pawn
   * \param moveList The move list to hold the moves
   */
  void generatePawnMoves(uint8_t index,
                         Color sideToMove,
                         MoveList & moveList) const;

  /*!
   * \brief Generates non-pawn moves
   *
   * This function generates all moves for a piece at the
   * specified index. This function is called for all
   * pieces with the exception of pawns.
   *
   * The rowIncrement and colIncrement parametes specify
   * in which direction the piece will be moving.
   *
   * The slider parameter determines whether this piece is a
   * sliding piece (e.g. bishop, queen, rook). Sliding pieces
   * can continue moving along the board until they encounter
   * another piece or move off the board. Non-sliding pieces
   * (kings, knights) can only move once.
   *
   * \param index The index of the piece
   * \param rowIncrement The displacement in row for each move
   * \param colIncrement The displacement in column for each move
   * \param slider true if the piece is a sliding piece
   * \param moveList The list to hold the moves
   */
  void generateSliderMoves(uint8_t index,
                           int8_t rowIncrement,
                           int8_t colIncrement,
                           bool slider,
                           MoveList & moveList) const;

  /*! \brief Initializes the board to it's initial state
   *
   *  This function initializes the board to the initial state
   *  at the start of a game. It also initializes all
   *  other data stored by the object.
   */
  void initBoard();

  /*!
   * \brief Determines if a cell is attacked
   *
   * This function determines if the cell at the specified
   * index is attacked by a piece of the specified color.
   * This is primarily used by engines to determine if particular
   * move is valid. Any moves that put the king in check are
   * considered invalid moves.
   *
   * \param index The index of the square to check for attacks
   * \param attackColor The color of the pieces that are attacking.
   *
   * \return true if the cell at the index is attacked, false otherwise
   */
  bool isCellAttacked(uint8_t index, Color attackColor) const;

  // Members
  Piece mPieces[64];
  Color mColors[64];
};

}

#endif // #ifndef JCL_BOARD8X8_H
