#pragma once
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>

enum Colour { White = 1, Black = -1, Invalid = 0 };

enum CastlePerms {
  WhiteShort = 1,
  WhiteLong = 2,
  BlackShort = 4,
  BlackLong = 8
};

enum GameResult {
  NotOver = 0,
  // white wins
  WhiteCheckmate,
  BlackResigned,
  // black wins
  BlackCheckmate,
  WhiteResigned,
  // draw
  Stalemate,
  Threefold,
  InsufficientMaterial,
  FiftyMoveRule,
  Draw
};

class Board {
  Colour side_to_move;
  Piece pieces[64];
  int castle_perms;
  Square en_passant;
  int fifty_move;
  int full_move;

public:
  Board(const std::string &fen);
  std::string to_fen() const;
  std::vector<Move> generate_legal_moves() const;
  void make_move(Move move);
  void unmake_move();
  int static_evaluation() const;
  GameResult get_game_state() const;
  void print_board() const;
};
