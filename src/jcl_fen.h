/*!
 * \file jcl_fen.h
 *
 * This file contains the interface for the Fen object
 */

#ifndef JCL_FEN_H
#define JCL_FEN_H

#include <string>

#include "jcl_board.h"
#include "jcl_types.h"

namespace jcl
{

/*!
 * \brief Defines the Fen object
 *
 * The Fen object represents the chess board in Forsyth-Edwards Notation (FEN).
 * FEN objects represent a standard way of encoding the position and
 * state of the chess board to allow a game to be restarted at any
 * point in the game.
 *
 * The Fen object holds data such as castling rights, the full move counter,
 * the half move clock, the current enpassant capture column, along
 * with what type of piece, if any, is on each square of the board.
 *
 * The Fen object contains functions to read the object from a string,
 * or set the object from Board object. It also contains functions to
 * write the Fen object in a stanard FEN string.
 */
class Fen
{
public:

  /*!
   * \brief Constructor
   *
   * This function constructs a Fen object
   * and initializes all data as if a chess
   * game is at the initial board position.
   */
  Fen();

  /*!
   * \brief Gets the current castling rights
   *
   * This function returns the current castling rights
   * for the board. The castling rights are held in a
   * single 8 bit integer of which 4 bits is actually
   * used. Each bit of the least signficant 4 bits of
   * the number represents a single castling right.
   * The jcl_consts.h file shows the numeric value
   * for each of the castling rights.
   *
   * \return The current castling rights
   */
  uint8_t getCastlingRights() const;

  /*!
   * \brief Returns the current column for enpassant captures
   *
   * This function returns the current column associated
   * with enpassant captures. This value will be set even
   * if a current enpassant capture is not available. The
   * row for the enpassant capture can be determined by which
   * side is currently moving. If there is no column associated
   * with enpassant captures INVALID_ENPASSANT_COLUMN from the Board
   * object sis returned.
   *
   * \return The current enpassant capture column
   */
  uint8_t getEnPassantColumn() const;

  /*!
   * \brief Gets the current full move number
   *
   * The full move number represents the number of white
   * moves that have been replied to with a black move.
   *
   * \return The number of full moves played
   */
  uint32_t getFullMoveCounter() const;

  /*!
   * \brief Returns the half move clock number
   *
   * The half move clock number keeps track of the
   * number of half moves that have taken place. This
   * number is reset each time a pawn is moved or a
   * capture takes place. This number is used to determine
   * if a game has stalemated by the fifty-move rule.
   *
   * \return The current half move clock
   */
  uint32_t getHalfMoveClock() const;

  /*!
   * \brief Returns the piece type
   *
   * This function returns the piece type located at the specified
   * row and column. The piece type will be one of the values
   * in \ref PieceType.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   *
   * \return The piece type located at the row and column
   */
  PieceType getPieceType(uint8_t row, uint8_t col) const;

  /*!
   * \brief Returns the side to move
   *
   * This function returns the current side available
   * to move.
   *
   * \return The current side to move
   */
  Color getSideToMove() const;

  /*!
   * \brief Sets the current castling rights
   *
   * This function sets the current castling rights
   * for the board. The castling rights are held in a
   * single 8 bit integer of which 4 bits is actually
   * used. Each bit of the least signficant 4 bits of
   * the number represents a single castling right.
   * The jcl_consts.h file shows the numeric value
   * for each of the castling rights.
   *
   * \param value The new value for castling rights
   */
  void setCasltingRights(uint8_t value);

  /*!
   * \brief Sets the current column for enpassant captures
   *
   * This function sets the current column associated
   * with enpassant captures. The row for the enpassant capture
   * can be determined by which side is currently moving.
   * If there is no column associated with enpassant captures
   * the INVALID_ENPASSANT_COLUMN value should be used.
   *
   * \param value The new enpassant capture column
   */
  void setEnPassantColumn(uint8_t value);

  /*!
   * \brief Sets the data from a Board object
   *
   * This function will set the data in the Fen
   * object from the supplied Board object. This
   * essentially saves the state of the board
   * so it can be reloaded at a later time.
   *
   * \param board The board object from which to set the data
   *
   * \return true if successful, false otherwise
   */
  bool setFromBoard(const Board * board);

  /*!
   * \brief Sets the data from a string
   *
   * This function sets the data in the Fen object
   * from standard FEN string.
   *
   * The FEN string will be checked for syntatic validity in that it
   * must have the proper format and the values of the string
   * must be able to be interpreted. If the string is
   * not valid the Fen object will not be changed and
   * false will be returned.
   *
   * The FEN string will not, however, be checked for semantic
   * validity. That means no checks to see if the pieces in the
   * string make sense for a chess game. For example, if the FEN
   * string says there are 64 black queens on the board this
   * will not generate an error.
   *
   * \param fenString A standard FEN string
   *
   * \return true if the function is successful, false otherwise
   */
  bool setFromString(const std::string & fenString);

  /*!
   * \brief Sets the current full move number
   *
   * The full move number represents the number of white
   * moves that have been replied to with a black move.
   *
   * \param value The number of full moves played
   */
  void setFullMoveCounter(uint32_t value);

  /*!
   * \brief Sets the half move clock number
   *
   * The half move clock number keeps track of the
   * number of half moves that have taken place. This
   * number is reset each time a pawn is moved or a
   * capture takes place. This number is used to determine
   * if a game has stalemated by the fifty-move rule.
   *
   * \param value The current half move clock
   */
  void setHalfMoveClock(uint32_t value);

  /*!
   * \brief Sets the piece type
   *
   * This function sets the piece type located at the specified
   * row and column. The piece type will be one of the values
   * in \ref PieceType.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   *
   * \return The piece type located at the row and column
   */
  void setPieceType(uint8_t row, uint8_t col, PieceType pieceType);

  /*!
   * \brief Sets the side to move
   *
   * This function sets the current side available
   * to move.
   *
   * \param value The current side to move
   */
  void setSideToMove(Color value);

  /*!
   * \brief Writes the Fen data to a string
   *
   * This function writes the Fen data object to a standard
   * FEN string.
   *
   * \return The FEN string associated with the current data.
   */
  std::string toString() const;

private:

  /*!
   * \brief Initializes the Fen object
   *
   * This function initializes the Fen object to a default
   * state.
   */
  void init();

  uint8_t mCastlingRights;
  uint8_t mEnPassantColumn;
  uint32_t mHalfMoveClock;
  uint32_t mFullMoveCounter;
  Color mSideToMove;
  PieceType mPieceType[8][8];
};

}

#endif // #ifndef JCL_FEN_H
