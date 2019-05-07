#ifndef OHIO_SIGNAL_INCLUDED
#define OHIO_SIGNAL_INCLUDED

#include <boost/hana/functional/compose.hpp>

#include "basic.hpp"
#include "arrows.hpp"

namespace hana = boost::hana;
namespace ohio {

/// @file
///
/// @brief Signals are functions of time in milliseconds --
///        to be fed time and return anything
///
/// Signal Functions are functions which return Signals
///
/// Combinators are functions which take signal functions and return
/// other signal functions
//
/// @todo make 1000 a macro to allow for microseconds 1000000 perhaps
/// @todo consider allowing input of more than one variable (t)
/// @todo develop notation for distinguishing between signals and signalfunctions
/// @todo develop any clock rate

/// Identity : Return Input (e.g bypass) -- id_ : id_(x) -> x
auto id_ = [](auto &&x) { return x; };

/// "Lift" true onto a signal --  true_ : true_(t) -> true
auto true_ = [](auto &&x) { return true; };

/// "Lift" false onto a signal --  false_ : false_(t) -> true
auto false_ = [](auto &&x) { return false; };

/// just : Return wrapped Input (e.g bypass) -- id_ : id_(x) -> x
auto just_ = [](auto &&x) { return maybe<TYPE(x)>(x); };

/// switch_once_ takes
//  a signalfunction sf1 that takes
//      a t and returns
///     a pair (f(t), event(t))
/// a signalfunction sf2 that takes
//     the result of event(t) and returns
///    a signal f(t)
/// see "linear_" for an example use
auto switch_once_ = [](auto &&sf1, auto &&sf2) {
  bool bSwitch = false;
  return [=](auto &&xs) mutable {
    // calculate (value, event) pair of outputs
    auto x = sf1 (FORWARD (xs));
    // if event has happened...
    if (hana::second (x))
      {
        // ... set bSwitch to true
        bSwitch = true;
      }
    // now, if bSwitch is true...
    if (bSwitch)
      {
        // ...pass event to continuation function,
        // which may do different things depending on its value
        return sf2 (FORWARD (hana::second (x))) (FORWARD (xs));
      }
    // otherwise return original output
    return hana::first (x);
  };
};

/// if signal is true, return result TODO looks f'ed
//auto if_ = [](auto &&x) {
//  return [=](auto &&e) {
//    using T = TYPE (e);
//    if (e)
//      return maybe<T> (x);
//    return maybe<T> ();
//  };
//};

/// conditional event (unused)
auto cond_ = [](auto &&f) {
  return [=](auto &&t) {
    if (f (t))
      return maybe<bool> (true);
    return maybe<bool> ();
  };
};

///detects change of large amt
auto trigger_on_change_ = [](float &&amt, float &&start) {
  return [&](float &&t) {
    if ((t - amt) > start)
      {
        start = t;
        return true;
      }
    return false;
  };
};


/// detect change from false to true of incoming signal
auto trigger_on_edge_ = []() {
  bool bNewEdge = true;
  return [=](bool &&b) mutable {
    if (b && bNewEdge)
      {
        bNewEdge = false;
        return true;
      }
    else if (!b)
      {
        bNewEdge = true;  /// reset edge to true
      }
    return false;
  };
};

/// trigger an increasing signal has been reset (use with mod_)
// really, trigger on no longer increasing
// NOTE: should potentially be shared_ptr<double> ?
auto trigger_on_reset_ = []() {
  double start;
  start = 0;
  return [=](double &&t) mutable {
    if (t >= start)
      {
        start = t;
        return false;
      }
    else //if (t==0)
      {
        start = 0;
      }
    return true;
  };
};


/// trigger on evaluation to given argument
auto trigger_once = [](auto &&sig, auto &&ref)
{
  return [=](auto &&t) {
     return is_gt_(sig(FORWARD(t)), ref(FORWARD(t)));
  };
};


/// reset incoming signal to 0
auto reset_ = []() {
  bool bStart;
  int rTime;
  bStart = false;
  rTime = 0;
  return [=](int &&t) mutable {
    if (!bStart)
      {
        bStart = true;
        rTime = t;
      }
    return t - rTime;
  };
};

/// Generate Oscillator from incoming Time at rate x times per second
auto osc_ = [](float &&x) {
  //divide incoming time in milliseconds, convert to radians, calculate sine
  return hana::compose (sin_, to_radians_, divide_by_ (1000.0 / (360.0 * x)));
};

/// Generate Impulse from incoming Time at rate x times per second
auto impulse_ = [](float &&x) {
  //find modulus of incoming signal % 1000/x, check if current value is less than previous
  return hana::compose (trigger_on_reset_ (),
                        mod_ (1000.0 / static_cast<float &&> (x)));
};

/// Generate Ramp UP output from incoming Time at rate x times per second
auto ramp_ = [](float &&x) {
  ///map to value in range of [0,1)
  auto tmp = 1000.0 / x;
  return hana::compose (divide_by_ (tmp), mod_ (tmp));
};



/// Generate Ramp DOWN output from incoming Time at rate x times per second
auto rampdown_ = [](auto &&x) {
  ///map to value in range of [0,1)
  auto tmp = 1000.0 / x;
  return hana::compose (subtract_from_ (1), divide_by_ (tmp), mod_ (tmp));
};


/// Generate Sawtooth output from incoming Time at rate x times per second
auto saw_ = [](auto &&x) {
  //map to value in range of (-1,1)
  auto tmp = 1000.0 / x;

  auto f = [=](auto &&t) {
    auto up = t / tmp;
    auto down = 1 - up;
    return (4 * (up < .5 ? up : down)) - 1;
  };

  return hana::compose (f, mod_ (tmp));
};

/// These signal functions return functions of time
/// constant signal function  @todo should all return optional value?
auto constant_ = [](auto &&x) {
  using T = TYPE (x);
  return [=](auto &&... xs) {
    return x;  //maybe<T>(x);
  };
};


/// Generate single linear ramp over nseconds (switches to constant 1 at first reset)
auto linear_ = [](float &&x) {
  return switch_once_ (
    //sf1 pipes: a reset into a ramp up and splits into just the signal and a trigger
    hana::pipe_ (reset_ (), ramp_ (1.0 / x),
                 hana::split_ (id_, trigger_on_reset_ ())),
    //when the trigger returns true, this sf starts being called on the input
    [](auto &&xs) { return constant_ (1.0); });
};

///takes a func of range [0,1] and evaluates over nsec (compose with every_ to make over_)
auto eval_over_ = [](auto &&nsec, auto &&func) {
  return hana::compose (FORWARD (func), linear_ (FORWARD (nsec)));
};

///calls func of range [1,0] @todo see linear_ and fix this
auto overdown_ = [](auto &&sec, auto &&func) {
  return hana::compose (func, rampdown_ (1.f / sec), reset_ ());
};



/// Event stream
auto triggerval_ = [](auto &&bFlag) {

  bool bSet = false;

  return [=, &bFlag](auto &&... xs) mutable {
    if (bFlag && !bSet)
      {
        bSet = true;
        return maybe<bool> (true);
      }
    else if (!bFlag)
      {
        bSet = false;
      }
    return maybe<bool> ();
  };

};

/// Event stream -- returns a true event whenever bFunc() evaluates to true
/// @todo bFunct should take xs ... arguments?
auto trigger_ = [](auto &&bFunc) {

  bool bSet = false;

  return [=](auto &&... xs) mutable {
    if (bFunc () && !bSet)
      {
        bSet = true;
        return maybe<bool> (true);
      }
    else if (!bFunc ())
      {
        bSet = false;
      }
    return maybe<bool> ();
  };

};


}  //ohio::


