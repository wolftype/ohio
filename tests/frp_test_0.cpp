/*
 * =============================================================================
 * ohio: arrowized functional reactive programming
 * Copyright (C) 2016  Pablo Colapinto
 * All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * =============================================================================
 * =============================================================================
 *
 *       Filename:  stack2.cpp
 *
 *    Description: frp omg
 *
 *        Version:  1.0
 *        Created:  03/05/2016 17:41:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =============================================================================
 */
/// @filename
///
/// sf is a signal function -- a type that takes a time-varying signal and
/// returns another time-varying signal
/// ... xs is typically the time inputs to a signal
///
/// in the simplest case a signal takes a real-valued number and returns a value
/// examples are sin_, mult_by_,
/// @todo use std::optional for runtime maybes
/// @todo perfect forward the functions

#include <math.h>
#include <map>

#include <thread>
#include <future>
#include <iostream>
#include <vector>
#include <functional>
#include "basic.hpp"
#include "arrows.hpp"
#include "time.hpp"
#include "thread.hpp"
#include "event.hpp"
#include "signal.hpp"


using std::cout;
using std::endl;

using namespace ohio;

namespace hana = boost::hana;


/// Evaluate Input
auto is_true_ = [](auto&& x) { return [&](){ return x ? true : false;}; };



/// check if optional value is real or empty
auto has_value_ = [](auto&& opt){
  return [&](){
    return opt ? true : false;
  };
};

/// given an optional, get value
auto get_optional_value_ = [](auto&& opt){
  return *opt;
};



/// do signal function f n times
/// hana::fold_right as a sequence of functions: f(a, f(b, f(c, f(d, e))))
auto sequence_ = [](auto&& ... fs){
  return hana::fold_right( hana::make_tuple(fs...), seq_);
};

/// continuous function of arguments xs, say of time, for instance behavior_(sin_)
auto behavior_ = [](auto&& f){
  return [=](auto&& ... xs){
    return f( std::forward< typename std::decay< decltype(xs) >::type > (xs)... );
  };
};




template<class T>
struct _vel {
  T tmp;
  decltype( listener_( stdin_ ) ) e = listener_( stdin_ );

  template<class X, class ... XS>
  decltype(auto) operator() (X&& x, XS&& ... xs){
    auto etag = tag_( e(), constant_(tmp+1) );
    tmp = rswitch_( constant_(tmp) )( hana::make_pair( static_cast<X&&>(x), etag) );/// if triggered, call trigger function
    return tmp;
  }
};

auto fwd_event = [](auto&& f){
  auto e = f;
  return [=](auto&& ... xs) mutable {
    return e();
  };
};

auto vel_ = [](auto&& v, auto&& f){
  using F = typename std::decay< decltype(f) >::type;
  fwd_event( listener_( std::forward<F>(f) ) );

  auto e = listener_( std::forward<F>(f) );
  float tmp=v;
  return [=](auto&& x, auto&&...xs) mutable {
    auto etag = tag_( e(), constant_(tmp+1) );
    tmp = rswitch_( constant_(tmp) )( hana::make_pair( x, etag) );/// if triggered, call trigger function
    return tmp;
  };
};

auto trigger_val_ = [](auto&& v, auto&& f, auto&& r){
  using F = typename std::decay< decltype(f) >::type;
  fwd_event( listener_( std::forward<F>(f) ) );

  auto e = listener_( std::forward<F>(f) );
  float tmp=v;
  return [=](auto&&...xs) mutable {
    //auto etag = tag_( e(), constant_(r(tmp)) );
    //tmp = rswitch_( constant_(tmp) )( hana::make_pair( x, etag) );/// if triggered, call trigger function
    if (e()) tmp = r(tmp);
    return tmp;
  };
};

template<class R, class ... Xs>
struct _process {

  virtual R operator()(Xs&& ... xs) const& =0;
  virtual R operator()(Xs&& ... xs) & =0;
  virtual R operator()(Xs&& ... xs) && =0;
};

constexpr hana::detail::create<_process> process{};

template<class F, class ... Ts>
struct _calc : _process<int, Ts... >{//int, int> {
  F func;

  template<class ... Xs>
  _calc(F f, Xs...) : func(f){}

  int operator()(int&& a, int&& b) const& {
    return func(static_cast<int&&>(a), static_cast<int&&>(b));
  }
  int operator()(int&& a, int&& b) & {
    return func(static_cast<int&&>(a), static_cast<int&&>(b));
  }
  int operator()(int&& a, int&& b) && {
    return func(static_cast<int&&>(a), static_cast<int&&>(b));
  }
};

constexpr hana::detail::create<_calc> calc{};

auto plus = calc( [](auto&& x, auto&& y){ return x+y; }, 1, 2 );
auto mult = calc( [](auto&& x, auto&& y){ return x*y; }, 1, 2 );

auto do_ = [](auto&& str, auto&& ... xs){
  using proc = _process<int,int,int>;
  if (str=="plus") return (proc*)(&plus);
  if (str=="mult") return (proc*)(&mult);
  return (proc*)(&plus);
};



