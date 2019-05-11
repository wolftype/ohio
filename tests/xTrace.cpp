/**
 * Trace \unicode characters backwards from cursor
 * using \return to beginning of line, and ramp signal
 *
 *
 * note: the run.sh script does the following for you:
 *   keypresses are sent directly to the program,
    -- so disable icanon and echo n a bash shell (NOT fish) with:

   stty -icanon -echo

   two definitions of just (it is JUST, there is a measurement):
   both LIFT a value x into a maybe

   if_ (x) -> maybe
     passes along a value if x is true

   signal.hpp (def_(x))
   2 - x is lifted no matter what



 */

#include "graph.hpp"
#include "signal.hpp"
#include "event.hpp"
#include "behavior.hpp"
#include <sstream>
#include <iomanip>

using namespace ohio;
using namespace std;
using hana::pipe_;
using hana::split_;

int main ()
{
  AppStartTime = now ();
  auto tick = wait_ (.01);

  auto output = pipe_ (clearline_, cout_, flush_);
  auto results = pipe_ (mod_(1000), graph_ (.02));
  auto print = pipe_(results, output);

  // the function that continues when x is true
  // if (function-f-evaluates-true) do whatever
  // if (a)(x) then just x otherwise nothing
  // then (maybe a,b) b is passed a
  auto e = then_ (if_ (impulse_ (100)), print);

 // behavior b;
//  b.launch(e);

  while (tick ())
    {
//      e (time_ ());
    }

  return 1;
}
