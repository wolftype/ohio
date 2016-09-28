/*
 * =====================================================================================
 *
 *       Filename:  macros.hpp
 *
 *    Description:  
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

#ifndef  macros_INC
#define  macros_INC

namespace ohio {



    // decay all types
    #define TYPE(x) typename std::decay<decltype(x)>::type

    // forward all types
    #define FORWARD(x) std::forward<TYPE(x)>(x)

    // create an instance of a optional type @todo use this for flattening optional optionals ...
    #define MVAL(x) std::declval<typename decltype(x)::value_type>()

    // get type of optional type
    #define MTYPE(x) TYPE(MVAL(x))

    // get type of optional optional type
    #define MMTYPE(x) MTYPE(MVAL(MTYPE(x)))

}


#endif   /* ----- #ifndef macros_INC  ----- */
