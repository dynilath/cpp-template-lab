#include <iostream>
#include "./include/closure.hpp"
#include "./include/utils.hpp"

// This example is from Chapter 4, Compilers - Principles, Techniques, & Tools

// symbol definition
struct g_E : nonterminal{};
struct g_T : nonterminal{};
struct t_plus : terminal{};
struct t_star : terminal{};
struct g_F : nonterminal{};
struct t_lbrac : terminal{};
struct t_rbrac : terminal{};
struct t_id : terminal{};
struct g_Es :nonterminal{};

// E -> E + T
using g1 = generation<g_E,g_E,t_plus,g_T>;
// E -> T
using g2 = generation<g_E,g_T>;
// T -> T * F
using g3 = generation<g_T,g_T,t_star,g_F>;
// T -> F
using g4 = generation<g_T,g_F>;
// T -> ( E )
using g5 = generation<g_F,t_lbrac,g_E,t_rbrac>;
// F -> id
using g6 = generation<g_F,t_id>;
// E' -> E
using gR = generation<g_Es,g_E>;

using my_symbols = ::std::tuple<g_Es,g_E,g_T,g_F,t_plus,t_star,t_lbrac,t_rbrac,t_id>;
using my_generations = ::std::tuple<gR,g1,g2,g3,g4,g5,g6>;

// E' -> . E
using i1 = generation<g_Es, dot_placeholder, g_E>;

int main(int argc, char const *argv[])
{
    ::std::cout << ">> my_generations" << ::std::endl;
    print_closure(my_generations());

    using gR_closure = lr_closure<::std::tuple<>,::std::tuple<i1>,::std::tuple<>,my_generations>::result;
    ::std::cout << ">> gR_closure" << ::std::endl;
    print_closure(gR_closure());
    
    using lr0 = lr_zero<::std::tuple<>,::std::tuple<gR_closure>,::std::tuple<>,my_symbols,my_generations>::result;
    ::std::cout << ">> lr0" << ::std::endl;
    print_closure_set(lr0());

    return 0;
}
