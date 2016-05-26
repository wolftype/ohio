/*
 * =====================================================================================
 *
 *       Filename:  memory.cpp
 *
 *    Description:  testing for memory leaks
 *
 *        Version:  1.0
 *        Created:  05/20/2016 19:05:22
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
using namespace std;

int main(){

  AppStartTime = now();
  
  bool bTrigger = false;

  auto print = [](auto&& t){ cout << t << endl; return true; };
  auto trigger = [&](){ bTrigger = !bTrigger; return true; };
//
  auto e1 = ohio::tag2_( triggerval_( bTrigger ), print );
  auto e2 = every_(.1, trigger );
  auto e3 = after2_(5, trigger);
  auto e4 = over_(10, [](float t){ cout << t << endl; return true; } );
  auto e5 = every_(1,e4);
  auto e6 = after_(2,e4); /// this does not work yet
//  
  auto b = behavior();
  b.launch(e1);
//
//
  auto f = when_( e6, proc_ );

  while ( wait_(.001)() ){
    f( time_() );
//      e2( time_() );
   }


}
