
#include "board.hpp"

#include "utils.hpp"

#include <array>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

Board::Board(const std::string &fen) {
  const std::vector<std::string> tokens = split_string(fen, ' ');
  const std::string fen_pieces = remove_char(tokens[0], '/');
  const std::string side_to_move_str = tokens[1], castle_perms_str = tokens[2],
                    en_passant_str = tokens[3], fifty_move_str = tokens[4],
                    full_move_str = tokens[5];

  int square = 0;
  for (const char c : fen_pieces) {
    if ('1' <= c && c <= '8') {
      const int num = c - '0';
      for (int i = 0; i < num; i++) {
        pieces[square++] = InvalidPiece;
      }
    } else if (c != '/') {
      pieces[square++] = char_to_piece(c);
    }
  }

  side_to_move = side_to_move_str == "w" ? White : Black;

  castle_perms = 0;
  for (const char c : castle_perms_str) {
    switch (c) {
    case 'K':
      castle_perms |= WhiteShort;
      break;
    case 'Q':
      castle_perms |= WhiteLong;
      break;
    case 'k':
      castle_perms |= BlackShort;
      break;
    case 'q':
      castle_perms |= BlackLong;
      break;
    default:
      break;
    }
  }

  en_passant = string_to_square(en_passant_str);
  fifty_move = stoi(fifty_move_str);
  full_move = stoi(full_move_str);
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
  const static std::string border = "   +---+---+---+---+---+---+---+---+";
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
  temp << '\n' << '\n';
  temp << "FEN: " << to_fen() << std::endl;
  std::cout << temp.str() << std::endl;
}

// Is the given square attacked by the given side?
bool Board::is_square_attacked(const Square sq, const Colour side) const {
  const int file = square_file(sq);
  const int rank = square_rank(sq);

  if (piece_colour(pieces[sq]) == side)
    return false;

  // Pawns
  const int capture_left = side == White ? 7 : -9;
  const int capture_right = side == White ? 9 : -7;
  const Piece side_pawn = side == White ? WhitePawn : BlackPawn;

  // TODO: Check if these are correct
  if (file > 0 && rank > 1 && pieces[sq + capture_left] == side_pawn) {
    return true;
  }
  if (file < 7 && rank > 1 && pieces[sq + capture_right] == side_pawn) {
    return true;
  }

  // Knights

  const Piece side_knight = side == White ? WhiteKnight : BlackKnight;
  const static std::array<std::pair<int, int>, 8> knight_offsets = {
      std::make_pair(1, 2),   std::make_pair(2, 1),  std::make_pair(-1, -2),
      std::make_pair(-2, -1), std::make_pair(1, -2), std::make_pair(-1, 2),
      std::make_pair(-2, 1),  std::make_pair(2, -1)};
  for (const auto &[dx, dy] : knight_offsets) {
    const int new_file = file + dx;
    const int new_rank = rank + dy;
    if (0 <= new_file && new_file <= 7 && 0 <= new_rank && new_rank <= 7) {
      const int target_sq = square_from_file_rank(new_file, new_rank);
      if (pieces[target_sq] == side_knight)
        return true;
    }
  }

  // King
  const Piece side_king = side == White ? WhiteKing : BlackKing;
  const static std::array<std::pair<int, int>, 4> diagonal_offsets = {
      std::make_pair(-1, 1), std::make_pair(-1, -1), std::make_pair(1, -1),
      std::make_pair(1, 1)};
  const static std::array<std::pair<int, int>, 4> ortho_offsets = {
      std::make_pair(-1, 0), std::make_pair(1, 0), std::make_pair(0, -1),
      std::make_pair(0, 1)};

  for (const auto &[dx, dy] : diagonal_offsets) {
    const int new_file = file + dx;
    const int new_rank = rank + dy;
    if (0 <= new_file && new_file <= 7 && 0 <= new_rank && new_rank <= 7) {
      const int target_sq = square_from_file_rank(new_file, new_rank);
      if (pieces[target_sq] == side_king)
        return true;
    }
  }
  for (const auto &[dx, dy] : ortho_offsets) {
    const int new_file = file + dx;
    const int new_rank = rank + dy;
    if (0 <= new_file && new_file <= 7 && 0 <= new_rank && new_rank <= 7) {
      const int target_sq = square_from_file_rank(new_file, new_rank);
      if (pieces[target_sq] == side_king)
        return true;
    }
  }

  // Slidey ortho pieces
  const Piece side_bishop = side == White ? WhiteBishop : BlackBishop;
  const Piece side_rook = side == White ? WhiteRook : BlackRook;
  const Piece side_queen = side == White ? WhiteQueen : BlackQueen;

  for (const auto &[dx, dy] : ortho_offsets) {
    int cur_file = file + dx, cur_rank = rank + dy;
    while (true) {
      if (!(0 <= cur_file && cur_file <= 7 && 0 <= cur_rank && cur_rank <= 7))
        break;
      const int target_sq = square_from_file_rank(cur_file, cur_rank);
      const Piece target_piece = pieces[target_sq];
      if (piece_colour(target_piece) == side &&
          (target_piece == side_rook || target_piece == side_queen)) {
        return true;
      } else if (target_piece == InvalidPiece) {
        cur_file += dx;
        cur_rank += dy;
      } else { // Encountered own coloured piece
        break;
      }
    }
  }

  // Slidey diagonal pieces
  for (const auto &[dx, dy] : diagonal_offsets) {
    int cur_file = file + dx, cur_rank = rank + dy;
    while (true) {
      if (!(0 <= cur_file && cur_file <= 7 && 0 <= cur_rank && cur_rank <= 7))
        break;
      const int target_sq = square_from_file_rank(cur_file, cur_rank);
      const Piece target_piece = pieces[target_sq];
      if (piece_colour(target_piece) == side &&
          (target_piece == side_bishop || target_piece == side_queen)) {
        return true;
      } else if (target_piece == InvalidPiece) {
        cur_file += dx;
        cur_rank += dy;
      } else { // Encountered own coloured piece
        break;
      }
    }
  }

  return false;
}

