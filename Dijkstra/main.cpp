#include <iostream>
#include <tuple>
#include <type_traits>

template <typename V1_, typename V2_, unsigned Weight_> struct Edge {
  using V1 = V1_;
  using V2 = V2_;
  static constexpr auto Weight = Weight_;
};

template <typename V_, unsigned W_> struct dijk_node {
  using V = V_;
  static constexpr auto Weight = W_;
};

template <typename T> struct result_type {
  using result = T;
};
template <unsigned V> struct result_value {
  static constexpr auto value = V;
};

template <typename... T> struct is_marked;
template <typename V> struct is_marked<std::tuple<>, V> : std::false_type {};
template <typename... nVs, unsigned... nWs, typename T0, unsigned W0,
          typename V>
struct is_marked<std::tuple<dijk_node<T0, W0>, dijk_node<nVs, nWs>...>, V>
    : std::conditional_t<std::is_same_v<T0, V>, std::true_type,
                         is_marked<std::tuple<dijk_node<nVs, nWs>...>, V>> {};

template <typename V, typename... Nodes>
inline constexpr auto is_marked_v = is_marked<std::tuple<Nodes...>, V>::value;

template <typename... T> struct min_of_nodes;
template <typename rV, unsigned rW>
struct min_of_nodes<std::tuple<>, dijk_node<rV, rW>>
    : result_type<dijk_node<rV, rW>> {};
template <typename nV1, unsigned nW1, typename... Nodes, typename rV,
          unsigned rW>
struct min_of_nodes<std::tuple<dijk_node<nV1, nW1>, Nodes...>,
                    dijk_node<rV, rW>>
    : std::conditional_t<
          (nW1 < rW), min_of_nodes<std::tuple<Nodes...>, dijk_node<nV1, nW1>>,
          min_of_nodes<std::tuple<Nodes...>, dijk_node<rV, rW>>> {};
template <typename nV1, unsigned nW1, typename... Nodes>
struct min_of_nodes<std::tuple<dijk_node<nV1, nW1>, Nodes...>>
    : min_of_nodes<std::tuple<Nodes...>, dijk_node<nV1, nW1>> {};

template <typename... T> struct connect_result_single;
template <typename... Marked, typename V1, typename V2, unsigned Ws,
          typename... Results>
struct connect_result_single<std::tuple<Marked...>, Edge<V1, V2, Ws>,
                             std::tuple<>, std::tuple<Results...>>
    : result_type<std::tuple<Results...>> {};
template <typename... Marked, typename... nMs, typename nV, unsigned nW,
          typename V1, typename V2, unsigned Ws, typename... Results>
struct connect_result_single<std::tuple<Marked...>, Edge<V1, V2, Ws>,
                             std::tuple<dijk_node<nV, nW>, nMs...>,
                             std::tuple<Results...>>
    : std::conditional_t<
          std::is_same_v<nV, V1> && !is_marked_v<V2, Marked...>,
          connect_result_single<std::tuple<Marked...>, Edge<V1, V2, Ws>,
                                std::tuple<nMs...>,
                                std::tuple<dijk_node<V2, Ws + nW>, Results...>>,
          std::conditional_t<
              std::is_same_v<nV, V2> && !is_marked_v<V1, Marked...>,
              connect_result_single<
                  std::tuple<Marked...>, Edge<V1, V2, Ws>, std::tuple<nMs...>,
                  std::tuple<dijk_node<V1, Ws + nW>, Results...>>,
              connect_result_single<std::tuple<Marked...>, Edge<V1, V2, Ws>,
                                    std::tuple<nMs...>,
                                    std::tuple<Results...>>>> {};

template <typename... T> struct connect_result;
template <typename... Marked, typename... Results>
struct connect_result<std::tuple<Marked...>, std::tuple<>,
                      std::tuple<Results...>>
    : result_type<std::tuple<Results...>> {};
