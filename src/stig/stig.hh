#ifndef OB_STIG_HH
#define OB_STIG_HH

#include <string>

namespace OB::Stig
{

void search(std::string const& host, std::string const& query, std::string const& sort, std::string const& order, std::size_t page, std::size_t per_page, std::string const& token, std::string const& color);
void readme(std::string const& host, std::string const& repo, std::string const& ref);

} // namespace OB::Stig

#endif // OB_STIG_HH
