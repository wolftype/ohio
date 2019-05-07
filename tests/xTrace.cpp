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

#include "signal.hpp"
#include "event.hpp"
#include "graph.hpp"
#include <sstream>
#include <iomanip>

using namespace ohio;
using namespace std;
using hana::pipe_;
using hana::split_;

auto output = cout_with_("", "\r");


int main ()
{
  AppStartTime = now ();
  auto tick = wait_ (.5);

// auto e = pipe_(impulse_(1), graph_(10), print_);
 auto results = pipe_(graph_(.01), output);

 // the function that continues when x is true
 // if function-f-evaluates-true do-this
 auto e = then_(if_(impulse_(1)), results);

//  // plotter takes a start and end
//  // this will print something at XY based both calculator
//  // creates string that will generate the \unicode at XY
//  auto pstr = pipe_ (and_ (calc_x_val, calc_y_val),
//                        merge_ (plotter_ (TREX)));
//
  while (tick ())
    {
      e (time_());
    }

  return 1;
}
