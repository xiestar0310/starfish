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

  std::string starting_fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Board board = Board(starting_fen);
  std::cout << starting_fen << std::endl;
  std::cout << board.to_fen() << std::endl;
  board.print_board();
}
