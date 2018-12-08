#include "ob/string.hh"

#include <cstddef>

#include <string>
#include <vector>
#include <regex>
#include <optional>
#include <limits>

namespace OB::String
{

std::vector<std::string> split(std::string const& str, std::string const& delim, std::size_t times)
{
  std::vector<std::string> vtok;
  std::size_t start {0};
  auto end = str.find(delim);

  while ((times-- > 0) && (end != std::string::npos))
  {
    vtok.emplace_back(str.substr(start, end - start));
    start = end + delim.length();
    end = str.find(delim, start);
  }
  vtok.emplace_back(str.substr(start, end));

  return vtok;
}

bool assert_rx(std::string str, std::regex rx)
{
  std::smatch m;
  if (std::regex_match(str, m, rx, std::regex_constants::match_not_null))
  {
    return true;
  }
  return false;
}

std::optional<std::vector<std::string>> match(std::string const& str, std::regex rx)
{
  std::smatch m;
  if (std::regex_match(str, m, rx, std::regex_constants::match_not_null))
  {
    std::vector<std::string> v;
    for (auto const& e : m)
    {
      v.emplace_back(std::string(e));
    }
    return v;
  }
  return {};
}

} // namespace OB::String
