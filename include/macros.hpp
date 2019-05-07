/*
 * =====================================================================================
 *
 *       Filename:  macros.hpp
 *
 *        Version:  1.0
 *        Created:  05/12/2016 17:03:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef macros_INC
#define macros_INC

namespace ohio {

// decay all types
#define TYPE(x) typename std::decay<decltype (x)>::type

// forward all types
#define FORWARD(x) std::forward<TYPE (x)> (x)

// create an instance of a optional type @todo use this for flattening optional optionals ...
#define MVAL(x) std::declval<typename decltype (x)::value_type> ()

// get type of optional type
#define MTYPE(x) TYPE (MVAL (x))

// get type of optional optional type
#define MMTYPE(x) MTYPE (MVAL (MTYPE (x)))

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

// 105 - 108 are the ijkl keys
#define LEFT 104
#define RIGHT 108
#define UP 107
#define DOWN 106



#define CLEARLINE "\033[D\033[K"      /* Move left and clear line*/
#define TREX "\U0001F996"             /* Dinosaur */
}


#endif /* ----- #ifndef macros_INC  ----- */
