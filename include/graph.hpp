#ifndef GRAPHING_HAPPENING
#define GRAPHING_HAPPENING

#include <iomanip>
#include <sstream>
#include "basic.hpp"
#include "macros.hpp"
#include "arrows.hpp"

namespace ohio {

using hana::pipe_;

/// endline
auto endl_ = [](auto &&xs) {
  std::cout << std::endl;
  return true;
};

/// flush
auto flush_ = [](auto &&xs) {
  std::cout << std::flush;
  return true;
};


/// Print anything do not carriage return
auto cout_ = [](auto &&xs) {
  std::cout << xs;
  return true;
};

/// Print anything do not carriage return
auto cout_with_ = [](auto &&start, auto &&end) {
  return [=](auto &&y) {
    std::cout << start << y << end;
    return true;
  };
};

/// Print anything do not carriage return
auto coutall_ = [](auto &&... xs) {
  return hana::transform (hana::make_tuple (xs...), cout_);
};

/// Print anything
auto print_ = [](auto &&xs) {
  std::cout << "printing: " << xs << std::endl;
  return true;
};

/// Print with
auto print_with_ = [](auto &&p) {
  return [=](auto &&xs) {
    std::cout << p << ": " << xs << std::endl;
    return true;
  };
};

/// Print a bunch of things
auto printall_ = [](auto &&... xs) {
  return hana::transform (hana::make_tuple (xs...), print_);
};


// returns string representing value of t x mult
// - o--------------------o +
auto graph_ = [](float mult) {
  return [=](float t) {
    std::stringstream stream;
    float mag = abs (t);

    int x = mag * mult;

    if (t < 0)
      {
        for (int i = 0; i < (10 - x); ++i)
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
        for (int i = 0; i < (10 - x); ++i)
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

// returns string representing value of t x mult
//   o
//   |
//   |
//   |
//   |
//   |
//
auto vplot_ = [](float mult) {
  return [=](float t) {
    std::stringstream stream;
    float mag = abs (t);

    int x = mag * mult;

    if (x > 10)
      x = 10;
    if (x < 0)
      x = 0;

    for (int i = 0; i <= 10; ++i)
      stream << "\033[1A\033[K";

    stream << BLUE;
    for (int i = 0; i <= 10 - x; ++i)
      stream << "\n" << 10 - i;
    stream << RED << "o";
    for (int i = x; i > 0; --i)
      stream << "\n" << i - 1;

    return stream.str ();
  };
};


auto clear_ = [](float height) {
  std::stringstream stream;
  //    for (int i = 0; i <= 10; ++i)
  stream << "\033[2J";
  return stream.str ();
};

//binary send cursor to x,y
auto move_by_ = [](float x, float y) {
  std::stringstream stream;

  if (y != 0)
    stream << "\033[" << abs (y) << (y > 0 ? "A" : "B");

  if (x != 0)
    stream << "\033[" << abs (x) << (x > 0 ? "C" : "D");

  return stream.str ();
};

//binary send cursor to x,y
auto move_to_ = [](float x, float y) {
  std::stringstream stream;
  stream << "\033[" << (int) y << ";" << (int) x << "H ";
  return stream.str ();
};


// make a string and send cursor back to beginning of it
auto string_right_ = [](auto &&p) {
  std::string t (p);
  std::stringstream stream;
  stream << t << "\033[" << t.length () << "D";
  return stream.str ();
};

// make a char and send cursor back 1
auto place_= [](auto &&c) {
  std::stringstream stream;
  stream << c << "\033[" << 1 << "D";
  return stream.str ();
};
// todo make a string backwards?
auto string_left_ = [](auto &&p) {
  std::string t (p);
  std::stringstream stream;
  stream << t << "\033[" << t.length () << "D";
  return stream.str ();
};

//place a character at a coordinate,
//takes a pair of values representing X and Y
auto putXY_ = [](auto &&glyph) {
  return pipe_ (merge_ (move_to_), place_ (glyph));
};

// prepend a string to incoming signal
auto prepend_ = [](auto &&start) {
  return [=](auto &&in) {
    std::stringstream stream;
    stream << start << in;
    return stream.str ();
  };
};


// append a string to incoming signal
auto append_ = [](auto &&end) {
  return [=](auto &&in) {
    std::stringstream stream;
    stream << in << end;
    return stream.str ();
  };
};

/// erase to end of line
auto erase_ = prepend_("\033[K");
/// go to begining of line
auto return_ = prepend_("\r");
/// clears current line (effectively returns then erases);
auto clearline_ = pipe_(erase_, return_);


}  // ohio::

#endif
