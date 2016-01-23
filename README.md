# Silver
Better than Bronze, Worse than Gold. Exactly.

### ...What?
A chess engine in C. That's what.

Even though right now it's closer to a Cheese engine, the objective is to make it fully featured.
I intend to make it _UCI_compliant to be able to pit against the big dogs and see how ~~bad~~
this puppy actually does when it's **"ready"**.

I'll also be collecting the _Perft_ scores for my move generation mechanism, to provide a better
context for this, performance-wise.

### What it does so far
So far, the engine is capable of generating all legal moves for a certain player, given a board position.
The implementation is modular enough to easily allow for checkmate/stalemate/draw detection in any end-game scenario, with little more code (it is in fact my next goal). Further on I'll be implementing a search mechanism based on **Alpha-beta search** and **Iterative deepening** and a static evaluation mechanism with hopefully-not-so-obvious heuristics, to improve move scoring and the decision making ability.

### How it works
Provide the program with a [**FEN** compliant string](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) and it will list all the possible moves for the current player in [algebraic notation](https://en.wikipedia.org/wiki/Algebraic_notation_(chess)), ordered by **score** (best to worst), along with a drawn board and the _Zobrist_ hash code for the position. The scoring mechanism is, so far, simple: It takes into account the moving piece with big bonuses for captures and promotions, varying with piece captured or promoted to. As I said, there's no evaluation mechanism yet to provide an acceptably precise score for each move.

Here's an example input...
```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```
...And it's following output
```
    A B C D E F G H
  *-----------------*
8 | r n b q k b n r | 8
7 | p p p p p p p p | 7
6 | . . . . . . . . | 6
5 | . . . . . . . . | 5
4 | . . . . . . . . | 4
3 | . . . . . . . . | 3
2 | P P P P P P P P | 2
1 | R N B Q K B N R | 1
  *-----------------*
    A B C D E F G H

Zobrist hash key for position: 8748045942876076066

h3
h4
g3
g4
f3
f4
e3
e4
d3
d4
c3
c4
b3
b4
a3
a4
Nh3
Nf3
Nc3
Na3
```

### Building
I chose CMake for the build to keep it easily scalable and dependency-resolving-friendly. "_OH LOOK HE'S USING CMAKE WHAT A PUSSY!_" - seriously, I don't give a shit. It's too easy and beautiful and portable not to use. F\*ck you. Plus it integrates with CLion. I'm using it. I'm also on Windows. Obvious choice.

As for actually building it, if you're on Windows, I suggest setting up an environment with [CygWin](https://www.cygwin.com/install.html) and [here's a good reason why](http://stackoverflow.com/questions/771756/what-is-the-difference-between-cygwin-and-mingw). If you're ~~decent~~ already on Linux/\*NIX (Linux, Mac OSX, CygWin, etc...) you probably already have `make` from `base-devel` or some other base package group, just install CMake with whatever it is your package management utility and you're good to go.

If you want to completely ignore anything I just said, also, [there are pre-compiled binaries available](https://cmake.org/download/) for whatever platform you're on, so you can just chill.

On the the project's root dir (containing the `CMakeLists` file), run
```
cmake .
```
It will generate the CMake build files and the corresponding Makefile. Then, on top of that, run
```
make
```
and the "ChessEngine" executable will be linked together and built. You're done. Also, it doesn't matter where you run `cmake` from, as long as you point to the dir with the `CMakeLists` file (which you already know if you're familiar with build systems). It is also advisable to run it from a subdir, for example, to keep the organization intact (like, create a subidr `build` on the root dir, cd into it, then `cmake ..` and `make` and all the output will be on your current subdir).

If you're just interested in running it there are **ready binaries** listed on the _releases_ tab. Just download it and run the application.

### Interested?
If you got turned on by any of this, well then, first of all, _you're fucked up_ and, second of all, _welcome to the club_.

Also, I suggest you read pretty much all you can from [Bruce Moreland's notes on Chess Programming](http://web.archive.org/web/20071213180053/http://www.seanet.com/~brucemo/topics/topics.htm) and the [Chess Programming Wiki](https://chessprogramming.wikispaces.com/Home), these were my main go-to resources for acquiring knowledge into chess programming.

[The CPW Engine](https://chessprogramming.wikispaces.com/CPW-Engine) is a good read if you want to leverage the code-writing process, if you have an idea of what you're doing/want to do (the code is pretty not-so-pretty and poorly commented, I'm afraid, since play power and low resource usage seem to be the main goals). Tom Kerrigan's [TSCP](http://www.tckerrigan.com/Chess/TSCP/) is, on the other hand, very well commented. I've acquired some pretty good insights reading it's code. For such a simple code (the focus *is* readability), the play power is **very** respectable.

If you think this project to have any potential of becoming something worth a glance then **you are welcome ~~as fuck~~ to contribute to this repo, man**.
