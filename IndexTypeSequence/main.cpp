#include <iostream>
#include <utility>
#include <tuple>

inline constexpr auto npos = size_t(-1);

template<typename T>
struct result_type {
    using result = T;
};

template<bool Matched, size_t Result, typename Remain>
struct within_result {
    static constexpr bool matched = Matched;
    static constexpr size_t result = Result;
    using remains = Remain;
};

template<typename, typename, typename, typename>
struct within_impl;

template<typename T, typename Tuple, size_t...Is>
struct within_impl<T, Tuple, std::index_sequence<Is...>, std::index_sequence<>> : result_type<within_result<false, npos, std::index_sequence<Is...>>> {};

template<typename T, typename Tuple, size_t...Is, size_t J, size_t ...Js>
struct within_impl<T, Tuple, std::index_sequence<Is...>,std::index_sequence<J, Js...>> : std::conditional_t<std::is_same_v<T,std::tuple_element_t<J,Tuple>>,
    result_type<within_result<true, J, std::index_sequence<Is..., Js...>>>,
    within_impl<T, Tuple, std::index_sequence<Is..., J>, std::index_sequence<Js...>>>{};

template<typename T, typename Tuple, typename Indexes>
using within = within_impl<T, Tuple, std::index_sequence<>, Indexes>::result;

template<typename, typename, typename, typename, typename>
struct list_within_impl;

template<typename Tuple1, size_t ...Vs, typename Tuple2, size_t...Is>
struct list_within_impl<Tuple1, std::index_sequence<Vs...>, Tuple2, std::index_sequence<Is...>, std::index_sequence<>> : result_type<std::index_sequence<Is...>>{};

template<typename Tuple1, size_t ...Vs, typename Tuple2, size_t...Is, size_t J, size_t ...Js>
struct list_within_impl<Tuple1, std::index_sequence<Vs...>, Tuple2, std::index_sequence<Is...>, std::index_sequence<J, Js...>> :
    std::conditional_t<within<std::tuple_element_t<J,Tuple2>, Tuple1, std::index_sequence<Vs...>>::matched,
    list_within_impl<Tuple1, typename within<std::tuple_element_t<J, Tuple2>, Tuple1, std::index_sequence<Vs...>>::remains, Tuple2,
        std::index_sequence<Is..., within<std::tuple_element_t<J, Tuple2>, Tuple1, std::index_sequence<Vs...>>::result>, std::index_sequence<Js...>>,
    list_within_impl<Tuple1, std::index_sequence<Vs...>, Tuple2, std::index_sequence<Is..., npos>, std::index_sequence<Js...>>> {};

template<typename Tuple1, typename Tuple2>
using list_within = list_within_impl<Tuple1, std::make_index_sequence<std::tuple_size_v<Tuple1>>, Tuple2, std::index_sequence<>, std::make_index_sequence<std::tuple_size_v<Tuple2>>>::result;

template<size_t ...Is>
void print_index(std::index_sequence<Is...>) {
    ((std::cout << Is << " "), ...);
}

struct incomplete;

int main() {
    using dict = std::tuple<int, int, incomplete&, char>;
    using tuple_y = std::tuple<float,char, int, int, float, incomplete&, char>;

    using fr = list_within<dict, tuple_y>;

    print_index(fr{});
}

