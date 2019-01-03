## Tetris CSFML
[![Build Status](https://travis-ci.org/Oxore/tetris-csfml.svg?branch=master)](https://travis-ci.org/Oxore/tetris-csfml)
![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png "WTFPL")

This is unfinished tetris project written in pure C with CSFML library. It is under development. Basically I am learning some techniques and patterns for C and trying to use them here.

![Main menu](https://i.imgur.com/kOKz2Qk.png "Main menu")

![In game](https://i.imgur.com/UmZGEOD.png "In game")

__Dependencies__

- `SFML` - Graphics and controls
- `CSFML` - C SFML bindings
- `libyaml` - YAML parser
- [`munit`](https://github.com/nemequ/munit) - Testing framework (submodule)
- [`libf8`](https://github.com/Oxore/libf8) - Unicode helper functions (submodule)

__Compilation and use__

Compilation:
```
make
```
File named "tetris" is the final binary. Run it. Hit *S* to start game.

- `S` - start a game.
- `Left arrow` - move shape left.
- `Right arrow` - move shape right.
- `Up arrow` - rotate shape clockwise.
- `Space` - hard drop.
- `P` - pause.

Force exclude producing test binary:
```
make NOTEST=1
```

Clean:
```
make clean
```

__Compilation with libs in prefix__

Probably your package manager does not have `SFML` and/or `CSFML` in repos.
You can build them in prefix and then specify `PREFIX` path when running `make`:
```
make PREFIX=$(pwd)/deps/prefix
```

It is better to use absolute path, so you can then put produced `tetris` binary to any other place and have prefix location unchanged.

__Windows__

Look at the [repo releases](https://github.com/Oxore/tetris-csfml/releases)!
Windows compilation flow currently is unsupported, but sometimes I put Windows binary releases there.

### TODO:

- Add screenshots and gameplay gif
- [x] Levels speed and scores calibration
- [x] Scoring system as in Tetris for NES which is described on [this page](https://tetris.wiki/Scoring)
- [x] [Wall kick](https://tetris.wiki/Wall_kick). Implemented order: left wall, right wall, floor
- [x] Hard drop
- [x] Phantom shape
- [x] Game Over screen
- Table of records
- Settings menu with external `config.yml` file which allows
  - Change key bindings
  - Change colorscheme and define new
  - Number of levels
- [x] Three next shapes
- [x] Pause
- Look with sprites and shaders
- Interprocess communication interface for control from other process (I want to connect machine learning algorithm and make it play and learn)