template <typename... Marked, typename E, typename... Es, typename... Results>
struct connect_result<std::tuple<Marked...>, std::tuple<E, Es...>,
                      std::tuple<Results...>>
    : connect_result<std::tuple<Marked...>, std::tuple<Es...>,
                     typename connect_result_single<
                         std::tuple<Marked...>, E, std::tuple<Marked...>,
                         std::tuple<Results...>>::result> {};

template <typename Marked, typename Edges, typename = void>
struct try_add_connect_result : result_type<Marked> {};
template <typename... Marked, typename Edges>
struct try_add_connect_result<
    std::tuple<Marked...>, Edges,
    std::void_t<typename min_of_nodes<typename connect_result<
        std::tuple<Marked...>, Edges, std::tuple<>>::result>::result>>
    : result_type<std::tuple<
          Marked...,
          typename min_of_nodes<typename connect_result<
              std::tuple<Marked...>, Edges, std::tuple<>>::result>::result>> {};

template <typename... T> struct mark_min_connect_result;
template <typename... Marked, typename... Es>
struct mark_min_connect_result<std::tuple<Marked...>, std::tuple<Es...>>
    : try_add_connect_result<std::tuple<Marked...>, std::tuple<Es...>> {};

template <typename... T> struct mark_till_fix_point;
template <typename... Marked, typename... Es>
struct mark_till_fix_point<std::tuple<Marked...>, std::tuple<Es...>>
    : std::conditional_t<
          std::is_same_v<std::tuple<Marked...>,
                         typename mark_min_connect_result<
                             std::tuple<Marked...>, std::tuple<Es...>>::result>,
          result_type<std::tuple<Marked...>>,
          mark_till_fix_point<
              typename mark_min_connect_result<std::tuple<Marked...>,
                                               std::tuple<Es...>>::result,
              std::tuple<Es...>>> {};

template <typename... T> struct get_result;
template <typename T>
struct get_result<std::tuple<>, T> : result_value<static_cast<unsigned>(-1)> {};
template <typename... Marked, typename mV, unsigned mW, typename T>
struct get_result<std::tuple<dijk_node<mV, mW>, Marked...>, T>
    : std::conditional_t<std::is_same_v<mV, T>, result_value<mW>,
                         get_result<std::tuple<Marked...>, T>> {};

template <typename... T> struct dijkstra_solve;
template <typename... Vs, typename... Edges, typename V0>
struct dijkstra_solve<std::tuple<Vs...>, std::tuple<Edges...>, V0> {
  using result = typename mark_till_fix_point<std::tuple<dijk_node<V0, 0>>,
                                              std::tuple<Edges...>>::result;
  template <typename T> using target = get_result<result, T>;
};

struct V1 {};
struct V2 {};
struct V3 {};
struct V4 {};
struct V5 {};
struct V6 {};

using E1 = Edge<V1, V2, 7>;
using E2 = Edge<V1, V3, 14>;
using E3 = Edge<V1, V4, 9>;
using E4 = Edge<V2, V4, 10>;
using E5 = Edge<V2, V5, 15>;
using E6 = Edge<V3, V6, 9>;
using E7 = Edge<V3, V4, 2>;
using E8 = Edge<V4, V5, 11>;
using E9 = Edge<V5, V6, 6>;

using Vs = std::tuple<V1, V2, V3, V4, V5, V6>;
using Es = std::tuple<E1, E2, E3, E4, E5, E6, E7, E8, E9>;
using solved = dijkstra_solve<Vs, Es, V1>;

int main() {
  std::cout << "V1 distance to V1 : " << solved::target<V1>::value << "\n";
  std::cout << "V2 distance to V1 : " << solved::target<V2>::value << "\n";
  std::cout << "V3 distance to V1 : " << solved::target<V3>::value << "\n";
  std::cout << "V4 distance to V1 : " << solved::target<V4>::value << "\n";
  std::cout << "V5 distance to V1 : " << solved::target<V5>::value << "\n";
  std::cout << "V6 distance to V1 : " << solved::target<V6>::value << "\n";
}