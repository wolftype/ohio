#ifndef OHIO_EVENT_INCLUDED
#define OHIO_EVENT_INCLUDED

#include <atomic>
#include <sstream>

#include "arrows.hpp"
#include "thread.hpp"
#include "time.hpp"
#include "basic.hpp"
#include "signal.hpp"

namespace hana = boost::hana;
namespace ohio{

  /*-----------------------------------------------------------------------------
   *  SOME io
   *-----------------------------------------------------------------------------*/
  auto stdin_ = []( auto&& ... xs){
    std::string str;
    std::cin >> str;
    return str;
  };


//  auto event_ =[]( auto&& f){
//    using T = typename std::decay< decltype(f) >::type;
//    
//    return [=](auto&& ... xs){
//      
//    }
//  };
//
  /*-----------------------------------------------------------------------------
   *  SWITCHING (See YAMPA)
   *-----------------------------------------------------------------------------*/
  /// execute signal function sf1 until event e, then execute function sf2
  /// sf1 takes an xs and returns a pair and sf2 takes an event and an xs and returns a b
  auto switch_ = [](auto&& sf1, auto&& sf2 ){
    return [=](auto&& ... xs){
       /// calculate pair of outputs
      auto x = sf1( std::forward< typename std::decay< decltype(xs) >::type > (xs)...);
       // if event has happened...
      if ( hana::second(x) ) {
        // ...pass event to continuation function, which may do different things depending on its value
        return sf2( hana::second(x) ) ( std::forward< typename std::decay< decltype(xs) >::type > (xs)... ) ;
      }
       // otherwise return original output
      else return hana::first(x);
    };
  };

  /// recursive switch, pipe a pair into it, second of which is an event wrapped function
  auto rswitch_ = [](auto&& sf1){
    return[=](auto&& x){
      if ( hana::second(x) ){
        return (*hana::second(x))(hana::first(x));
      } else return sf1( hana::first(x) );
    };
  };

  /// tag event e with value b
  auto tagbase_ = [](auto&& e, auto&& b) {
    using T = typename std::decay< decltype(b) >::type;
    if (e) return maybe<T>( std::forward<T>(b) );
    else return maybe<T>();
  };

  auto tag_ = [](auto&& es, auto&& b){
    return [=](auto&& ...xs )  mutable { //why do we need mutable here?
      return tagbase_( es( std::forward< typename std::decay< decltype(xs) >::type>(xs)...), b );
    };
  };

  auto tag2_ = [](auto&& es, auto&& b){
    auto m = maybe<TYPE(b)>(); 
    return [=](auto&& ...xs )  mutable -> maybe<TYPE(b)>& { 
      m = tagbase_( es( std::forward< typename std::decay< decltype(xs) >::type>(xs)...), b );
      return m;
    };
  };


  /*-----------------------------------------------------------------------------
   *  Events triggers
   *-----------------------------------------------------------------------------*/
  /// An Event Stream returns an Optional Value as a function of time, either it exists or it doesn't
  //auto estream_ =[](const auto& tmp){
  auto estream_ =[](auto&& tmp){
    using T = typename std::decay< decltype( tmp.get() ) >::type;
    std::shared_future<T> fut( tmp);
    return [=](auto&& ... xs)  {

      if (fut.valid() != false){

        auto res = fut.wait_for( std::chrono::milliseconds( 0 ) );
        if (res == std::future_status::ready){
          return std::experimental::optional<T>( fut.get() );
        }
      } else {
        printf("invalid\n");
      }
      //printf("empty\n");
      return std::experimental::optional<T>();
   };
  };

  /// return event contained in future if delivered or else nothing
  auto listen_ =[](auto&& fut){
    using T = typename std::decay< decltype( fut.get() ) >::type;
    if (fut.valid() != false){
      auto res = fut.wait_for( std::chrono::milliseconds( 1 ) );
      if (res == std::future_status::ready){
        return maybe<T>( fut.get() );
      }
    } else {
      printf("invalid\n");
    }
    //printf("empty\n");
    return maybe<T>();
  };

  /// forward arguments to avoid linker errors
  auto fwd_future = [](auto&& f){
    std::shared_future< decltype(f()) > fut;
    return [=]() mutable {
      auto t = fut;
      f();
    };
  };

  /// forward arguments to avoid linker errors
  auto fwd_future_args = [](auto&& f){
    std::shared_future< decltype(f()) > fut;
    return [=]() mutable { //changed to include ... args
      auto t = fut;
      printf("fwd_future_args...\n");
      //f();//xs...);
    };
  };

