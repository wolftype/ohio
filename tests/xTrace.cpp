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

#include "basic.hpp"
#include "signal.hpp"
#include "graph.hpp"
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
  auto tick = wait_ (.05);

  // introducing three commands:
  //
  // 1. our results will take some input and map it to a graph, which is really
  // just a string of characters
  auto results = pipe_ (div_ (100), mod_ (10), mult_ (-1), graph_ (1));

  // 2. to see our characters, we'll want an 'output' command, which prepends
  // a command to clear the current row of characters and
  // then sends the whole string to std::cout and then std::flush
  auto output = pipe_ (clearline_, cout_, flush_);

  // 3. finally, this command will print the results
  auto view = pipe_ (results, output);

  // introducing some event logic: (see event.hpp and behavior.hpp)
  // events return maybes, basically the thing and an extra bit of information
  // about whether the thing happened
  //
  // 1. if an impulse signal has been registered (which i will 10 times a second)
  // print the results of the signal.  this passes the results of the impulse function
  // into the view -- different from do_when_,
  auto e = if_then_ (impulse_ (10), view);
  // note that another version of if_then_ is when_, except that instead of
  // the first argument being a function that returns a boolean, it is a function
  // that return return maybes.

  // 2. a behavior will launch event e in another thread until the timer event returns,
  // and then e will stop its logic. e must return a maybe type
  behavior b;
  b.launch (e).until (timer_ (20));


  // GREAT. now, that that's happening, let's do more.
  // this command moves the cursor up to the row above
  auto move = [](int k, int x, int y) {
    return if_then_ (eq_ (k),
                     pipe_ (constant_ (move_by_ (x, y)), cout_, flush_));
  };

  // this will get the getchar and then move up if required
  //auto ctrl = repeat_pipe_ (getchar_, if_then_(eq_(UP), move_up));
  auto logic = all_ (move (UP, 0, 1), move (DOWN, 0, -1));
  auto ctrl = pipe_ (getchar_, logic, just_);

  behavior b1;
  b1.launch (ctrl);  //.until (timer_ (3));

  while (tick ())
    {
      //interject(100);
    }

  return 1;
}

// kind of want towrite it this way:
//  auto e2 = pipe_ (if_(impulse_(100)), then2_(view));
