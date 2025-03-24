/*!
 * \file jcl_util.h
 *
 * This file declares some utility functions used in the jcl library
 */

#ifndef JCL_UTIL_H
#define JCL_UTIL_H

#include <string>
#include <vector>

namespace jcl
{
/*!
   * \brief Splits a string
   *
   * This function splits a string based on the supplied separator
   *
   * \param tokens Upon return holds the tokens of the split string
   * \param text The text string to split
   * \param sep The separator to use to split the string
   */
  void split(std::vector<std::string> &tokens, const std::string &text, char sep);

  /*!
   * \brief Trims leading white space
   *
   * This function trims all leading white space from the supplied
   * string and returns the string.
   *
   * \param s The string to trim
   *
   * \return The string with leading white space removed
   */
  std::string& ltrim(std::string &s);

  /*!
   * \brief Trims trailing white space
   *
   * This function trims all trailing white space from the supplied
   * string and returns the string.
   *
   * \param s The string to trim
   *
   * \return The string with trailing white space removed
   */
  std::string& rtrim(std::string &s);

  /*!
   * \brief Trims both leading and trailing white space
   *
   * This function trims all leading and trailing white space from the supplied
   * string and returns the string.
   *
   * \param s The string to trim
   *
   * \return The string with leading and trailing white space removed
   */
  std::string& trim(std::string &s);
}

#endif // JCL_UTIL_H
