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

  std::string starting_fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/"
                             "1PP1QPPP/R4RK1 w - - 0 10";
  Board board = Board(starting_fen);
  std::cout << starting_fen << std::endl;
  std::cout << board.to_fen() << std::endl;
}
