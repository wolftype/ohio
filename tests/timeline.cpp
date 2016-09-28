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



//auto flatten_ = [](auto&& mt){
//    using T = MMTYPE(mt); // value_type of mt
//    return [=](auto&& xs) mutable{
//        T(*mt);
//    };
//};

template<class T>
decltype(auto) decay( T&& f){
  //  cout << "not maybe" << endl;
    return FORWARD(f);
}

template<class T>
decltype(auto) decay( maybe<T>&& m){
   // cout << "maybe" << endl;
    if (m) return decay( *m );
    return decay( T() );
}

auto dproc_ = [](auto&& f){
  return FORWARD(f) ( time_() );
};

auto userCB = [](auto&& s) {
    cout << s << endl;
    return true;
};

auto userCB2 = [](auto&& s) {
    cout << decay(s) << endl;
    return true;
};

int main(){

  AppStartTime = now();

  auto e1 = at_(.3,  constant_("hello"));
  auto e2 = at_(1,   constant_("goodbye"));
  auto e3 = at_(1.3, constant_("again"));
  auto e4 = at_(1.4, constant_("again1"));
  auto e5 = at_(1.5, constant_("again2"));
  auto e6 = at_(1.6, constant_(123));
  auto e7 = at_(3,   constant_(5.0f));
  auto e8 = at_(5,   constant_(&AppStartTime));

  auto e9 = every_(.1, constant_("boo-yah"));
  auto e10 = after_(3, e9 );//[=](auto&&xs) mutable { return FORWARD(e9); } );//(FORWARD(xs)); } );//constant_("boo-yah"));


  auto b1 = spawn_(userCB,e1,e2,e3,e4,e5,e6,e7,e8);
  auto b2 = spawn_(userCB2, e10);
  printf("spawn events:...\n");
  b1();
  printf("non-blocking\n");
  b2();

  decay( maybe<int>(0) );
  decay( constant_("boo_yah")() );
  while( wait_(.001)() ){};

  return 0;
}



//not used, or rather moved to event
//auto once_ = [](auto&& sec){

//  bool bSet = false;

//  return [=](auto&& t) mutable {
//    if ( !bSet && ( (sec*1000) - t ) <= 0 ) {
//      bSet = true;
//      return true;
//    }
//    return false;
//  };
//};
