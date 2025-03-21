#include "jcl_util.h"

#include <algorithm>
#include <cctype>

namespace jcl
{

void split(std::vector<std::string> &tokens, const std::string &text, char sep)
{
  int start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
}

std::string& ltrim(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
  return s;
}

// Trim from the right
std::string& rtrim(std::string& s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
               return !std::isspace(ch);
             }).base(), s.end());
  return s;
}

// Trim from both ends
std::string & trim(std::string& s)
{
  return ltrim(rtrim(s));
}

// trim from start
// std::string& ltrim(std::string &s)
// {
//   s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
//   return s;

//   //s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
//   //return s;
// }

// // trim from end
// std::string& rtrim(std::string &s)
// {
//   s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}), s.end());
//   return s;
//   //s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
//   //return s;
// }

// // trim from both ends
// std::string& trim(std::string &s)
// {
//   return ltrim(rtrim(s));
// }

} // namespace jcl

