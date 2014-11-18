peg-solitaire.c
======

![screenshot](http://www.leaseweblabs.com/wp-content/uploads/2014/11/peg-solitaire.png?new)

Console version of the game "peg-solitaire" for GNU/Linux

### Gameplay

You can move the cursor in four directions using the arrow keys: up, down, left, and right. Use the enter/return key to select (or deselect) a peg. Pegs can then be moved using the arrow keys. Restart and undo are with 'r' and 'u'. A peg can only move if it can jump over a adjacent peg and land on an empty space. A peg is represented by a circle and a empty space by a dot. You win the game when you end with one peg in the center position. When there are no more moves possible the game is over.

### Requirements

- C compiler
- GNU/Linux

### Installation

```
wget https://raw.githubusercontent.com/mevdschee/peg-solitaire.c/master/peg-solitaire.c
gcc -o peg-solitaire peg-solitaire.c
./peg-solitaire
```

### Contributing

Contributions are very welcome. 
