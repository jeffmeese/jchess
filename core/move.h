#ifndef MOVE_H
#define MOVE_H

#include "types.h"

#include "boardbase.h"

#include <list>
#include <string>

class Move
{
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
  Move();
  Move(uchar sourceRow, uchar sourceCol, uchar destRow, uchar destCol, Piece piece);

public:
  Piece capturePiece() const;
  uchar castlingRights() const;
  uchar destCol() const;
  uchar destRow() const;
  uchar enPassantCol() const;
  Type type() const;
  uint fullMoveCounter() const;
  uint halfMoveClock() const;
  Piece piece() const;
  Piece promotedPiece() const;
  uchar sourceCol() const;
  uchar sourceRow() const;
  void setCapturedPiece(Piece pieceType);
  void setCastlingRights(uchar castling);
  void setDestCol(uchar col);
  void setDestRow(uchar row);
  void setEnpassantColumn(uchar col);
  void setType(Type type);
  void setFullMoveCounter(uint value);
  void setHalfMoveClock(uint value);
  void setPiece(Piece piece);
  void setPromotedPiece(Piece piece);
  void setSourceCol(uchar col);
  void setSourceRow(uchar row);

public:
  bool isCapture() const;
  bool isCastle() const;
  bool isEpPush() const;
  bool isEpCapture() const;
  bool isPromotion() const;
  bool isPromotionCapture() const;
  bool isValid() const;
  std::string toSmithNotation() const;

private:
  uchar mCastlingRights;
  uchar mDestCol;
  uchar mDestRow;
  uchar mEnPassantColumn;
  uchar mSourceCol;
  uchar mSourceRow;
  uint mFullMoveCounter;
  uint mHalfMoveClock;
  Type mType;
  Piece mCapturedPiece;
  Piece mPiece;
  Piece mPromotedPiece;
};

inline Move::Move()
  : mSourceRow(0)
  , mSourceCol(0)
  , mDestRow(0)
  , mDestCol(0)
  , mPiece(Piece::None)
  , mCapturedPiece(Piece::None)
  , mPromotedPiece(Piece::None)
  , mType(Type::Quiet)
  , mEnPassantColumn(8)
  , mCastlingRights(BoardBase::CASTLE_NONE)
{
}

inline Move::Move(uchar sourceRow, uchar sourceCol, uchar destRow, uchar destCol, Piece piece)
  : mSourceRow(sourceRow)
  , mSourceCol(sourceCol)
  , mDestRow(destRow)
  , mDestCol(destCol)
  , mPiece(piece)
  , mCapturedPiece(Piece::None)
  , mPromotedPiece(Piece::None)
  , mType(Type::Quiet)
  , mEnPassantColumn(8)
  , mCastlingRights(BoardBase::CASTLE_NONE)
{
}

inline Piece Move::capturePiece() const
{
  return mCapturedPiece;
}

inline uchar Move::castlingRights() const
{
  return mCastlingRights;
}

inline uchar Move::destCol() const
{
  return mDestCol;
}

inline uchar Move::destRow() const
{
  return mDestRow;
}

inline uchar Move::enPassantCol() const
{
  return mEnPassantColumn;
}

inline Move::Type Move::type() const
{
  return mType;
}

inline uint Move::fullMoveCounter() const
{
  return mFullMoveCounter;
}

inline uint Move::halfMoveClock() const
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

inline bool Move::isEpPush() const
{
  return (mType == Type::EpPush);
}

inline bool Move::isEpCapture() const
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

inline Piece Move::piece() const
{
  return mPiece;
}

inline Piece Move::promotedPiece() const
{
  return mPromotedPiece;
}

inline uchar Move::sourceCol() const
{
  return mSourceCol;
}

inline uchar Move::sourceRow() const
{
  return mSourceRow;
}

inline void Move::setCapturedPiece(Piece piece)
{
  mCapturedPiece = piece;
}

inline void Move::setCastlingRights(uchar castling)
{
  mCastlingRights = castling;
}

inline void Move::setDestCol(uchar col)
{
  mDestCol = col;
}

inline void Move::setDestRow(uchar row)
{
  mDestRow = row;
}

inline void Move::setEnpassantColumn(uchar col)
{
  mEnPassantColumn = col;
}

inline void Move::setType(Move::Type flags)
{
  mType = flags;
}

inline void Move::setFullMoveCounter(uint value)
{
  mFullMoveCounter = value;
}

inline void Move::setHalfMoveClock(uint value)
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

inline void Move::setSourceCol(uchar col)
{
  mSourceCol = col;
}

inline void Move::setSourceRow(uchar row)
{
  mSourceRow = row;
}

#endif // MOVE_H
