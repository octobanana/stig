#include "stig/stig.hh"

#include "stig/lang.hh"

#include "ob/ansi_escape_codes.hh"
namespace aec = OB::ANSI_Escape_Codes;

#include "ob/belle.hh"
namespace Belle = OB::Belle;

#include "lib/json.hh"
using Json = nlohmann::json;

#include <unistd.h>

#include <cstddef>
#include <cmath>
#include <ctime>

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <optional>

namespace OB::Stig
{

// prototypes
void on_error(std::string const& str);
void on_http_error(Belle::Client& app);
std::pair<std::string, std::string> fuzzy_time(long int const sec);
void search_print(Json const& js, std::pair<int, int> rate, std::size_t page, std::size_t per_page, std::string const& color);

void on_error(std::string const& str)
{
  throw std::runtime_error(
    str +
    "\nview the help output with '-h'"
  );
}

void on_http_error(Belle::Client& app)
{
  app.on_http_error([&](auto& ctx)
  {
    on_error(ctx.ec.message());
  });
}

template<typename T>
std::optional<T> json_value(Json const& js)
{
  if (js.is_null())
  {
    return {};
  }
  return js.get<T>();
}

std::pair<std::string, std::string> fuzzy_time(long int const sec)
{
  std::pair<std::string, std::string> res;

  long int constexpr t_second {1};
  long int constexpr t_minute {t_second * 60};
  long int constexpr t_hour   {t_minute * 60};
  long int constexpr t_day    {t_hour * 24};
  long int constexpr t_week   {t_day * 7};
  long int constexpr t_month  (t_day * 30.4);
  long int constexpr t_year   {t_month * 12};

  std::time_t const now {std::time(nullptr)};
  long int const dif {now - sec};

  auto const fuzzy_string = [&](long int const time_ref, std::string const time_str)
  {
    long int const fuzzy (dif / time_ref);

    res.first = std::to_string(fuzzy);
    res.second = time_str;
  };

  if (dif >= t_year)
  {
    fuzzy_string(t_year, "Y");
  }
  else if (dif >= t_month)
  {
    fuzzy_string(t_month, "M");
  }
  else if (dif >= t_week)
  {
    fuzzy_string(t_week, "W");
  }
  else if (dif >= t_day)
  {
    fuzzy_string(t_day, "D");
  }
  else if (dif >= t_hour)
  {
    fuzzy_string(t_hour, "h");
  }
  else if (dif >= t_minute)
  {
    fuzzy_string(t_minute, "m");
  }
  else if (dif >= t_second)
  {
    fuzzy_string(t_second, "s");
  }
  else
  {
    res.first = "0";
    res.second = "s";
  }

  return res;
}

void search(std::string const& query, std::string const& sort, std::string const& order, std::size_t page, std::size_t per_page, std::string const& token, std::string const& color)
{
  Json js;
  std::pair<int, int> rate;

  Belle::Client app {"api.github.com", 443, true};
  on_http_error(app);

  Belle::Request req;
  req.target("/search/repositories");
  req.method(Belle::Method::get);

  req.set(Belle::Header::accept, "application/vnd.github.mercy-preview+json");

  req.params().emplace("q", query);
  req.params().emplace("sort", sort);
  req.params().emplace("order", order);
  req.params().emplace("page", std::to_string(page));
  req.params().emplace("per_page", std::to_string(per_page));

  if (! token.empty())
  {
    req.params().emplace("access_token", token);
  }

  app.on_http(req.move(), [&js, &rate](auto& ctx)
  {
    if (ctx.res.result() != Belle::Status::ok)
    {
      std::stringstream ss; ss
      << "HTTP request"
      << " " << ctx.res.result_int()
      << " " << ctx.res.reason();
      on_error(ss.str());
      return;
    }

    rate.first = std::stoi(std::string(ctx.res.at("X-RateLimit-Remaining")));
    rate.second = std::stoi(std::string(ctx.res.at("X-RateLimit-Limit")));
    js = Json::parse(std::move(ctx.res.body()));
  });

  auto total = app.queue().size();
  auto completed = app.connect();

  if (completed != total)
  {
    on_error("an HTTP error occurred");
  }

  search_print(js, rate, page, per_page, color);
}

void search_print(Json const& js, std::pair<int, int> rate, std::size_t page, std::size_t per_page, std::string const& color)
{
  // color auto
  bool is_color {isatty(STDOUT_FILENO)};

  if (color == "on")
  {
    // color on
    is_color = true;
  }
  else if (color == "off")
  {
    // color off
    is_color = false;
  }

  for (auto const& e : js["items"])
  {
    auto const owner = e["owner"]["login"].get<std::string>();
    auto const repo = e["name"].get<std::string>();
    auto const stars = e["stargazers_count"].get<std::size_t>();
    auto const fork = e["fork"].get<bool>();
    auto const forks = e["forks_count"].get<std::size_t>();
    auto const issues = e["open_issues_count"].get<std::size_t>();
    auto const lang = json_value<std::string>(e["language"]).value_or("");
    auto const desc = json_value<std::string>(e["description"]).value_or("");

    std::string fork_symbol {"<"};
    if (fork)
    {
      fork_symbol = ">";
    }

    std::pair<std::string, std::string> updated;
    std::tm t = {};
    std::stringstream ss (e["pushed_at"].get<std::string>());
    if (ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ"))
    {
      updated = fuzzy_time(std::mktime(&t));
    }

    std::cout
    << aec::wrap(owner, {aec::fg_magenta, aec::bold}, is_color)
    << "/"
    << aec::wrap(repo, {aec::fg_white, aec::bold}, is_color)
    << " *"
    << aec::wrap(stars, {aec::fg_green, aec::bold}, is_color)
    << " " << fork_symbol
    << aec::wrap(forks, {aec::fg_blue, aec::bold}, is_color)
    << " !"
    << aec::wrap(issues, {aec::fg_cyan, aec::bold}, is_color)
    << " ["
    << aec::wrap(lang, {aec::fg_true(Lang::color(lang)), aec::bold}, is_color)
    << "] "
    << aec::wrap(updated.first, {aec::fg_yellow, aec::bold}, is_color)
    << aec::wrap(updated.second, {aec::fg_yellow, aec::bold}, is_color)
    << "\n  "
    << desc
    << "\n\n";
  }

  auto const total_results = js["total_count"].get<std::size_t>();
  auto const total_pages = static_cast<std::size_t>(std::ceil(static_cast<double>(total_results) / per_page));

  std::size_t i1 {1};
  std::size_t i2 {per_page > total_results ? total_results : per_page};
  if (page > 1)
  {
    i2 = page * per_page;
    i1 = i2 - per_page + 1;
    if (i2 > total_results)
    {
      i2 = total_results;
      i1 = total_results - (total_results % per_page);
    }
  }

  std::cout
  << aec::wrap(i1, {aec::fg_magenta, aec::bold}, is_color)
  << "-"
  << aec::wrap(i2, {aec::fg_magenta, aec::bold}, is_color)
  << "/"
  << aec::wrap(total_results, {aec::fg_white, aec::bold}, is_color)
  << " results | "
  << aec::wrap(page, {aec::fg_magenta, aec::bold}, is_color)
  << "/"
  << aec::wrap(total_pages, {aec::fg_white, aec::bold}, is_color)
  << " pages | "
  << aec::wrap(rate.first, {aec::fg_magenta, aec::bold}, is_color)
  << "/"
  << aec::wrap(rate.second, {aec::fg_white, aec::bold}, is_color)
  << " limit"
  << "\n";
}

void readme(std::string const& repo, std::string const& ref)
{
  std::string res;

  Belle::Client app {"api.github.com", 443, true};
  on_http_error(app);

  Belle::Request req;
  req.target("/repos/" + repo + "/readme");
  req.method(Belle::Method::get);

  req.set(Belle::Header::accept, "application/vnd.github.VERSION.raw");

  if (! ref.empty())
  {
    req.params().emplace("ref", ref);
  }

  app.on_http(req.move(), [&res](auto& ctx)
  {
    if (ctx.res.result() != Belle::Status::ok)
    {
      std::stringstream ss; ss
      << "HTTP request"
      << " " << ctx.res.result_int()
      << " " << ctx.res.reason();
      on_error(ss.str());
      return;
    }

    res = std::move(ctx.res.body());
  });

  auto total = app.queue().size();
  auto completed = app.connect();

  if (completed != total)
  {
    on_error("an HTTP error occurred");
  }

  std::cout << res << std::flush;
}

} // namespace OB::Stig
