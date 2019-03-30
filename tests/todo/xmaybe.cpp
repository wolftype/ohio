#include "basic.hpp"


using namespace std;

using namespace ohio;




template<class T >
decltype(auto) mdecay( T&& f){
    cout << "not maybe A" << endl;
    return FORWARD(f);
}

template<class T >
decltype(auto) mdecay(const T& f){
    cout << "not maybe B" << endl;
    return f;
}

template<class T>
decltype(auto) mdecay( std::experimental::optional<T>&& m){
    cout << "maybe A" << endl;
    if (m) return FORWARD( mdecay( FORWARD(*m) ) );
    return FORWARD( mdecay( T() ) );
}

template<class T>
decltype(auto) mdecay( const maybe<T>& m){
    cout << "maybe B" << endl;
    if (m) return mdecay( *m );
    return mdecay( T() );
}

int main(){

    maybe< maybe<int> > x( maybe<int>(1) );
    mdecay( FORWARD(x) );
    return 0;
}
