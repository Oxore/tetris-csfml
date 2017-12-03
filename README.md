## Tetris CSFML
[![Build Status](https://travis-ci.org/Oxore/tetris-csfml.svg?branch=master)](https://travis-ci.org/Oxore/tetris-csfml)
![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png "WTFPL")

This is unfinished tetris project written in pure C with CSFML library. It is under development.

__Dependencies__

`SFML`
`CSFML`
`libyaml`

__Compilation and use__

Compilation:
```
make all
```
File named "tetris" is the final binary. Run it. Hit *S* to start game.

Cleaning:
```
make clean
```

__TODO:__

- Add screenshots and gameplay gif
- Levels speed and scores calibration
- Scoring system as in Tetris for NES which is described on [this page](https://tetris.wiki/Scoring)
- [Wall kick](https://tetris.wiki/Wall_kick)
- Table of records
- Settings menu with external `config.yml` file which allows
  - Change key bindings
  - Change colorscheme and define new
  - Number of levels
- Three next shapes
- Main menu with beauties
- Look with sprites
- Interprocess communication interface for control from other process (I want to connect machine learning algorithm and make it play and learn)
