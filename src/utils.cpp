#include "utils.hpp"

std::vector<std::string> split_string(const std::string &str, const char del) {
  std::vector<std::string> tokens;
  std::string temp = "";
  for (char c : str) {
    if (c == del) {
      tokens.push_back(temp);
      temp = "";
    } else {
      temp.push_back(c);
    }
  }
  tokens.push_back(temp);
  return tokens;
}

std::string remove_char(const std::string &str, const char del) {
  std::string new_str;
  for (char c : str) {
    if (c != del) {
      new_str.push_back(c);
    }
  }
  return new_str;
}
