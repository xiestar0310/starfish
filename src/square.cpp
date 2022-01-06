#include "square.hpp"

Square square_from_file_rank(const int file, const int rank) {
  return static_cast<Square>(8 * (7 - rank) + file);
}

// Return the file (file a - file h) -> (0 - 7)
int square_file(const Square sq) { return sq % 8; }

// Return the rank (rank 1 - rank 8) -> (0 - 7)
int square_rank(const Square sq) { return 7 - sq / 8; }

Square string_to_square(const std::string &str) {
  if (str == "-") {
    return InvalidSquare;
  }

  const int file = str[0] - 'a'; // 0 - 7
  const int rank = str[1] - '1'; // 0 - 7
  return square_from_file_rank(file, rank);
}

std::string string_from_square(const Square sq) {
  if (sq == InvalidSquare)
    return "-";
  const int file = square_file(sq);
  const int rank = square_rank(sq);
  return {static_cast<char>(file + 'a'), static_cast<char>(rank + '1')};
}
