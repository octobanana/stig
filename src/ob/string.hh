#ifndef OB_STRING_HH
#define OB_STRING_HH

#include <cstddef>

#include <string>
#include <vector>
#include <regex>
#include <optional>
#include <limits>

namespace OB::String
{

std::vector<std::string> split(std::string const& str, std::string const& delim, std::size_t size = std::numeric_limits<std::size_t>::max());

bool assert_rx(std::string str, std::regex rx);

std::optional<std::vector<std::string>> match(std::string const& str, std::regex rx);

} // namespace OB::String

#endif // OB_STRING_HH
