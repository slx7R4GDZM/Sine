# Building Sine

## Requirements

- SFML 2.0
- CMake 3.13.0
- MinGW-w64 6.3.0 POSIX Threads on Windows
- GCC 6.3.0 on Linux

## Building on Windows

Run the following commands to build Sine:

```
cd Sine/Build
cmake -B Release -G "MinGW Makefiles"
cd Release
mingw32-make
```

To change where CMake looks for SFML you can use configuration settings like:\
`cmake -B Release -D sfml-library-path="C:/SFML/lib" -G "MinGW Makefiles"`

| Cache variable    | Default path                                  |
|-------------------|-----------------------------------------------|
| sfml-include-path | C:/Program Files (x86)/SFML/include           |
| sfml-library-path | C:/Program Files (x86)/SFML/lib               |
| freetype          | C:/Program Files (x86)/SFML/lib/libfreetype.a |

## Building on Linux

You need to install CMake and SFML.\
On Ubuntu, this is: `sudo apt install cmake libsfml-dev`

Run the following commands to build Sine:

```
cd Sine/Build
cmake -B Release
cd Release
make
```
