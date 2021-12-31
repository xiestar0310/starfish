// A simple program that computes the square root of a number
#include <cmath>
#include <cstdlib>
#include <glog/logging.h>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  // Initialize Google’s logging library.
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Hello World";
}
