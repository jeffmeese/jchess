/*!
 * \file jcl_board.h
 *
 * This file contains the interface for the Board object
 */

#ifndef JCL_BOARD_H
#define JCL_BOARD_H

#include <cstdint>
#include <map>
#include <string>

#include "jcl_fen.h"
#include "jcl_move.h"
#include "jcl_movelist.h"
#include "jcl_types.h"

namespace jcl
{

/*!
 * \brief Defines a chess board
 *
 * The Board class defines an abstract base class
 * from which other chess board representations will
 * derive. This class holds a few values that are
 * common to all board types (e.g castling rights, ...)
 * and declares other functions which derived board classes must implement.
 *
 * This class keeps track of all of its own board
 * state such as en-passant capture columns, castling rights,
 * move clocks, and where the king of each color resides
 * on the board so derived objects do not need to update
 * this information when moves are made or unmade.
 */
class Board
{
public:
  static constexpr uint8_t CASTLE_NONE              = 0x00;
  static constexpr uint8_t CASTLE_WHITE_KING        = 0x01;
  static constexpr uint8_t CASTLE_WHITE_QUEEN       = 0x02;
  static constexpr uint8_t CASTLE_BLACK_KING        = 0x04;
  static constexpr uint8_t CASTLE_BLACK_QUEEN       = 0x08;
  static constexpr uint8_t INVALID_ENPASSANT_COLUMN = 8;

  /*!
   * \brief Constructor
   *
   * Defines a default Board object
   */
  Board();

  /*!
   * \brief Generates all moves
   *
   * This function is called to generate all pseudo-legal moves
   * for all pieces on the board for the player that is currently
   * moving. The \ref makeMove, \ref isCellAttacked, and \ref unmakeMove
   * functions can be used to identify moves in the returned move
   * list that are not actually valid since the leave the king in check.
   *
   * \param moveList The move list to hold the moves
   *
   * \return true if successful, false otherwise
   */
  bool generateMoves(MoveList & moveList) const;

  /*!
   * \brief Generates moves for a particular sqaure
   *
   * This function is called to generate all pseudo-legal moves
   * for all pieces on the board for the player that is currently
   * moving. The \ref makeMove, \ref isCellAttacked, and \ref unmakeMove
   * functions along with the \ref getKingRow and \ref getKingColumn functions
   * can be used to identify moves in the returned move
   * list that are not actually valid since the leave the king in check.
   *
   * \param moveList The move list to hold the moves
   *
   * \return true if successful, false otherwise
   */
  bool generateMoves(uint8_t row, uint8_t col, MoveList & moveList) const;

  /*!
   * \brief Returns the column for the king
   *
   * This function returns the column of the board
   * where the king of the specified color resides.
   *
   * \return The column of the king square
   */
  uint8_t getKingColumn(Color color) const;

  /*!
   * \brief Returns the row for the king
   *
   * This function returns the row of the board
   * where the king of the specified color resides.
   *
   * \return The row of the king square
   */
  uint8_t getKingRow(Color color) const;

  /*!
   * \brief Returns the piece type
   *
   * This function returns the piece type located at the specified
   * row and column on the board. The piece type will be one of
   * the values in \ref PieceType.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   *
   * \return The piece type located at the row and column
   */
  PieceType getPieceType(uint8_t row, uint8_t col) const;

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
   * with enpassant captures INVALID_ENPASSANT_COLUMN is returned.
   *
   * \return The current enpassant capture column
   */
  uint8_t getEnpassantColumn() const;

  /*!
   * \brief Gets the current full move number
   *
   * The full move number represents the number of white
   * moves that have been replied to with a black move.
   *
   * \return The number of full moves played
   */
  uint32_t getFullMoveNumber() const;

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
   * \brief Returns the side to move
   *
   * This function returns the current side available
   * to move.
   *
   * \return The current side to move
   */
  Color getSideToMove() const;

  /*!
   * \brief Determines if a cell is attacked.
   *
   * This function will determine whether the square at the
   * specified row and column is attacked by a piece of the
   * specified attackColor. This is a bit like an inverse
   * of the generateMoves funtions.
   *
   * \param row The row of the square
   * \param col The column of the square
   * \param attackColor The color of the attacking pieces
   *
   * \return true if the square is attacked, false otherwise
   */
  bool isCellAttacked(uint8_t row, uint8_t col, Color attackColor) const;

