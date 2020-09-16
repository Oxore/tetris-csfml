# Tetris CSFML
[![Build Status](https://travis-ci.org/Oxore/tetris-csfml.svg?branch=master)](https://travis-ci.org/Oxore/tetris-csfml)
[![Build status](https://ci.appveyor.com/api/projects/status/bnxbfbwamn1pp5ut?svg=true)](https://ci.appveyor.com/project/Oxore/tetris-csfml)
![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png "WTFPL")

This is unfinished tetris project written in pure C with CSFML library. It is under development. Basically I am learning some techniques and patterns for C and trying to use them here.

Archived, because it no longer serves it's purpose. I have already learned C to the level I wanted and I don't want to develop this Tetris in C anymore. I'm done. I wanted to see how far can I go with it and it's this far. And one of the most significant part of why I abandon this project is multiplatform build support. I wanted it to be available for my friends on Windows and OSX without need for thinking about dependency management - just pull binaries from CI that have everything linked statically. And it is hard for me to maintain builds of this project for any platform except GNU/Linux.

![Main menu](https://i.imgur.com/kOKz2Qk.png "Main menu")

![In game](https://i.imgur.com/UmZGEOD.png "In game")

## Dependencies

- [`SFML`](https://github.com/SFML/sfml) - Graphics and controls (submodule)
- [`CSFML`](https://github.com/SFML/csfml) - C SFML bindings (submodule)
- [`munit`](https://github.com/nemequ/munit) - Testing framework (submodule)
- [`libf8`](https://github.com/Oxore/libf8) - Unicode helper functions (submodule)

Get repository with all dependencies by using the following command:

    git clone --recurse-submodules https://github.com/Oxore/tetris-csfml.git

Or if you already did `git clone` without `--recurse-submodules`, then run:

    git submodule update --init --recursive

## Build instructions

See `.travis.yml` for building instructions on Linux or OSX used by CI jobs and
`.appveyor.yml` and `scripts/appveyor-build-static.bash` for building
instructions on Windows. For Windows though you can take already built
[artifacts](https://ci.appveyor.com/project/Oxore/tetris-csfml/build/artifacts)
from Appveyor.

Here are instructions for building on OSX or GNU/Linux system:

You will need `make` and C/C++ compiler (`gcc` or `clang`) installed on your
system.

Assuming your current working directory is a cloned `tetris-csfml` repository.

If you already have SFML and CSFML installed on your system, then just run

    make

But chances you have SFML and CSFML installed are low. You may not even have
CSFML and SFML available in your system package manager repositories. Then you
need to build them from sources and install in prefix. You will need `cmake`
installed on your system for building SFML and CSFML.

Define `PREFIX` shell environment variable:

    export PREFIX=$(pwd)/prefix

Prefix is just a directory with other directories like `lib`, `bin`, `include`
and so on, that repeats directory structure of your system. It is used to
install CSFML and SFML for current user only without need to pollute the system
by installing them bypassing the system package manager.

Create directories that are necessary to build SFML and CSFML:

    mkdir -p $PREFIX deps/SFML/build deps/CSFML/build

Build SFML and install it in `$PREFIX`

    pushd deps/SFML/build \
            && cmake \
              -DBUILD_SHARED_LIBS=OFF \
              -DCMAKE_INSTALL_PREFIX=$PREFIX \
              -DSFML_MISC_INSTALL_PREFIX=$PREFIX \
              -DSFML_DEPENDENCIES_INSTALL_PREFIX=$PREFIX \
              .. \
            && make install \
            && popd

Build CSFML bindings and install it in `$PREFIX`

    pushd deps/CSFML/build \
            && cmake \
              -DBUILD_SHARED_LIBS=OFF \
              -DCSFML_LINK_SFML_STATICALLY=TRUE \
              -DCMAKE_PREFIX_PATH=$PREFIX -DCMAKE_INSTALL_PREFIX=$PREFIX \
              .. \
            && make install \
            && popd

Finally build `tetris` executable, statically linked against SFML and CSFML
that are installed in `$PREFIX`:

    make PREFIX=$PREFIX SFML_STATIC=y

After build finishes you get the desired `tetris` executable file in the
current directory.

## Usage

Default key bindings:

- `S` - start a game.
- `Left arrow` - move shape left.
- `Right arrow` - move shape right.
- `Up arrow` - rotate shape clockwise.
- `Space` - hard drop.
- `P` - pause.

## Windows

The [Appveyor CI/CD](https://ci.appveyor.com/project/Oxore/tetris-csfml)
performs build on every commit and keep build artifacts for 6 months.

Every new version (on every new git tag) is automatically deployed to the
[repo releases](https://github.com/Oxore/tetris-csfml/releases) by Appveyor
CI/CD as a draft and author adds a description and submits the release.


## TODO:

- Add screenshots and gameplay gif
- [x] Levels speed and scores calibration
- [x] Scoring system as in Tetris for NES which is described on [this page](https://tetris.wiki/Scoring)
- [x] [Wall kick](https://tetris.wiki/Wall_kick). Implemented order: left wall, right wall, floor
- [x] Hard drop
- [x] Phantom shape
- [x] Game Over screen
- [x] Table of records
- [ ] Settings menu with external `config.json` file which allows
  - [ ] Change key bindings
    - [x] Support in config file
    - Support in menu
  - Change colorscheme and define new one
  - Levels customization (probably useless feature)
    - Select and deselect (skip) specific levels
    - Customize levels' speed
- [x] Three next shapes
- [x] Pause
- Look with sprites and shaders
- Interprocess communication interface for control from other process (I want to connect machine learning algorithm and make it play and learn)
- Port on SDL
