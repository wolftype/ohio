#ifndef OHIO_THREAD_INCLUDED
#define OHIO_THREAD_INCLUDED


#include <thread>
#include <future>
#include <iostream>

namespace ohio{

using std::cout;
using std::endl;
/*-----------------------------------------------------------------------------
 *  Threading functions returning and listening to future values
 *-----------------------------------------------------------------------------*/

/// start function in a new thread and detach, returning true
auto thread_ = [](auto&& f){
  //forward
  return [=](auto&& ... xs){
    std::thread(f, std::forward< typename std::decay< decltype(xs) >::type > (xs)...).detach();
    return true;
  };
};

/// start function in a new thread and detach, returning the future value
auto thread_future_ = [](auto&& f){
  //forward
  return [=](auto&& ... xs){
    using T = TYPE(f(xs...));
    std::packaged_task<T(TYPE(xs)...)> task(f);
    std::future<T> fut = task.get_future();
    std::thread(std::move(task)).detach();
    return fut;
  };
};

/// start function in a new thread and detach, returning true
auto thread_args_ = [](auto&& f, auto&& ... xs){
  //forward
  return [=]() mutable {
    std::thread(f, std::forward< typename std::decay< decltype(xs) >::type > (xs)...).detach();
    return true;
  };
};

/// start async function in a new thread
/// returns shared future immediately, can be passed to multiple listeners that wait for it
/// T may be a function itself
/// todo -- add a deferred only policy?
auto async_ = [](auto&& f){
  //using F = typename std::decay< decltype(f) >::type;
  return [=](auto&& ... xs) {
    using T = decltype( f(std::forward< typename std::decay< decltype(xs) >::type > (xs)...) );
    cout << "launching shared async thread" << endl;
    std::shared_future<T> sf = std::async( f, std::forward< typename std::decay< decltype(xs) >::type > (xs)...);
    return sf;
    //std::shared_future<T> (
    //  std::async( f, std::forward< typename std::decay< decltype(xs) >::type > (xs)...) );
  };
};

/// start async function lazy or synchronous, returns non-shared future fetchable with .get()
auto asyncf_ = [](auto&& f){
  return [=](auto&& ... xs) {
    cout << "launching non-shared async thread" << endl;
    return std::async( f, std::forward< typename std::decay< decltype(xs) >::type > (xs)...);
  };
};

/// start async function lazy, returns non-shared future, triggered with .get()
auto dasync_ = [](auto&& f){
  return [=](auto&& ... xs){
    cout << "launching deferred thread" << endl;
    using T = decltype( f(std::forward< typename std::decay< decltype(xs) >::type > (xs)...) );
    return std::shared_future<T> ( std::async( std::launch::deferred, f, std::forward< typename std::decay< decltype(xs) >::type > (xs)...) );
  };
};


/// await a future variable then do something with it
auto thread_then_ =[](auto&& f){
  return [=](auto&& fut){
    return f( fut.get() );
  };
};

auto rasync_ = [](auto&& f){
  async_( std::forward< typename std::decay< decltype(f)>::type >(f) );
};


/// listen on another thread
//auto listen_ = [](){
//  return async_( then_(id_) );
//};

} // ohio::
#endif /* end of include guard: OHIO_THREAD_INCLUDED */