  /*!
   * \brief Makes a move
   *
   * This function performs the specified move on the board.
   * Board state and piece positions will be updated based
   * on the move parameters.
   *
   * \param move The move to make
   *
   * \return true if the move is successful, false otherwise
   */
  bool makeMove(const Move * move);

  /*!
   * \brief Resets the board
   *
   * This function resets to the board to its initial state
   * as if a new game is being played. This function will
   * call the doReset function to allow derived classes
   * to do their own internal reset.
   */
  void reset();

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
  void setCastlingRights(uint8_t value);

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
   * \brief Sets the side to move
   *
   * This function sets the current side available
   * to move.
   *
   * \param value The current side to move
   */
  void setSideToMove(Color value);

  /*!
   * \brief Sets the piece type
   *
   * This function sets the piece type at the specified row
   * and column to the specified piece type.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   * \param pieceType The piece type
   *
   * \return true if successful, false otherwise
   */
  bool setPieceType(uint8_t row, uint8_t col, PieceType pieceType);

  /*!
   * \brief Sets the board position
   *
   * This function sets the board from a string containing
   * a FEN notation for the board. If the FEN string cannot
   * be interpreted properly, false will be returned.
   *
   * \param fenString A string containing a FEN represention of the board
   *
   * \return true if the undo is successful, false otherwise
   */
  bool setPosition(const std::string & fenString);

  /*!
   * \brief Unmakes a move
   *
   * This function undoes a particular move on the board. It
   * will reset all board state as if the move had never been
   * made.
   *
   * \param move The move to undo
   *
   * \return true if the undo is successful, false otherwise
   */
  bool unmakeMove(const Move * move);

protected:

  /*!
   * \brief Generates a move list
   *
   * This function generates all psedo-legal moves for the
   * current position. The \ref isCellAttacked function can
   * be used to determine which moves are actually legal by
   * determining whether a move places the king in check.
   * Derived classes must override this function to generate
   * all pseudo-legal for the current board position.
   *
   * \param moveList The move list to update
   *
   * \return true if the function is successful, false otherwise
   */
  virtual bool doGenerateMoves(MoveList & moveList) const = 0;

  /*!
   * \brief Generates a move list
   *
   * This function generates all psedo-legal moves for the
   * piece at the specified row and column for the current
   * position. The \ref isCellAttacked function can be used
   * to determine which moves are actually legal by determining
   * whether a move places the king in check. Derived classes
   * must override this function to generate all pseudo-legal
   * moves for the piece at the specified row and column.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   * \param moveList The move list to update
   *
   * \return true if the function is successful, false otherwise
   */
  virtual bool doGenerateMoves(uint8_t row, uint8_t col, MoveList & moveList) const = 0;

  /*!
   * \brief Returns the piece type
   *
   * This function returns the piece type at the specified
   * row and column on the board. Derived classes must override
   * this function to return the piece type at the specified
   * row and column.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   *
   * \return The piece tyoe at the board position
   */
  virtual PieceType doGetPieceType(uint8_t row, uint8_t col) const = 0;

  /*!
   * \brief Determines if a cell is attacked
   *
   * This function determines if the cell at the
   * specified row and column is attacked by a
   * piece of the color, attackColor. Derived classes
   * must override this function to determine if a cell
   * at the specified row and column is attacked.
   *
   * \param row The row of the cell
   * \param col The column of the cell
   * \param attackColor The color of the attacking pieces
   *
   * \return true if the cell is attacked, false otherwise
   */
  virtual bool doIsCellAttacked(uint8_t row, uint8_t col, Color attackColor) const = 0;

  /*!
   * \brief Makes a move
   *
   * This function performs the specified move on the board.
   * Derived classes must implement this function to update
   * all internal state data for the board associated with the
   * move. Derived classes should also update this class when the
   * move updates data associated with the class (e.g. castlingRights,
   * enpassant capture columns, and so on)
   *
   * \param move The move to perform
   *
   * \return true if the move is successful, false otherwise
   */
  virtual bool doMakeMove(const Move * move) = 0;

  /*!
   * \brief Resets the board state
   *
   * This function must be implemented by derived
   * class to reset all internal data when the board
   * is reset. Derived class must override this function
   * to reset any board state to its initial state.
   */
  virtual void doReset() = 0;

