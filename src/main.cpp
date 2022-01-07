// A simple program that computes the square root of a number
#include "board.hpp"
#include "piece.hpp"
#include "square.hpp"
#include <cmath>
#include <cstdlib>
#include <glog/logging.h>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  // Initialize Google’s logging library.
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Hello World";

  std::string starting_fen = "4k3/8/8/1b6/8/8/8/4K2R w K - 0 1";
  Board board = Board(starting_fen);
  board.print_board();
  std::vector<Move> pseudo_legal_moves = board.generate_pseudo_legal_moves();
  std::cout << "Found " << pseudo_legal_moves.size()
            << " legal moves:" << std::endl;
  for (Move move : pseudo_legal_moves) {
    std::cout << string_from_square(move.from) << " to "
              << string_from_square(move.to) << std::endl;
  }
}
