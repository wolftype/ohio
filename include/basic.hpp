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


#ifndef  basic_INC
#define  basic_INC

#include <math.h>
#include <iostream>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>
#include <experimental/optional>

#include "time.hpp"
#include "macros.hpp"


namespace hana = boost::hana;
namespace ohio {

template<class T>
using maybe = std::experimental::optional<T>;


auto print_ = [](auto&& xs){
  std::cout << "printing: " << xs << std::endl;
  return true;
};

auto printall_ = [](auto&& ... xs) {
  return hana::transform( hana::make_tuple(xs...),  print_);
};

auto zero_ = [](auto&& ... xs){
  return 0;
};

auto do_ = [](auto&& f){
  return f();
};

/// send time through process f
auto proc_ = [](auto&& f){
  return f( time_() );
};

/*-----------------------------------------------------------------------------
 *  Basic functions a -> a
*-----------------------------------------------------------------------------*/
/// Sine
auto sin_ = [](float t){
  return sin(t);
};

/// Takes an X in degrees and returns in radians
auto to_radians_ = [](auto&& x){ return x * (3.14/180.0); };

/// Integer Modulus (e.g. for ramps)
auto mod_ = [](int&& x){
  return [=](int&& t){
     return t % x;
  };
};

auto secs_to_milli = [](auto&& secs){ return secs * 1000; };
auto secs_to_micro = [](auto&& secs){ return secs * 1000000; };


/*-----------------------------------------------------------------------------
 *  Curried Signals
 *-----------------------------------------------------------------------------*/

/// Signal eq_(<X>) Takes a Y and Compares it with X
auto eq_ = [](auto&& x){
  return[=](auto&& y){ return x==y ? true : false; };
};

/// Signal gt_(<X>) Takes a Y and Compares it with X
auto gt_ = [](auto&& x){
  return [=](auto&& y){ return y>x ? true : false; };
};

/// Signal lt_(<X>) Takes a Y and Compares it with X
auto lt_ = [](auto&& x){
  return [=](auto&& y){ return y<x ? true : false; };
};

/// Signal divide_by_(<X>) Takes a Y and divides it by X
auto divide_by_ = [](auto&& x){
  return [=](auto&& y){ return y/x; };
};

/// Signal subtract from (<X>) takes a Y and subtracts it from X
auto subtract_from_ = [](auto&& X){
  return [=](auto&& y){ return X-y; };
};

/*-----------------------------------------------------------------------------
 *  /// binary operations (use with merge_ to combine two streams)
 *-----------------------------------------------------------------------------*/
auto plus_ = [](auto&& a, auto&& b){
  return a + b;
};

auto div_ = [](auto&& a, auto&& b){
  return a / b;
};

auto mult_ = [](auto&& a, auto&& b){
  return a * b;
};




} // ohio::

#endif   /* ----- #ifndef basic_INC  ----- */
