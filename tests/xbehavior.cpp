/// launching behaviors

#include "behavior.hpp"
#include <vector>

using namespace ohio;

/// A buffer of N elements (stored as bytes, old school)
/// feed it time in seconds over which to listen to incoming signal
struct buffer_t{

    float sec = 0;

    //std::unique_ptr<char[]> val;
    std::vector<unsigned char> val = {};

    template<typename T>
    void init(){

    }

};

auto buffer = buffer_t{1}; // a one second buffer

//template<typename T>
struct delayt{

 int num = 10; 						///< set by calling function e.g. return delayt{100};
 int idx = 0;						///< idx into buffer
 bool bStart = false;
 std::unique_ptr<char[]> buffer;
 std::unique_ptr<bool[]> bool_buffer;

 template<typename T>
 void init(){
     cout << "delay num: " << num << endl;
     cout << typeid(T).name() << endl;
     buffer = std::unique_ptr<char[]> ( new char[num*sizeof(T)] );
     bool_buffer = std::unique_ptr<bool[]> ( new bool[num] );
 }

 template<typename SF>
 constexpr decltype(auto) operator()(SF&& sf){
    using T =  TYPE(MVAL(sf(0))(0)); // Value Type: sf is a signal function, sf(0) is a possible signal, and sf(0)(0) is its value
    init<T>();
    return[sf,this](auto&& xs) mutable {
        auto val = ((T*)buffer.get())[idx];									//value at idx
        auto flag = bool_buffer[idx];
        auto te = FORWARD(sf)(FORWARD(xs)); 								//te will be a signal returning maybe<T> . . .
        int tmpidx = idx;
        idx++;
        if (idx==num && !bStart) { cout << "START" << endl; bStart = true; } //trigger start reading buffer
        if (idx==num) idx = 0;												 //loop back to beginning of buffer

        if (te) {
            ((T*)buffer.get())[tmpidx] = (*te)(xs);  //te(xs) will be a value
            bool_buffer[tmpidx] = true;
        } else {
            ((T*)buffer.get())[tmpidx]= T();
            bool_buffer[tmpidx] = false;
        }
         return flag ? constant_( maybe<T>(val) ) : constant_(maybe<T>());
    };
 }

};

auto delay_ = [](auto&& sec){
 return[=](auto&& e) mutable { 		// e is a function that will return an optional type
   using T = int;//MTYPE(e(0)); 			// hmmm are we sure this is a maybe<T>?
   int len = sec*1000;
   int idx = 0;
   bool bStart = false;
   bool bFirst = true;
   //auto buffer = std::make_shared<char, std::default_delete<char[]> >( new char[0] );
   //std::shared_ptr<char> buffer ( new char[0], std::default_delete<char[]>() );
   std::shared_ptr<T> buffer ( new T[len], std::default_delete<T[]>() );
   return[&buffer](auto&& xs) mutable{
       // return buffer
//       if (bFirst){
//           std::shared_ptr<char> tmpbuffer ( new char[len*sizeof(T), std::default_delete<char[]>() );
//          // auto tmpbuf = std::make_<char[]>( len * sizeof(T) );
//           buffer = tmpbuffer;//std::move(tmpbuf);
//           bFirst = false;
//       }
        auto tmp = buffer;//.get();//[0] = 1;// ((T*)&buffer[0])[idx];
        return 1;
//       auto te = FORWARD(e)(FORWARD(xs));
//       if (te) {
//        ((T*)&buffer[0])[idx] = (*te)(xs);//std::move(*te);//( FORWARD(xs));
//        idx++;
//        if (idx==len) idx = 0;
//        if (bStart) return maybe<T>(tmp);
//        if (idx==len && !bStart) bStart = true;
//       }
//       return maybe<T>();
   };
 };
};

int main(){

    AppStartTime = now();


    auto b1 = behavior();

    // Signal Function: evaluate sf every .2 seconds (returns a new function to be evaluated signal constant)
    auto e1 = every_(.2, [](auto&& t){ return constant_(std::string("hello")); } );//constant_(1));//

    auto s = "hello";
    cout << typeid( decltype(s) ).name() << endl;
    // SF: will delay evaluation of e1 by 1 second, still returning a constant_(1)
     //auto delay = delayt(1)(e1);
    // Delay the evaluated signal result
    delayt delay{100};
    auto d = delay( e1 );
   // b1.launch( delay );

    auto tick = wait_(.001);
    while ( tick() ){
       // printf("hello\n");
       auto m = d( time_() )(0);
       if (m)   cout << *m << endl;
       //if (m) cout << *m << endl;
    }


    while(true){}

    return 0;
}