void Board::get_pawn_moves(std::vector<Move> &move_list,
                           const Square location) const {
  // Find start rank and end rank
  const Piece pawn = pieces[location];
  const Colour colour = piece_colour(pawn);
  const int start_rank = colour == White ? 1 : 6;
  const int last_rank = colour == White ? 6 : 1;
  const int this_rank = square_rank(location);
  const int this_file = square_file(location);
  const int forward = colour == White ? -8 : 8;
  const int capture_left = colour == White ? -9 : 7;
  const int capture_right = colour == White ? -7 : 9;
  const Colour opposite_colour = side_to_move == White ? Black : White;
  const Piece opposite_king = side_to_move == White ? BlackKing : WhiteKing;
  assert(piece_colour(pawn) == side_to_move);

  if (this_rank == start_rank) {
    // 2 squares forward
    if (pieces[location + forward] == InvalidPiece &&
        pieces[location + 2 * forward] == InvalidPiece)
      move_list.emplace_back(location, location + 2 * forward, DoublePawn,
                             InvalidPiece, InvalidPiece);
  }
  if (this_rank != last_rank) {
    // 1 square up
    if (pieces[location + forward] == InvalidPiece)
      move_list.emplace_back(location, location + forward, Quiet, InvalidPiece,
                             InvalidPiece);
    if (this_file > 0) {
      // Capture left
      if (piece_colour(pieces[location + capture_left]) == opposite_colour)
        move_list.emplace_back(location, location + capture_left, Capture,
                               InvalidPiece, pieces[location + capture_left]);
      if (location + capture_left == en_passant)
        move_list.emplace_back(location, en_passant, EnPassant, InvalidPiece,
                               pieces[location - 1]);
    }
    if (this_file < 7) {
      // Capture right
      if (piece_colour(pieces[location + capture_right]) == opposite_colour)
        move_list.emplace_back(location, location + capture_left, Capture,
                               InvalidPiece, pieces[location + capture_right]);
      if (location + capture_right == en_passant)
        move_list.emplace_back(location, en_passant, EnPassant, InvalidPiece,
                               pieces[location + 1]);
    }
  } else {
    const Piece my_knight = side_to_move == White ? WhiteKnight : BlackKnight;
    const Piece my_bishop = side_to_move == White ? WhiteBishop : BlackBishop;
    const Piece my_rook = side_to_move == White ? WhiteRook : BlackRook;
    const Piece /* oh no */ my_queen =
        side_to_move == White ? WhiteQueen : BlackQueen;

    // Promotions
    if (pieces[location + forward] == InvalidPiece) {
      move_list.emplace_back(location, location + forward, Promotion, my_knight,
                             InvalidPiece);
      move_list.emplace_back(location, location + forward, Promotion, my_bishop,
                             InvalidPiece);
      move_list.emplace_back(location, location + forward, Promotion, my_rook,
                             InvalidPiece);
      move_list.emplace_back(location, location + forward, Promotion, my_queen,
                             InvalidPiece);
    }
    // Capture promotions
    if (this_file > 0 &&
        piece_colour(pieces[location + capture_left]) == opposite_colour &&
        pieces[location + capture_left] == opposite_king) {
      move_list.emplace_back(location, location + capture_left, CapturePromote,
                             my_knight, pieces[location + capture_left]);
      move_list.emplace_back(location, location + capture_left, CapturePromote,
                             my_bishop, pieces[location + capture_left]);
      move_list.emplace_back(location, location + capture_left, CapturePromote,
                             my_rook, pieces[location + capture_left]);
      move_list.emplace_back(location, location + capture_left, CapturePromote,
                             my_queen, pieces[location + capture_left]);
    }
    if (this_file < 7 &&
        piece_colour(pieces[location + capture_right]) == opposite_colour &&
        pieces[location + capture_right] == opposite_king) {
      move_list.emplace_back(location, location + capture_right, CapturePromote,
                             my_knight, pieces[location + capture_right]);
      move_list.emplace_back(location, location + capture_right, CapturePromote,
                             my_bishop, pieces[location + capture_right]);
      move_list.emplace_back(location, location + capture_right, CapturePromote,
                             my_rook, pieces[location + capture_right]);
      move_list.emplace_back(location, location + capture_right, CapturePromote,
                             my_queen, pieces[location + capture_right]);
    }
  }
}