  /// forward arguments to avoid linker errors
  auto fwd_shared_args = [](auto&& f){
    using F = typename std::decay< decltype(f) >::type;
    using T = std::shared_ptr< F >;
    T ptr;
    maybe<T> opt;
    return [=]() mutable { //consider change to include ... args
      auto t = ptr;
      auto m = opt;
      printf("fwd_shared_args...\n");
    };
  };
  
  /// generates new event listener repeatedly
  auto listener_ = [](auto&& f){
    fwd_future_args( f )(); //! need to predeclare signature
    using F = typename std::decay< decltype(f) >::type;
    using T = typename std::decay< decltype( async_( std::forward<F>(f) )().get() )>::type;

    std::shared_future<T> fut = async_( std::forward<F>(f) )() ;

    return [=](auto&& ...xs) mutable {
      auto opt = listen_( std::forward< decltype(fut) >(fut) );
      if (opt) {
        fut = async_( std::forward<F>(f) )( std::forward< typename std::decay< decltype(xs)>::type >(xs)... );
        return opt;
      }
        return maybe<T>();
    };
  };

  /// await for results of function f then call function cb on results
  /// DEPRECATED use callback2_ instead
  auto poll_ = [](auto&& f, auto&& cb, float&& pollrate, auto&& ... xs){
    using F = typename std::decay< decltype(f) >::type;
    using CB = typename std::decay< decltype(cb)>::type;
  
    auto e = listener_( std::forward<F>(f) ); //event
  
    auto func = [&,e](auto&& ... ts) mutable { //copy in listener, but pass cb and udata by ref
      auto result = e(ts...);
      if (result)
        //std::forward<CB>(cb)(*result, std::forward< typename std::decay< decltype(xs)>::type>(xs)...);
        cb(*result, std::forward< typename std::decay< decltype(xs)>::type>(xs)...);
    };

    //
     return thread_args_( loop_always_no_args_( func, wait_(pollrate) ) );
  };


  /// await for results of function f then call function cb on results
  auto poll2_ = [](auto&& f, auto&& cb, float&& pollrate, auto&& ... xs){
    using F = typename std::decay< decltype(f) >::type;
    using CB = typename std::decay< decltype(cb)>::type;
  
    auto e = listener_( std::forward<F>(f) ); //event
  
    auto func = [&,e](auto&& ... ts) mutable { //copy in listener, but pass cb and udata by ref
      auto result = e(ts...);
      if (result)
        //std::forward<CB>(cb)(*result, std::forward< typename std::decay< decltype(xs)>::type>(xs)...);
        cb(*result, std::forward< typename std::decay< decltype(xs)>::type>(xs)...);
    };

    //
     return thread_args_( loop_always_no_args_( func, wait_(pollrate) ) );
  };



  /*-----------------------------------------------------------------------------
   *  Closed Loop Triggers
   *-----------------------------------------------------------------------------*/
  /// detects when a counter has hits zero, returning original argument
  auto counter_=[](auto&& start){
    auto tmp = start;
    return [=]() mutable {
      if (tmp > 0) {
        tmp -= 1;
        return maybe<int>(); //false;
      }
      else return maybe<int>(start); //true, and original value
    };
  };

  /// shorthand counter set by seconds
  auto ct_ = [](auto&& sec){ return counter_(secs_to_milli( std::forward<float>(sec) ) ); };


  // never evaluates to true
  auto never_ = [](auto&& ...xs){ return false; };

  /// an event now
  auto now_ =[](auto&& e){
    using T = typename std::decay< decltype(e) >::type;
    return[=](auto&& ... xs){
      return maybe<T>(e);
    };
  };

  /// time event, counts down
  auto after2_ = [](auto&& sec, auto&& e){
    using T = typename std::decay< decltype(e) >::type;
    auto ct = ct_(sec);
    return[=](auto&& ... xs) mutable {
      if ( ct() ) return maybe<T>( std::forward<T>(e) );
      else return maybe<T>();
    };
  };

  auto after_ = [](auto&& sec, auto&& e){
    using T = typename std::decay< decltype(e) >::type;
    auto my = maybe<T>( e );
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
   // auto te = e;
    auto imp = impulse_( 1.0/sec );
    auto my = maybe<T>( std::forward<T>(e) ); //declared here and returned reference
    auto mn = maybe<T>();
    return[=](auto&& t) mutable -> maybe<T>& {
      if ( imp(t) ){ 
        return my; //maybe<T>( std::forward<T>(e) );
      }
      else {
        return mn;//maybe<T>();
      }
    };
  };

