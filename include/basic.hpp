/*
 * =====================================================================================
 *
 *       Filename:  basic.hpp
 *
 *    Description:  basic lifts onto signals
 *
 *        Version:  1.0
 *        Created:  03/11/2016 20:00:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#ifndef basic_INC
#define basic_INC

#include <math.h>
#include <iostream>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>
#include <experimental/optional>

#include "time.hpp"
#include "macros.hpp"

namespace hana = boost::hana;
namespace ohio {

template <class T>
using maybe = std::experimental::optional<T>;

template <class T>
decltype (auto) maybeValue (T &&f)
{
  return FORWARD (f);
}

template <class T>
decltype (auto) maybeValue (maybe<T> &&m)
{
  if (m)
    return maybeValue (*m);
  return maybeValue (T ());
}

// apply f
auto transform_ = [](auto &&f) {
  return [=](auto &&x) { return hana::transform (x, f); };
};


/// Return Zero
auto zero_ = [](auto &&... xs) { return 0; };

/// Call a function
auto do_ = [](auto &&f) { return f (); };

/// send time through process f
auto proc_old_ = [](auto &&f) { return FORWARD (f) (time_ ()); };

struct proc_t
{

  template <class F>
  constexpr decltype (auto) operator() (F &&f) const &
  {
    return FORWARD (f) (time_ ());
  }
};

proc_t proc_;

/*-----------------------------------------------------------------------------
 *  Basic functions a -> a
*-----------------------------------------------------------------------------*/
/// Sine
auto sin_ = [](float t) { return sin (t); };

/// Takes an X in degrees and returns in radians
auto to_radians_ = [](auto &&x) { return x * (3.14 / 180.0); };

/// Integer Modulus (e.g. for ramps)
auto mod_ = [](auto &&x) {
  return [=](auto &&t) { return (int) t % (int) x; };
};

/*
/// Linear clamp
auto linear_ = [](float&& sec){
    return[=](int&& x){
        auto val = ((float)x/1000.0)/sec;
        return val > 1 ? 1 : val;
    };
};
*/


auto secs_to_milli = [](auto &&secs) { return secs * 1000; };
auto secs_to_micro = [](auto &&secs) { return secs * 1000000; };


/*-----------------------------------------------------------------------------
 *  Curried Signals
 *-----------------------------------------------------------------------------*/

/// Signal eq_(<X>) Takes a Y and Compares it with X
auto eq_ = [](auto &&x) {
  return [=](auto &&y) { return y == x ? true : false; };
};

/// Signal gt_(<X>) Takes a Y and Compares it with X
auto gt_ = [](auto &&x) {
  return [=](auto &&y) { return y > x ? true : false; };
};

/// Signal lt_(<X>) Takes a Y and Compares it with X
auto lt_ = [](auto &&x) {
  return [=](auto &&y) { return y < x ? true : false; };
};

/// Signal divide_by_(<X>) Takes a Y and divides it by X
auto divide_by_ = [](auto &&x) { return [=](auto &&y) { return y / x; }; };

/// Arithmetic  (basic lifting)
auto div_ = [](auto &&x) { return [=](auto &&y) { return y / x; }; };
auto min_ = [](auto &&x) { return [=](auto &&y) { return y - x; }; };
auto plus_ = [](auto &&x) { return [=](auto &&y) { return y + x; }; };
auto mult_ = [](auto &&x) { return [=](auto &&y) { return x * y; }; };
auto subtract_from_ = [](auto &&x) { return [=](auto &&y) { return x - y; }; };

/*-----------------------------------------------------------------------------
 *  /// binary operations (use with merge_ to combine two streams)
 *-----------------------------------------------------------------------------*/
auto xplus_ = [](auto &&a, auto &&b) { return a + b; };

auto xdiv_ = [](auto &&a, auto &&b) { return a / b; };

auto xmult_ = [](auto &&a, auto &&b) { return a * b; };

auto xadd_ = [](auto &&a) { return [=](auto &&b) { return plus_ (a, b); }; };

auto clamp_ = [](auto &&min, auto &&max) {
  return [=](auto &&b) { return b < min ? min : b > max ? max : b; };
};


}  // ohio::

#endif /* ----- #ifndef basic_INC  ----- */
