## ohio: A tiny experimental framework for interactive signal processing
note: very experimental and in development (not really recommended for use yet)

###  About

`ohio` is the beginning of a rough sketch of a teeny library for arrow-based
functional reactive programming [FRP] built entirely with C++14 generic lambdas
and some boost::hana (notably `tuple`, `transform`, `fix`, `fold`, `compose`)

When it is finished, you could make:

* a terminal game (console output included)
* music (audio library not included)
* a composition engine

### Composing Signal Functions:

The basic idea of FRP is that you combine very basic signals (like a sinewave or
a keyboard key) into signal functions (functions that create signals) and then
start and stop the behaviors.

The application you write is an explicit graph of functions that is marched through
either by regularly reading a clock or listening in on a thread for a trigger.

Other than the name `ohio` itself, all the functions are given as-obvious-as-possible
names.  Functional programming can get mysterious at times though.  Sometimes
I wonder how this even all works.

### Run Examples

Pretty print some basic floating point functions of time:
```
./run.sh tests/xSignals.cpp
```

Use H,J,K,L to move a TREX unicode character around the terminal:
```
./run.sh tests/xListen.cpp
```

The run script calls `stty -echo -icanon` so that keyboard input goes straight to the application
and is not echoed.

dependencies: hana (included as a submodule)

    git clone https://github.com/wolftype/ohio.git
    git submodule init
    git submodule update
    ./run.sh tests/<filename.cpp> -c

###  Q: Why is this library called `ohio`?
###  A: I'm no longer certain who was on the line.
