  /**
  * Graph a bunch of signals in the console
  */

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

  // A bunch of signals that return/range float values with frequency argument/domain
  auto osc = osc_ (2);
  auto ramp = ramp_ (4);
  auto rampdown = rampdown_ (3);
  auto impulse = impulse_ (1);
  auto saw = saw_ (.5);

  // all of them together
  auto f = all_ (osc, ramp, rampdown, impulse, saw);

  // an independent process, that graphs a printout of a result (that's all this does)
  // (that's a single line of output on the screen)
  auto proc = pipe_(transform_(pipe_ (graph_(10), coutall_)), endl_);

  // transform the function by the print out process
  auto e = pipe_ (f, proc);

  // the spell has been cast! now we just keep cranking through time. . .
  while (tick ())
     e (time_ ());

  return 0;
}