namespace boost {
namespace hana {

/// Simple signal: f(t) is a function of time t and can return anything
template <typename F>
struct _signal
{
  F f;

  constexpr decltype (auto) operator() (int &&t) const &
  {
    return f (static_cast<int &&> (t));
  }

  constexpr decltype (auto) operator() (int &&t) &
  {
    return f (static_cast<int &&> (t));
  }

  constexpr decltype (auto) operator() (int &&t) &&
  {
    return std::move (f) (static_cast<int &&> (t));
  }
};

detail::create<_signal> signal_{};


/// delay line: millisecond clock
template <typename F>
struct _delay
{
  F f;

  //    using T = TYPE(f(0));
  //    std::array<T> buffer;
};

detail::create<_delay> delay_{};
/*
 = [](auto&& sec){
  return[=](auto&& func) {
    using T = TYPE(func(0)); //hmmm
    int len = sec*1000;
    int idx = 0;
    bool bStart = false;
    T buffer[len];
    return[=](auto&& xs) mutable{
        auto tmp = buffer[idx];
        buffer[idx] = func(xs);
        idx++;
        if (idx==len) idx = 0;
        if (bStart) return maybe<T>(tmp);
        if (idx==len && !bStart) bStart = true;
        return maybe<T>();
    };
  };
};
*/
}
}  //boost::hana::

#endif /* end of include guard: OHIO_SIGNAL_INCLUDED */