int main(){

    /// Start Clock
    AppStartTime = now();

  //  auto fut = async_(stdin_)();
    auto kbinput = listener_( stdin_ );


    do{

      wait_(.001)();
      auto c = kbinput();
      if (c) cout << (*do_(*c))(1, 2) << endl;

      //cout << v(1) << endl;
      //cout << (1) << endl;

    } while ( true  );


  //CLOCK
  /// Ticker
  std::atomic_bool t;
  t.store(false);

  loop_always_( tckr_(.01, t), wait_(.01) )();

  return 0;
}




/*-----------------------------------------------------------------------------
 *  // OLD NON ATOMIC
 *-----------------------------------------------------------------------------*/
//auto trigger_ = []( std::shared_ptr<bool> t){
//    return [&](){
//      return *(std::atomic_load(&t));
//    };
//};

//auto changed_ = []( std::shared_ptr<int> t ){
//      auto val = std::make_shared<int>(0);//std::atomic_load(&t);
//
//      return [&]() {
//
//        if( *val == *std::atomic_load(&t) ) {
//          return false;
//        } else {
//          //*val = *std::atomic_load(&t);
//          std::atomic_store(&val, t);
//        }
//        return true;
//    };
//};


//auto increment_ = []( std::shared_ptr<int> t ){
//    return [&](){
//      std::shared_ptr<int> tmp = std::atomic_load(&t);
//      (*tmp)++;
//      std::atomic_store(&t, tmp);
//    };
//};


//auto tckr_ = [](float sec, std::shared_ptr<bool> t) {
//
//  return [=,&t](){
//    auto time = now();
//    auto b = std::atomic_load(&t);
//    *b = !*b;
//    std::atomic_store(&t, b);
//    if (*b) printf("use count: %ld\n", t.use_count() );
//    auto lapsed = milliseconds( now() - time );
////    printf("mcs lapsed: %lld\n", lapsed);
//    sleep_for_( sec );// - (lapsed/1000.0) );
//  };
//};


/*-----------------------------------------------------------------------------
 *  OLD SWITCHEROOS
 *-----------------------------------------------------------------------------*/
///// execute function f until event e
//auto until_ = [](auto&& e, auto&& f){
//  return [=](auto&& ... xs){
//    decltype(f(xs...)) x;
//    while ( !e() ){ //poll rate sleep_for_ is to be embedded in e()
//      x = f( xs ... ); // cannot return or else we'll exit loop, ADD perfect foward
//    }
//    return x;
//  };
//};


///// execute a function pair until it evaluates to true and then return its value
//auto switcheroo_ = [](auto&& ef, auto&& f1, auto&& f2){
//  return [=](auto&& ... xs){
//    decltype( f1(xs...) ) x;
//    do {
//      x = f1(xs...);      // perfect forward
//    } while (!ef(x));     // hana::first(x)) ); // e.g. first_( true_ )(x)
//    return f2(x);         // hana::second(x);
//  };
//};

// loop_(f)

//auto do_until_ = [](auto&& f){
//  return switcheroo_(  hana::first, f, hana::second );
//};
//
//auto until_value_ = [](auto&& f){
//  return until_(
//                value_( std::forward< typename std::decay< decltype(f) >::type >(f) ),
//                std::forward< typename std::decay< decltype(f) >::type >(f)
//               );
//};


///// execute signal function sf1 until event e, then execute function sf2
//auto do_while_ = [](auto&& sf1, auto&& sf2 ){
//  return [=](auto&& ... xs){
//    decltype( sf1(xs...) ) x;
//   do {
//      x = sf1( std::forward< typename std::decay< decltype(xs) >::type > (xs)...);
//    } while ( !hana::second(x) );
//
//    return sf2( hana::second(x) );
//  };
//};


//auto ntimes_ = [](int n, auto&& f){
//  return [=](){
//    int tmp = n;
//    do{
//      f(); // f may sleep
//      tmp-=1;
//    } while (tmp>0);
//  };
//};




/*-----------------------------------------------------------------------------
 *  old listen
 *-----------------------------------------------------------------------------*/

 ///// future event
//auto listen_for_ =[](float&& sec){
//  return [&](auto&& f){
//      using T = typename std::decay< decltype( f.get() ) >::type;
//
//      if (f.valid() != false){
//
//       auto res = f.wait_for( std::chrono::milliseconds( (int)(sec*1000) ) );
//        if (res == std::future_status::ready){
//          return std::experimental::optional<T>( f.get() );
//       }
//
//      } else {
//        printf("invalid\n");
//      }
//
//      return std::experimental::optional<T>();
//  };
//};


//auto listen2_ = [](float&& sec){
//  return [&](auto&& fut){
//    return do_until_( listen_for_(  std::forward<float>(sec) ) )( std::forward< typename std::decay< decltype(fut) >::type >(fut) );
//  };
//};
