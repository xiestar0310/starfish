
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
  square_t from;
  square_t to;
  MoveType type;
  Piece promotion_piece;
  Piece captured_piece;

public:
  Move()
      : from(InvalidSquare), to(InvalidSquare), type(Quiet),
        promotion_piece(InvalidPiece), captured_piece(InvalidPiece) {}
  Move(const square_t from, const square_t to, const MoveType type,
       const Piece promotion_piece, const Piece captured_piece)
      : from(from), to(to), type(type), promotion_piece(promotion_piece),
        captured_piece(captured_piece) {}
};
