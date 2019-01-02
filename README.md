# Stig
A CLI tool for searching GitHub from the terminal.

![stig example](https://raw.githubusercontent.com/octobanana/stig/master/assets/stig.png)

## About
Stig is a CLI tool for searching GitHub from the terminal.
With the ability to sort and filter results, Stig makes it easy to find what you're looking for.
Stig can also print out a repositories README.md to stdout, so you can quickly learn more about a project.

### Features
* search GitHub from the terminal
* use flags and options to filter the results
* print the readme of a specified repo to stdout

### Result Breakdown
A typical search result item will look like the following:
```
- owner
|          - repo
|          |    - stars
|          |    |   - forks
|          |    |   |  - issues
|          |    |   |  |  - language
|          |    |   |  |  |     - last updated
|          |    |   |  |  |     |  - summary
|          |    |   |  |  |     |  |
octobanana/stig *12 <3 !4 [C++] 5h
  A CLI tool for searching GitHub from the terminal.
```

A forked repository will show a `>` symbol, instead of the default `<` symbol.

The last updated symbols are mapped to the following:
```
s : seconds
m : minutes
h : hours
D : days
W : weeks
M : months
Y : years
```

At the end of the results, a summary will be shown:
### Summary Breakdown
```
- current results
|   - total results
|   |            - current page
|   |            | - total pages
|   |            | |          - requests remaining
|   |            | |          | - requests limit
|   |            | |          | |
1-5/81 results | 1/17 pages | 9/10 limit
```

### Terminal Compatibility
A terminal emulator that supports __ansi escape codes__ and __true color__
is required when colored output is enabled.
The majority of the popular terminal emulators should support both.
While having the colored output enabled provides the best experience,
it can be adjusted using the `--color` option,
taking either `on`, `off`, or `auto` as inputs, with auto being the default value.

### GitHub Token
By default, the GitHub API allows up to __10__ search queries per minute.
To extend the limit to __30__ search queries per minute, you can pass a GitHub token with the `--token` option.

For more information regarding creating a new personal access token,
refer to the following [GitHub help article](https://help.github.com/articles/creating-a-personal-access-token-for-the-command-line/).

### GitHub Enterprise Compatibility
It is possible to use a custom API endpoint for compatibility with GitHub
Enterprise installations using the `--host` option. The host should be
formatted as, `subdomain.domain.tld`. It's expected that the endpoint is served over __HTTPS__ on port __443__.

> #### Important
> When using the `--host` option, only use the `--token` option with
> a trusted custom API endpoint.
> If an incorrect host is passed along with a token,
> it would be wise to immediately revoke the token and create a new one.

## Examples
```sh
# query 'stig' showing '20' results from page '1'
$ stig --query 'stig' --number 20 --page 1

# query 'stig' with filter 'language:cpp'
$ stig --query 'stig' --filter 'language:cpp'

# query 'stig' and pipe into less
$ stig --query 'stig' | less

# query 'all' sorted by 'stars' with filter 'language:js'
$ stig --query '' --sort 'stars' --filter 'language:js'

# query 'http server' with filters 'language:cpp' and 'stars:>10'
$ stig --query 'http server' --filter 'language:cpp stars:>10'

# output the README.md for 'octobanana/stig' on default branch
$ stig --readme 'octobanana/stig'

# output the README.md for 'octobanana/stig' on branch 'master'
$ stig --readme 'octobanana/stig/master'

# output the README.md for 'octobanana/stig' on default branch and pipe into less
$ stig --readme 'octobanana/stig' | less

# output the program help
$ stig --help

# output the program version
$ stig --version
```

## Build
### Environment
* Linux (supported)
* BSD (untested)
* macOS (untested)

### Requirements
* C++17 compiler
* Boost >= 1.67
* OpenSSL >= 1.1.0
* CMake >= 3.8

### Dependencies
* ssl (libssl)
* crypto (libcrypto)
* pthread (libpthread)
* boost (libboost_system)

### Libraries:
* my [belle](https://github.com/octobanana/belle) library, for making HTTPS requests, included as `./src/ob/belle.hh`
* my [parg](https://github.com/octobanana/parg) library, for parsing CLI args, included as `./src/ob/parg.hh`
* nlohmann's [json](https://github.com/nlohmann/json) library, for working with JSON, included as `./src/lib/json.hh`

The following shell command will build the project in release mode:
```sh
./build.sh
```
To build in debug mode, run the script with the `--debug` flag.

## Install
The following shell command will install the project in release mode:
```sh
./install.sh
```
To install in debug mode, run the script with the `--debug` flag.

## License
This project is licensed under the MIT License.

Copyright (c) 2018-2019 [Brett Robinson](https://octobanana.com/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
