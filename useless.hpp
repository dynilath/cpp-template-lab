#include <string>
#include <utility>
#include <algorithm>
#include <iostream>

template<std::size_t N>
struct useless_string
{
    char p[N]{};
 
    constexpr useless_string(char const(&pp)[N]){ std::ranges::copy(pp, p); };
};

template<useless_string U>
constexpr auto operator""_useless(){ return U; }

template<useless_string U>
struct useless
{
    int v;
    useless(): v(0) {};
    useless(int v): v(v) {};
    useless(const useless& u): v(u.v) {};
    useless(useless&& u):v{0} {std::swap(v, u.v);};
    ~useless() { if(v) std::cout << U.p;};
};