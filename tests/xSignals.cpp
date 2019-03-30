#include "signal.hpp"
#include "arrows.hpp"
#include "graph.hpp"

using namespace ohio;
using namespace std;
using hana::pipe_;
using hana::split_;

int main ()
{
  AppStartTime = now ();
  auto tick = wait_ (.05);

  auto osc = osc_ (2);
  auto ramp = ramp_ (4);
  auto rampdown = rampdown_ (3);
  auto impulse = impulse_ (1);
  auto saw = saw_ (.5);

  auto f = all_ (osc, ramp, rampdown, impulse, saw);
  auto proc = pipe_ (graph_(10), coutall_);

  auto e = pipe_ (f, transform_ (proc), endl_);

  while (tick ())
    {
      e (time_ ());
    }

  return 0;
}
