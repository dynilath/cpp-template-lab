#include <iostream>
#include <utility>
#include <tuple>

inline constexpr auto npos = size_t(-1);

template<size_t Result, typename Remain>
struct within_result {
    static constexpr size_t result = Result;
    using remains = Remain;
};

template<typename T, typename Tuple, size_t...Is, size_t J, size_t ...Js>
auto within_fn_impl(Tuple, std::index_sequence<Is...>, std::index_sequence<J, Js...>) {
    using testT = std::tuple_element_t<J, Tuple>;

    if constexpr (std::same_as<testT, T>) return within_result<J, std::index_sequence<Is..., Js...>>{};
    else if constexpr (sizeof...(Js) == 0)  return within_result<npos, std::index_sequence<Is..., J, Js...>>{};
    else return decltype(within_fn_impl<T>(std::declval<Tuple>(), std::index_sequence<Is..., J>{}, std::index_sequence<Js...>{})){};
}

template<size_t ...Is>
consteval auto pack_size(std::index_sequence<Is...>) {
    return sizeof...(Is);
}

template<size_t R, size_t ...Is, size_t ...Rs>
consteval auto extends_to(std::index_sequence<Is...> v1, std::index_sequence<Rs...> v2){
    if constexpr(sizeof...(Is) < sizeof...(Rs)) return decltype(extends_to<R>(std::index_sequence<Is...,R>{},v2)){};
    else return decltype(v1){};
}

template<typename T, typename Tuple, typename Indexes>
using within = decltype(within_fn_impl<T>(std::declval<Tuple>(), std::index_sequence<>{}, Indexes{}));

template<typename Tuple1, size_t ...Vs, typename Tuple2, size_t...Is, size_t J, size_t ...Js>
auto list_within_fn_impl(Tuple1, std::index_sequence<Vs...>, Tuple2, std::index_sequence<Is...>, std::index_sequence<J, Js...>){
    using within_result = within<std::tuple_element_t<J, Tuple2>, Tuple1, std::index_sequence<Vs...>>;
    using remains = typename within_result::remains;

    if constexpr (sizeof...(Js) == 0 ) return std::index_sequence<Is..., within_result::result>{};
    else if constexpr(pack_size(remains{}) == 0) return extends_to<npos>(
        std::index_sequence<Is..., within_result::result>{},
        std::make_index_sequence<std::tuple_size_v<Tuple2>>{});
    else return decltype(list_within_fn_impl(
        std::declval<Tuple1>(), remains{}, std::declval<Tuple2>(),
        std::index_sequence<Is..., within_result::result>{},
        std::index_sequence<Js...>{})){};
}

template<typename Tuple1, typename Tuple2>
using list_within = decltype(list_within_fn_impl(std::declval<Tuple1>(),
    std::make_index_sequence<std::tuple_size_v<Tuple1>>{},
    std::declval<Tuple2>(), std::index_sequence<>{},
    std::make_index_sequence<std::tuple_size_v<Tuple2>>{}));

template<size_t ...Is>
void print_index(std::index_sequence<Is...>) {
    ((std::cout << Is << " "), ...);
}

struct incomplete;

using dict = std::tuple<int, int, incomplete&, char>;
using tuple_y = std::tuple<float,char, int, int, float, incomplete&, char>;
using fr2 = list_within<dict, tuple_y>;

int main() {
    print_index(fr2{});
}

