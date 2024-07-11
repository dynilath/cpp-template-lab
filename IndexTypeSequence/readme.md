# indexing type sequence

[Online Compiler Explorer(godbolt)](https://gcc.godbolt.org/z/f84dhvYhd)

```C++
template<size_t ...Is>
void print_index(std::index_sequence<Is...>) {
    ((std::cout << Is << " "), ...);
}

struct incomplete;

using tuple_x = std::tuple<int, int, incomplete&, char>;
using tuple_y = std::tuple<float,char, int, int, float, incomplete&, char>;

using fr = list_within<tuple_x, tuple_y>;

int main() {
    print_index(fr{});
}
```

output:
```
18446744073709551615 3 0 1 18446744073709551615 2 18446744073709551615 
```