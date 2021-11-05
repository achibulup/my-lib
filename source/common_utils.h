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
#include <sstream>
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
using std::string_view;
using std::wstring_view;
#else
class string_view
{
    public:
    using size_type = size_t;
    using const_iterator = const char*;
    using iterator = const_iterator;


    constexpr string_view() noexcept : i_ptr(), i_len() {}
    constexpr string_view(const char *p, size_t ln) noexcept
    : i_ptr(p), i_len(ln) {}
    constexpr string_view(const char *b, const char *e) noexcept
    : i_ptr(b), i_len(e - b) {}
    template<size_t Nm>
    constexpr string_view(const char (&str)[Nm]) noexcept
    : i_ptr(str), i_len(Nm - 1) {}
    string_view(const std::string &str) 
    : i_ptr(str.c_str()), i_len(str.size()) {}

    constexpr char operator [] (size_type i) const noexcept
    {
        return this->i_ptr[i];
    }
    constexpr size_type size() const noexcept
    {
        return this->i_len;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return i_ptr;
    }
    constexpr iterator begin() const noexcept
    {
        return cbegin();
    }
    constexpr const_iterator cend() const noexcept
    {
        return i_ptr + i_len;
    }
    constexpr iterator end() const noexcept
    {
        return cend();
    }

    constexpr const char* data() const noexcept
    {
        return i_ptr;
    }

    operator std::string () const
    {
        return std::string(cbegin(), cend());
    }

    private:
    const_iterator i_ptr;
    size_type i_len;
};
template <typename ostr>
ostr& operator << (ostr &os, string_view s)
{
    for(char c : s)
      os << c;
    return os;
}
using wstr_arg = const std::wstring&;
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

template<typename From, typename To>
using cast_t = To;

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





/// string utilities

using std::to_string;
template<typename Tp, 
        typename = EnableIf_t<std::is_convertible<Tp, std::string>::value>>
std::string to_string(Tp &&x)
{
    return std::forward<Tp>(x);
}

void string_format_helper(std::ostringstream& result) {}
template<typename Tp, typename ...Args>
void string_format_helper(std::ostringstream& result, 
                          const Tp &first, const Args& ...args)
{
    result << first;
    string_format_helper(result, args...);
}
template<typename ...Args>
std::string string_format(const Args& ...args)
{
    std::ostringstream result;
    string_format_helper(result, args...);
    return result.str();
}



template<typename Tp>
Tp convert(string_view);
#if ACHIBULUP__Cpp17_later
template<> int convert<int>(string_view str)
{
    int res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned int convert<unsigned int>(string_view str)
{
    unsigned int res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> short convert<short>(string_view str)
{
    short res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned short convert<unsigned short>(string_view str)
{
    unsigned short res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> long convert<long>(string_view str)
{
    long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned long convert<unsigned long>(string_view str)
{
    unsigned long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> long long convert<long long>(string_view str)
{
    long long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
template<> unsigned long long convert<unsigned long long>(string_view str)
{
    unsigned long long res = 0;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}
#else
template<> int 
convert<int>(string_view str)
{
    return std::stoll(str);
}
template<> unsigned int 
convert<unsigned int>(string_view str)
{
    return std::stoull(str);
}
template<> short 
convert<short>(string_view str)
{
    return std::stoll(str);
}
template<> unsigned short 
convert<unsigned short>(string_view str)
{
    return std::stoull(str);
}
template<> long 
convert<long>(string_view str)
{
    return std::stoll(str);
}
template<> unsigned long 
convert<unsigned long>(string_view str)
{
    return std::stoull(str);
}
template<> long long 
convert<long long>(string_view str)
{
    return std::stoll(str);
}
template<> unsigned long long 
convert<unsigned long long>(string_view str)
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
    , typename = EnableIf_t<std::is_arithmetic<Type>::value 
                         || std::is_pointer<Type>::value>>
class ReadOnlyProperty
{
  private:
    constexpr ReadOnlyProperty() noexcept : value() {}
    constexpr ReadOnlyProperty(Type val) noexcept : value(val) {}

    constexpr ReadOnlyProperty(const ReadOnlyProperty&) noexcept = default;
    ACHIBULUP__constexpr_fun14  
    ReadOnlyProperty(ReadOnlyProperty &&mov) noexcept : value(Move(mov.value)) {}

    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator = (const ReadOnlyProperty &) & noexcept = default;
    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator = (ReadOnlyProperty &&mov) & noexcept
    {
        Move_assign(this->value, mov.value);
        return *this;
    }

    ACHIBULUP__constexpr_fun14
    ReadOnlyProperty& operator = (Type val) & noexcept
    {
        this->value = val;
        return *this;
    }

    ACHIBULUP__constexpr_fun14
    static void swap(ReadOnlyProperty &a, ReadOnlyProperty &b) noexcept
    {
        std::swap(a.value, b.value);
    }

    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator ++ () & noexcept
    {
        ++this->value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    Type operator ++ (int) & noexcept
    {
        return this->value++;
    }
    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator -- () & noexcept
    {
        --this->value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    Type operator -- (int) & noexcept
    {
        return this->value--;
    }
    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator += (Type v) & noexcept
    {
        this->value += v;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    ReadOnlyProperty& operator -= (Type v) & noexcept
    {
        this->value -= v;
        return *this;
    }

  public:
    constexpr operator Type() const noexcept
    {
        return (*this)();
    }

    constexpr Type operator () () const noexcept
    {
        return value;
    }

  private:
    Type value;

    friend Container;
};


template<typename Type, class Container
    , typename = EnableIf_t<std::is_arithmetic<Type>::value 
                         || std::is_pointer<Type>::value>>
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

  public:  
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator = (const AutoProperty &) & noexcept = default;

    constexpr operator Type() const noexcept
    {
        return (*this)();
    }

    constexpr Type operator () () const noexcept
    {
        return value;
    }

    ACHIBULUP__constexpr_fun14
    AutoProperty& operator = (Type val) & noexcept
    {
        this->value = val;
        return *this;
    }

    ACHIBULUP__constexpr_fun14
    friend void swap(AutoProperty &a, AutoProperty &b) noexcept
    {
        AutoProperty::swap(a, b);
    }

    
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator ++ () & noexcept
    {
        ++this->value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    Type operator ++ (int) & noexcept
    {
        return this->value++;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator -- () & noexcept
    {
        --this->value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    Type operator -- (int) & noexcept
    {
        return this->value--;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator += (Type v) & noexcept
    {
        this->value += v;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator -= (Type v) & noexcept
    {
        this->value -= v;
        return *this;
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