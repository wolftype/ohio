///tmp listeners

auto ttlisten = []( auto&& f ){
  //auto e = tlisten(f);
  auto e = tlisten(f);
  return [=](auto&&...xs) {
    auto tmp = 1;//f(xs...);
    return 1;
  };
};


///sa std::experimental::optional to clear data
/// listen for asynchronous function future and launch again when received
auto rlisten = [](auto&& f){
  using T = typename std::decay< decltype(f) >::type;
  auto nf = f;
  auto tmp = async_( std::forward<T>(f) )();
  return[=](auto&& ... xs) mutable {
    auto opt = estream_( tmp ) //std::forward< typename std::decay<decltype(tmp)>::type>(tmp) )
      ( std::forward< typename std::decay< decltype(xs)>::type>(xs)...);//loop_(procs)(xs...);
    if (opt) {
      tmp = async_( std::forward<T>(f) )(); // crashes if we don't forward here
     }
    return opt;
  };
};

auto tlisten = [](auto&& f){
  using ST = std::shared_future<int>;
  ST tmp;
  return [tmp](auto&& ... xs) mutable {
    ST st = tmp ;//estream_(tmp);
    return tmp;//st;
  };
};

auto listen = [](auto&& procs){
 // auto lp = loop_(procs);
  auto tmp = procs;
  return[=](auto&& ... xs) mutable {
    auto fut = tmp(xs...);//loop_(procs)(xs...);
    if (fut) {
      auto nf = async_( stdin_)();
      tmp = estream_(nf);
     }
      return fut;
    };
};




/// feed starting velocity and trigger function to be launched asynchronously
auto velocity = [](auto&& v0, auto&& e){
  auto tmp = v0;
  //using F = typename std::decay< decltype(f)>::type;
  //auto e = listenr( stdin_ );// ttlisten( tlisten( [](){return 1;} ) );//stdin_ )();//std::forward<F>(f) );
  using T = typename std::decay< decltype(e) >::type;
  return [=](auto&& x) mutable {
    auto etag = e();//e;//tag_( e(), constant_(tmp+1) ); /// add one
    //tmp = rswitch_( constant_(tmp) )( hana::make_pair(x, etag) ); /// if triggered, call trigger function
    return tmp;
  };
};
