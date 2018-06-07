#ifndef GAME_H
#define GAME_H

#include "board.h"

#include <memory>
#include <string>

//class Board;
class Engine;
class Move;
class MoveList;

class Game
{
public:
  Game(Board * board, Engine * engine);
  virtual ~Game();

public:
  Board * board();
  Engine * engine();
  const Board * board() const;
  const Engine * engine() const;
  bool isBlackCheckmate() const;
  bool isOver() const;
  bool isStalemate() const;
  bool isWhiteCheckmate() const;
  void setBoard(Board * board);
  void setEngine(Engine * engine);

public:
  void doMove(const Move & move);
  void undoLastMove();
  void restart();
  void setPosition(const std::string & fenString);

private:
  void checkForMate();

private:
  bool mBlackCheckmate;
  bool mStalemate;
  bool mWhiteCheckmate;
  Board * mBoard;
  Engine * mEngine;
  std::unique_ptr<MoveList> mCompletedMoves;
};

#endif // #ifndef GAME_H
