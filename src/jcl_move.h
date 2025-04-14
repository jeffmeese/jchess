/*!
 * \file jcl_move.h
 *
 * This file contains the interface for the Move object
 */

#ifndef JCL_MOVE_H
#define JCL_MOVE_H

#include "jcl_types.h"

#include <string>

namespace jcl
{

/*!
 * \brief Defines a chess move
 *
 * The Move object represents a single move in chess game.
 * It encodes all information relevant to both do and the
 * move and undo the move.
 *
 * A move is defined by its source row, source column,
 * destination row, and destination column, and what piece is
 * moving. A move can also capture an opposing piece or be one
 * that promotes a pawn. There are also special moves such as
 * castling and enpassant captures that can also be encoded
 * in the Move object.
 */
class Move
{
public:

  struct Position
  {
    uint8_t row;
    uint8_t col;
  };

  struct BoardState
  {
    uint8_t castlingRights;
    uint8_t enPassantColumn;
    uint32_t halfMoveClock;
    uint32_t fullMoveCounter;
  };

  struct Pieces
  {
    Piece piece;
    Piece capturePiece;
    Piece promotionPiece;
  };

  /*!
   * \brief Defines a move type
   *
   * There are multiple types of moves that can happen
   * during a chess game. This enumeration provides a
   * decscription of each of the types of moves.
   */
  enum class Type
  {
    Quiet = 0,            /*!< Defines a quiet move */
    Capture = 1,          /*!< Defines a capture move */
    EpCapture = 2,        /*!< Defines an enpassant capture move */
    Castle = 3,           /*!< Defines a castling move */
    DoublePush = 4,       /*!< Defines a pawn double push move that allows for enpassant captures */
    Promotion = 5,        /*!< Defines a promotion of a pawn */
    PromotionCapture = 6, /*!< Defines a promotion of a pawn where an opposing piece is captured */
    Null = 7              /*!< Defines a NULL move, where a player does not move at all */
  };

  /*!
   * \brief Constructor
   *
   * This function constructs a move from the supplied parameters. The sourceRow
   * and sourceCol parameters define the starting point for the move. The
   * destRow and destCol define the ending point for the move. The piece
   * paramter defines the piece that is moving.
   *
   * \param sourceRow The starting row for the move
   * \param sourceCol The starting column for the move
   * \param destRow The ending row for the move
   * \param destCol The ending column for the move
   * \param piece The piece that is moving
   */
  Move(uint8_t sourceRow,
       uint8_t sourceCol,
       uint8_t destRow,
       uint8_t destCol,
       uint8_t castlingRights,
       uint8_t enPassantColumn,
       uint32_t halfMoveClock,
       uint32_t fullMoveCounter,
       Piece piece,
       Type type,
       Piece capturePiece = Piece::None,
       Piece promotionPiece = Piece::None);


  Move(const Position & sourcePosition,
       const Position & destinationPosition,
       const Pieces & pieces,
       const BoardState & boardState,
       Type type);
  /*!
   * \brief Returns the capture piece
   *
   * This function returns the captured piece for the move if any.
   *
   * \return The captured piece
   */
  Piece getCapturedPiece() const;

  /*!
   * \brief Returns the castling rights before the move
   *
   * This function returns the castling rights of the board
   * prior to the move being played. This allows the castling rights
   * to be reset if this move is undone.
   *
   * \return The castling rights before the move
   */
  uint8_t getCastlingRights() const;

  /*!
   * \brief Returns the desination column for the move
   *
   * This function returns the ending column for the move.
   * Along with the destination row the ending square on the
   * board is defined.
   *
   * \return The destination column for the move
   */
  uint8_t getDestinationColumn() const;

  /*!
   * \brief Returns the desination row for the move
   *
   * This function returns the ending row for the move.
   * Along with the destination column the ending square on the
   * board is defined.
   *
   * \return The destination row for the move
   */
  uint8_t getDestinationRow() const;

