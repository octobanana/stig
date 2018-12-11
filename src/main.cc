#include "ob/parg.hh"
using Parg = OB::Parg;

#include "ob/string.hh"
namespace String = OB::String;

#include "stig/stig.hh"
namespace Stig = OB::Stig;

#include <string>
#include <iostream>

int program_options(Parg& pg);

int program_options(Parg& pg)
{
  pg.name("stig").version("0.1.1 (11.12.2018)");
  pg.description("A CLI tool for searching Git repositories on GitHub.");
  pg.usage("[flags] [options] [--] [arguments]");
  pg.usage("[-q|--query str] [-p|--page int] [-n|--number int] [-s|--sort stars|forks|updated|best] [-o|--order asc|desc] [-f|--filter key:value[ key:value]...] [--token str] [-c|--color on|off|auto]");
  pg.usage("[-r|--readme user/repo[/ref]]");
  pg.usage("[-v|--version]");
  pg.usage("[-h|--help]");
  pg.info("Examples", {
    "stig --query 'stig' --number 20 --page 1",
    "stig --query 'stig' --filter 'language:cpp'",
    "stig --query 'stig' | less",
    "stig --query '' --sort 'stars' --filter 'language:js'",
    "stig --query 'http server' --filter 'language:cpp stars:>10'",
    "stig --readme 'octobanana/stig'",
    "stig --readme 'octobanana/stig/master'",
    "stig --readme 'octobanana/stig' | less",
    "stig --help",
    "stig --version",
  });
  pg.info("Exit Codes", {"0 -> normal", "1 -> error"});
  pg.info("Repository", {
    "https://github.com/octobanana/stig.git",
  });
  pg.info("Homepage", {
    "https://octobanana.com/software/stig",
  });
  pg.author("Brett Robinson (octobanana) <octobanana.dev@gmail.com>");

  // general flags
  pg.set("help,h", "print the help output");
  pg.set("version,v", "print the program version");

  // query options
  pg.set("query,q", "", "str", "the query string");
  pg.set("page,p", "1", "int", "the page number to get");
  pg.set("number,n", "10", "int", "the number of results to show per page");
  pg.set("sort,s", "best", "stars|forks|updated|best", "how to sort the search results, default is best match");
  pg.set("order,o", "desc", "asc|desc", "the order to sort the search results, default is desc");
  pg.set("filter,f", "", "key:value[ key:value]...", "filter results with space seperated key:value pairs");
  pg.set("token", "", "str", "used to validate against GitHub, enables a greater number of requests before being rate-limited");
  pg.set("color,c", "auto", "on|off|auto", "used to determine the output color preference, default is auto");

  // readme options
  pg.set("readme,r", "", "user/repo[/ref]", "print a repos README.md to stdout");

  // pg.set_pos();
  // pg.set_stdin();

  int status {pg.parse()};

  if (status > 0 && pg.get_stdin().empty())
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "expected arguments" << "\n";

    return -1;
  }

  if (status < 0)
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << pg.error() << "\n";

    if (pg.similar().size() > 0)
    {
      std::cerr << "Did you mean: ";
      for (auto const& e : pg.similar())
      {
        std::cerr << e << " ";
      }
      std::cerr << "\n";
    }
    else
    {
      std::cerr << "No suggestions found.\n";
    }

    return -1;
  }

  if (pg.get<bool>("help"))
  {
    std::cerr << pg.help();

    return 1;
  }

  if (pg.get<bool>("version"))
  {
    std::cerr << pg.name() << " v" << pg.version() << "\n";

    return 1;
  }

  if (! pg.find("query") && ! pg.find("readme"))
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "expected either '--query' or '--readme'" << "\n";

    return -1;
  }

  if (pg.find("query") && pg.find("readme"))
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "choose either '--query' or '--readme'" << "\n";

    return -1;
  }

  if (pg.find("page") && pg.get<std::size_t>("page") < 1)
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "option '--page' must be greater than zero" << "\n";

    return -1;
  }

  if (pg.find("number") && (pg.get<std::size_t>("number") < 1 || pg.get<std::size_t>("number") > 100))
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "option '--number' must be between 1 and 100" << "\n";

    return -1;
  }

  if (pg.find("order") && ! pg.find("sort"))
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "option '--order' must be paired with '--sort'" << "\n";

    return -1;
  }

  if (pg.find("sort") && ! String::assert_rx(pg.get("sort"), std::regex("^stars|forks|updated|best$")))
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "option '--sort' contains an invalid value, try 'stars', 'forks', or 'updated'" << "\n";

    return -1;
  }

  if (pg.find("order") && ! String::assert_rx(pg.get("order"), std::regex("^asc|desc$")))
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "option '--order' contains an invalid value, try 'asc' or 'desc'" << "\n";

    return -1;
  }

  if (pg.find("color") && ! String::assert_rx(pg.get("color"), std::regex("^on|off|auto$")))
  {
    std::cerr << pg.help() << "\n";
    std::cerr << "Error: " << "option '--color' contains an invalid value, try 'on', 'off', or 'auto'" << "\n";

    return -1;
  }

  return 0;
}

int main(int argc, char *argv[])
{
  Parg pg {argc, argv};
  int pstatus {program_options(pg)};
  if (pstatus > 0) return 0;
  if (pstatus < 0) return 1;

  try
  {
    // query
    if (pg.find("query"))
    {
      auto query = pg.get("query");
      auto filter = pg.get("filter");

      if (! filter.empty())
      {
        auto filters = String::split(filter, " ");

        for (auto const& kv : filters)
        {
          auto k_v = String::split(kv, ":");

          if (k_v.size() != 2)
          {
            throw std::runtime_error(
              "filter '" + kv + "' is invalid" +
              "\nexpected 'key:value[ key:value]...'" +
              "\nview the help output with '-h'"
            );
          }

          query += " " + k_v.at(0) + ":" + k_v.at(1);
        }
      }

      if (query.empty())
      {
        throw std::runtime_error(
          "query string is empty"
          "\nview the help output with '-h'"
        );
      }

      auto sort = pg.get("sort");

      if (sort == "best")
      {
        sort = "";
      }

      auto order = pg.get("order");
      auto page = pg.get<std::size_t>("page");
      auto per_page = pg.get<std::size_t>("number");
      auto token = pg.get("token");
      auto color = pg.get("color");

      Stig::search(query, sort, order, page, per_page, token, color);
    }

    // readme
    else if (pg.find("readme"))
    {
      auto const path = pg.get("readme");

      struct match
      {
        enum
        {
          path,
          repo,
          ref
        };
      };

      auto const valid_repo = String::match(path,
        std::regex("^([^\\/]+?\\/[^\\/]+)(?:/([^\\/]+?))?$"));

      if (! valid_repo)
      {
        throw std::runtime_error(
          "repo path is invalid"
          "\nview the help output with '-h'"
        );
      }

      auto const repo = valid_repo.value().at(match::repo);
      auto const ref = valid_repo.value().at(match::ref);

      Stig::readme(repo, ref);
    }
  }
  catch(std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << "\n";
  }
  catch(...)
  {
    std::cerr << "Error: an unexpected error occurred\n";
  }

  return 0;
}
