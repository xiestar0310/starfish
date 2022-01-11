
#pragma once

#include "piece.hpp"
#include "square.hpp"

enum MoveType {
  ShortCastle,
  LongCastle,
  EnPassant,
  Promotion,
  DoublePawn,
  Capture,
  CapturePromote,
  Quiet
};

struct Move {
  square_t from = InvalidSquare;
  square_t to = InvalidSquare;
  MoveType type = Quiet;
  piece_t promotion_piece = InvalidPiece;
  piece_t captured_piece = InvalidPiece;

public:
  Move(const square_t from, const square_t to, const MoveType type,
       const piece_t promotion_piece, const piece_t captured_piece)
      : from(from), to(to), type(type), promotion_piece(promotion_piece),
        captured_piece(captured_piece) {}
};
