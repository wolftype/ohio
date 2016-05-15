## OHIO: (Experimental) Arrow-based FRP model with C++14


Composing Signal Functions

dependencies: hana (included as a submodule)
note: very experimental and in development (not recommended for use, yet . . .)

    git clone https://github.com/wolftype/ohio.git
    git submodule init
    git submodule update 
    ./run.sh tests/<filename.cpp> -c 

###  Q: Why is this library called `ohio`?  
Maybe it stands for "Oh, IO!" as in, use this for input/output of state.
Another possibility is that we are using a reactive pull-based POLLING, and
OHIO (the U.S. State) is famous for its polling (of people).  Yet another
possible reason is that it is a place, a destination, that one can travel to,
and so we get there somehow, via the railroad perhaps, and that metaphor is
also useful in assembling functions.  Yup.


###  Q: What are its goals?
###  A: To make things happen

###  Q: How?
###  A: Signal Functions which are functions that take signals and return other signals

###  Q: What is a Signal?
###  A: A function of time
