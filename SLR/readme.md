# SLR in C++ templates

```C++
// This example is from Chapter 4, Compilers - Principles, Techniques, & Tools

// symbol definition
struct g_E : nonterminal {};
struct g_T : nonterminal {};
struct t_plus : terminal {};
struct t_star : terminal {};
struct g_F : nonterminal {};
struct t_lbrac : terminal {};
struct t_rbrac : terminal {};
struct t_id : terminal {};
struct g_Es : nonterminal {};

// E -> E + T
using g1 = generation<g_E, g_E, t_plus, g_T>;
// E -> T
using g2 = generation<g_E, g_T>;
// T -> T * F
using g3 = generation<g_T, g_T, t_star, g_F>;
// T -> F
using g4 = generation<g_T, g_F>;
// T -> ( E )
using g5 = generation<g_F, t_lbrac, g_E, t_rbrac>;
// F -> id
using g6 = generation<g_F, t_id>;
// E' -> E
using gR = generation<g_Es, g_E>;

using my_symbols =
    ::std::tuple<g_Es, g_E, g_T, g_F, t_plus, t_star, t_lbrac, t_rbrac, t_id>;
using my_generations = ::std::tuple<gR, g1, g2, g3, g4, g5, g6>;

// E' -> . E
using i1 = generation<g_Es, dot_placeholder, g_E>;

int main(int argc, char const *argv[]) {
  ::std::cout << ">> my_generations" << ::std::endl;
  print_closure(my_generations());

  using gR_closure = lr_closure<::std::tuple<>, ::std::tuple<i1>,
                                ::std::tuple<>, my_generations>::result;
  ::std::cout << ">> gR_closure" << ::std::endl;
  print_closure(gR_closure());

  using lr0 = lr_zero<::std::tuple<>, ::std::tuple<gR_closure>, ::std::tuple<>,
                      my_symbols, my_generations>::result;
  ::std::cout << ">> lr0" << ::std::endl;
  print_closure_set(lr0());

  return 0;
}
```

output:
```
>> my_generations
struct g_Es struct g_E
struct g_E struct g_E struct t_plus struct g_T
struct g_E struct g_T
struct g_T struct g_T struct t_star struct g_F
struct g_T struct g_F
struct g_F struct t_lbrac struct g_E struct t_rbrac
struct g_F struct t_id

>> gR_closure
struct g_Es struct dot_placeholder struct g_E
struct g_E struct dot_placeholder struct g_E struct t_plus struct g_T
struct g_E struct dot_placeholder struct g_T
struct g_T struct dot_placeholder struct g_T struct t_star struct g_F
struct g_T struct dot_placeholder struct g_F
struct g_F struct dot_placeholder struct t_lbrac struct g_E struct t_rbrac
struct g_F struct dot_placeholder struct t_id

>> lr0
L0
struct g_Es struct dot_placeholder struct g_E
struct g_E struct dot_placeholder struct g_E struct t_plus struct g_T
struct g_E struct dot_placeholder struct g_T
struct g_T struct dot_placeholder struct g_T struct t_star struct g_F
struct g_T struct dot_placeholder struct g_F
struct g_F struct dot_placeholder struct t_lbrac struct g_E struct t_rbrac
struct g_F struct dot_placeholder struct t_id

L1
struct g_Es struct g_E struct dot_placeholder
struct g_E struct g_E struct dot_placeholder struct t_plus struct g_T

L2
struct g_E struct g_T struct dot_placeholder
struct g_T struct g_T struct dot_placeholder struct t_star struct g_F

L3
struct g_T struct g_F struct dot_placeholder

L4
struct g_F struct t_lbrac struct dot_placeholder struct g_E struct t_rbrac
struct g_E struct dot_placeholder struct g_E struct t_plus struct g_T
struct g_E struct dot_placeholder struct g_T
struct g_T struct dot_placeholder struct g_T struct t_star struct g_F
struct g_T struct dot_placeholder struct g_F
struct g_F struct dot_placeholder struct t_lbrac struct g_E struct t_rbrac
struct g_F struct dot_placeholder struct t_id

L5
struct g_F struct t_id struct dot_placeholder

L6
struct g_E struct g_E struct t_plus struct dot_placeholder struct g_T
struct g_T struct dot_placeholder struct g_T struct t_star struct g_F
struct g_T struct dot_placeholder struct g_F
struct g_F struct dot_placeholder struct t_lbrac struct g_E struct t_rbrac
struct g_F struct dot_placeholder struct t_id

L7
struct g_T struct g_T struct t_star struct dot_placeholder struct g_F
struct g_F struct dot_placeholder struct t_lbrac struct g_E struct t_rbrac
struct g_F struct dot_placeholder struct t_id

L8
struct g_F struct t_lbrac struct g_E struct dot_placeholder struct t_rbrac
struct g_E struct g_E struct dot_placeholder struct t_plus struct g_T

L9
struct g_E struct g_E struct t_plus struct g_T struct dot_placeholder
struct g_T struct g_T struct dot_placeholder struct t_star struct g_F

L10
struct g_T struct g_T struct t_star struct g_F struct dot_placeholder

L11
struct g_F struct t_lbrac struct g_E struct t_rbrac struct dot_placeholder
```