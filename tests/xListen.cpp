
#include "signal.hpp"
#include "event.hpp"
#include "behavior.hpp"
#include "graph.hpp"
#include <sstream>
#include <iomanip>

using namespace ohio;
using namespace std;
using hana::pipe_;
using hana::split_;

auto input_ = [](auto&& ... xs)
{
 return [=](auto&& ... ys)
 {
   return 0;
 };
};

int main ()
{
  AppStartTime = now ();
  auto tick = wait_ (.05);

  auto c = constant_(10);
  auto f = pipe_(graph_(1), coutall_);
  auto e = pipe_(c, f, endl_);


  auto e1 = every_(1, [](auto&& xs){ cout << "1" << endl; return true;});
  //behaviors launch events, not signals
  behavior b;
  b.launch (e1);

//  thread_future_(stdin_)();

//  while (tick ())
//    {
//      e1 (time_ ());
//    }
//
  return 0;
}
