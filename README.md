# Silver
Better than Bronze, Worse than Gold. A very humble attempt at a chess engine
written in pure C.

## What it does so far
So far, the engine is capable of generating all legal moves for a certain
player, given a board position. This means no automation, no AI, no robust
decision making. Yet. Don't give up on me, pls.

The implementation, however, is modular enough to easily allow for
checkmate/stalemate/draw detection in any end-game scenario, with little more
code (it is in fact my next goal). Further on I plan on implementing a search
mechanism based on alpha-beta search and iterative deepening, and a static
evaluation mechanism with hopefully-not-so-obvious heuristics, to improve move
scoring and the still-non-existing decision making ability.

## Status and objectives
Right now it's closer to a cheese engine, but I plan to make it fully fledged. I
intend to make it _UCI_ compliant to be able to pit it against other engines in
automated contests, when it's minimally ready. I'll also be collecting _Perft_
scores for my move generation mechanism, to understand better what performance
means in this context.

## How it works
Provide the program with a [FEN-compliant string](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
and it will list all the possible moves for the current player in [algebraic notation](https://en.wikipedia.org/wiki/Algebraic_notation_(chess)),
ordered by score (best to worst), along with a drawn board and the _Zobrist_
hash code for the position. The scoring mechanism is, so far, simple: It takes
into account the moving piece with big bonuses for captures and promotions,
varying with piece captured or promoted to. As I said, there's no evaluation
mechanism yet to provide an acceptably precise score for each move.

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

## Dependencies
You'll need CMake and [there are pre-compiled binaries available](https://cmake.org/download/)
for whatever platform you're on. If you're on Unix, you can use your package
manager to install it (that includes brew, on Mac). If you're on Windows, I
suggest setting up an environment with [CygWin](https://www.cygwin.com/install.html)
and [here's a good reason why](http://stackoverflow.com/questions/771756/what-is-the-difference-between-cygwin-and-mingw).

## Building
Standard CMake build: do everything in a separate `build` dir or similar, to
keep things clean. From the project's root dir, you can go something like this:
```
mkdir build && cd build
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=on -DCMAKE_BUILD_TYPE=Release ..
```
This is what a typical cmake command I issue looks like, mostly all preference.
I don't have a `Release` target yet and `Debug` is not only useful but still
very necessary. Then you can just `make` and run the binary:
```
make
./ChessEngine
```
Warnings will follow. This is still very barebone-ish, but I'm gettin' there.


## Interested?
One primarily does a thing like this to learn it, but there is no greater
satisfaction than inspiring other people. If that's the case, I can help...

- First, I suggest you read pretty much all you can from [Bruce Moreland's notes on Chess Programming](http://web.archive.org/web/20071213180053/http://www.seanet.com/~brucemo/topics/topics.htm)
and the [Chess Programming Wiki](https://chessprogramming.wikispaces.com/Home),
these were my main go-to resources for acquiring knowledge into chess
programming. For theory and application as well.
- Then, [the CPW Engine](https://chessprogramming.wikispaces.com/CPW-Engine) is
a good read to leverage the code-writing process, if you have an idea of what
you're doing/want to do (the code is pretty not-so-pretty and poorly commented,
I'm afraid, since play power and low resource usage seem to be the main goals).
- Tom Kerrigan's [TSCP](http://www.tckerrigan.com/Chess/TSCP/) is, on the other
hand, very well commented. I've acquired some pretty good insights reading it's
code. For such a simple code (the focus *is* readability), the play power is
**very** respectable.

If you think this project to have any potential of becoming something worth a
glance then **you're more than welcome to contribute to it**.
