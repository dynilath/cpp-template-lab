#include <iostream>
#include <utility>

template<typename T, size_t ...Ns>
consteval auto indexing_array_impl(std::index_sequence<Ns...>) {
    using value_type = std::remove_extent_t<decltype(T::value)>;
    return std::integer_sequence<value_type, T::value[Ns]...>{};
}

template<typename T> requires std::same_as<std::remove_cvref_t<decltype(T::value[0])>, int>
consteval auto indexing_array() {
    constexpr auto ext = std::extent_v<decltype(T::value)>;
    return indexing_array_impl<T>(std::make_index_sequence<ext>{});
}

template<typename T> requires std::same_as<std::remove_cvref_t<decltype(T::value)>,int>
consteval auto indexing_array() {
    return std::integer_sequence<decltype(T::value), T::value>{};
}

template<typename T> requires (!requires { T::value; })
consteval auto indexing_array() {
    return std::integer_sequence<const int>{};
}

template <typename T, T... Ls, T... Rs, typename... Us>
consteval auto join_index_sequence(std::integer_sequence<T, Ls...>, std::integer_sequence<T, Rs...>, Us... us) {
    if constexpr (sizeof...(Us) == 0) return std::integer_sequence<T, Ls..., Rs...>{};
    else return join_index_sequence(std::integer_sequence<T, Ls..., Rs...>{}, us...);
}

template<typename ...Ts>
using join_index_sequence_t = decltype(join_index_sequence(indexing_array<Ts>()...));

struct ex0 {};

struct ex1 {
    static constexpr int value[]{3, 63};
};

struct ex2 {
    static constexpr int value{5};
};

template <int... Ns>
void print_sequence(std::integer_sequence<const int, Ns...>) {
    ((std::cout << Ns << ' '), ...);
}

int main() {
    print_sequence(join_index_sequence_t<ex0, ex1, ex2>{});
}