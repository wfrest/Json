#include <iostream>
#include <type_traits>

struct Test
{
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value, T>::type get() const;

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type get() const
    {
        std::cout << "111" << std::endl;
        return static_cast<T>(10);
    }
};

template <>
bool Test::get<bool>() const
{
    std::cout << "222" << std::endl;
    return false;
}

template <>
std::string Test::get<std::string>() const
{
    std::cout << "3333" << std::endl;
    return "1111";
}

int main()
{
    Test t;
    std::cout << t.get<int>();
    std::cout << t.get<bool>();
}
