# Gather Value

Gather integer values from a list of types.

```cpp
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
```

output:
```
3 63 5
```