  /*!
   * \brief Returns the enpassant capture column
   *
   * This function returns the column associated with
   * enpassant captures before the move when one is
   * available. If an enpassant capture is not available
   * this function will return INVALID_ENPASSANT_COLUMN.
   *
   * This allows the enpassant capture column
   * to be reset if this move is undone.
   *
   * \return The enpassant capture column
   */
  uint8_t getEnPassantColumn() const;

  /*!
   * \brief Returns the full move counter before the move
   *
   * This function returns the full move counter before
   * the move is performed. This allows the full move counter
   * to be reset if the move is undone.
   *
   * \return The full move counter before the move
   */
  uint32_t getFullMoveCounter() const;

  /*!
   * \brief Returns the half move clock before the move
   *
   * This function returns the half move clock before
   * the move is performed. This allows the half move clock
   * to be reset if the move is undone.
   *
   * \return The half move clock before the move
   */
  uint32_t getHalfMoveClock() const;

  /*!
   * \brief Returns the piece associated with the move
   *
   * This function returns the piece associated with the move.
   * This will be one of the values in the \ref Piece enumeration
   *
   * \return The piece associated with the move
   */
  Piece getPiece() const;

  /*!
   * \brief Returns the promoted piece associated with the move if any.
   *
   * This function returns the piece that a pawn will be promoted to
   * if this is a promotion move.
   *
   * \return The promoted piece
   */
  Piece getPromotedPiece() const;

  /*!
   * \brief Returns the source column for the move
   *
   * This function returns the starting column for the move.
   * Along with the starting row the starting square on the
   * board is defined.
   *
   * \return The source column for the move
   */
  uint8_t getSourceColumn() const;

  /*!
   * \brief Returns the source row for the move
   *
   * This function returns the starting row for the move.
   * Along with the source column the starting square on the
   * board is defined.
   *
   * \return The destination row for the move
   */
  uint8_t getSourceRow() const;

  /*!
   * \brief Returns the move type
   *
   * This function returns the type of move
   * being performed. It returns one of the
   * values from the \ref Type enumeration
   *
   * \return The move type
   */
  Type getType() const;

  /*!
   * \brief Returns whether this move is a capture
   *
   * This function returns whether this move captures
   * an opponent piece, whether it be a standard capture
   * or a capture where a pawn promotion also captures
   * a piece.
   *
   * \return true if this move contains a capture, false otherwise
   */
  bool isCapture() const;

  /*!
   * \brief Returns whether this move is a castle
   *
   * This function returns whether this move is one
   * associated with castling. Castling moves are
   * only available in certain situations.
   *
   * \return true if this move is a castling move, false otherwise
   */
  bool isCastle() const;

  /*!
   * \brief Returns whether this move is a double pawn push
   *
   * This function returns whether this move is one that
   * is moving a pawn two squares from its original position
   * at the start of the game. When a pawn is pushed two sqaures
   * there is a possibility that it could be captured with an
   * enpassant capture move if the board conditions are right.
   *
   * \return true if this move is a double pawn push, false otherwise
   */
  bool isDoublePush() const;

  /*!
   * \brief Returns whether this move is an enpassant capture
   *
   * This function returns whether this move is an enpassant
   * capture move. Enpassant captures are special pawn moves
   * that are only available in certain situations.
   *
   * \return true if this move is a enpassant capture, false otherwise
   */
  bool isEnPassantCapture() const;

  /*!
   * \brief Returns whether this move is a quiet move
   *
   * A quiet move is one that does not involve a capture
   * or promotion and is not a castling move.
   *
   * \return true if this move is a promotion, false otherwise
   */
  bool isQuiet() const;

  /*!
   * \brief Returns whether this move involves a promotion
   *
   * This function returns whether this move involves promoting
   * a pawn to a more powerful piece. Promotions happen when a pawn
   * makes it to the rank associated with the opponents king.
   *
   * \return true if this move is a promotion, false otherwise
   */
  bool isPromotion() const;

  /*!
   * \brief Returns whether this move involves a promotion and a capture
   *
   * This function returns whether this move involves promoting
   * a pawn to a more powerful piece as well as capturing
   * an opponent piece. Promotions happen when a pawn
   * makes it to the rank associated with the opponents king.
   *
   * \return true if this move is a promotion capture, false otherwise
   */
  bool isPromotionCapture() const;

