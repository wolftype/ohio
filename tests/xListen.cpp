/**
 * Listen for keyboard input
 * note: control requires keypresses are sent directly to program,
   and output disables echo
   before hitting "Enter" -- so disable icanon and echo n a bash shell (NOT fish) with:

   stty -icanon -echo

 */

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

// this function will keep tabs, either decreasing or increasing
// a tally which was pressed
auto start_tabs = [](auto &&dec, auto &&inc) {
  std::shared_ptr<int> tally = std::make_shared<int> ();
  return [=](auto &&x) {
    if (x == dec)
      *tally -= 1;
    else if (x == inc)
      *tally += 1;
    return *tally;
  };
};

// this function will keep tabs,either decreasing or increasing
// a tally which was pressed
// positive values only please
// todo: == replace with is_equal_
auto start_tally = [](auto &&dec, auto &&inc) {
  std::shared_ptr<int> tally = std::make_shared<int> ();
  return [=](auto &&x) {
    if (x == dec && *tally > 0)
      *tally -= 1;
    else if (x == inc)
      *tally += 1;
    return *tally;
  };
};

auto diff = [](auto &&dec, auto &&inc) {
  return [=](auto &&x) {
    if (x == dec)
      return -1;
    else if (x == inc)
      return 1;
    return 0;
  };
};

int main ()
{
  AppStartTime = now ();
  auto tick = wait_ (1.0);

  // print a graph
  auto trace = pipe_ (graph_ (1), prepend_ ("\r"));
  // auto plot = pipe_ (plot_ (1), cout_with_ ("", ""));

  //this will calculate ABSOLUTE x and y position based on keys pressed
  //see macros (uses vim bindings LEFT=h, RIGHT=l, DOWN=j, UP=k)
  auto x_val = start_tally (LEFT, RIGHT);
  auto y_val = start_tally (UP, DOWN);
  auto abs = and_(x_val, y_val);

  //this will calculate RELATIVE x and y position based on keys pressed
  auto x_diff = diff (LEFT, RIGHT);
  auto y_diff = diff (DOWN, UP);
  auto rel = and_(x_diff, y_diff);


  auto abs_place = putXY_ (TREX);

  auto output = pipe_(coutall_, flush_);
  //auto graph = pipe_ (abs, place, output);

  //merge the values into a movement, then print the unicode there
   auto graph = pipe_ (rel, merge_(move_by_), append_(place_(CHICKEN)), output);

  // This will print the time since last it was called
  // Q: How to get that time to print as well?
  //  auto print_time = pipe_ (start_timer_ (), print_with_ ("timer"));

  // this will launch and repeat
  auto fut = repeat_pipe_ (getchar_, graph);

//  std::cout << string_right_("hello") << move_by_(1,2) << string_right_("XXX") << move_by_(0,-2) << std::flush;
//
  while (tick ())
    {
    }

  return 1;
}
