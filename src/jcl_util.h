#ifndef JCL_UTIL_H
#define JCL_UTIL_H

#include <string>
#include <vector>

namespace jcl
{
  void split(std::vector<std::string> &tokens, const std::string &text, char sep);
  std::string& ltrim(std::string &s);
  std::string& rtrim(std::string &s);
  std::string& trim(std::string &s);
}

#endif // JCL_UTIL_H
