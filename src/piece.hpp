
#pragma once
#include "colour.hpp"

// NOTE: If you change the numbers for piece, don't forget to change the values
// here
enum Piece {
  WhitePawn = 0,
  WhiteKnight = 1,
  WhiteBishop = 2,
  WhiteRook = 3,
  WhiteQueen = 4,
  WhiteKing = 5,

  BlackPawn = 8,
  BlackKnight = 9,
  BlackBishop = 10,
  BlackRook = 11,
  BlackQueen = 12,
  BlackKing = 13,

  InvalidPiece = 15,
};
using piece_t = int;

constexpr piece_t char_to_piece(const char c) {
  const char *lookup_table = "PNBRQK  pnbrqk  ";
  for (piece_t piece = 0; piece < 16; ++piece) {
    if (c == lookup_table[piece])
      return piece;
  }
  return InvalidPiece;
}

// NOTE: If you change the numbers for piece, don't forget to change the values
// here
constexpr char char_from_piece(const piece_t piece) {
  const char *lookup_table = "PNBRQK  pnbrqk  ";
  return lookup_table[piece];
}

constexpr colour_t piece_colour(const piece_t piece) {
  if (piece == InvalidPiece)
    return InvalidColour;
  return (piece < 8) ? White : Black;
}
