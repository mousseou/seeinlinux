# Puzzle-15

*Originally completed on September - December 2019*

## Task
Given an initial 16-tile board, find the path necessary to meet a goal state:

1   2   3   4
5   6   7   8
9   10  11  12
13  14  15  0

`generate.c`, a file that was entirely provided by instructor Ding, will generate a puzzle requiring N moves to finish. Take special note of the one line initial state printed.
`solve.c`, a file partially supplied by instructor Ding, will do the work of actually finding a path to the one line initial state and printing at every step of the way.

We're using [State Space Search](https://en.wikipedia.org/wiki/State_space_search) and [A* search algorithm](https://en.wikipedia.org/wiki/A*_search_algorithm).