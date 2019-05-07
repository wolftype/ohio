#ifndef OHIO_THREAD_INCLUDED
#define OHIO_THREAD_INCLUDED


#include <thread>
#include <future>
#include <iostream>
#include <boost/hana/functional/fix.hpp>

namespace ohio {

using std::cout;
using std::endl;
/*-----------------------------------------------------------------------------
 *  Threading functions returning and listening to future values
 *-----------------------------------------------------------------------------*/

/// start function in a new thread and detach, returning true
auto thread_ = [](auto &&f) {
  //forward
  return [=](auto &&... xs) {
    std::thread (f,
                 std::forward<typename std::decay<decltype (xs)>::type> (xs)...)
      .detach ();
    return true;
  };
};

/// start function in a new thread and detach, returning the future value
/// see future_pipe_ and repeat_pipe_ for an example
auto thread_future_ = [](auto &&f) {
  //forward
  return [=](auto &&... xs) {
    using T = TYPE (FORWARD (f) (FORWARD (xs)...));
    std::packaged_task<T (TYPE (xs)...)> task (f);
    std::shared_future<T> fut = task.get_future ();
    std::thread (std::move (task), FORWARD (xs)...).detach ();
    return fut;
  };
};

/// start function in a new thread and detach, returning true
auto thread_args_ = [](auto &&f, auto &&... xs) {
  //forward
  return [=]() mutable {
    std::thread (FORWARD (f), FORWARD (xs)...).detach ();
    return true;
  };
};

/// start async function in a new thread
/// returns shared future immediately, can be passed to multiple listeners that wait for it
/// T may be a function itself
/// todo -- add a deferred only policy?
auto async_ = [](auto &&f) {
  //using F = typename std::decay< decltype(f) >::type;
  return [=](auto &&... xs) {
    using T = decltype (
      f (std::forward<typename std::decay<decltype (xs)>::type> (xs)...));
    cout << "launching shared async thread" << endl;
    std::shared_future<T> sf =
      std::async (f, std::forward<typename std::decay<decltype (xs)>::type> (
                       xs)...);
    return sf;
    //std::shared_future<T> (
    //  std::async( f, std::forward< typename std::decay< decltype(xs) >::type > (xs)...) );
  };
};

/// start async function lazy or synchronous, returns non-shared future fetchable with .get()
auto asyncf_ = [](auto &&f) {
  return [=](auto &&... xs) {
    cout << "launching non-shared async thread" << endl;
    return std::async (f,
                       std::forward<typename std::decay<decltype (xs)>::type> (
                         xs)...);
  };
};

/// start async function lazy, returns non-shared future, triggered with .get()
auto dasync_ = [](auto &&f) {
  return [=](auto &&... xs) {
    cout << "launching deferred thread" << endl;
    using T = decltype (
      f (std::forward<typename std::decay<decltype (xs)>::type> (xs)...));
    return std::shared_future<T> (
      std::async (std::launch::deferred, f,
                  std::forward<typename std::decay<decltype (xs)>::type> (
                    xs)...));
  };
};

auto rasync_ = [](auto &&f) {
  async_ (std::forward<typename std::decay<decltype (f)>::type> (f));
};


/// await a future variable then do something with it
//  launch this in another thread with thread_args_ or thread_future
//  see repeat_pipe_ for an example
auto future_then_ = [](auto &&f) {
  return [=](auto &&fut) mutable {
    auto x = fut.get ();
    return f (x);
  };
};

/// await a future variable then do something WITHOUT it
//  launch this in another thread with thread_args_
//  see repeat_pipe_ for an example
auto future_when_ = [](auto &&f, auto &&... xs) {
  return [=](auto &&fut) mutable -> decltype (f (xs...)) {
    auto x = fut.get ();
    return f (xs...);
  };
};

// pipe result of f1 into f2 once it's available
auto future_pipe_ = [](auto &&f1, auto &&f2) {
  return [=](auto &&... xs) mutable {
    auto fut = thread_future_ (f1);
    auto then = future_then_ (f2);
    return pipe_ (fut, then) (xs...);
  };
};


//detached repeat: f1 is called in another thread.  when it returns,
//result is fed into f2 and the whole process repeats
auto repeat_pipe_ = hana::fix (
  [](auto self, auto f1, auto f2) -> std::shared_future<decltype (f2 (f1 ()))> {
    //First launch f1, the variable fut will hold the return value once it's ready
    auto fut = thread_future_ (f1) ();
    // Two things await this future:
    //A. a function that, when passed a future, will call f2 on it once it's ready
    auto func = future_then_ (f2);
    //B. a function that, when passed a future, will call whatever once it's ready
    auto repeat = future_when_ (self, f1, f2);
    //Now, launch a thread to repeat this whole shebang
    thread_args_ (repeat, fut) ();
    //and now launch a thread that awaits the future and calls f2 with the result
    auto fut2 = thread_future_ (func) (fut);
    return fut2;
  });

}  // ohio::
#endif /* end of include guard: OHIO_THREAD_INCLUDED */
