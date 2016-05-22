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

  // A Behavior has an id, boolean flag (whether to stop it), and future return value
//  @todo template<class F> could provide user defined callbacks
 struct behavior{
   
  // behavior(int x){}
   
  // F f;
   ~behavior(){
      stop();
      //handle shared future and ptr ?
   }

   template<class ... e>
   behavior& launch(e&& ... es )  {
     stop();                               // stops behavior if it is already running
     bDone = false;
     auto tmp = callback3_( proc_, bStop, pollrateLaunch );  
     mReturn = tmp( std::forward< typename std::decay< decltype(es)>::type >(es)...);
     bStarted = true;
     return *this;
   }

   void stop(){
     //@todo check that mReturn is valid, i.e. has been launched
     if (bStarted) interrupt_( bStop, mReturn )();
     bDone = true;
   }

   /// Execute behavior until ev returns true
   template<class T>
   behavior& until(T&& ev){
      auto tmp = once_(pollrateFinish, ev); 
      thread_([tmp,this](){ tmp.get(); this->stop(); })();
      return *this;
   }

   /// Execute behavior for n seconds
   behavior& over(float nsec){
      auto tmp = once_(pollrateFinish, after_(nsec, true ) ); 
      thread_([tmp,this](){ tmp.get(); this->stop(); })();
      return *this;
   }   

   int mId;
   bool bStarted = false;  //set to true after first launch
   bool bDone = false; //set to true after stop() is called
   std::shared_ptr<bool> bStop = std::make_shared<bool>(false);
   std::shared_future<bool> mReturn;
    
   float pollrateLaunch = .001;  // default rate of polling for launching thread
   float pollrateFinish = .001;  // default rate of polling for thread checking finish condition

 };


} //ohio::

#endif   /* ----- #ifndef behavior_INC  ----- */
