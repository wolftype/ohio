#OH, HI!

Q: What is role of std::experimental::optional (aka maybe)


#Signal: function of time t
#SignalFunction: function of signal
#Behavior

## time.cpp
Per thread timing functions
---

* `time_ : time_() -> float`  Time in Milliseconds since AppStartTime
* `now : now -> float`  System time in Milliseconds

* `wait_ : wait_(float) -> (float->float)`  Signal in Milliseconds since AppStartTime

* `start_timer_ : start_timer_() -> (_ -> float)` Time in Milliseconds since last called


##thread.cpp
Asynchronous functions
* `thread_ : \y -> \x -> y(x)`
* `asyncf_: \y -> \x -> \ -> y(x)`
* `asyncf_: \y -> \x -> \ -> y(x)`


## basic.cpp
Lift basic functions onto signals
---

* `eq_ : \y -> (\x -> bool)`
* `gt_ : \y -> (\x -> bool)`
* `lt_ : \y -> (\x -> bool)`

---

* `add_to_: \y -> (\x -> (x + y))`
* `subtract_from_ : \y -> (\x -> (y - x))`
* `mult_by_: \y -> (\x -> (y * x))`
* `divide_by_ : \y -> (\x -> (x / y))`
* `mod_: \y -> (\x -> (x % y))`

* `plus_ : \(x,y) -> (x + y)`
* `div_  : \(x,y) -> (x / y)`
* `mult_ : \(x,y) -> (x * y)`

## arrows.cpp
Control switching between processes
---
* `and_ :  \(f1,f2) -> (\y -> (f1(y), f2(y)))`
* `both_ :  \(f1,f2) -> (\(x,y) -> (f1(x), f2(y)))`
* `first_ :  \f -> (\(x,y) -> (f(x), y))`
* `second_ :  \f -> (\(x,y) -> (x, f(y)))`
* `merge_:  \f -> (\(x,y) -> f(x,y))`
* `pipe_:  \(f1, f2) -> (\y -> f2 (f1(y)))`
* `split_: \(f1, f2) -> (\y -> (f1(y), f2(y))`

## signal.cpp
Generate functions of time

* `id_ :   \y -> y`
* `true_ : \y -> bool`
* `just_ : \y -> maybe y`

* `if_ : \y -> (\x -> z) -> maybe z`

* `trigger_on_reset_ () : \y -> bool`
* `trigger_on_change_ (a, b) : \y -> bool`

### sig(y) where y is frequency
* `impulse_ : \y -> \t -> bool`
* `ramp_ :    \y -> \t -> [0,1)`
* `saw_ :     \y -> \t -> (-1,1)`
* `osc_ :     \y -> \t -> (-1,1)`


##event.cpp
