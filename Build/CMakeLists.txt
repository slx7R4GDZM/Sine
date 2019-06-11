cmake_minimum_required(VERSION 3.13)
project(Sine LANGUAGES CXX)
if(NOT (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel))
  message(STATUS "The only accepted build types are Debug and MinSizeRel, defaulting to MinSizeRel")
  set(CMAKE_BUILD_TYPE MinSizeRel)
endif()
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binary)

if(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
  string(APPEND CMAKE_CXX_FLAGS " -s")
endif()
if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
    string(APPEND CMAKE_CXX_FLAGS " -static")
    add_link_options(-mwindows)
    add_definitions(-DSFML_STATIC)
  endif()
endif()

set(src
  ../Source/Game/Game.cpp
  ../Source/Game/Game.h
  ../Source/Game/Space-Object.cpp
  ../Source/Game/Space-Object.h
  ../Source/Game/Space-Object/Asteroid.cpp
  ../Source/Game/Space-Object/Asteroid.h
  ../Source/Game/Space-Object/Photon.cpp
  ../Source/Game/Space-Object/Photon.h
  ../Source/Game/Space-Object/Saucer.cpp
  ../Source/Game/Space-Object/Saucer.h
  ../Source/Game/Space-Object/Ship.cpp
  ../Source/Game/Space-Object/Ship.h
  ../Source/Graphics/Graphics-Handler.cpp
  ../Source/Graphics/Graphics-Handler.h
  ../Source/Graphics/Vector-Generator.cpp
  ../Source/Graphics/Vector-Generator.h
  ../Source/Input/Input-Handler.cpp
  ../Source/Input/Input-Handler.h
  ../Source/Other/Common-Things.cpp
  ../Source/Other/Common-Things.h
  ../Source/Other/Constants.h
  ../Source/Other/Text.h
  ../Source/Other/Vectors.h
  ../Source/Settings/Settings-Handler.cpp
  ../Source/Settings/Settings-Handler.h
  ../Source/Main.cpp
)

add_executable(Sine ${src})
if(WIN32)
  set(sfml-include-path "C:/Program Files (x86)/SFML/include" CACHE PATH "")
  set(sfml-library-path "C:/Program Files (x86)/SFML/lib" CACHE PATH "")

  include_directories(${sfml-include-path})
  link_directories(${sfml-library-path})

  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    find_library(sfml-graphics sfml-graphics-d PATHS ${sfml-library-path})
    find_library(sfml-window sfml-window-d PATHS ${sfml-library-path})
    find_library(sfml-system sfml-system-d PATHS ${sfml-library-path})

    target_link_libraries(Sine ${sfml-graphics} ${sfml-window} ${sfml-system})
  else()
    find_library(freetype freetype PATHS ${sfml-library-path})
    find_library(sfml-graphics sfml-graphics-s PATHS ${sfml-library-path})
    find_library(sfml-window sfml-window-s PATHS ${sfml-library-path})
    find_library(sfml-system sfml-system-s PATHS ${sfml-library-path})

    target_link_libraries(Sine ${sfml-graphics} ${freetype} ${sfml-window} gdi32 opengl32 ${sfml-system} winmm)
  endif()
else()
  find_package(SFML COMPONENTS graphics window system REQUIRED)
  target_link_libraries(Sine sfml-graphics sfml-window sfml-system)
endif()
