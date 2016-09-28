/*
 * =====================================================================================
 *
 *       Filename:  memory.cpp
 *
 *    Description:  testing for memory leaks
 *
 *        Version:  1.0
 *        Created:  05/20/2016 19:05:22
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
using namespace std;


  ///starts clock at 0
  auto reset4_ = [](){
    cout << "init reset" << endl;

    bool bStart;
    int rTime;

    bStart = false;
    rTime = 0;
    return [=](int&& t) mutable {
      if (!bStart){
        bStart = true; rTime = t;
        cout << "STARTED at " << t << " " << endl;  /// prints if first time
      }
      return t-rTime;
    };
  };

  struct reset__ {

    bool bStart = false;
    int rTime = 0;

    int operator () (int&& t){
      if (!bStart){
        bStart = true; rTime = t;
        cout << "STARTED at " << t << " " << endl;  /// prints if first time
      }
      return t-rTime;
    }
  };


  auto after4_ = [](auto&& sec, auto&& e){
    using T = typename std::decay< decltype(e) >::type;
    auto reset = reset__();
    return[=](int&& xs) mutable -> maybe<T> {
      if ( reset( std::forward<int>(xs) ) > (sec * 1000) ) {
        return maybe<T>(std::forward<T>(e));
      }
      else return maybe<T>();
    };
  };


  /// timed event, counts down in millisecond clock then triggers an event of type e
  auto every4_ = [](auto&& sec, auto&& e){
    using T = typename std::decay< decltype(e) >::type;
    using S = typename std::decay< decltype(sec)>::type;
    auto imp = impulse_( 1.0/sec );
    auto te = e;
    return[=](auto&& t) mutable -> maybe<T> {
      using F = typename std::decay<decltype(t)>::type;
      if ( imp( std::forward< F >(t)) ){
        return maybe<T>( std::forward<T>(e) );
      }
      else {
        return maybe<T>();
      }
    };
  };


   /// not using reference of maybe...
  auto when4_ = [](auto&& e, auto&& func){

    auto ce = e(0);
    using T = decltype( func( MVAL( ce ) ) );
    auto my = maybe<T>();
    auto mn = maybe<T>();

    return [=](int&& xs) mutable -> maybe<T> {
      auto te = e( FORWARD(xs) ); // no ref returned
      if (te) {
        my = maybe<T>( FORWARD(func)(*(FORWARD(e)( FORWARD(xs) ))) );
        return my;
      }
      my = maybe<T>();
      return mn;
    };
  };




///takes a func of range [0,1] and maps to time sec
auto over4_ = [](auto&& sec, auto&& func){
  using F = typename std::decay<decltype(func)>::type;
  return hana::compose( std::forward<F>(func), ramp_(1.f/sec), reset4_() );
};


int main(){

  AppStartTime = now();

  bool bTrigger = false;

  auto print = [](auto&& t){ cout << t << endl; return true; };
  auto trigger = [&](){ bTrigger = !bTrigger; return true; };
//
  auto reset = reset4_();
  auto e1 = ohio::tag_( triggerval_( bTrigger ), print );
  auto e2 = every_(.1, constant_(trigger) );
  auto e3 = after_(5, constant_(trigger) );

  auto e4 = over4_(3, [](float t){ cout << t << endl; return true; } );
  auto e5 = every4_(.5,e4);
//  auto e6 = after4_(.2,e4); /// this does not work yet
//
  auto b = behavior();
  b.launch(e1);
//
//
  auto f = when4_( e5, proc_ );

  while ( wait_(.001)() ){
   // f( time_() );
    e5( time_() );// << endl;
//      e2( time_() );
   }


}
