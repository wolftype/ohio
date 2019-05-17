#ifndef OHIO_EVENT_INCLUDED
#define OHIO_EVENT_INCLUDED

#include <atomic>
#include <sstream>

#include "basic.hpp"
#include "arrows.hpp"
#include "thread.hpp"
#include "time.hpp"
#include "signal.hpp"

/**
  *
  * Events are functions that return maybe types
  *
  * just_ takes a type and makes it a maybe type
  * just_ (constant_(v)) for instance, lifts the value v so
  * that it maybe evaluated
  *
  * NOTE: a lof of these functions pass captured mutable
  * variables but most or all these variable should be
  * shared_ptrs to remain coherent across threads.
  *
  * This will allow them to be called in threads other
  * than the ones in which they are captured.
  *
  */

namespace hana = boost::hana;
namespace ohio {

/// just : Return wrapped Input (e.g bypass) -- id_ : id_(x) -> x
auto just_ = [](auto &&x) { return maybe<TYPE(x)>(x); };


/*-----------------------------------------------------------------------------
   *  SOME io (move to basic.hpp)
   *-----------------------------------------------------------------------------*/
auto stdin_ = [](auto &&... xs) {
  std::string str;
  std::cin >> str;
  return str;
};

auto nothing_ = [](auto &&... xs) { return; };

auto getchar_ = [](auto &&... xs) {
  nothing_ (xs...);
  return getchar ();
};

/// execute signal function sf1 until event e, then execute function sf2
/// sf1 takes an xs... and returns a hana::pair of the result of its own
// calculation and also a maybe event, which when fired is passed to
// sf2 which does something with the event and takes an xs....
//
// example of this:?
auto switch_ = [](auto &&sf1, auto &&sf2) {
  return [=](auto &&... xs) {
    /// calculate pair of outputs
    auto x =
      sf1 (std::forward<typename std::decay<decltype (xs)>::type> (xs)...);
    // if event has happened...
    if (hana::second (x))
      {
        // ...pass event to continuation function, which may do different things depending on its value
        return sf2 (hana::second (x)) (
          std::forward<typename std::decay<decltype (xs)>::type> (xs)...);
      }
    // otherwise return original output
    else
      return hana::first (x);
  };
};



/// recursive switch, pipe a pair into it, second of which is an function event
auto rswitch_ = [](auto &&sf1) {
  return [=](auto &&x) {
    if (hana::second (x))
      {
        return (*hana::second (x)) (hana::first (x));
      }
    else
      return sf1 (hana::first (x));
  };
};

/// tag event e with value b
auto tagbase_ = [](auto &&e, auto &&b) {
  using T = typename std::decay<decltype (b)>::type;
  if (e)
    return maybe<T> (std::forward<T> (b));
  else
    return maybe<T> ();
};

auto tag_ = [](auto &&es, auto &&b) {
  return [=](auto &&... xs) mutable {  //why do we need mutable here?
    return tagbase_ (es (FORWARD (xs)...), b);
  };
};

/*
  auto tag2_ = [](auto&& es, auto&& b){
    auto m = maybe<TYPE(b)>();
    return [=](auto&& ...xs )  mutable -> maybe<TYPE(b)>& {
      m = tagbase_( es( std::forward< typename std::decay< decltype(xs) >::type>(xs)...), b );
      return m;
    };
  };
*/

/*-----------------------------------------------------------------------------
   *  Events triggers
   *-----------------------------------------------------------------------------*/
/// An Event Stream returns an Optional Value as a function of time, either it exists or it doesn't
//auto estream_ =[](const auto& tmp){
auto estream_ = [](auto &&tmp) {
  using T = typename std::decay<decltype (tmp.get ())>::type;
  std::shared_future<T> fut (tmp);
  return [=](auto &&... xs) {

    if (fut.valid () != false)
      {

        auto res = fut.wait_for (std::chrono::milliseconds (0));
        if (res == std::future_status::ready)
          {
            return std::experimental::optional<T> (fut.get ());
          }
      }
    else
      {
        printf ("invalid\n");
      }
    //printf("empty\n");
    return std::experimental::optional<T> ();
  };
};

/// return event contained in future if delivered or else nothing
auto listen_ = [](auto &&fut) {
  using T = typename std::decay<decltype (fut.get ())>::type;
  if (fut.valid () != false)
    {
      auto res = fut.wait_for (std::chrono::milliseconds (1));
      if (res == std::future_status::ready)
        {
          return maybe<T> (fut.get ());
        }
    }
  else
    {
      printf ("invalid\n");
    }
  //printf("empty\n");
  return maybe<T> ();
};

/// forward arguments to avoid linker errors
auto fwd_future = [](auto &&f) {
  std::shared_future<decltype (f ())> fut;
  return [=]() mutable {
    auto t = fut;
    f ();
  };
};

/// forward arguments to avoid linker errors
auto fwd_future_args = [](auto &&f) {
  std::shared_future<decltype (f ())> fut;
  return [=]() mutable {  //changed to include ... args
    auto t = fut;
    printf ("fwd_future_args...\n");
    //f();//xs...);
  };
};

/// forward arguments to avoid linker errors
auto fwd_shared_args = [](auto &&f) {
  using F = typename std::decay<decltype (f)>::type;
  using T = std::shared_ptr<F>;
  T ptr;
  maybe<T> opt;
  return [=]() mutable {  //consider change to include ... args
    auto t = ptr;
    auto m = opt;
    printf ("fwd_shared_args...\n");
  };
};

/// generates new event listener repeatedly
auto listener_ = [](auto &&f) {
  fwd_future_args (f) ();  //! need to predeclare signature
  using F = typename std::decay<decltype (f)>::type;
  using T = typename std::decay<decltype (
    async_ (std::forward<F> (f)) ().get ())>::type;

  std::shared_future<T> fut = async_ (std::forward<F> (f)) ();

  return [=](auto &&... xs) mutable {
    auto opt = listen_ (std::forward<decltype (fut)> (fut));
    if (opt)
      {
        fut = async_ (std::forward<F> (f)) (
          std::forward<typename std::decay<decltype (xs)>::type> (xs)...);
        return opt;
      }
    return maybe<T> ();
  };
};

/// await for results of function f then call function cb on results
/// DEPRECATED use callback_ instead
auto poll_ = [](auto &&f, auto &&cb, float &&pollrate, auto &&... xs) {
  using F = typename std::decay<decltype (f)>::type;
  using CB = typename std::decay<decltype (cb)>::type;

  auto e = listener_ (std::forward<F> (f));  //event

  auto func = [&, e](
    auto &&... ts) mutable {  //copy in listener, but pass cb and udata by ref
    auto result = e (ts...);
    if (result)
      //std::forward<CB>(cb)(*result, std::forward< typename std::decay< decltype(xs)>::type>(xs)...);
      cb (*result,
          std::forward<typename std::decay<decltype (xs)>::type> (xs)...);
  };

  //
  return thread_args_ (loop_always_no_args_ (func, wait_ (pollrate)));
};


/// await for results of function f then call function cb on results
auto poll2_ = [](auto &&f, auto &&cb, float &&pollrate, auto &&... xs) {
  using F = typename std::decay<decltype (f)>::type;
  using CB = typename std::decay<decltype (cb)>::type;

  auto e = listener_ (std::forward<F> (f));  //event

  auto func = [&, e](
    auto &&... ts) mutable {  //copy in listener, but pass cb and udata by ref
    auto result = e (ts...);
    if (result)
      //std::forward<CB>(cb)(*result, std::forward< typename std::decay< decltype(xs)>::type>(xs)...);
      cb (*result,
          std::forward<typename std::decay<decltype (xs)>::type> (xs)...);
  };

  //
  return thread_args_ (loop_always_no_args_ (func, wait_ (pollrate)));
};



/*-----------------------------------------------------------------------------
   *  Closed Loop Triggers
   *-----------------------------------------------------------------------------*/
/// detects when a counter has hits zero, returning original argument
auto counter_ = [](auto &&start) {
  auto tmp = start;
  return [=]() mutable {
    if (tmp > 0)
      {
        tmp -= 1;
        return maybe<int> ();  //false;
      }
    else
      return maybe<int> (start);  //true, and original value
  };
};

/// shorthand counter set by seconds
auto ct_ = [](auto &&sec) {
  return counter_ (secs_to_milli (std::forward<float> (sec)));
};


// never evaluates to true
auto never_ = [](auto &&... xs) { return false; };

/// an event now
auto now_ = [](auto &&e) {
  using T = TYPE (e);
  return [=](auto &&... xs) { return maybe<T> (e); };
};


/// hold event's last value
auto hold_ = [](auto &&val) {
  auto tmp = val;
  return [=](auto &&e) mutable {
    if (e)
      tmp = *e;
    return tmp;
  };
};

/// accumulate (check this)
auto accum_ = [](auto &&val) {
  using T = typename std::decay<decltype (val)>::type;
  auto tmp = val;
  return [=](auto &&e) mutable {
    if (e)
      {
        tmp = (*e) (tmp);
        return maybe<T> (tmp);
      }
    return maybe<T> ();  //check this
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


//just_ is in signals.hpp, it takes and does x -> maybe x.
//here we want an x(f) -> maybe (apply f)
//ah, what?
//or maybe an fmap
//or, huh, what?
//
//anyway, x must be a function whose return can be evaluated as true.
//
// if (x) then just x otherwise nothing
auto if_ = [](auto &&x) { return when_ (x, id_); };


/// every nsecs call e(t).  Signature: (a->b) -> (a->maybe b)
auto every_ = [](auto &&nsecs, auto &&e) {
  return when_ (impulse_ (1.0 / nsecs), FORWARD (e));
};


//  /// after nsec, start returning e(t). Signature: (a->b) -> (a->maybe b)
//  auto after2_ = [](auto&& nsec, auto&& e){
//    auto reset = reset_();
//    return switch_once_(
//                        //sf1
//                        hana::split_(
//                            //bypass
//                            [](auto&& xs) mutable {
//                                using T = TYPE(FORWARD(e)(FORWARD(xs)));
//                                return T(); //maybe
//                            },
//                            //trigger
//                            [=](auto&& xs) mutable { return reset(FORWARD(xs)) > (nsec * 1000); }
//                        ),
//                        //sf2
//                        [=](auto&&xs) mutable {
//                            cout <<"switched" << endl;
//                            return FORWARD(e);
//                            }
//                        );
//   };

/// after nsec, start returning e(t). Signature: (a->b) -> (a->maybe b)
auto after_ = [](auto &&nsec, auto &&e) {
  auto reset = reset_ ();
  return [=](int &&xs) mutable {
    using T = TYPE (FORWARD (e) (FORWARD (xs)));  //if T is a maybe then what?
    if (reset (std::forward<int> (xs)) > (nsec * 1000))
      {
        return maybe<T> (FORWARD (e) (FORWARD (xs)));
      }
    else
      return maybe<T> ();
  };
};

auto timer_ = [](int sec) { return after_ (sec, true_); };

/// at nsecs, evaluate e(t) once: Signature: (a->b) -> (a->maybe b)
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



/// call func over duration at rate
auto over_ = [](auto &&rate, auto &&duration, auto &&func) {
  return every_ (FORWARD (rate),
                 eval_over_ (FORWARD (duration), FORWARD (func)));
};

/// will generate one event e2 when another e1 happens
//  auto on_ = [](auto&& e1, auto&& e2){
//    using T = typename std::decay< decltype(e) >::type;
//    return [=](auto&& t) mutable {
//      if ( !bSet && ( (sec*1000) - t ) <= 0 ) {
//        bSet = true;
//        return maybe<T>(e);
//      }
//      return maybe<T>();
//    };


/// when e returns an optional event, apply func to result of e
/// different from when_ in that func is applied to result as opposed
/// to signal
auto then_ = [](auto &&e, auto &&func) {
  return [=](auto &&... xs) mutable {
    //e returns a maybe
    auto me = FORWARD (e) (FORWARD (xs)...);
    using T = TYPE (func (std::declval<typename decltype (me)::value_type> ()));
    // using T = TYPE( func( me ) );
    if (me)
      return maybe<T> (FORWARD (func) (*me));  //actual operation
    return maybe<T> ();
  };
};

auto if_then_ = [](auto &&iffy, auto &&thenny) {
  return then_ (if_ (iffy), thenny);
};

//auto then2_ = [](auto &&func) {
//  return [=](auto &&e) {
//    //e returns a maybe
//    using T = TYPE (func (std::declval<typename decltype (e)::value_type> ()));
//    return true;
//  };
//};
//
//    if (e)
//      return maybe<T> (func (*e));
//    return maybe<T> ();
//  };
//};


/// debug practice for spawn_ below
auto spawn_test_ = [](auto &&func, auto &&... e) {
  return hana::split_ (when_ (e, func)...);
};

/// poll style event handler
/// executes list of event sig funcs e, calling callback on their optional return values when there is one
auto spawn_ = [](auto &&callback, auto &&... e) {
  auto sched = hana::split_ (zero_, then_ (e, callback)...);
  return thread_ ([=](auto &&... xs) mutable {
    while (wait_ (.001) ())
      {
        sched (time_ ());
      }
  });
};

/// cascades through list of events e, calling callback on it when true
auto spawn_one_ = [](auto &&callback, auto &&e) {
  auto sched = when_ (e, callback);
  //return sched;
  return thread_ ([=](auto &&... xs) mutable {
    while (wait_ (.001) ())
      {
        sched (time_ ());
      }
  });
};



///launch a thread that applies a callback onto a potential event whenever it returns something
/// ... thread is detached and lives for life of program
auto callback_eternal_ = [](auto &&callback, auto &&pollrate) {

  return [=](auto &&... e) mutable {

    auto sched = hana::split_ (zero_, then_ (e, callback)...);

    thread_ ([=](auto &&... xs) mutable {
      while (wait_ (pollrate) ())
        {
          sched (time_ ());
        }
    }) ();
  };
};

///launch a thread that applies a callback onto the maybe result of an event
// (i.e. whenever it returns something)
/// ... thread is detached and lives until interrupt flag is set (by any thread)
/// ... at which point the thread returns a future
//  Q: compare with future_then, which does not "poll", but just waits until
//  e returns
/// @todo explain why the split_ is necessary
/// @todo possibly return a value from the scheduled ops? also, split into concurrent threads?
/// @sa spawn_ and behavior, which use id_ in the  callback
auto callback_ = [](auto &&cb, std::shared_ptr<bool> &interrupt_flag,
                    auto &&pollrate) {


  return [=, &interrupt_flag](auto &&... e) mutable {

    //e returns maybe an x. if so, callback processes x
    auto sched = hana::split_ (zero_, then_ (e, cb)...);

    // return shared_future (when this returns, interrupt has been set)
    return async_ ([=, &interrupt_flag](auto &&... xs) mutable {
      *interrupt_flag = false;
      while (wait_ (pollrate) ())
        {
          //interruption point... set from another thread
          if (*interrupt_flag)
            {
              // cout << "callback interrupted" << endl;
              break;
            }
          sched (time_ ());
        }
      return true;
    }) ();
  };
};


/// returns event of pointer interrupt flag
auto interrupt_ = [](std::shared_ptr<bool> &flag,
                     std::shared_future<bool> &fut) {

  return [&](auto &&... xs) {
    *flag = true;           // set shared interrupt flag to true
    auto tmp = fut.get ();  // wait for future to to return
    cout << "INTERRUPT_" << endl;
    return maybe<bool> (tmp);  // return value
  };
};



/// launch single events at pollrate -- once e returns something, we finish and return the calculated value
/// used in behavior to poll for interrupt
auto launch_until_ = [](float pollrate, auto &&e) {

  return async_ ([=]() mutable {
    auto tmp = e (0);
    while (wait_ (pollrate) () && !tmp)
      {
        tmp = e (time_ ());
      }
    return tmp;
  }) ();
};

/*-----------------------------------------------------------------------------
   *  Event Listening to Atomic Variables
   *-----------------------------------------------------------------------------*/

///  value of t
auto get_atomic_value_ = [](auto &&t) { return [&]() { return t.load (); }; };

/// has value of t changed in some other thread
auto changed_ = [](auto &&t) {  // std::shared_ptr< std::atomic<int> >

  decltype (t.load ()) val = 0;

  return [&]() {
    auto tmp = t.load ();

    if (val == tmp)
      {
        return false;
      }
    else
      {
        val = tmp;
      }
    return true;
  };
};

/// increment atomic
auto increment_ = [](auto &&t) {
  return [&](auto &&... xs) { return t.fetch_add (1); };
};

}  //ohio::

namespace boost {
namespace hana {

/// unsure of this...
/*
template<typename T>
struct _maybe {
   ohio::maybe<T> val;

   constexpr decltype(auto) operator()(bool&& t) const&{
    if(t) return val;
    return ohio::maybe<T>();
   }

   constexpr decltype(auto) operator()(bool&& t) &{
       if(t) return val;
       return ohio::maybe<T>();
   }

   constexpr decltype(auto) operator()(bool&& t) && {
       if(t) return std::move(val);
       return std::move(ohio::maybe<T>());
   }
};

detail::create<_maybe> maybe_{};

*/


/// When IMP(t) is true, return F(t), otherwise return nothing
/// @returns a maybe of type decltype(F(t))
template <typename Imp, typename F>
struct _when
{
  Imp imp;
  F f;

  template <typename X>
  constexpr decltype (auto) operator() (X &&t) const &
  {
    using R = TYPE (f (static_cast<X &&> (t)));
    //if ( imp(static_cast<X&&>(t)) ) return ohio::maybe<F>( f );//FORWARD(f) );
    //return ohio::maybe<F>();
    if (imp (static_cast<X &&> (t)))
      return ohio::maybe<R> (f (static_cast<X &&> (t)));
    return ohio::maybe<R> ();
  }

  template <typename X>
  constexpr decltype (auto) operator() (X &&t) &
  {
    using R = TYPE (f (static_cast<X &&> (t)));
    //if ( imp(static_cast<X&&>(t)) ) return ohio::maybe<F>( f);//FORWARD(f) );
    //return ohio::maybe<F>();
    if (imp (static_cast<X &&> (t)))
      return ohio::maybe<R> (f (static_cast<X &&> (t)));
    return ohio::maybe<R> ();
  }

  template <typename X>
  constexpr decltype (auto) operator() (X &&t) &&
  {
    using R = TYPE (f (static_cast<X &&> (t)));
    //if ( imp(static_cast<X&&>(t)) ) return ohio::maybe<F>( std::move(f) );
    //return ohio::maybe<F>();
    if (imp (static_cast<X &&> (t)))
      return ohio::maybe<R> (std::move (f) (static_cast<X &&> (t)));
    return ohio::maybe<R> ();
  }
};

detail::create<_when> when2_{};
}
}  //boost::hana::
#endif /* end of include guard: OHIO_EVENT_INCLUDED */


//this old stuff returns maybes, we prefer to r
/*
/// will generate one event e at specific time sec (relative to AppStartTime)
auto at2_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  bool bSet;// = false;
  bSet = false;
  return [=](auto&& t) mutable {
    if ( !bSet && ( (sec*1000) - t ) <= 0 ) {
      bSet = true;
      return maybe<T>(e);
    }
    return maybe<T>();
  };
};


/// really this is "at_" function (execute once at time)
auto at3_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  auto my = maybe<T>( std::forward<T>(e) );
  auto mn = maybe<T>();
  auto reset = reset_();
  bool bDone; bDone = false;
  return[=](int&& xs) mutable -> maybe<T>& {
    if ( reset( std::forward<int>(xs) ) > (sec * 1000) && !bDone) {
      bDone = true;
      return my;
    }
    else return mn;
  };
};
*/
/* OLD
auto after1_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  auto my = maybe<T>( e );
  auto mn = maybe<T>();
  auto reset = reset_();
  return[=](int&& xs) mutable -> maybe<T>& {
    if ( reset( std::forward<int>(xs) ) > (sec * 1000) ) {
      return my;
    }
    else return mn;
  };
};

/// really this is "at_" function
auto after2_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  auto my = maybe<T>( std::forward<T>(e) );
  auto mn = maybe<T>();
  auto reset = reset_();
  bool bDone; bDone = false;
  return[=](int&& xs) mutable -> maybe<T>& {
    if ( reset( std::forward<int>(xs) ) > (sec * 1000) && !bDone) {
      bDone = true;
      return my;
    }
    else return mn;
  };
};

/// do not return reference
auto after3_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  auto reset = reset_();
  return[=](int&& xs) mutable -> maybe<T> {
    if ( reset( std::forward<int>(xs) ) > (sec * 1000) ) {
      return maybe<T>(std::forward<T>(e));
    }
    else return maybe<T>();
  };
};

/// call event e() in n seconds (via sleep)
//  auto in_ = [](float&& sec, auto&& e){
//     return [=](auto&& ... xs){
//        sleep_for_(sec);
//        return e(xs...);
//     };
//  };

/// timed event, counts down in millisecond clock then triggers an event of type e
auto every3_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  using S = typename std::decay< decltype(sec)>::type;
  auto imp = impulse_( 1.0/sec );
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

/// timed event, counts down in millisecond clock then triggers an event of type e
auto every2_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  using S = typename std::decay< decltype(sec)>::type;
  auto imp = impulse_( 1.0/sec );
  auto my = maybe<T>(e);  // declared here and returned as reference ...
  auto mn = maybe<T>();
  return[=](int&& t) mutable -> maybe<T>& { //... here
    using F = typename std::decay<decltype(t)>::type;
    if ( imp( std::forward< F >(t)) ){
      return my; //maybe<T>( std::forward<T>(e) );
    }
    else {
      return mn;//maybe<T>();
    }
  };
};

/// timed event, counts down in millisecond clock then triggers an event of type e
auto every1_ = [](auto&& sec, auto&& e){
  using T = typename std::decay< decltype(e) >::type;
  auto ct = ct_(sec);
  auto te = e;
  cout << "milli: " << secs_to_milli(sec) << endl;
  return[=](auto&& ... xs) mutable {
    if ( ct() ) {
      ct = ct_(sec);
      return maybe<T>( std::forward<T>(te) );
    }
    else {
      return maybe<T>();
    }
  };
};
*/

/*
  /// applies func to event stream if event has happened (change to apply_when_ above...)
  /// @returns maybe<T> where T is func(*(e(xs...)))
  auto when1_ = [](auto&& e, auto&& func){
    return [=](auto&& ... xs) mutable {
      auto te = e( std::forward< typename std::decay< decltype(xs)>::type...>(xs)...);
      using T = decltype( func( std::declval<typename decltype(te)::value_type>() ) );
      using F = typename std::decay< decltype(func)>::type;
      if (te) return maybe<T>( std::forward<F>(func)(*te) );
      return maybe<T>();
    };
  };

  /// using reference of maybe...
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


   /// not using reference of maybe...
  auto when0_ = [](auto&& e, auto&& func){

    auto ce = e(0);
    using T = decltype( func( MVAL( ce ) ) );
    auto my = maybe<T>();
    auto mn = maybe<T>();

    return [=](int&& xs) mutable -> maybe<T> {
      auto& te = FORWARD(e)( FORWARD(xs) );  // note: reference returned...
      if (te) {
        my = maybe<T>( FORWARD(func)(*te) );
        return my;
      }
      my = maybe<T>();
      return mn;
    };
  };

   /// not using reference of maybe...
  auto when3_ = [](auto&& e, auto&& func){

    auto ce = e(0);
    using T = decltype( func( MVAL( ce ) ) );
    auto my = maybe<T>();
    auto mn = maybe<T>();

    return [=](int&& xs) mutable -> maybe<T> {
      auto te = FORWARD(e)( FORWARD(xs) );
      if (te) {
        my = maybe<T>( FORWARD(func)(*te) );
        return my;
      }
      return mn;
    };
  };


*/



/*
///spawn as a higher order function
auto callback_ = [](auto&& callback, std::shared_ptr<bool>& flag, std::promise<bool>& interrupt){

  //std::promise<bool> interrupt;

  return [=,&flag,&interrupt](auto&& ... e) mutable {

    auto sched = hana::split_( zero_, when_(e,callback)...);

    thread_( [=, &flag](auto&& ... xs) mutable {
      *flag = false;
      while( wait_(.001)() ){
        //interruption point...
        if ( *flag ) {
          cout << "break" << endl;
          interrupt.set_value(true);
          break;
        }
        sched( time_() ); }
      }
     )();

     return interrupt.get_future();
  };
};
*/
