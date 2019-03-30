/*
 * =====================================================================================
 *
 *       Filename:  arrows.hpp
 *
 *    Description:  simple combinators putting signal functions together
 *
 *        Version:  1.0
 *        Created:  03/11/2016 16:29:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto, gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef  arrows_INC
#define  arrows_INC

#include <boost/hana/fold_right.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/pair.hpp>

namespace hana = boost::hana;
namespace ohio {

/*-----------------------------------------------------------------------------
 *  ARROW combinators, take processes and return functions that switch processes
 *-----------------------------------------------------------------------------*/

/// do function f1 and then function f2 on same input
auto seq_ = [](auto&& f1, auto&& f2){
  return[=](auto&& ... xs){
    f1(xs...);
    f2(xs...);
  };
};

auto all_ = [](auto&& ... sf){
  return [=](auto&& ... xs) mutable {
    return hana::make_tuple (sf(std::forward<typename std::decay< decltype(xs)>::type >(xs)...)...);
  };
};

/// do signal function sf1 and signal function sf2 on same input returns a Product Typeclass
auto and_ = [](auto&& sf1, auto&& sf2){
  return[=](auto&& ... xs) mutable {
    return hana::make_pair(
      //std::forward< typename std::decay< decltype(sf1) >::type >(sf1)
      sf1( std::forward< typename std::decay< decltype(xs) >::type >(xs)...),
      sf2( std::forward< typename std::decay< decltype(xs) >::type >(xs)...)
      //sf2(xs...)
     );
  };
};

/// do function f1 and function f2 in parallel on first and second of input
auto both_ = [](auto&& f1, auto&& f2){
  return [=](auto&& ... xs){ //xs are Product types
    return hana::make_pair(
      f1( hana::first(xs) ... ),
      f2( hana::second(xs) ... )
     );
  };
};

/// do function f1 on first of input
auto first_ = [](auto&& f){
  return [=](auto&& xs){ //xs is a Product type
    return hana::make_pair( f( hana::first(xs) ), hana::second(xs) );
  };
};

/// do function f on second
auto second_ = [](auto&& f){
  return [=](auto&& xs){ //xs is a Product type
    return hana::make_pair( hana::first(xs), f( hana::second(xs) ) );
  };
};


///// pipe output of f1 into input of f2 @sa hana::demux etc
//auto pipe_ = [](auto&& f1, auto&& f2){
//  return [=](auto&& ... xs) mutable {
//    return f2( f1 ( std::forward< typename std::decay< decltype(xs) >::type > (xs)...) );
//  };
//};

/// make a pair
auto pair_ = [](auto&& a, auto&& b){
  return hana::make_pair(a,b);
};

/// merge a pair with binary function f (e.g add_, print_, mult_)
auto merge_ = [](auto&& f){
  return [=](auto&& xs){ //xs is a Product type
    return f( hana::first(xs), hana::second(xs) );
  };
};

auto mergeall_ = [](auto&& f){
  return [=](auto&& ... xs) mutable{
    return f( FORWARD(xs)...);
  };
};

/// do function f (which returns a pair) on input and return first of result
auto loop_ = [](auto&& f){ // a first of something
  return [=](auto&& ... xs){ //f(xs..) is a Product type
    return hana::first( f(xs...) );
  };
};

/// hana::fold_right as a pipeline of functions: f(a, f(b, f(c, f(d, e))))
//auto pipeline_ = [](auto&& ... fs){
//  return hana::fold_right( hana::make_tuple(std::forward< typename std::decay< decltype(fs) >::type > (fs)... ), pipe_);
//};

}//ohio::

namespace boost { namespace hana {

    template <typename F, typename G>
    struct _pipe {
        F f; G g;

        template <typename X, typename ...Xs>
        constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) const& {
            return g(
                f(static_cast<X&&>(x)),
                static_cast<Xs&&>(xs)...
            );
        }

        template <typename X, typename ...Xs>
        constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) & {
            return g(
                f(static_cast<X&&>(x)),
                static_cast<Xs&&>(xs)...
            );
        }

        template <typename X, typename ...Xs>
        constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) && {
            return std::move(g)(
                std::move(f)(static_cast<X&&>(x)),
                static_cast<Xs&&>(xs)...
            );
        }
    };

    struct _make_pipe {
        template <typename F, typename G, typename ...H>
        constexpr decltype(auto) operator()(F&& f, G&& g, H&& ...h) const {
            return detail::variadic::foldr1(detail::create<_pipe>{},
                static_cast<F&&>(f),
                static_cast<G&&>(g),
                static_cast<H&&>(h)...
            );
        }
    };

    constexpr _make_pipe pipe_{};





 //   template <typename F, typename G>
 //   struct _all {
 //       F f; G g;

 //       template <typename X, typename ...Xs>
 //       constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) const& {
 //           return hana::make_pair(
 //               f(static_cast<X&&>(x),static_cast<Xs&&>(xs)...),
 //               g(static_cast<X&&>(x),static_cast<Xs&&>(xs)...)
 //           );
 //       }

 //       template <typename X, typename ...Xs>
 //       constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) & {
 //           return hana::make_pair(
 //               f(static_cast<X&&>(x),static_cast<Xs&&>(xs)...),
 //               g(static_cast<X&&>(x),static_cast<Xs&&>(xs)...)
 //           );;
 //       }

 //       template <typename X, typename ...Xs>
 //       constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) && {
 //           return hana::make_pair(
 //               std::move(f)(static_cast<X&&>(x),static_cast<Xs&&>(xs)...),
 //               std::move(g)(static_cast<X&&>(x),static_cast<Xs&&>(xs)...)
 //           );
 //       }
 //   };

 //   /// split signal into many
 //   struct _make_all {
 //       template <typename F, typename G, typename ...H>
 //       constexpr decltype(auto) operator()(F&& f, G&& g, H&& ...h) const {
 //           return detail::variadic::foldr1(detail::create<_all>{},
 //               static_cast<F&&>(f),
 //               static_cast<G&&>(g),
 //               static_cast<H&&>(h)...
 //           );
 //       }
 //   };

 //   constexpr _make_all all_{};





    /// Split signal to two functions F and G (call with split_(auto&& f, auto&& g) )
    template <typename F, typename G>
    struct _split {
        F f; G g;

        template <typename ...Xs>
        constexpr decltype(auto) operator()(Xs&& ...xs) const& {
            return make_pair( f(static_cast<Xs&&>(xs)...), g(static_cast<Xs&&>(xs)...) );
        }

        template <typename ...Xs>
        constexpr decltype(auto) operator()(Xs&& ...xs) & {
            return make_pair( f(static_cast<Xs&&>(xs)...), g(static_cast<Xs&&>(xs)...) );
        }

        template <typename ...Xs>
        constexpr decltype(auto) operator()(Xs&& ...xs) && {
            return make_pair( std::move(f)(static_cast<Xs&&>(xs)...), std::move(g)(static_cast<Xs&&>(xs)...) );
        }
    };

    /// split signal into many
    struct _make_split {
        template <typename F, typename G, typename ...H>
        constexpr decltype(auto) operator()(F&& f, G&& g, H&& ...h) const {
            return detail::variadic::foldr1(detail::create<_split>{},
                static_cast<F&&>(f),
                static_cast<G&&>(g),
                static_cast<H&&>(h)...
            );
        }
    };


    constexpr _make_split split_{};


}} // boost::hana::

#endif   /* ----- #ifndef arrows_INC  ----- */
