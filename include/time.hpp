/*
 * =====================================================================================
 *
 *       Filename:  time.hpp
 *
 *    Description:  timers
 *
 *        Version:  1.0
 *        Created:  03/11/2016 19:42:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#ifndef  time_INC
#define  time_INC

#include <map>
#include <thread>

namespace ohio {

/*-----------------------------------------------------------------------------
 *  std::chrono TIME FUNCTIONS
 *-----------------------------------------------------------------------------*/

/// Global variable set at initiation of Application
std::chrono::time_point<std::chrono::steady_clock> AppStartTime;

std::map< std::string, int> inputMap;

/// Get Time Now
auto now() { return std::chrono::steady_clock::now(); }

/// convert duration into milliseconds
auto milliseconds = []( auto duration ) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
};

/// convert duration into microseconds
auto microseconds = []( auto duration ){
  return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
};

/// Return Time Since App Start in milliseconds
auto time_ = []() -> int { return milliseconds( now() - AppStartTime ); };


/*-----------------------------------------------------------------------------
 *  std::this_thread Sleep and Atomic Ticker
 *-----------------------------------------------------------------------------*/

/// little lambda that yields
auto sleep_for_ = [](float sec)
{
    int milli = sec * 1000;
    std::this_thread::sleep_for( std::chrono::milliseconds(milli) );
};

/// make a function that will sleep when called
auto wait_ = [](float sec){
  return [=](){
    sleep_for_(sec);
    return true;
  };
};

/// pass to functions that do not wait
//auto true_ = [](){ return true; };

/// pass to functions that never call
auto false_ = [](){ return false; };


/*-----------------------------------------------------------------------------
 *  Atomic CLOCK tick tocker
 *-----------------------------------------------------------------------------*/
/// a tick that flips atomic value and sleeps
auto tckr_ = [](auto&& sec, auto&& t) {
  return [&](){
    t = !t;
    sleep_for_( sec );
    return t ? true : false;
  };
};







/*-----------------------------------------------------------------------------
 *  in place sequencing
 *-----------------------------------------------------------------------------*/
//auto after_ =[](float&& sec, auto&& b){
//  return seq_( wait_(sec), event_(b) );
//};

/// for n seconds, repeat f
auto loop_for_ = [](float sec, auto&& f){
  return[=](auto&& ... xs){
    int milli = sec * 1000;
    auto start = now();
    auto end = start + std::chrono::milliseconds(milli);
    while ( now() < end ){
      f( xs...); ///<  @todo add std::forward and std::decay
    }
    return f(xs...);
  };
};


/// loop function f forever at rate of clock()
auto loop_always_ = [](auto&& f, auto&& clock){
  return [=](auto&& ... xs) mutable {
    while( clock() ){ // sleep_for_ is embedded in clock()
      f(xs...); // add perfect forward
    }
    return true;
  };
};

/// loop function f forever at rate of clock() --- no args for now
/// because of instantiation issue (see fwd_future_args in event.hpp)
auto loop_always_no_args_ = [](auto&& f, auto&& clock){
  return [=]() mutable {
    while( clock() ){ // sleep_for_ is embedded in clock()
      f(); // add perfect forward
    }
    return true;
  };
};

/// repeat function f every n seconds
auto loop_repeat_ = [](float sec, auto&& f){
  return loop_always_( f, wait_(sec) );
};

/// execute function f when event e is triggered
auto do_when_ = [](auto&& e, auto&& f){
  return[=](){
    while (!e()) {} //poll rate sleep_for_ is embedded in e()
    return f();
  };
};

/// do signal function f n times
auto do_ntimes_ = [](int n, auto&& f){
  return [=](){
    int tmp = n;
    do{
      f(); // f may sleep
      tmp-=1;
    } while (tmp>0);
  };
};

}; //ohio::


#endif   /* ----- #ifndef time_INC  ----- */
