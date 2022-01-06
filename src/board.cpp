#include "board.hpp"
#include "utils.hpp"
#include <sstream>
#include <string>
#include <vector>

Board::Board(const std::string &fen) {
  std::vector<std::string> tokens = split_string(fen, ' ');
  std::string fen_pieces = remove_char(tokens[0], '/');

  int square = 0;
  for (char c : fen_pieces) {
    if ('1' <= c && c <= '8') {
      int num = c - '0';
      for (int i = 0; i < num; i++) {
        pieces[square] = InvalidPiece;
        square++;
      }
    } else if (char_to_piece(c) != InvalidPiece) {
      pieces[square] = char_to_piece(c);
      square++;
    }
  }

  if (tokens[1] == "w") {
    side_to_move = White;
  } else {
    side_to_move = Black;
  }

  castle_perms = 0;

  for (char c : tokens[2]) {
    if (c == 'K') {
      castle_perms += WhiteShort;
    } else if (c == 'Q') {
      castle_perms += WhiteLong;
    } else if (c == 'k') {
      castle_perms += BlackShort;
    } else if (c == 'q') {
      castle_perms += BlackLong;
    }
  }

  if (tokens[3] == "-") {
    en_passant = InvalidSquare;
  } else {
    en_passant = string_to_square(tokens[3]);
  }

  fifty_move = stoi(tokens[4]);

  full_move = stoi(tokens[5]);
}

std::string Board::to_fen() const {
  std::stringstream result;

  int num_empty = 0;
  for (int square = 0; square < 64; ++square) {
    if (square > 0 && square % 8 == 0) {
      if (num_empty > 0) {
        result << num_empty;
        num_empty = 0;
      }
      result << "/";
    }
    const Piece piece = pieces[square];
    if (piece == InvalidPiece) {
      num_empty++;
    } else {
      if (num_empty > 0) {
        result << num_empty;
        num_empty = 0;
      }
      result << char_from_piece(piece);
    }
  }
  if (num_empty != 0)
    result << num_empty;

  result << ' ';
  result << (side_to_move == White ? 'w' : 'b');
  result << ' ';

  if (castle_perms == 0) {
    result << '-';
  } else {
    if (castle_perms & WhiteShort)
      result << 'K';
    if (castle_perms & WhiteLong)
      result << 'Q';
    if (castle_perms & BlackShort)
      result << 'k';
    if (castle_perms & BlackLong)
      result << 'q';
  }

  result << ' ';
  result << string_from_square(en_passant);
  result << ' ';
  result << fifty_move;
  result << ' ';
  result << full_move;
  return result.str();
}

void Board::print_board() const {
  std::string border = "   +---+---+---+---+---+---+---+---+";
  std::stringstream temp;
  int counter = 8;
  for (int i = 0; i < 64; i++) {
    if (i % 8 == 0) {
      temp << '\n';
      temp << border;
      temp << '\n';
      temp << ' ';
      temp << counter;
      temp << ' ';
      temp << '|';
      counter--;
    }

    temp << ' ';
    temp << char_from_piece(pieces[i]);
    temp << ' ';
    temp << '|';
  }
  temp << '\n';
  temp << border;
  temp << '\n';
  temp << "     a   b   c   d   e   f   g   h";
  temp << '\n';
  std::cout << temp.str() << std::endl;
}

/*
Colour side_to_move;
Piece pieces[64];
int castle_perms;
Square en_passant;
int fifty_move;
int full_move;
*/

/*
std::string Board::to_fen() const {}
std::vector<Move> Board::generate_legal_moves() const;
void Board::make_move(Move move);
void Board::unmake_move();
int Board::static_evaluation() const;
GameResult Board::get_game_state() const;
void Board::print_board() const;

*/
