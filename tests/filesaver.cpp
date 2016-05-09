/*
 * =============================================================================
 * Copyright (C) 2010  Pablo Colapinto
 * All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * =============================================================================
*/

#include "ohio.hpp"
#include "json/json.h"

using namespace ohio;

template<typename T>
void save(const T& udata){
  printf("no specification for saving typename\n"); //use traits to print typename
}



//template<typename T>
auto cmdlineCB = [](const std::string& s, auto&& udata){//}, const T& udata ){
  if (s == "save") save(udata);
  //if (s == "quit")
};

struct mydata {
    float f = 5;
};

 template<> void save( const mydata& udata){
   printf("saving mydata\n");
   cout << udata.f << endl;
 }

//   //read data into file
//   Json::Value root;
//   std::stringstream ss; udata.f >> ss;=
//   ss >> root["f"];
// }

int main(){

  mydata data;

  auto cmdline = poll_( stdin_, cmdlineCB, 1, data );
  data.f = 3;
  cmdline();

  printf("asynchronous polling, so this is not blocked\n");

  /// Just keep going by sleeping main thread
  while( wait_(10.0)() ){}

  return 0;
}
