/*
 * =====================================================================================
 *
 *       Filename:  timeline.cpp
 *
 *    Description:  scheduling events
 *
 *        Version:  1.0
 *        Created:  05/02/2016 19:22:15
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
using namespace boost;


auto userCB_ = [](){
  return [](auto&& s) {
    cout << s << endl;
  };
};

auto userCB = [](auto&& s) {
    cout << s << endl;
    return true;
};


auto once_ = [](auto&& sec){
  
  bool bSet = false;

  return [=](auto&& t) mutable {
    if ( !bSet && ( (sec*1000) - t ) <= 0 ) {
      bSet = true;
      return true;
    } 
    return false;
  };
};


int main(){

  AppStartTime = now();

  auto e1 = at_(.3, "hello");
  auto e2 = at_(1, "goodbye");
  auto e3 = at_(1.3, "again");
  auto e4 = at_(1.4, "again1");
  auto e5 = at_(1.5, "again2");
  auto e6 = at_(1.6, 123);
  auto e7 = at_(3, 5.0f);

  auto b = and_(e1, e2);
  auto d = spawn_(userCB, e1,e2,e3,e4,e5,e6,e7);
  d();
  printf("non-blocking\n");
  while( wait_(.001)() ){
   // d( time_() );
    
  };

  return 0;
}
