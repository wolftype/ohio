/*
 * =====================================================================================
 *
 *       Filename:  behavior.hpp
 *
 *    Description:  behaviors!
 *
 *        Version:  1.0
 *        Created:  05/07/2016 15:43:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */



#ifndef  behavior_INC
#define  behavior_INC

#include "event.hpp"


namespace ohio{

/// A Behavior has an id, shared interrupt, and future return value, and some booleans
///  @todo templating this class could allow for user-defined callbacks when launched
///  @todo add std::string mName member for printing out when it has been launched / interrupted
///  @todo how to handle shared_ptr and shared_future on destruction
///  @todo add a `then( e )` method
struct behavior{
   
  // F f;
   ~behavior(){
      stop();
      //handle shared future and ptr ?
   }

   /// launch behavior callbacks
   template<class ... e>
   behavior& launch(e&& ... es )  {
     stop();                               // stop behavior if it is already running
     bDone = false;
     auto tmp = callback_( proc_, bInterruptPtr, pollrateLaunch );
     mReturn = tmp( FORWARD(es)...);	   // future value which will return when interrupted
     //std::forward< typename std::decay< decltype(es)>::type >(es)...);
     bStarted = true;
     return *this;
   }

   /// stop behavior
   ///@todo check that mReturn is valid, i.e. has been launched
   void stop(){
     if (bStarted) interrupt_( bInterruptPtr, mReturn )();
     bDone = true;
   }

   /// Execute behavior until ev returns true
   template<class T>
   behavior& until(T&& ev){
      auto tmp = launch_until_(pollrateFinish, ev);        //<-- launch a looping thread that polls until ev(t) returns
      thread_([tmp,this](){ tmp.get(); this->stop(); })(); //<-- launch a thread that blocks until ev(t) returns a value
      return *this;
   }

   /// Execute behavior for n seconds
   behavior& over(float nsec){
      auto tmp = launch_until_(pollrateFinish, after_(nsec, constant_(true) ) );
      thread_([tmp,this](){ tmp.get(); this->stop(); })();
      return *this;
   }   

   int mId; 				/// id number (or make a char * or string)
   bool bStarted = false;   /// set to true after first launch
   bool bDone = false; 		/// set to true after stop() is called
   /// shared interrupt pointer
   std::shared_ptr<bool> bInterruptPtr = std::make_shared<bool>(false);
   /// future return value
   std::shared_future<bool> mReturn;
    
   float pollrateLaunch = .001;  /// default rate of polling for launching thread
   float pollrateFinish = .001;  /// default rate of polling for thread checking finish condition

 };


} //ohio::

#endif   /* ----- #ifndef behavior_INC  ----- */