void Board::get_knight_moves(std::vector<Move> &move_list,
                             const Square location) const {
  const int file = square_file(location);
  const int rank = square_rank(location);
  const static std::array<std::pair<int, int>, 8> knight_offsets = {
      std::make_pair(1, 2),   std::make_pair(2, 1),  std::make_pair(-1, -2),
      std::make_pair(-2, -1), std::make_pair(1, -2), std::make_pair(-1, 2),
      std::make_pair(-2, 1),  std::make_pair(2, -1)};
  for (const auto &[dx, dy] : knight_offsets) {
    const int new_file = file + dx;
    const int new_rank = rank + dy;
    if (0 <= new_file && new_file <= 7 && 0 <= new_rank && new_rank <= 7) {
      const Square to_square = square_from_file_rank(new_file, new_rank);
      const Piece to_piece = pieces[to_square];
      if (to_piece == InvalidPiece) {
        move_list.emplace_back(location, to_square, Quiet, InvalidPiece,
                               InvalidPiece);
      } else if (piece_colour(to_piece) != side_to_move) {
        move_list.emplace_back(location, to_square, Capture, InvalidPiece,
                               to_piece);
      }
    }
  }
}

void Board::piece_move_helper(std::vector<Move> &move_list,
                              const Square location, int t1, int t2) const {
  const int file = square_file(location);
  const int rank = square_rank(location);
  // do all 4 possible diagonals

  bool invalid = false;
  int new_t1 = t1;
  int new_t2 = t2;
  while (!invalid) {
    if ((file + new_t1 < 8) && (rank + new_t2 < 8) && (0 <= file + new_t1) &&
        (0 <= rank + new_t2)) {
      Square target = square_from_file_rank(file + new_t1, rank + new_t2);
      Piece target_piece = pieces[target];
      if (target_piece == InvalidPiece) {
        // if its invalid, add to move list
        move_list.push_back(
            Move(location, target, Quiet, InvalidPiece, InvalidPiece));
        new_t1 += t1;
        new_t2 += t2;
      } else if (piece_colour(target_piece) != side_to_move) {
        move_list.push_back(
            Move(location, target, Capture, InvalidPiece, target_piece));
        invalid = true;
      } else {
        invalid = true;
      }
    } else {
      invalid = true;
    }
  }
}