  /// timed event, counts down in millisecond clock then triggers an event of type e
  auto every_ = [](auto&& sec, auto&& e){
    using T = typename std::decay< decltype(e) >::type;
    using S = typename std::decay< decltype(sec)>::type;
    auto imp = impulse_( 1.0/sec );
    auto my = maybe<T>( e ); //declared here and returned reference
    auto mn = maybe<T>();
    return[=](int&& t) mutable -> maybe<T>& {
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
  auto every2_ = [](auto&& sec, auto&& e){
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

  /// hold event's last value
  auto hold_ = [](auto&& val){
    auto tmp = val;
    return [=](auto&& e) mutable {
      if (e) tmp = *e;
      return tmp;
    };
  };

  /// accumulate (check this)
  auto accum_ = [](auto&& val){
    using T = typename std::decay< decltype(val) >::type;
    auto tmp = val;
    return[=](auto&& e) mutable {
      if (e) {
        val = (*e)(val);
        return maybe<T>(val);
      }
      return maybe<T>(); //check this
    };
  };

  /// will generate one event e at specific time sec (relative to AppStartTime)
  auto at_ = [](auto&& sec, auto&& e){
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
//  };

  
  /// applies func to event stream if event has happened
  /// @returns maybe<T> where T is func(*(e(xs...))) 
  auto when_ = [](auto&& e, auto&& func){
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

  
  
  /// debug practice for spawn_ below
  auto spawn_test_ = [](auto&& func, auto&& ... e ){
    return hana::split_( when_(e,func) ... );
  };
  
  /// poll style event handler
  /// cascades through list of events e, calling callback on it when true
  auto spawn_ = [](auto&& callback, auto&& ... e){
    auto sched = hana::split_( when_(e,callback) ... );
    return thread_( [=](auto&& ... xs) mutable { while( wait_(.001)() ){ sched( time_() ); } } );
  };

  /// cascades through list of events e, calling callback on it when true
  auto spawn_one_ = [](auto&& callback, auto&& e){
    auto sched = when_(e,callback);
    //return sched;
    return thread_( [=](auto&& ... xs) mutable { while( wait_(.001)() ){ sched( time_() ); } } );
  };

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


  ///spawn as a higher order function
  auto callback2_ = [](auto&& callback, auto&& pollrate){
    
    
    return [=](auto&& ... e) mutable {
      
      auto sched = hana::split_( zero_, when_(e,callback)...);

      thread_( [=](auto&& ... xs) mutable {     
        while( wait_(pollrate)() ){ 
          sched( time_() ); } 
        }
       )();
    };
  };

  ///spawn as a higher order function
  auto callback3_ = [](auto&& callback, std::shared_ptr<bool>& flag, float pollrate){
    
    
    return [=,&flag](auto&& ... e) mutable {
      
      auto sched = hana::split_( zero_, when2_(e, callback)... );  /// NOTE changed to when2_
          //std::forward<typename std::decay<decltype(e)>::type >(e), callback)...);

      // return shared_future (when this returns, interrupt has been set)
      return async_( 
        [=, &flag](auto&& ... xs) mutable {     
          *flag = false;
          while( wait_(pollrate)() ){ 
            //interruption point... set from another thread
            if ( *flag ) {
              cout << "callback3 interrupted" << endl;
              break;
            }
           sched( time_() ); 
          }
          return true;         
         }
       )();
    };
  };
  

  /// returns event of pointer interrupt flag
  auto interrupt_ = [](std::shared_ptr<bool>& flag, std::shared_future<bool>& fut){
    
    return [&](auto&& ... xs){
      *flag = true;                  // set shared interrupt flag
      auto tmp = fut.get();
      cout << "INTERRUPT GOTTEN" << endl;
      return maybe<bool>( tmp );  // wait for thread to return and get value
    };
  };



  /// launch single events at pollrate -- once e returns something, we finish and return the contained value
  auto once_ = [](float pollrate, auto&& e){
    
    return async_( [=]() mutable { 
            auto tmp = e(0);
            while( wait_(pollrate)() && !tmp ){ 
              tmp = e( time_() ); 
            } 
            return tmp;
          } )();
  };

  /*-----------------------------------------------------------------------------
   *  Event Listening to Atomic Variables
   *-----------------------------------------------------------------------------*/

  ///  value of t
  auto get_atomic_value_ = []( auto&& t){
      return [&](){
        return t.load();
      };
  };

  /// has value of t changed in some other thread
  auto changed_ = [](auto&& t){ // std::shared_ptr< std::atomic<int> >

        decltype( t.load() ) val = 0;

        return [&]() {
          auto tmp = t.load();

          if( val == tmp ) {
            return false;
          } else {
            val = tmp;
          }
          return true;
      };
  };


  /// increment atomic
  auto increment_ = []( auto&& t ){
      return [&](auto&& ... xs){
        return t.fetch_add(1);
      };
  };



} //ohio::
#endif /* end of include guard: OHIO_EVENT_INCLUDED */
