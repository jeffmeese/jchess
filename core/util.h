#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

namespace util
{
  void split(std::vector<std::string> &tokens, const std::string &text, char sep);
  std::string& ltrim(std::string &s);
  std::string& rtrim(std::string &s);
  std::string& trim(std::string &s);
}

#endif // UTIL_H
