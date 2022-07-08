/// some helpful constant string stuffs

/// - StringConstant<char...> : a conbination of std::integral_constant and std::intrgral_sequence
/// the static variable value is a constexpr const char[] correspond to string literal of cs
/// it also has conversion op and function call op that return the string

/// ACHIBULUP__NAMEOF(x) : return a StringConstant correspond to "x"
/// ACHIBULUP__NAMEOF_T(x) : type of the object returned

/// (normally you would like to create a macro for it like #define NAMEOF(x) ACHIBULUP__NAMEOF(x))


#ifndef STRING_LITERAL_H_INCLUDED
#define STRING_LITERAL_H_INCLUDED
#include "common_utils.hpp"
namespace Achibulup
{

template<char ...cs>
constexpr const char string_literal[] = {cs..., '\0'};

template<char ...cs>
struct StringConstant
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
    constexpr operator string_view () const noexcept
    {
        return value;
    }
    constexpr string_view operator () () const noexcept
    {
        return *this;
    }
    constexpr char operator [] (size_t index) const noexcept
    {
        return value[index];
    }
    template<typename ostr>
    friend auto operator << (ostr &os, StringConstant)
    -> decltype(os << value)
    {
        return os << value;
    }
};

template<char c>
using CharConstant = std::integral_constant<char, c>;


namespace n_StrLit
{
template<typename, typename>
struct cstr_concat;
template<char ...cs1, char ...cs2>
struct cstr_concat<StringConstant<cs1...>, StringConstant<cs2...>>
{
    using type = StringConstant<cs1..., cs2...>;
};
template<char ...cs, char c>
struct cstr_concat<StringConstant<cs...>, CharConstant<c>>
{
    using type = StringConstant<cs..., c>;
};
template<char c, char ...cs>
struct cstr_concat<CharConstant<c>, StringConstant<cs...>>
{
    using type = StringConstant<c, cs...>;
};
template<size_t len, char first, char ...others>
struct CStrPrefix
{
    using type = typename cstr_concat<StringConstant<first>,
                            typename CStrPrefix<len - 1, others...>::type
                          >::type;
};
template<char first, char ...others>
struct CStrPrefix<0, first, others...>
{
    using type = StringConstant<>;
};
template<size_t len, char ...cs>
using CStrPrefix_t = typename CStrPrefix<len, cs...>::type;
template<size_t N>
constexpr char nget(const char (&str)[N], size_t idx)
{
    return idx < N ? str[idx] : 0;
}


template<typename Integer, Integer I, 
         int SIGN = (std::is_same<Integer, char>::value || !I) ? 0 : ((I > 0) ? 1 : -1)>
struct ToStringHelper //<Integer, I, 1>
{
    using type = typename cstr_concat<typename ToStringHelper<Integer, I / 10>::type,
                                      CharConstant<'0' + (I % 10)>>::type;
};
template<typename Integer, Integer I>
struct ToStringHelper<Integer, I, -1>
{
    using type = typename cstr_concat<CharConstant<'-'>,
                                      typename ToStringHelper<Integer, -I>::type>::type;
};
template<typename Integer, Integer I>
struct ToStringHelper<Integer, I, 0>
{
    using type = StringConstant<>;
};
template<char c>
struct ToStringHelper<char, c, 0>
{
    using type = StringConstant<c>;
};



template<typename> struct ToStringFix;
template<char ...cs>
struct ToStringFix<StringConstant<cs...>>
{
    using type = StringConstant<cs...>;
};
template<>
struct ToStringFix<StringConstant<>>
{
    using type = StringConstant<'0'>;
};


template<typename Integer, Integer I>
struct ToString
{
    using type = typename ToStringFix<
                    typename ToStringHelper<Integer, I>::type>::type;
};
}




template<typename Integer, Integer I>
using ToString_t = typename n_StrLit::ToString<Integer, I>::type;


template<char ...cs1, char ...cs2>
constexpr StringConstant<cs1..., cs2...> 
operator + (StringConstant<cs1...>, StringConstant<cs2...>) noexcept
{
    return {};
}
template<char ...cs, char c>
constexpr StringConstant<cs..., c> 
operator + (StringConstant<cs...>, CharConstant<c>) noexcept
{
    return {};
}
template<char c, char ...cs>
constexpr StringConstant<c, cs...> 
operator + (CharConstant<c>, StringConstant<cs...>) noexcept
{
    return {};
}


template<typename Integer, Integer I>
ToString_t<Integer, I> toString(std::integral_constant<Integer, I>)
{
    return {};
}




#define ACHIBULUP__nget8(x,i) \
::Achibulup::n_StrLit::nget(x,i),::Achibulup::n_StrLit::nget(x,i+1),\
::Achibulup::n_StrLit::nget(x,i+2),::Achibulup::n_StrLit::nget(x,i+3),\
::Achibulup::n_StrLit::nget(x,i+4),::Achibulup::n_StrLit::nget(x,i+5),\
::Achibulup::n_StrLit::nget(x,i+6),::Achibulup::n_StrLit::nget(x,i+7)

#define ACHIBULUP__NAMEOF_str(x) \
::Achibulup::n_StrLit::CStrPrefix_t<sizeof(x)-1,\
ACHIBULUP__nget8(x,0),ACHIBULUP__nget8(x,8),ACHIBULUP__nget8(x,16),\
ACHIBULUP__nget8(x,24),ACHIBULUP__nget8(x,32),ACHIBULUP__nget8(x,40)>{}

#define ACHIBULUP__NAMEOF(...) ACHIBULUP__NAMEOF_str(#__VA_ARGS__)
#define ACHIBULUP__NAMEOF_T(...) decltype(ACHIBULUP__NAMEOF(__VA_ARGS__))
#define ACHIBULUP__TOSTRING(...) Achibulup::ToString_t<decltype(__VA_ARGS__), (__VA_ARGS__)>()
#define ACHIBULUP__TOSTRING_T(...) Achibulup::ToString_t<decltype(__VA_ARGS__), (__VA_ARGS__)>
}

#endif //STRING_LITERAL_H_INCLUDED