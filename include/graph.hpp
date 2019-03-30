#include <iomanip>
#include <sstream>

namespace ohio {

// returns string representing value of t x mult
// - o--------------------o +
auto graph_ = [](float mult) {
  return [=](float t) {
    std::stringstream stream;
    float mag = abs (t);

    int x = mag * mult;

    if (t < 0)
      {
        for (int i = 0; i < (mult - x); ++i)
          stream << " ";
        stream << RED << "o";
      }
    else
      {
        for (int i = 0; i < 10; ++i)
          stream << " ";
      }

    stream << BLUE;
    for (int i = 0; i < x; ++i)
      stream << "-";

    if (t > 0)
      {
        stream << RED << "o";
        for (int i = 0; i < (mult - x); ++i)
          stream << " ";
      }
    else
      {
        for (int i = 0; i < 10; ++i)
          stream << " ";
      }

    stream << std::setw (3);
    stream << "\t";

    return stream.str ();
  };
};

}  // ohio::