  /*!
   * \brief Returns whether this move is a valid chess move
   *
   * This function returns whether this move is a valid
   * chess move. It only checks that a piece is associated with
   * the move and where the source and desination squares are valid.
   * It does not take the current state of the game into account.
   *
   * \return true if this move is valid, false otherwise
   */
  bool isValid() const;

  /*!
   * \brief Writes the move to a string in smith notation
   *
   * This function creates a string in standard smit notation
   * defining the move.
   *
   * \return A string containing the smith notation for the move
   */
  std::string toSmithNotation() const;

private:
  uint8_t mCastlingRights;
  uint8_t mDestinationColumn;
  uint8_t mDestinationRow;
  uint8_t mEnPassantColumn;
  uint8_t mSourceColumn;
  uint8_t mSourceRow;
  uint32_t mFullMoveCounter;
  uint32_t mHalfMoveClock;
  Type mType;
  Piece mCapturedPiece;
  Piece mPiece;
  Piece mPromotedPiece;
};

inline Piece Move::getCapturedPiece() const
{
  return mCapturedPiece;
}

inline uint8_t Move::getCastlingRights() const
{
  return mCastlingRights;
}

inline uint8_t Move::getDestinationColumn() const
{
  return mDestinationColumn;
}

inline uint8_t Move::getDestinationRow() const
{
  return mDestinationRow;
}

inline uint8_t Move::getEnPassantColumn() const
{
  return mEnPassantColumn;
}

inline Move::Type Move::getType() const
{
  return mType;
}

inline uint32_t Move::getFullMoveCounter() const
{
  return mFullMoveCounter;
}

inline uint32_t Move::getHalfMoveClock() const
{
  return mHalfMoveClock;
}

inline bool Move::isCapture() const
{
  return (mType == Type::Capture || mType == Type::PromotionCapture || mType == Type::EpCapture);
}

inline bool Move::isCastle() const
{
  return (mType == Type::Castle);
}

inline bool Move::isDoublePush() const
{
  return (mType == Type::DoublePush);
}

inline bool Move::isEnPassantCapture() const
{
  return (mType == Type::EpCapture);
}

inline bool Move::isQuiet() const
{
  return (mType == Type::Quiet);
}

inline bool Move::isPromotion() const
{
  return (mType == Type::Promotion);
}

inline bool Move::isPromotionCapture() const
{
  return (mType == Type::PromotionCapture);
}

inline Piece Move::getPiece() const
{
  return mPiece;
}

inline Piece Move::getPromotedPiece() const
{
  return mPromotedPiece;
}

inline uint8_t Move::getSourceColumn() const
{
  return mSourceColumn;
}

inline uint8_t Move::getSourceRow() const
{
  return mSourceRow;
}

// inline void Move::setCapturedPiece(Piece piece)
// {
//   mCapturedPiece = piece;
// }

// inline void Move::setCastlingRights(uint8_t castling)
// {
//   mCastlingRights = castling;
// }

// inline void Move::setDestinationColumn(uint8_t col)
// {
//   mDestinationColumn = col;
// }

// inline void Move::setDestinationRow(uint8_t row)
// {
//   mDestinationRow = row;
// }

// inline void Move::setEnpassantColumn(uint8_t col)
// {
//   mEnPassantColumn = col;
// }

// inline void Move::setType(Move::Type flags)
// {
//   mType = flags;
// }

// inline void Move::setFullMoveCounter(uint32_t value)
// {
//   mFullMoveCounter = value;
// }

// inline void Move::setHalfMoveClock(uint32_t value)
// {
//   mHalfMoveClock = value;
// }

// inline void Move::setPiece(Piece piece)
// {
//   mPiece = piece;
// }

// inline void Move::setPromotedPiece(Piece piece)
// {
//   mPromotedPiece = piece;
// }

// inline void Move::setSourceColumn(uint8_t col)
// {
//   mSourceColumn = col;
// }

// inline void Move::setSourceRow(uint8_t row)
// {
//   mSourceRow = row;
// }

}

#endif // #ifndef JCL_MOVE_H
