/// some helpful constant string stuffs

/// - string_constant<char...> : a conbination of std::integral_constant and std::intrgral_sequence
/// the static variable value is a constexpr const char[] correspond to string literal of cs
/// it also has conversion op and function call op that return the string

/// ACHIBULUP__NAMEOF(x) : return a string_constant correspond to "x"
/// ACHIBULUP__NAMEOF_T(x) : type of the object returned

/// (normally you would like to create a macro for it like #define NAMEOF(x) ACHIBULUP__NAMEOF(x))


#ifndef STRING_LITERAL_H_INCLUDED
#define STRING_LITERAL_H_INCLUDED
#include "common_utils.h"
namespace Achibulup
{

template<char ...cs>
constexpr const char string_literal[] = {cs..., '\0'};

template<char ...cs>
struct string_constant
{
    static constexpr auto &value = string_literal<cs...>;
    static constexpr size_t size()
    {
        return sizeof...(cs);
    }
    static constexpr size_t length()
    {
        return size();
    }
    constexpr operator decltype(value)& () const noexcept
    {
        return value;
    }
    template<typename Tp,
             typename = typename std::enable_if<
                 std::is_convertible<decltype(value)&, Tp>::value
             >::type
            >
    constexpr operator Tp () const
    {
        return value;
    }
    constexpr char operator [] (size_t index) const noexcept
    {
        return value[index];
    }
    template<typename ostr>
    friend auto operator << (ostr &os, string_constant)
    -> decltype(os << value)
    {
        return os << value;
    }
};


namespace n_NAMEOF_helper
{
template<typename, typename>
struct cstr_concat;
template<char ...cs1, char ...cs2>
struct cstr_concat<string_constant<cs1...>, string_constant<cs2...>>
{
    using type = string_constant<cs1..., cs2...>;
};
template<size_t len, char first, char ...others>
struct cstr_prefix
{
    using type = typename cstr_concat<string_constant<first>,
                            typename cstr_prefix<len - 1, others...>::type
                          >::type;
};
template<char first, char ...others>
struct cstr_prefix<0, first, others...>
{
    using type = string_constant<>;
};
template<size_t len, char ...cs>
using cstr_prefix_t = typename cstr_prefix<len, cs...>::type;
template<size_t N>
constexpr char nget(const char (&str)[N], size_t idx)
{
    return idx < N ? str[idx] : 0;
}

}


#define ACHIBULUP__nget8(x,i) \
::Achibulup::n_NAMEOF_helper::nget(x,i),::Achibulup::n_NAMEOF_helper::nget(x,i+1),\
::Achibulup::n_NAMEOF_helper::nget(x,i+2),::Achibulup::n_NAMEOF_helper::nget(x,i+3),\
::Achibulup::n_NAMEOF_helper::nget(x,i+4),::Achibulup::n_NAMEOF_helper::nget(x,i+5),\
::Achibulup::n_NAMEOF_helper::nget(x,i+6),::Achibulup::n_NAMEOF_helper::nget(x,i+7)

#define ACHIBULUP__NAMEOF_str(x) \
::Achibulup::n_NAMEOF_helper::cstr_prefix_t<sizeof(x)-1,\
ACHIBULUP__nget8(x,0),ACHIBULUP__nget8(x,8),ACHIBULUP__nget8(x,16),\
ACHIBULUP__nget8(x,24),ACHIBULUP__nget8(x,32),ACHIBULUP__nget8(x,40)>{}

#define ACHIBULUP__NAMEOF(...) ACHIBULUP__NAMEOF_str(#__VA_ARGS__)
#define ACHIBULUP__NAMEOF_T(...) decltype(ACHIBULUP__NAMEOF(__VA_ARGS__))
}

#endif //STRING_LITERAL_H_INCLUDED