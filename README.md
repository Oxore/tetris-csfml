# Tetris CSFML
[![Build Status](https://travis-ci.org/Oxore/tetris-csfml.svg?branch=master)](https://travis-ci.org/Oxore/tetris-csfml)
[![Build status](https://ci.appveyor.com/api/projects/status/bnxbfbwamn1pp5ut?svg=true)](https://ci.appveyor.com/project/Oxore/tetris-csfml)
![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png "WTFPL")

This is unfinished tetris project written in pure C with CSFML library. It is under development. Basically I am learning some techniques and patterns for C and trying to use them here.

![Main menu](https://i.imgur.com/kOKz2Qk.png "Main menu")

![In game](https://i.imgur.com/UmZGEOD.png "In game")

## Dependencies

- [`SFML`](https://github.com/SFML/sfml) - Graphics and controls (submodule)
- [`CSFML`](https://github.com/SFML/csfml) - C SFML bindings (submodule)
- [`munit`](https://github.com/nemequ/munit) - Testing framework (submodule)
- [`libf8`](https://github.com/Oxore/libf8) - Unicode helper functions (submodule)

## Compilation and usage

Compilation process currently is a terrible thing, because I am suffering from
and struggling with my love to `make` and it's ugliness, `cmake` limitations and
CSFML slow-pace development. See `.travis.yml` for building instructions on
Linux or OSX and `.appveyor.yml` and `scripts/appveyor-build-static.bash` for
building instructions on Windows. For Windows though you can take already built
[artifacts](https://ci.appveyor.com/project/Oxore/tetris-csfml/build/artifacts)
from Appveyor.

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
