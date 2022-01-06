#include "piece.hpp"
#include <string>

Piece char_to_piece(const char c) {
  if (c == 'r') {
    return BlackRook;
  } else if (c == 'n') {
    return BlackKnight;
  } else if (c == 'b') {
    return BlackBishop;
  } else if (c == 'q') {
    return BlackQueen;
  } else if (c == 'k') {
    return BlackKing;
  } else if (c == 'p') {
    return BlackPawn;
  } else if (c == 'R') {
    return WhiteRook;
  } else if (c == 'N') {
    return WhiteKnight;
  } else if (c == 'B') {
    return WhiteBishop;
  } else if (c == 'Q') {
    return WhiteQueen;
  } else if (c == 'K') {
    return WhiteKing;
  } else if (c == 'P') {
    return WhitePawn;
  }
  return InvalidPiece;
}

// NOTE: If you change the numbers for piece, don't forget to change the values
// here
char char_from_piece(const Piece piece) {
  static std::string lookup_table = "PNBRQK  pnbrqk  ";
  return lookup_table[static_cast<int>(piece)];
}
