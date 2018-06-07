#include "game.h"

#include "board.h"
#include "engine.h"
#include "move.h"
#include "movelist.h"

#include <iostream>

static const double KnightSquareValue[64] =
{
   0,  5,  5, 10, 10,  5,  5,  0,
   0,  5, 10, 20, 20, 10,  5,  0,
   0, 10, 20, 30, 30, 20, 10,  0,
   0, 10, 20, 50, 50, 20, 10,  0,
   0, 10, 20, 50, 50, 20, 10,  0,
   0, 10, 20, 30, 30, 20, 10,  0,
   0,  5, 10, 20, 20, 10,  5,  0,
   0,  5,  5, 10, 10,  5,  5,  0
};

static const std::string START_FEN_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

Game::Game(Board * board, Engine * engine)
  : mBoard(board)
  , mEngine(engine)
  , mCompletedMoves(new MoveList)
{
  restart();
}

Game::~Game()
{

}

Board * Game::board()
{
  return mBoard;
}

const Board * Game::board() const
{
  return mBoard;
}

void Game::checkForMate()
{
  // Generate moves
  MoveList moveList;
  mBoard->generateMoves(moveList);

  // Check for legal moves
  uchar legalMoves = 0;
  for (uchar i = 0; i < moveList.size(); i++) {
    mBoard->makeMove(moveList[i]);
    uchar kingRow = mBoard->kingRow(mBoard->sideToMove());
    uchar kingCol = mBoard->kingColumn(mBoard->sideToMove());
    if (!mBoard->isCellAttacked(kingRow, kingCol, !mBoard->sideToMove()))
      legalMoves++;
    mBoard->unmakeMove(moveList[i]);
  }

  // Reset variables
  mBlackCheckmate = mWhiteCheckmate = mStalemate = false;

  uchar blackKingRow = mBoard->kingRow(Color::Black);
  uchar blackKingCol = mBoard->kingColumn(Color::Black);
  uchar whiteKingRow = mBoard->kingRow(Color::White);
  uchar whiteKingCol = mBoard->kingColumn(Color::White);

  // Check for checkmate
  if (legalMoves == 0) {
    if (mBoard->isWhiteToMove() && mBoard->isCellAttacked(whiteKingRow, whiteKingCol, Color::White))
      mWhiteCheckmate = true;
    else if (mBoard->isBlackToMove() && mBoard->isCellAttacked(blackKingRow, blackKingCol, Color::Black))
      mBlackCheckmate = true;
  }

  if (mWhiteCheckmate || mBlackCheckmate)
    return;

  // Check for stalemate
  if (legalMoves == 0) {
    if (mBoard->isWhiteToMove() && !mBoard->isCellAttacked(whiteKingRow, whiteKingCol, Color::White))
      mStalemate = true;
    if (mBoard->isBlackToMove() && !mBoard->isCellAttacked(blackKingRow, blackKingCol, Color::Black))
      mStalemate = true;
  }

  if (mBoard->halfMoveClock() > 50) {
    mStalemate = true;
  }
}

void Game::doMove(const Move &move)
{
  if (isOver())
    return;

  mBoard->makeMove(move);
  //checkForMate();
  mCompletedMoves->addMove(move);
}

Engine * Game::engine()
{
  return mEngine;
}

const Engine * Game::engine() const
{
  return mEngine;
}

bool Game::isBlackCheckmate() const
{
  return mBlackCheckmate;
}

bool Game::isOver() const
{
  return (mBlackCheckmate || mWhiteCheckmate || mStalemate);
}

bool Game::isStalemate() const
{
  return mStalemate;
}

bool Game::isWhiteCheckmate() const
{
  return mWhiteCheckmate;
}

void Game::restart()
{
  mBlackCheckmate = mWhiteCheckmate = mStalemate = false;
  mBoard->setPosition(START_FEN_POS);
  mCompletedMoves->clear();
}

void Game::setPosition(const std::string &fenString)
{
  mBlackCheckmate = mWhiteCheckmate = mStalemate = false;
  mBoard->setPosition(fenString);
  mCompletedMoves->clear();
}

void Game::undoLastMove()
{
  uchar totalMoves = mCompletedMoves->size();
  if (totalMoves == 0)
    return;

  Move move = mCompletedMoves->moveAt(totalMoves-1);
  mBoard->unmakeMove(move);
  mCompletedMoves->removeLast();
  //checkForMate();
}

void Game::setBoard(Board *board)
{
  mBoard = board;
}

void Game::setEngine(Engine *engine)
{
  mEngine = engine;
}
