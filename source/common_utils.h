#ifndef COMMON_UTILS_H_INCLUDED
#define COMMON_UTILS_H_INCLUDED

#define ACHIBULUP__Cpp11__cplusplus 201103L
#define ACHIBULUP__Cpp14__cplusplus 201402L
#define ACHIBULUP__Cpp17__cplusplus 201703L

#define ACHIBULUP__Cpp11 (__cplusplus == ACHIBULUP__Cpp11__cplusplus)
#define ACHIBULUP__Cpp14 (__cplusplus == ACHIBULUP__Cpp14__cplusplus)
#define ACHIBULUP__Cpp17 (__cplusplus == ACHIBULUP__Cpp17__cplusplus)

#define ACHIBULUP__Cpp11_later (__cplusplus >= ACHIBULUP__Cpp11__cplusplus)
#define ACHIBULUP__Cpp14_later (__cplusplus >= ACHIBULUP__Cpp14__cplusplus)
#define ACHIBULUP__Cpp17_later (__cplusplus >= ACHIBULUP__Cpp17__cplusplus)

#include <new>
#include <string>
#include <cstdint>
#include <utility>
#include <iterator>
#include <type_traits>
#if ACHIBULUP__Cpp17_later
#include <charconv>
#include <string_view>
#endif //ACHIBULUP__Cpp17_later

