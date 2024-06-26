#include <iostream>

template <size_t N>
struct any_ {
    template <typename T>
    operator T() const;
};

template <typename T, typename U>
struct impl_aggregate_contructible : std::false_type {};

template <typename T, size_t... Ns>
    requires(requires { T{any_<Ns>{}...}; })
struct impl_aggregate_contructible<T, std::index_sequence<Ns...>> : std::true_type {};

template <typename T, size_t N>
constexpr bool is_aggregate_contructible_v = impl_aggregate_contructible<T, std::make_index_sequence<N>>::value;

template <typename T, size_t H, size_t L>
consteval size_t aggregate_size_impl() noexcept {
    if constexpr (H == L ) return L;
    if constexpr (H == L + 1) {
        if constexpr (is_aggregate_contructible_v<T, H>) return H;
        else return L;
    }
    else if constexpr (is_aggregate_contructible_v<T, (H + L) / 2>) {
        return aggregate_size_impl<T, H, (H + L) / 2>();
    } else {
        return aggregate_size_impl<T, (H + L) / 2 - 1, L>();
    }
}

template <typename T>
consteval auto aggregate_size() noexcept {
    return aggregate_size_impl<T, 1024, 0>();
}


struct ex0 {};

struct ex1 {
    int value[2];
};

struct ex2 {
    int value;
};

struct ex3 {
    int a, b[3];
    double c;
    int d;
};

int main() {
    std::cout << "ex0 : " << aggregate_size<ex0>() << "\n";
    std::cout << "ex1 : " << aggregate_size<ex1>() << "\n";
    std::cout << "ex2 : " << aggregate_size<ex2>() << "\n";
    std::cout << "ex3 : " << aggregate_size<ex3>() << "\n";
}