  /*!
   * \brief Sets the piece type
   *
   * This function sets the piece type at the specified
   * row and column on the board. Derived classes must
   * override this function to update internal board
   * state based on setting the piece type at this position.
   *
   * \param row The row of the piece
   * \param col The column of the piece
   * \param pieceType The piece type to set at the board position
   *
   * \return true if the function is successful, false otherwise
   */
  virtual bool doSetPieceType(uint8_t row, uint8_t col, PieceType pieceType) = 0;

  /*!
   * \brief Sets the board position
   *
   * This function must be override by derived classes to
   * update the board state based on the specified FEN
   * data.
   *
   * \param fen The fen data of the board position
   *
   * \return true if the function is successful, false otherwise
   */
  virtual bool doSetPosition(const Fen & fenData) = 0;

  /*!
   * \brief Pushes a move on to the move list
   *
   * This function pushes a move on to the move list and stores
   * information in the move that determine how to undo the move.
   *
   * The from and to parameters determine the starting and ending
   * point for the move. The piece parameter specifies the type of
   * piece that is moving. The capturedPiece argument specifies the
   * piece that is captured by the move if any. The promotedPiece
   * argument specifies the piece a pawn is promoted to when the
   * move is a promotion. The type parameter specifies the type
   * of move that is being perform. See the \ref Move class for a
   * description of the types of moves available.
   *
   * \param from The starting index of the move
   * \param to The ending index of the move
   * \param piece The piece that is moving.
   * \param capturedPiece The piece captured by the move, if any
   * \param promotedPiece The promoted piece type for promotion moves
   * \param type The move type
   * \param moveList The move list to hold the move
   */
  void pushMove(uint8_t sourceRow,
                uint8_t sourceCol,
                uint8_t destRow,
                uint8_t destCol,
                Piece piece,
                Piece capturedPiece,
                Piece promotedPiece,
                Move::Type type,
                MoveList & moveList) const;

  /*!
   * \brief Unmakes a move
   *
   * This function undoes the specified move on the board.
   * Derived classes must implement this function to update
   * all internal state data for the board associated with the
   * move. Derived classes should also update this class when the
   * move updates data associated with the class (e.g. castlingRights,
   * enpassant capture columns, and so on)
   *
   * \param move The move to undo
   *
   * \return true if the undo is successful, false otherwise
   */
  virtual bool doUnmakeMove(const Move * move) = 0;

private:

  /*!
   * \brief Initializes the board
   *
   * This function initializes the board to a default state.
   */
  void init();

  /*!
   * \brief Updates the castling rights
   *
   * This function updates the castling rights
   * for the board based on the move parameters
   *
   * \param move The move
   */
  void updateCastlingRights(const Move * move);

  /*!
   * \brief Updates the move clocks
   *
   * This function updates the move clocks
   * for the board based on the move parameters
   *
   * \param move The move
   */
  void updateMoveClocks(const Move * move);

  // Members
  uint8_t mCastlingRights;              // Current castling rights
  uint8_t mEnPassantColumn;             // Current en-passant capture column
  uint32_t mFullMoveCounter;            // Current full move counter
  uint32_t mHalfMoveClock;              // Current half move clock
  Color mSideToMove;                    // Current side to move
  std::map<Color, uint8_t> mKingColumn; // Column for king for each side
  std::map<Color, uint8_t> mKingRow;    // Row for king for each side
};

inline uint8_t Board::getCastlingRights() const
{
  return mCastlingRights;
}

inline uint8_t Board::getEnpassantColumn() const
{
  return mEnPassantColumn;
}

inline uint32_t Board::getFullMoveNumber() const
{
  return mFullMoveCounter;
}

inline uint32_t Board::getHalfMoveClock() const
{
  return mHalfMoveClock;
}

inline Color Board::getSideToMove() const
{
  return mSideToMove;
}

inline void Board::setCastlingRights(uint8_t value)
{
  mCastlingRights = value;
}

inline void Board::setEnPassantColumn(uint8_t value)
{
  mEnPassantColumn = value;
}

inline void Board::setFullMoveCounter(uint32_t value)
{
  mFullMoveCounter = value;
}

inline void Board::setHalfMoveClock(uint32_t value)
{
  mHalfMoveClock = value;
}

inline void Board::setSideToMove(Color value)
{
  mSideToMove = value;
}

}

#endif // #ifndef JCL_BOARD_H
