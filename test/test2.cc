#include <iostream>
#include <type_traits>

namespace detail
{

template <typename T>
struct is_string
{
    static constexpr bool value = false;
};

template <class T, class Traits, class Alloc>
struct is_string<std::basic_string<T, Traits, Alloc>>
{
    static constexpr bool value = true;
};

template <typename C>
struct is_char
    : std::integral_constant<bool, std::is_same<C, char>::value ||
                                       std::is_same<C, char16_t>::value ||
                                       std::is_same<C, char32_t>::value ||
                                       std::is_same<C, wchar_t>::value>
{
};

template <typename T>
struct is_number
    : std::integral_constant<bool, std::is_arithmetic<T>::value &&
                                       !std::is_same<T, bool>::value &&
                                       !detail::is_char<T>::value>
{
};

} // namespace detail

// todo : different :
// typename std::enable_if<detail::is_number<T>::value>::type
// typename std::enable_if<detail::is_number<T>::value, T>::type
// typename std::enable_if<detail::is_number<T>::value, bool>::type
//
// Q1 : How to specify std::nullptr_t
// Q2 : How to add std::string&& ? or const char*
struct Test
{
    template <typename T, typename std::enable_if<detail::is_number<T>::value,
                                                  bool>::type = true>
    void print(const T& t)
    {
        std::cout << t << std::endl;
    }
    template <typename T, typename std::enable_if<!detail::is_number<T>::value,
                                                  bool>::type = true>
    void print(const T& t);

    // template <typename T>
    // void print(T&& t);
};

template <>
void Test::print(const std::string& val)
{
    std::cout << val << std::endl;
}

// template <>
// void Test::print(const std::string&& val)
// {
//     std::cout << val << std::endl;
// }
//
template <>
void Test::print(const bool& t)
{
    if (t)
    {
        std::cout << "111" << std::endl;
    }
    else
    {
        std::cout << "222" << std::endl;
    }
}

// template <>
// void Test::print(const std::nullptr_t& t)
// {
//     if (!t)
//         std::cout << "333 " << std::endl;
// }

int main()
{
    Test t;
    t.print(1);
    t.print(false);
    const std::string a = "123";
    t.print(a);
    // t.print("123");
}
