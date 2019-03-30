/*
 * =====================================================================================
 *
 *       Filename:  reference.cpp
 *
 *    Description:  what is retained -- also some questions:

            Some differences here: rather than return maybe events which
            must then be evaluated, the signals here return the evaluated
            events (or else nothing).  In previous implementations,
            the template in maybe<T> was the event e itself, and not the
            evaluated e(t) event.  this led to the need to return references
            to events.

            Q: how to manage feedback (e.g. build a neuron with delay and recurrence)
 *
 *        Version:  1.0
 *        Created:  06/30/2016 21:01:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#include <iostream>
#include <experimental/optional>

#include "signal.hpp"

using namespace std;
using namespace ohio;
using namespace boost::hana;

/*
/// may or may not contain a value
template<typename T>
struct maybe_{

    T&& val = 0;// = nullptr;
    bool engaged = false;
    maybe_(){}
    maybe_(T&& t) : val(static_cast<T&&>(t)), engaged(true){
      //  val = std::move(static_cast<T&&>(t));
    }
    maybe_(const T& t) : engaged(true){
    //    val = t;
    }
    operator bool() const{
        if (engaged) return true;
        return false;
    }
    T& operator *(){ return val; }
};
*/
struct reset_s_
{
  bool bStart = true;
  int rTime = 0;

  //const& return type not allowed because we are modifying members
  //    constexpr decltype(auto) operator()(int&& t) const& {
  //        if (bStart){
  //          bStart = false; rTime = t;
  //          cout << "STARTED at " << t << " " << endl;  /// prints if first time
  //        }
  //        return t-rTime;
  //      };

  constexpr decltype (auto) operator() (int &&t) &
  {
    if (bStart)
      {
        bStart = false;
        rTime = t;
        cout << "STARTED at " << t << " " << endl;  /// prints if first time
      }
    cout << "continue..." << endl;
    return t - rTime;
  };

  constexpr decltype (auto) operator() (int &&t) &&
  {
    if (bStart)
      {
        bStart = false;
        rTime = t;
        cout << "STARTED at " << t << " " << endl;  /// prints if first time
      }
    cout << "continue..." << endl;
    return t - std::move (rTime);
  };
};

/// Reset Clock
auto reset2_ = []() {
  cout << "init reset" << endl;

  bool bStart;
  int rTime;

  bStart = true;
  rTime = 0;
  return [=](int &&t) mutable {
    if (bStart)
      {
        bStart = false;
        rTime = t;
        cout << "STARTED at " << t << " " << endl;  /// prints if first time
      }
    return t - rTime;
  };
};

/// whenever e returns true, evaluate func, otherwise return nothing
auto when_ = [](auto &&e, auto &&func) {

  return [=](int &&xs) mutable {
    using T = decltype (FORWARD (func) (FORWARD (xs)));
    if (FORWARD (e) (FORWARD (xs)))
      {
        return maybe<T> (FORWARD (func) (FORWARD (xs)));
      }
    return maybe<T> ();
  };
};

/// after nsec, start returning e(t).
auto after_ = [](auto &&nsec, auto &&e) {
  auto reset = reset_ ();
  return [=](int &&xs) mutable {
    using T = TYPE (FORWARD (e) (FORWARD (xs)));
    if (reset (std::forward<int> (xs)) > (nsec * 1000))
      {
        return maybe<T> (FORWARD (e) (FORWARD (xs)));
      }
    else
      return maybe<T> ();
  };
};

/// at nsecs, evaluate e(t) once
auto at_ = [](auto &&sec, auto &&e) {
  auto reset = reset_ ();
  bool bDone;
  bDone = false;
  return [=](int &&xs) mutable {
    using T = TYPE (FORWARD (e) (FORWARD (xs)));
    if (reset (std::forward<int> (xs)) > (sec * 1000) && !bDone)
      {
        bDone = true;
        return maybe<T> (FORWARD (e) (FORWARD (xs)));
      }
    return maybe<T> ();
  };
};

/// once e1 returns true, evaluate e2 once
auto once_ = [](auto &&e1, auto &&e2) {
  bool bDone;
  bDone = false;
  return [=](int &&xs) mutable {
    using T = TYPE (FORWARD (e2) (FORWARD (xs)));
    if (FORWARD (e1) (FORWARD (xs)) && !bDone)
      {
        bDone = true;
        return maybe<T> (FORWARD (e2) (FORWARD (xs)));
      }
    return maybe<T> ();
  };
};

/// every nsecs call e(t)
auto every_ = [](auto &&nsecs, auto &&e) {
  return when_ (impulse_ (1.0 / nsecs), FORWARD (e));
};

/// call func over duration at rate
auto over_ = [](auto &&duration, auto &&rate, auto &&func) {
  return every_ (FORWARD (rate),
                 eval_over_ (FORWARD (duration), FORWARD (func)));
};

int main ()
{

  AppStartTime = now ();


  // after 2 seconds, for 3 seconds, at a frequency of .1 seconds:
  auto e8 = after_ (2, over_ (3, .1, [](float t) {
                      cout << t << endl;
                      return t;
                    }));

  auto e2 = e8(6000);

  if (e2)
    printf ("yes");
  else
    printf ("no");

//    once_ (trigger_on_gt_ (e8, just_(.8)), [](auto &&xs) { printf ("success"); return true; });
//
  auto tick = wait_ (.001);
  while (tick ())
    {
      e8 (time_ ());
//      e2 (time_ ());
    }

  return 0;
}
