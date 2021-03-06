
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

  square_t square = 0;
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
  for (square_t square = 0; square < 64; ++square) {
    if (square > 0 && square % 8 == 0) {
      if (num_empty > 0) {
        result << num_empty;
        num_empty = 0;
      }
      result << "/";
    }
    const piece_t piece = pieces[square];
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
  int rank = 8;
  for (square_t sq = 0; sq < 64; sq++) {
    if (sq % 8 == 0) {
      temp << "\n"
           << border << "\n"
           << " " << rank << " |";
      rank--;
    }
    temp << " " << char_from_piece(pieces[sq]) << " |";
  }
  temp << "\n"
       << border << "\n"
       << "     a   b   c   d   e   f   g   h"
       << "\n\n";
  temp << "FEN: " << to_fen() << std::endl;
  std::cout << temp.str() << std::endl;
}

// Is the given square attacked by the given side?
bool Board::is_square_attacked(const square_t sq, const colour_t side) const {
  if (piece_colour(pieces[sq]) == side)
    return false;

  const int file = square_file(sq);
  const int rank = square_rank(sq);

  // Pawns
  const int capture_left = side == White ? 7 : -9;
  const int capture_right = side == White ? 9 : -7;
  const piece_t side_pawn = side == White ? WhitePawn : BlackPawn;

  // TODO: Check if these are correct
  if (file > 0 && 0 <= sq + capture_left && sq + capture_left < 64 &&
      pieces[sq + capture_left] == side_pawn) {
    return true;
  }
  if (file < 7 && 0 <= sq + capture_right && sq + capture_right < 64 &&
      pieces[sq + capture_right] == side_pawn) {
    return true;
  }

  // Knights
  const piece_t side_knight = side == White ? WhiteKnight : BlackKnight;
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
  const piece_t side_king = side == White ? WhiteKing : BlackKing;
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
  const piece_t side_bishop = side == White ? WhiteBishop : BlackBishop;
  const piece_t side_rook = side == White ? WhiteRook : BlackRook;
  const piece_t side_queen = side == White ? WhiteQueen : BlackQueen;

  for (const auto &[dx, dy] : ortho_offsets) {
    int cur_file = file + dx, cur_rank = rank + dy;
    while (true) {
      if (!(0 <= cur_file && cur_file <= 7 && 0 <= cur_rank && cur_rank <= 7))
        break;
      const int target_sq = square_from_file_rank(cur_file, cur_rank);
      const piece_t target_piece = pieces[target_sq];
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
      const piece_t target_piece = pieces[target_sq];
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
                           const square_t location) const {
  // Find start rank and end rank
  const piece_t pawn = pieces[location];
  const colour_t colour = piece_colour(pawn);
  const int start_rank = colour == White ? 1 : 6;
  const int last_rank = colour == White ? 6 : 1;
  const int this_rank = square_rank(location);
  const int this_file = square_file(location);
  const int forward = colour == White ? -8 : 8;
  const int capture_left = colour == White ? -9 : 7;
  const int capture_right = colour == White ? -7 : 9;
  const colour_t opposite_colour = -side_to_move;
  const piece_t opposite_king = side_to_move == White ? BlackKing : WhiteKing;
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
    const piece_t my_knight = side_to_move == White ? WhiteKnight : BlackKnight;
    const piece_t my_bishop = side_to_move == White ? WhiteBishop : BlackBishop;
    const piece_t my_rook = side_to_move == White ? WhiteRook : BlackRook;
    const piece_t /* oh no */ my_queen =
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
                             const square_t location) const {
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
      const square_t to_square = square_from_file_rank(new_file, new_rank);
      const piece_t to_piece = pieces[to_square];
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

void Board::get_slide_move_helper(std::vector<Move> &move_list,
                                  const square_t location, const int dx,
                                  const int dy) const {
  const int file = square_file(location);
  const int rank = square_rank(location);

  int new_dx = dx;
  int new_dy = dy;
  while (true) {
    const int new_file = file + new_dx;
    const int new_rank = rank + new_dy;
    if (new_file < 0 || new_file >= 8 || new_rank < 0 || new_rank >= 8)
      break;

    const square_t target = square_from_file_rank(new_file, new_rank);
    const piece_t target_piece = pieces[target];
    if (target_piece == InvalidPiece) {
      // if its invalid, add to move list
      move_list.push_back(
          Move(location, target, Quiet, InvalidPiece, InvalidPiece));
      new_dx += dx;
      new_dy += dy;
    } else if (piece_colour(target_piece) != side_to_move) {
      move_list.push_back(
          Move(location, target, Capture, InvalidPiece, target_piece));
      break;
    } else {
      break;
    }
  }
}

void Board::get_bishop_moves(std::vector<Move> &move_list,
                             const square_t location) const {
  get_slide_move_helper(move_list, location, 1, 1);
  get_slide_move_helper(move_list, location, -1, 1);
  get_slide_move_helper(move_list, location, 1, -1);
  get_slide_move_helper(move_list, location, -1, -1);
}

void Board::get_rook_moves(std::vector<Move> &move_list,
                           const square_t location) const {
  get_slide_move_helper(move_list, location, 1, 0);
  get_slide_move_helper(move_list, location, -1, 0);
  get_slide_move_helper(move_list, location, 0, 1);
  get_slide_move_helper(move_list, location, 0, -1);
}

void Board::get_queen_moves(std::vector<Move> &move_list,
                            const square_t location) const {
  get_bishop_moves(move_list, location);
  get_rook_moves(move_list, location);
}

void Board::get_king_moves(std::vector<Move> &move_list,
                           const square_t location) const {
  const int file = square_file(location);
  const int rank = square_rank(location);
  static const std::array<std::pair<int, int>, 8> king_offsets = {
      std::make_pair(1, 0),  std::make_pair(1, 1),  std::make_pair(1, -1),
      std::make_pair(0, 1),  std::make_pair(0, -1), std::make_pair(-1, 1),
      std::make_pair(-1, 0), std::make_pair(-1, -1)};

  for (const auto &[dx, dy] : king_offsets) {
    const int new_file = file + dx;
    const int new_rank = rank + dy;
    if (0 <= new_file && new_file <= 7 && 0 <= new_rank && new_rank <= 7) {
      const square_t to_square = square_from_file_rank(new_file, new_rank);
      const piece_t to_piece = pieces[to_square];
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
  for (square_t sq = 0; sq < 64; ++sq) {
    const piece_t piece = pieces[sq];
    if (piece_colour(piece) != side_to_move)
      continue;
    switch (piece) {
    case WhiteKing:
    case BlackKing:
      get_king_moves(result, sq);
      break;
    case WhitePawn:
    case BlackPawn:
      get_pawn_moves(result, sq);
      break;
    case WhiteKnight:
    case BlackKnight:
      get_knight_moves(result, sq);
      break;
    case WhiteBishop:
    case BlackBishop:
      get_bishop_moves(result, sq);
      break;
    case WhiteRook:
    case BlackRook:
      get_rook_moves(result, sq);
      break;
    case WhiteQueen:
    case BlackQueen:
      get_queen_moves(result, sq);
      break;
    default:
      __builtin_unreachable();
    }
  }
  return result;
}

std::vector<Move> Board::generate_legal_moves() const {
  std::vector<Move> result;
  Board tmp(*this);
  const std::vector<Move> pseudo_legal_moves = generate_pseudo_legal_moves();
  for (const Move move : pseudo_legal_moves) {
    if (tmp.make_move(move)) {
      result.push_back(move);
    }
    tmp.unmake_move();
  }
  return result;
}

square_t Board::get_king_square(const colour_t side) const {
  const piece_t king = side == White ? WhiteKing : BlackKing;
  for (square_t sq = 0; sq < 64; ++sq) {
    if (pieces[sq] == king)
      return sq;
  }
  assert(false && "King was not found");
}

// Makes the supplied move on the board: returns true if the resulting position
// is legal: that is, if the move does not result in being in check.
bool Board::make_move(const Move move) {
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
      en_passant = move.to + 8;
    } else {
      en_passant = move.to - 8;
    }
  }
  const colour_t old_side_to_move = side_to_move;
  const colour_t new_side_to_move = old_side_to_move * -1;
  side_to_move = new_side_to_move;

  fifty_move++; // TODO: Reset this if a pawn moved or a capture was played
  if (old_side_to_move == Black)
    full_move++;

  return !is_square_attacked(get_king_square(old_side_to_move),
                             new_side_to_move);
}
