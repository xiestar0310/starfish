
#pragma once

#include "colour.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"
#include "utils.hpp"

#include <iostream>
#include <string>

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
  colour_t side_to_move;
  piece_t pieces[64];
  int castle_perms;
  square_t en_passant;
  int fifty_move;
  int full_move;

public:
  constexpr static const char *start_fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

public:
  Board(const std::string &fen = start_fen);
  std::string to_fen() const;
  std::vector<Move> generate_pseudo_legal_moves() const;
  std::vector<Move> generate_legal_moves() const;
  bool make_move(const Move move);
  void unmake_move();
  int static_evaluation() const;
  GameResult get_game_state() const;
  void print_board() const;
  square_t king_square(const colour_t side) const;
  bool is_square_attacked(const square_t sq, const colour_t side) const;

  static inline square_t get_en_passant_capture(const square_t en_passant,
                                                const colour_t side_to_move) {
    return en_passant - 8 * side_to_move;
  }
  inline void add_piece(const square_t add, const piece_t piece) {
    pieces[add] = piece;
  }
  inline void remove_piece(const square_t remove) {
    pieces[remove] = InvalidPiece;
  }
  inline void move_piece(const square_t from, const square_t to) {
    const piece_t piece = pieces[from];
    remove_piece(from);
    add_piece(to, piece);
  }

private:
  void get_pawn_moves(std::vector<Move> &move_list,
                      const square_t location) const;
  void get_knight_moves(std::vector<Move> &move_list,
                        const square_t location) const;
  void piece_move_helper(std::vector<Move> &move_list, const square_t location,
                         int t1, int t2) const;
  void get_bishop_moves(std::vector<Move> &move_list,
                        const square_t location) const;
  void get_rook_moves(std::vector<Move> &move_list,
                      const square_t location) const;
  void get_queen_moves(std::vector<Move> &move_list,
                       const square_t location) const;
  void get_king_moves(std::vector<Move> &move_list,
                      const square_t location) const;
};
