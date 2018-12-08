#include "ob/parg.hh"
using Parg = OB::Parg;

#include "stig/stig.hh"
namespace Stig = OB::Stig;

#include <string>
#include <iostream>

int program_options(Parg& pg);

int program_options(Parg& pg)
{
  pg.name("stig").version("0.0.0 (08.12.2018)");
  pg.description("A CLI tool for searching Git repositories on GitHub.");
  pg.usage("[flags] [options] [--] [arguments]");
  pg.usage("[-v|--version]");
  pg.usage("[-h|--help]");
  pg.info("Examples", {
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

  // flags
  pg.set("help,h", "print the help output");
  pg.set("version,v", "print the program version");

  pg.set_pos();
  pg.set_stdin();

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

  return 0;
}

int main(int argc, char *argv[])
{
  Parg pg {argc, argv};
  int pstatus {program_options(pg)};
  if (pstatus > 0) return 0;
  if (pstatus < 0) return 1;

  return 0;
}
