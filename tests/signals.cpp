/*
 * =====================================================================================
 *
 *       Filename:  signals.cpp
 *
 *    Description:  signal generators
 *
 *        Version:  1.0
 *        Created:  05/06/2016 20:17:14
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


  ///detects that input has stopped incrementing (use with mod_)
  auto has_reset_ = [](){
    
    int start = 0;
    return[=](auto&& t) mutable {
      if ( t > start ) {
        start = t;
        return false;
      } else {
        start = 0;
      }

      return true;

    };
  };


int main(){


  auto tsig = hana::compose( detect_reset_(), mod_(1000 * .5) );

 // auto tmod =  mod_(1000.0/x);
  auto sig = impulse_(.5);
  
  while ( wait_(.001)() ){
      
    cout << tsig( time_()) << endl;
  }

  return 0;
};