void Board::get_bishop_moves(std::vector<Move> &move_list,
                             const Square location) const {
  piece_move_helper(move_list, location, 1, 1);
  piece_move_helper(move_list, location, -1, 1);
  piece_move_helper(move_list, location, 1, -1);
  piece_move_helper(move_list, location, -1, -1);
}

void Board::get_rook_moves(std::vector<Move> &move_list,
                           const Square location) const {
  piece_move_helper(move_list, location, 1, 0);
  piece_move_helper(move_list, location, -1, 0);
  piece_move_helper(move_list, location, 0, 1);
  piece_move_helper(move_list, location, 0, -1);
}

void Board::get_queen_moves(std::vector<Move> &move_list,
                            const Square location) const {
  get_bishop_moves(move_list, location);
  get_rook_moves(move_list, location);
}

void Board::get_king_moves(std::vector<Move> &move_list,
                           const Square location) const {
  const int file = square_file(location);
  const int rank = square_rank(location);
  const std::array<std::pair<int, int>, 8> king_offsets = {
      std::make_pair(1, 0),  std::make_pair(1, 1),  std::make_pair(1, -1),
      std::make_pair(0, 1),  std::make_pair(0, -1), std::make_pair(-1, 1),
      std::make_pair(-1, 0), std::make_pair(-1, -1)};

  for (const auto &[dx, dy] : king_offsets) {
    const int new_file = file + dx;
    const int new_rank = rank + dy;
    if (0 <= new_file && new_file <= 7 && 0 <= new_rank && new_rank <= 7) {
      const Square to_square = square_from_file_rank(new_file, new_rank);
      const Piece to_piece = pieces[to_square];
      if (to_piece == InvalidPiece) {
        move_list.emplace_back(location, to_square, Quiet, InvalidPiece,
                               InvalidPiece);
      } else if (piece_colour(to_piece) != side_to_move) {
        move_list.emplace_back(location, to_square, Capture, InvalidPiece,
                               to_piece);
      }
    }
  }

  if (side_to_move == White) {
    if (castle_perms & WhiteShort && (pieces[F1] == InvalidPiece) &&
        (pieces[G1] == InvalidPiece) && !is_square_attacked(E1, Black) &&
        !is_square_attacked(F1, Black) && !is_square_attacked(G1, Black)) {
      move_list.emplace_back(location, G1, ShortCastle, InvalidPiece,
                             InvalidPiece);
    }
    if (castle_perms & WhiteLong && (pieces[B1] == InvalidPiece) &&
        (pieces[C1] == InvalidPiece) && (pieces[D1] == InvalidPiece) &&
        !is_square_attacked(E1, Black) && !is_square_attacked(D1, Black) &&
        !is_square_attacked(C1, Black)) {
      move_list.emplace_back(location, C1, LongCastle, InvalidPiece,
                             InvalidPiece);
    }
  } else {
    if (castle_perms & BlackShort && (pieces[F8] == InvalidPiece) &&
        (pieces[G8] == InvalidPiece) && !is_square_attacked(E8, White) &&
        !is_square_attacked(F8, White) && !is_square_attacked(G8, White)) {
      move_list.emplace_back(location, G8, ShortCastle, InvalidPiece,
                             InvalidPiece);
    }
    if (castle_perms & BlackLong && (pieces[B8] == InvalidPiece) &&
        (pieces[C8] == InvalidPiece) && (pieces[D8] == InvalidPiece) &&
        !is_square_attacked(E8, White) && !is_square_attacked(D8, White) &&
        !is_square_attacked(C8, White)) {
      move_list.emplace_back(location, C8, LongCastle, InvalidPiece,
                             InvalidPiece);
    }
  }
}
/*

For each piece, generate all possible legal moves
- get the piece type
- get white/black side to play
- get special moves

Pawn - N
- 1 or 2 squares forward
- only 2 if it's on the starting rank of it's colour
- En passant
- Promote
- Capture
- Capture & Promote

Knight - N
- 1 square forward or backwards + 2 squares left or right
- 2 squares forward or backwawrds + 1 square left or right
- Capture

Bishop - S
- x squares forward/backwards + x squares left/right
- Capture

Rook - S
- 1-7 squares forward/backwards or 1-7 squares left/right
- Capture

Queen - N
- Bishop + Rook

King - S
- 1 square forward/backwards or 1 square left/right or both
- Capture
- LongCastle
- ShortCastle

*/