namespace Achibulup{

#if ACHIBULUP__Cpp14_later
#define ACHIBULUP__constexpr_fun14 constexpr
#else
#define ACHIBULUP__constexpr_fun14
#endif //ACHIBULUP__Cpp14_later


using size_t = std::ptrdiff_t;

#if ACHIBULUP__Cpp17_later
using str_arg = std::string_view;
using wstr_arg = std::wstring_view;
using std::byte;
#else
using str_arg = const std::string&;
using wstr_arg = const std::wstring&;
using byte = unsigned char;
#endif

template<typename Tp>
inline constexpr size_t ssizeof() noexcept
{
    return sizeof(Tp);
}
template<typename Tp>
inline constexpr size_t ssizeof(Tp&&) noexcept
{
    return sizeof(Tp);
}

template<typename Tp>
struct Typeof_helper
{
    using type = Tp;
};

template<typename Tp>
using Typeof = typename Typeof_helper<Tp>::type;

template<bool constraint>
using EnableIf_t = typename std::enable_if<constraint>::type;


template<typename from, typename to>
struct copy_cvref
{
    using type = to;
};
template<typename from, typename to>
struct copy_cvref<from&, to>
{
    using type = typename copy_cvref<from, to>::type&;
};
template<typename from, typename to>
struct copy_cvref<from&&, to>
{
    using type = typename copy_cvref<from, to>::type&&;
};
template<typename from, typename to>
struct copy_cvref<const from, to>
{
    using type = const to;
};
template<typename from, typename to>
struct copy_cvref<volatile from, to>
{
    using type = volatile to;
};
template<typename from, typename to>
struct copy_cvref<const volatile from, to>
{
    using type = const volatile to;
};
template<typename from, typename to>
using copy_cvref_t = typename copy_cvref<from, to>::type;


/**
 * Extract the value from the variable and reset it to default if it is trivial;
  Otherwise, create a Move reference to it.
 * Usually used in Move constructors to reset the fields of the Moved variable
 * Note : this only accept lvalues as argument


Example:
MyClass(MyClass &&other) 
: myint_(Move(other.myint_)), mystring_(Move(other.mystring_)) {}
*/

template<typename Tp, typename = EnableIf_t<std::is_trivial<Tp>::value>>
inline ACHIBULUP__constexpr_fun14 Tp Move(Tp &var) noexcept
{
    Tp res = var;
    var = Tp();
    return res;
}
template<typename Tp, typename = EnableIf_t<!std::is_trivial<Tp>::value>>
inline constexpr Tp&& Move(Tp &var) noexcept 
{
    return std::move(var);
}

template<typename Tp, typename = EnableIf_t<std::is_trivial<Tp>::value>>
inline ACHIBULUP__constexpr_fun14 void Move_assign(Tp &var, Tp &val) 
        noexcept(noexcept(var = std::move(val)))
{
    var = val;
    val = {};
}
template<typename Tp, typename = EnableIf_t<!std::is_trivial<Tp>::value>, typename = void>
inline ACHIBULUP__constexpr_fun14 void Move_assign(Tp &var, Tp &val) 
        noexcept(noexcept(var = std::move(val)))
{
    var = std::move(val);
}



template<typename Tp>
Tp convert(str_arg);
#if ACHIBULUP__Cpp17_later
template<> int convert<int>(std::string_view str)
{
    int res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned int convert<unsigned int>(std::string_view str)
{
    unsigned int res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> short convert<short>(std::string_view str)
{
    short res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned short convert<unsigned short>(std::string_view str)
{
    unsigned short res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> long convert<long>(std::string_view str)
{
    long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned long convert<unsigned long>(std::string_view str)
{
    unsigned long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> long long convert<long long>(std::string_view str)
{
    long long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned long long convert<unsigned long long>(std::string_view str)
{
    unsigned long long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
#else
template<> int 
convert<int>(const std::string &str)
{
    return std::stoll(str);
}
template<> unsigned int 
convert<unsigned int>(const std::string &str)
{
    return std::stoull(str);
}
template<> short 
convert<short>(const std::string &str)
{
    return std::stoll(str);
}
template<> unsigned short 
convert<unsigned short>(const std::string &str)
{
    return std::stoull(str);
}
template<> long 
convert<long>(const std::string &str)
{
    return std::stoll(str);
}
template<> unsigned long 
convert<unsigned long>(const std::string &str)
{
    return std::stoull(str);
}
template<> long long 
convert<long long>(const std::string &str)
{
    return std::stoll(str);
}
template<> unsigned long long 
convert<unsigned long long>(const std::string &str)
{
    return std::stoull(str);
}
#endif



/** 
 * Simulating a C#-like read-only property
 * Type : the underlying value type
 * Container : the class containing this property
 * usage : use this as public class data member
 * Only getters(in the form of implicit conversion) are public
 * The containing class can access its private members such as constructors and setters
 
  Usage example : 

  Class Container
  {
    public:
      ReadOnlyProperty<int, Container> int_property;
  }
*/
template<typename Type, class Container
    , typename = EnableIf_t<std::is_fundamental<Type>::value>>
class ReadOnlyProperty
{
  private:
    constexpr ReadOnlyProperty() noexcept : value() {}
    constexpr ReadOnlyProperty(Type val) noexcept : value(val) {}

    ACHIBULUP__constexpr_fun14  
    ReadOnlyProperty(ReadOnlyProperty &&mov) noexcept : value(Move(mov.value)) {}
    constexpr ReadOnlyProperty(const ReadOnlyProperty&) noexcept = default;

    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator = (ReadOnlyProperty &&mov) & noexcept
    {
        Move_assign(this->value, mov.value);
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator = (const ReadOnlyProperty &) & noexcept = default;

  public:
    constexpr operator Type() const noexcept
    {
        return value;
    }

  private:
    ACHIBULUP__constexpr_fun14
    Type operator = (Type val) & noexcept
    {
        return this->value = val;
    }

    ACHIBULUP__constexpr_fun14
    static void swap(ReadOnlyProperty &a, ReadOnlyProperty &b) noexcept
    {
        std::swap(a.value, b.value);
    }

    Type value;

    friend Container;
};


template<typename Type, class Container
    , typename = EnableIf_t<std::is_fundamental<Type>::value>>
class AutoProperty
{
  private:
    constexpr AutoProperty() noexcept : value() {}
    constexpr AutoProperty(Type val) noexcept : value(val) {}

    ACHIBULUP__constexpr_fun14  
    AutoProperty(AutoProperty &&mov) noexcept : value(Move(mov.value)) {}
    constexpr AutoProperty(const AutoProperty&) noexcept = default;

    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator = (AutoProperty &&mov) & noexcept
    {
        Move_assign(this->value, mov.value);
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator = (const AutoProperty &) & noexcept = default;

  public:
    constexpr operator Type() const noexcept
    {
        return value;
    }

    ACHIBULUP__constexpr_fun14
    Type operator = (Type val) & noexcept
    {
        return this->value = val;
    }

    ACHIBULUP__constexpr_fun14
    friend void swap(AutoProperty &a, AutoProperty &b) noexcept
    {
        AutoProperty::swap(a, b);
    }

  private:
    Type value;
    
    ACHIBULUP__constexpr_fun14
    static void swap(AutoProperty &a, AutoProperty &b) noexcept
    {
        std::swap(a.value, b.value);
    }

    friend Container;
};

} //namespace Achibulup

#endif  //COMMON_UTILS_H_INCLUDED