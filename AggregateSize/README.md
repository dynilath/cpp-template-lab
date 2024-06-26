# Aggregate Size

How many parts an aggergate would use in aggergate initialization?

```cpp
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
```

output:
```
ex0 : 0
ex1 : 2
ex2 : 1
ex3 : 6
```