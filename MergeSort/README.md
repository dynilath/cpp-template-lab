# Merge Sort

Merge sort, but in template args.

[Online Compiler Explorer(godbolt)](https://gcc.godbolt.org/z/hnG4ojxY3)

```cpp
template <int... Args>
void print(int_list<Args...>) {
    ((std::cout << Args << ' '), ...);
    std::cout << std::endl;
}

using sorted = merge_sort<3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5>;

int main() { print(sorted{}); }
```

output:
```
1 1 2 3 3 4 5 5 5 6 9 
```

