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
   
  // F f;

   template<class ... e>
   void launch( e&& ... es )  {
     stop();                               // stops behavior if it is already running
     auto tmp = callback3_( do_, bStop );  
     mReturn = tmp( std::forward< typename std::decay< decltype(es)>::type >(es)...);
     bStarted = true;
   }

   void stop(){
     //@todo check that mReturn is valid, i.e. has been launched
     if (bStarted) interrupt_( bStop, mReturn )();
   }

   int mId;
   bool bStarted = false; //set to true after first launch
   std::shared_ptr<bool> bStop = std::make_shared<bool>(false);
   std::shared_future<bool> mReturn;

 };


} //ohio::

#endif   /* ----- #ifndef behavior_INC  ----- */
