#include "fen.h"

#include "util.h"

#include <map>
#include <sstream>

Fen::Fen()
{
  init();
}

bool Fen::canBlackCastleKingSide() const
{
  return mBlackCastleKingSide;
}

bool Fen::canBlackCastleQueenSide() const
{
  return mBlackCastleQueenSide;
}

bool Fen::canWhiteCastleKingSide() const
{
  return mWhiteCastleKingSide;
}

bool Fen::canWhiteCastleQueenSide() const
{
  return mWhiteCastleQueenSide;
}

int Fen::epColumn() const
{
  return mEnPassantCol;
}

uint Fen::fullMoveCounter() const
{
  return mFullMoveCounter;
}

uint Fen::halfMoveClock() const
{
  return mHalfMoveClock;
}

void Fen::init()
{
  static const PieceType whitePieces[] =
  {
    PieceType::WhiteRook, PieceType::WhiteKnight, PieceType::WhiteBishop, PieceType::WhiteQueen,
    PieceType::WhiteKing, PieceType::WhiteBishop, PieceType::WhiteKnight, PieceType::WhiteRook
  };

  static const PieceType blackPieces[] =
  {
    PieceType::BlackRook, PieceType::BlackKnight, PieceType::BlackBishop, PieceType::BlackQueen,
    PieceType::BlackKing, PieceType::BlackBishop, PieceType::BlackKnight, PieceType::BlackRook
  };

  mWhiteCastleKingSide = mWhiteCastleQueenSide = mBlackCastleKingSide = mBlackCastleQueenSide = true;
  mWhiteToMove = true;
  mEnPassantCol = -1;
  mHalfMoveClock = mFullMoveCounter = 0;

  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      mPieceType[row][col] = PieceType::None;

      if (row == 0) {
        mPieceType[row][col] = whitePieces[col];
      }

      if (row == 1) {
        mPieceType[row][col] = PieceType::WhitePawn;
      }

      if (row == 6) {
        mPieceType[row][col] = PieceType::BlackPawn;
      }

      if (row == 7) {
        mPieceType[row][col] = blackPieces[col];
      }
    }
  }
}

PieceType Fen::pieceType(uchar row, uchar col) const
{
  return mPieceType[row][col];
}

