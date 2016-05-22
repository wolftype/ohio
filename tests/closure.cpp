/*
 * =====================================================================================
 *
 *       Filename:  closure.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2016 15:44:11
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

/// Generate Impulse from incoming Time at rate x times per second
auto impulse2_ = [](float&& x){
  //find modulus of incoming signal % 1000/x, check if current value is less than previous
  return hana::compose( detect_reset_(), mod_(1000.0/x) );
};




  /// timed event, counts down in millisecond clock then triggers an event of type e
  auto every4_ = [](auto&& sec, auto&& e){
    using T = typename std::decay< decltype(e) >::type;
    using S = typename std::decay< decltype(sec)>::type;
   // auto te = e;
    auto imp = impulse_( 1.0/sec );
    auto my = maybe<T>( e ); //declared here and returned reference
    auto mn = maybe<T>();
    return[=](int&& t) mutable -> maybe<T>& {
      using F = typename std::decay<decltype(t)>::type;
      if ( imp( std::forward< F >(t)) ){ 
        cout << "y" << endl;
        return my; //maybe<T>( std::forward<T>(e) );
      }
      else {
        return mn;//maybe<T>();
      }
    };
  };

int main(){




  auto clos = [](){
    int x = 0;
    return [=]() mutable {
      x = x+1;
      cout << x << endl;
      return true;
    };
  };

  auto ev2 = [](auto&& e) {
    //e();
    //using T = typename std::decay<decltype(e)>::type;
    return[=]() mutable {
      e();
      return true;
      //std::forward<T>(e)();
    };
  };

  /// note this is how to pass back a recorded state`
  auto ev3 = [](auto&& e) {
    using T = typename std::decay<decltype(e)>::type;
    auto my = maybe<T>(e);
    auto mn = maybe<T>();

    return[=](auto&& t) mutable -> maybe<T>& { 
      //if (t>1000) 
        return my;
      //else return mn;
    };
  };

  

  auto tmp =  every_(.5, clos());//every3_(.5, clos() );  
  auto imp = impulse2_( 1.0/.5 );

  while ( wait_(.001)() ){
    //(*(tmp()))();
    auto& x = tmp( time_() ); 
   // if (imp(time_())) cout << "Y" << endl;
    if (x) (*x)();    
  }

  return 0;
};
