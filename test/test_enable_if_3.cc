#include <type_traits>
#include <iostream>

template <typename T, typename std::enable_if<std::is_same<T, bool>::value, T>::type = true>
T get()
{
    T t;
    std::cout << "1111" << std::endl;
    return t;
}

template <typename T, typename std::enable_if<!std::is_same<T, bool>::value, T>::type = true>
T get()
{
    T t;
    std::cout << "2222" << std::endl;
    return t;
}

void test01()
{
    auto a = get<bool>();
    auto b = get<int>();
}

int main()
{
    test01();
}