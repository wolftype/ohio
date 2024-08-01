// All the different when
//
//
// when1_
// when2_
//
// future_when_
//
#include "ohio.hpp"
using namespace std;
using namespace ohio;

int main()
{
  bool b;
 auto f1 = getchar_;//loop_for_(3,and_(print_,tckr_(.5,b)));//wait_(2);
 auto f2 = print_;
 //launch listener
// auto fut = thread_future_(f1)();

// auto func = future_then_(f2);
// auto fut2 = thread_future_(func)(fut);

// thread_args_(f1);

do_repeat_pipe_ (getchar_, print_);
 auto test = hana::fix (
    [](auto self, auto n) -> int{
      if (n == 0)
        return 1;
      else
        return n * self(n-1);
     }
  );
 cout << "gbye" << " " << test(8) << endl;

 while (true){}
 return 0;
}
