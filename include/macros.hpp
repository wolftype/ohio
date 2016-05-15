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

  #define TYPE(x) typename std::decay<decltype(x)>::type
  #define FORWARD(x) std::forward<TYPE(x)>(x)
  #define MVAL(x) std::declval<typename decltype(x)::value_type>()

}


#endif   /* ----- #ifndef macros_INC  ----- */
