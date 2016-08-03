## Tetris CSFML

This is unfinished tetris project written in pure C with CSFML-2.3-2 library. It is under development.

To compile the source code under __linux__ with gcc and run I use following commands:
```
gcc -g main.c functions.c shape_maps.c -o main -lcsfml-graphics -lcsfml-window -lcsfml-system

./main
```
Hit *Return* to start game.

__TODO:__

- "Game Over" with restart
- Levels and scores system with table of records
- Three next shapes
- Main menu with beauties
- Look with sprites
- Autoconf and autotools script