std::vector<Move> Board::generate_pseudo_legal_moves() const {
  std::vector<Move> result;
  for (int sq = 0; sq < 64; ++sq) {
    const Square square = static_cast<Square>(sq);
    const Piece piece = pieces[sq];
    if (piece_colour(piece) != side_to_move)
      continue;
    switch (piece) {
    case WhiteKing:
    case BlackKing:
      get_king_moves(result, square);
      break;
    case WhitePawn:
    case BlackPawn:
      get_pawn_moves(result, square);
      break;
    case WhiteKnight:
    case BlackKnight:
      get_knight_moves(result, square);
      break;
    case WhiteBishop:
    case BlackBishop:
      get_bishop_moves(result, square);
      break;
    case WhiteRook:
    case BlackRook:
      get_rook_moves(result, square);
      break;
    case WhiteQueen:
    case BlackQueen:
      get_queen_moves(result, square);
      break;
    default:
      __builtin_unreachable();
    }
  }
  return result;
}

std::vector<Move> Board::generate_legal_moves() const {
  // TODO:
  return {};
}

void Board::make_move(Move move) {
  switch (move.type) {
  case Quiet:
    move_piece(move.from, move.to);
    break;
  case Capture:
    remove_piece(move.to);
    move_piece(move.from, move.to);
    break;
  case Promotion:
    remove_piece(move.from);
    add_piece(move.to, move.promotion_piece);
    break;
  case CapturePromote:
    remove_piece(move.from);
    remove_piece(move.to);
    add_piece(move.to, move.promotion_piece);
    break;
  case EnPassant:
    move_piece(move.from, move.to);
    remove_piece(get_en_passant_capture(en_passant, side_to_move));
    en_passant = InvalidSquare;
    break;
  case LongCastle:
    if (side_to_move == White) {
      move_piece(move.from, move.to);
      move_piece(A1, D1);
    } else {
      move_piece(move.from, move.to);
      move_piece(A8, D8);
    }
    break;
  case ShortCastle:
    if (side_to_move == White) {
      move_piece(move.from, move.to);
      move_piece(H1, F1);
    } else {
      move_piece(move.from, move.to);
      move_piece(H8, F8);
    }
    break;
  case DoublePawn:
    move_piece(move.from, move.to);
    if (side_to_move == White) {
      en_passant = static_cast<Square>(static_cast<int>(move.to) + 8);
    } else {
      en_passant = static_cast<Square>(static_cast<int>(move.to) - 8);
    }
  }
  side_to_move = (side_to_move == White) ? Black : White;
  fifty_move++;
  if (side_to_move == Black)
    full_move++;
}

/*
Square from;
Square to;
MoveType type;
Piece promotion_piece;
Piece captured_piece;
*/

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
