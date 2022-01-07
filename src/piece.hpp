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

Piece char_to_piece(const char c);
char char_from_piece(const Piece piece);
Colour piece_colour(const Piece piece);
