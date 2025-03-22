#ifndef JCL_MOVE_H
#define JCL_MOVE_H

#include "jcl_types.h"

#include <string>

namespace jcl
{

class Move
{
  friend class MoveList;

public:
  enum class Type
  {
    Quiet = 0,
    Capture = 1,
    EpCapture = 2,
    Castle = 3,
    EpPush = 4,
    Promotion = 5,
    PromotionCapture = 6,
    Null = 7
  };

public:
  Move(uint8_t sourceRow, uint8_t sourceCol, uint8_t destRow, uint8_t destCol, Piece piece);

public:
  Piece getCapturedPiece() const;
  uint8_t getCastlingRights() const;
  uint8_t getDestinationColumn() const;
  uint8_t getDestinationRow() const;
  uint8_t getEnPassantColumn() const;
  Type getType() const;
  uint32_t getFullMoveCounter() const;
  uint32_t getHalfMoveClock() const;
  bool isCapture() const;
  bool isCastle() const;
  bool isDoublePush() const;
  bool isEnPassantCapture() const;
  bool isPromotion() const;
  bool isPromotionCapture() const;
  bool isValid() const;
  Piece getPiece() const;
  Piece getPromotedPiece() const;
  uint8_t getSourceColumn() const;
  uint8_t getSourceRow() const;
  void setCapturedPiece(Piece pieceType);
  void setCastlingRights(uint8_t castling);
  void setDestinationColumn(uint8_t col);
  void setDestinationRow(uint8_t row);
  void setEnpassantColumn(uint8_t col);
  void setType(Type type);
  void setFullMoveCounter(uint32_t value);
  void setHalfMoveClock(uint32_t value);
  void setPiece(Piece piece);
  void setPromotedPiece(Piece piece);
  void setSourceColumn(uint8_t col);
  void setSourceRow(uint8_t row);
  std::string toSmithNotation() const;

protected:
  //Move();

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
  return (mType == Type::EpPush);
}

inline bool Move::isEnPassantCapture() const
{
  return (mType == Type::EpCapture);
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

inline void Move::setCapturedPiece(Piece piece)
{
  mCapturedPiece = piece;
}

inline void Move::setCastlingRights(uint8_t castling)
{
  mCastlingRights = castling;
}

inline void Move::setDestinationColumn(uint8_t col)
{
  mDestinationColumn = col;
}

inline void Move::setDestinationRow(uint8_t row)
{
  mDestinationRow = row;
}

inline void Move::setEnpassantColumn(uint8_t col)
{
  mEnPassantColumn = col;
}

inline void Move::setType(Move::Type flags)
{
  mType = flags;
}

inline void Move::setFullMoveCounter(uint32_t value)
{
  mFullMoveCounter = value;
}

inline void Move::setHalfMoveClock(uint32_t value)
{
  mHalfMoveClock = value;
}

inline void Move::setPiece(Piece piece)
{
  mPiece = piece;
}

inline void Move::setPromotedPiece(Piece piece)
{
  mPromotedPiece = piece;
}

inline void Move::setSourceColumn(uint8_t col)
{
  mSourceColumn = col;
}

inline void Move::setSourceRow(uint8_t row)
{
  mSourceRow = row;
}

}

#endif // #ifndef JCL_MOVE_H
