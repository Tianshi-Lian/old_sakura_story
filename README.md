# Sakura Story
Sakura Story game.

## Build:
#### Global Dependencies:
- Python3  (may require aliasing `python3` to `python` on cmd/terminal)
- spdlog (Windows x64 binaries included)

### Windows:
#### Additional Dependencies:
- Visual Studio 2019 or MSBuild on PATH with C++ compiler

#### Usage:
- Use the built-in command line tool `cli gen` to generate a Visual Studio 2019 solution and project.
- Use the command `cli build` which will build the project using MSBuild for Windows x64.

### Linux:
#### Additional Dependencies:
- g++
- Make
- Alias in ~/.bash_aliases for `./cli.sh` to `cli` - alternatively use all commands as `./cli.sh command`.

#### Usage:
- Use the built-in command line tool `cli gen` to generate project Makefiles.
- Use the command `cli build` which will build the project using Make and g++ for Linux x64.
