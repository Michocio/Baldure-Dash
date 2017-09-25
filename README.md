<h1> Remake of Baldure Dash </h1>

Format of board items:
* EMPTY FIELD ' '
* SAND '.'
* WALL '#'
* STONE 'O'
* DIAMOND '$'
* EXIT 'X'
* GAMER '&'

Controlling gamer:
* LEFT 'a'
* UP 'w'
* DOWN 's'
* RIGHT 'd'

As in orginal game, stoneS are falling down if there is no ground under (that's  called gravity, you know ).
**See example file for basic board**.

After first empty line program in waiting for commands to move gamer. If you use input mode (without *-TUI*) just type them and than the program will output final board. 

<h1> Graphcial mode </h1>

With *TUI* option you can use game in real time. Graphcial mode is created using ncurses library. You should use option *-lncurses* during compilation. Enjoy real emotions :)