void Fen::readString(const std::string &fenString)
{
  typedef std::map<char, PieceType> PieceMap;
  typedef std::map<char, int> EnPassantMap;

  static PieceMap pieceTypes;
  static EnPassantMap enPassantMap;
  if (pieceTypes.size() == 0 ) {
    pieceTypes['p'] = PieceType::BlackPawn;
    pieceTypes['r'] = PieceType::BlackRook;
    pieceTypes['n'] = PieceType::BlackKnight;
    pieceTypes['b'] = PieceType::BlackBishop;
    pieceTypes['q'] = PieceType::BlackQueen;
    pieceTypes['k'] = PieceType::BlackKing;
    pieceTypes['P'] = PieceType::WhitePawn;
    pieceTypes['R'] = PieceType::WhiteRook;
    pieceTypes['N'] = PieceType::WhiteKnight;
    pieceTypes['B'] = PieceType::WhiteBishop;
    pieceTypes['Q'] = PieceType::WhiteQueen;
    pieceTypes['K'] = PieceType::WhiteKing;

    enPassantMap['a'] = 0;
    enPassantMap['b'] = 1;
    enPassantMap['c'] = 2;
    enPassantMap['d'] = 3;
    enPassantMap['e'] = 4;
    enPassantMap['f'] = 5;
    enPassantMap['g'] = 6;
    enPassantMap['h'] = 7;
  }

  init();

  std::string fs(fenString);
  util::trim(fs);

  // Split the distinct fen tokens
  std::vector<std::string> fenTokens;
  util::split(fenTokens, fs, ' ');

  // There must be at least four fields
  if (fenTokens.size() < 4) {
    std::ostringstream oss;
    oss << "Invalid number of tokens in fen string\n";
    oss << "Fen String: " << fenString << "\n";
    oss << "Number of tokens found: " << fenTokens.size() << "\n";
    throw std::invalid_argument(oss.str());
  }

  // Validate the piece string
  std::string pieceString = util::trim(fenTokens.at(0));
  if (pieceString.find_first_not_of("/12345678PNBRQKpnbrkq") != std::string::npos) {
    std::ostringstream oss;
    oss << "Error in piece specification\n";
    throw std::invalid_argument(oss.str());
  }

  // Validate the side string
  std::string sideString = util::trim(fenTokens.at(1));
  if (sideString.find_first_not_of("wb") != std::string::npos) {
    std::ostringstream oss;
    oss << "Error in side specification\n";
    throw std::invalid_argument(oss.str());
  }

  // Validate the castle string
  std::string castleString = util::trim(fenTokens.at(2));
  if (castleString.find_first_not_of("-KQkq") != std::string::npos) {
    std::ostringstream oss;
    oss << "Error in castle specification\n";
    throw std::invalid_argument(oss.str());
  }

  // Validate the en passant string
  std::string enPassantString = util::trim(fenTokens.at(3));
  if (enPassantString.find_first_not_of("-abcdefgh36") != std::string::npos) {
    std::ostringstream oss;
    oss << "Error in en passant specification\n";
    throw std::invalid_argument(oss.str());
  }

  // Validate the half move counter
  uint halfMoveClock = 0;
  if (fenTokens.size() > 4) {
    std::string halfMoveString = util::trim(fenTokens.at(4));
    if (halfMoveString.find_first_not_of("0123456789") != std::string::npos) {
      std::ostringstream oss;
      oss << "Invalid half move clock specification\n";
      throw std::invalid_argument(oss.str());
    }
    halfMoveClock = atoi(halfMoveString.c_str());
  }

  // Validate the full move counter
  uint fullMoveCounter = 0;
  if (fenTokens.size() > 5) {
    std::string fullMoveString = util::trim(fenTokens.at(5));
    if (fullMoveString.find_first_not_of("0123456789") != std::string::npos) {
      std::ostringstream oss;
      oss << "Invalid full move counter specification\n";
      throw std::invalid_argument(oss.str());
    }
    fullMoveCounter = atoi(fullMoveString.c_str());
  }

  // Split the piece string into rank strings
  std::vector<std::string> tokens;
  util::split(tokens, pieceString, '/');
  if (tokens.size() < 8) {
    std::ostringstream oss;
    oss << "Invalid piece string\n";
    oss << tokens.size() << "\n";
    throw std::invalid_argument(oss.str());
  }

  for (uint row = 0; row < 8; row++) {
    for (uint col = 0; col < 8; col++) {
      mPieceType[row][col] = PieceType::None;
    }
  }

  // Parse each rank string
  for (int rank = 7; rank >= 0; rank--) {
    std::string rankString = tokens.at(7-rank);
    int currentIndex = -1;
    uint file = 0;
    while (++currentIndex < rankString.length()) {
      char c = rankString.at(currentIndex);
      PieceMap::iterator itr = pieceTypes.find(c);

      // If a piece character was found
      // set the associated piece type
      if (itr != pieceTypes.end()) {
        mPieceType[rank][file] = itr->second;
        ++file;
      }
      // Skip the correct number of files for numbers
      else {
        int number = c - '0';
        file += number;
      }
    }
  }

  // Set side to move
  mWhiteToMove = true;
  if (sideString == "b")
    mWhiteToMove = false;

  // Parse the castle string
  mWhiteCastleKingSide = (castleString.find("K") != std::string::npos);
  mWhiteCastleQueenSide = (castleString.find("Q") != std::string::npos);
  mBlackCastleKingSide = (castleString.find("k") != std::string::npos);
  mBlackCastleQueenSide = (castleString.find("q") != std::string::npos);

  mEnPassantCol = -1;
  if (enPassantString.size() > 1) {
    mEnPassantCol = enPassantMap[enPassantString.at(0)];
  }

  mHalfMoveClock = halfMoveClock;
  mFullMoveCounter = fullMoveCounter;
}

void Fen::setCanBlackCastleKingSide(bool castle)
{
  mBlackCastleKingSide = castle;
}

void Fen::setCanBlackCastleQueenSide(bool castle)
{
  mBlackCastleQueenSide = castle;
}

void Fen::setCanWhiteCastleKingSide(bool castle)
{
  mWhiteCastleKingSide = castle;
}

void Fen::setCanWhiteCastleQueenSide(bool castle)
{
  mWhiteCastleQueenSide = castle;
}

void Fen::setEpColumn(int column)
{
  mEnPassantCol = column;
}

void Fen::setFullMoveCounter(uint value)
{
  mFullMoveCounter = value;
}

void Fen::setHalfMoveClock(uint value)
{
  mHalfMoveClock = value;
}

void Fen::setPieceType(uchar row, uchar col, PieceType pieceType)
{
  mPieceType[row][col] = pieceType;
}

void Fen::setWhiteToMove(bool whiteToMove)
{
  mWhiteToMove = whiteToMove;
}

bool Fen::whiteToMove() const
{
  return mWhiteToMove;
}
