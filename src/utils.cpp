
#include "utils.hpp"

#include <sstream>
#include <string>
#include <vector>

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
  std::string result;
  std::copy_if(str.begin(), str.end(), std::back_inserter(result),
               [del](const char c) { return c != del; });
  return result;
}
