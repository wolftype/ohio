#ifndef OHIO_SIGNAL_INCLUDED
#define OHIO_SIGNAL_INCLUDED

#include <boost/hana/functional/compose.hpp>

namespace hana = boost::hana;
namespace ohio{

/*-----------------------------------------------------------------------------
 *  Basic Signals (Functions of Time in milliseconds -- to be fed time_)
    @todo make 1000 a macro to allow for microseconds 1000000 perhaps
 *-----------------------------------------------------------------------------*/

  /// detects change from false to true of incoming signal
  auto detect_edge_ = [](){
    bool bNewEdge = true;
    return[=](bool&& b) mutable {
      if ( b && bNewEdge){
        bNewEdge = false;
        return true;
      } else if ( !b ){
        bNewEdge = true; /// reset edge to true
      }
      return false;
    };

  };


  ///detects change of large amt
  auto detect_change_=[](float&& amt, float&& start){

    return[&](float&& t){
      if ( (t-amt) > start ) {
        start = t;
        return true;
      }

      return false;

    };
  };


  ///detects that input has stopped incrementing (use with mod_)
  auto detect_reset_ = [](){
    int start = 0;
    return[=](auto&& t) mutable {
      if ( t > start ) {
        start = t;
        return false;
      } else {
        start = 0;
      }
      return true;
    };
  };


/// Generate Oscillator from incoming Time at rate x times per second
auto osc_ = [](float&& x){
  //divide incoming time in milliseconds, convert to radians, calculate sine
  return hana::compose( sin_, to_radians_, divide_by_(1000.0/(360.0*x)));
};

/// Generate Impulse from incoming Time at rate x times per second
auto impulse_ = [](float&& x){
  //find modulus of incoming signal % 1000/x, check if current value is less than previous
  return hana::compose( detect_reset_(), mod_(1000.0/x) );
};

/// Generate Ramp output from incoming Time at rate x times per second
auto ramp_ = [](float&& x){
  ///map to value in range of [0,1)
  auto tmp = 1000.0/x;
  return hana::compose( divide_by_(tmp), mod_(tmp) );
};

/// Generate Sawtooth output from incoming Time at rate x times per second
auto saw_ = [](auto&& x){
  //map to value in range of (-1,1)
  auto tmp = 1000.0/x;

  auto f = [=](auto&& t) {
    auto up = t/tmp;
    auto down = 1-up;
    return (4 * ( up < .5 ? up : down ) )-1;
  };

  return hana::compose(
      f,
      mod_(tmp)
  );
};

/// Signal Functions
/// constant signal function
auto constant_ = [](auto&& x){
  return [=](auto&& ... xs){
    return x;
  };
};

/// Return Input
auto id_ = [](auto&& x) { return x; };


} //ohio::
#endif /* end of include guard: OHIO_SIGNAL_INCLUDED */
