# ChessEngine

*ChessEngine* is a shared library implementation of the classic game of Chess.  It could be used as a back-end for a web-based version of Chess, a desktop version, and so forth.  It's written in C++ and provides best-move recommendations based on the mini-max algorithm.  A wxWidgets-based front-end is included in the project that provides example usage of the library and testing.  I've also deployed it to the win-get package repository, so you can install it with win-get as follows.

```
C:\> win-get install SpencerSoft.Chess
```

I might try to port it to Linux in the future since there's nothing windows-dependent in the code.

![snapshot](https://github.com/spencerparkin/ChessEngine/blob/master/Snapshot.png?raw=true)