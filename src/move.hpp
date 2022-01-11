
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
  Square from;
  Square to;
  MoveType type;
  Piece promotion_piece;
  Piece captured_piece;

public:
  Move()
      : from(InvalidSquare), to(InvalidSquare), type(Quiet),
        promotion_piece(InvalidPiece), captured_piece(InvalidPiece) {}
  Move(const Square from, const Square to, const MoveType type,
       const Piece promotion_piece, const Piece captured_piece)
      : from(from), to(to), type(type), promotion_piece(promotion_piece),
        captured_piece(captured_piece) {}
  Move(const int from, const int to, const MoveType type,
       const Piece promotion_piece, const Piece captured_piece)
      : from(static_cast<Square>(from)), to(static_cast<Square>(to)),
        type(type), promotion_piece(promotion_piece),
        captured_piece(captured_piece) {}
};
