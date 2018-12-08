# Stig
A CLI tool for searching Git repositories on GitHub.

## Build
Environment:
* Tested on Linux
* C++17 compiler
* CMake >= 3.8

Libraries:
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
