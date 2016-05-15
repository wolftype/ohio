/*
 * =====================================================================================
 *
 *       Filename:  signals.cpp
 *
 *    Description:  signal generators
 *
 *        Version:  1.0
 *        Created:  05/06/2016 20:17:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#include "ohio.hpp"

using namespace ohio;

  auto when2_ = [](auto&& e, auto&& func){
  
    auto ce = e(0);
    using T = decltype( func( MVAL( ce ) ) );
    auto my = maybe<T>();
    auto mn = maybe<T>();
    
    return [=](int&& xs) mutable -> maybe<T>& {
      auto& te = FORWARD(e)( FORWARD(xs) );
      if (te) {
        my = maybe<T>( FORWARD(func)(*te) );
        return my;
      }
      return mn;
    };
  };


/// send time through process f
auto proc2_ = [](auto&& f){
  using F = typename std::decay<decltype(f)>::type;
  return std::forward<F>(f)( time_() );
};


auto test_ = [](auto&& v){
  int x = v;
  return [=](auto&& ... t) mutable{
     x+=1;
     cout << x << endl;
    return x;
  };
};


int main(){

  AppStartTime = now();  
  
  auto go_func_ = [](auto&& t){
    cout << t << endl;
    return true;
  };

  auto sigfun = map_(5, go_func_);

  auto e = every_(.5, test_(2) );

  auto w = when2_(e, proc2_);

 // behavior b;
 // b.launch( e );

  while ( wait_(.001)() ){
   auto& x =  w( time_() );
  }

  return 0;
};
