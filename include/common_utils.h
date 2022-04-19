#ifndef COMMON_UTILS_H_INCLUDED
#define COMMON_UTILS_H_INCLUDED

#define ACHIBULUP__Cpp11__cplusplus 201103L
#define ACHIBULUP__Cpp14__cplusplus 201402L
#define ACHIBULUP__Cpp17__cplusplus 201703L

#define ACHIBULUP__Cpp11 (__cplusplus == ACHIBULUP__Cpp11__cplusplus)
#define ACHIBULUP__Cpp14 (__cplusplus == ACHIBULUP__Cpp14__cplusplus)
#define ACHIBULUP__Cpp17 (__cplusplus == ACHIBULUP__Cpp17__cplusplus)

#define ACHIBULUP__before_Cpp11 (__cplusplus < ACHIBULUP__Cpp11__cplusplus)
#define ACHIBULUP__before_Cpp14 (__cplusplus < ACHIBULUP__Cpp14__cplusplus)
#define ACHIBULUP__before_Cpp17 (__cplusplus < ACHIBULUP__Cpp17__cplusplus)

#define ACHIBULUP__Cpp11_prior (__cplusplus <= ACHIBULUP__Cpp11__cplusplus)
#define ACHIBULUP__Cpp14_prior (__cplusplus <= ACHIBULUP__Cpp14__cplusplus)
#define ACHIBULUP__Cpp17_prior (__cplusplus <= ACHIBULUP__Cpp17__cplusplus)

#define ACHIBULUP__Cpp11_later (__cplusplus >= ACHIBULUP__Cpp11__cplusplus)
#define ACHIBULUP__Cpp14_later (__cplusplus >= ACHIBULUP__Cpp14__cplusplus)
#define ACHIBULUP__Cpp17_later (__cplusplus >= ACHIBULUP__Cpp17__cplusplus)


#include <new>
#include <string>
#include <cstring>
#include <cstdint>
#include <utility>
#include <sstream>
#include <iterator>
#include <iostream>
#include <type_traits>
#if ACHIBULUP__Cpp17_later
#if __has_include(<charconv>)
#include <charconv>
#endif // __has_include(<charconv>)
#include <string_view>
#endif //ACHIBULUP__Cpp17_later

namespace Achibulup{

#if ACHIBULUP__Cpp14_later
#define ACHIBULUP__constexpr_fun14 constexpr
#else
#define ACHIBULUP__constexpr_fun14
#endif //ACHIBULUP__Cpp14_later


using size_t = std::ptrdiff_t;



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

template<bool constraint, typename Tp = int>
using EnableIf_t = typename std::enable_if<constraint, Tp>::type;

template<typename Tp>
using ReferenceFilter = EnableIf_t<std::is_reference<Tp>::value, Tp>;

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
inline ACHIBULUP__constexpr_fun14 Tp Move(Tp &src) noexcept
{
    Tp res = src;
    src = Tp();
    return res;
}
template<typename Tp, typename = EnableIf_t<!std::is_trivial<Tp>::value>>
inline constexpr Tp&& Move(Tp &src) noexcept 
{
    return std::move(src);
}

template<typename Tp, typename = EnableIf_t<std::is_trivial<Tp>::value>>
inline ACHIBULUP__constexpr_fun14 void MoveAssign(Tp &dest, Tp &src) 
        noexcept(noexcept(dest = std::move(src)))
{
    dest = src;
    src = {};
}
template<typename Tp, typename = EnableIf_t<!std::is_trivial<Tp>::value>, typename = void>
inline ACHIBULUP__constexpr_fun14 void MoveAssign(Tp &dest, Tp &src) 
        noexcept(noexcept(dest = std::move(src)))
{
    dest = std::move(src);
}





/// string utilities
// template<typename ostr>
// ostr&& insert(ostr &&os, const std::string &s)
// {
//     os << s;
//     return std::forward<ostr>(os);
// }
// template<typename ostr>
// ostr&& insert(ostr &&os, const char *s)
// {
//     os << s;
//     return std::forward<ostr>(os);
// }
// template<typename ostr>
// ostr&& insert(ostr &&os, char c)
// {
//     os << c;
//     return std::forward<ostr>(os);
// }


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
    constexpr string_view(const char *str) noexcept
    : i_ptr(str), i_len(strlen(str)) {}
    explicit constexpr string_view(const char &c) noexcept
    : i_ptr(&c), i_len(c != 0) {}
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

    explicit operator std::string () const
    {
        return std::string(cbegin(), cend());
    }

    friend std::ostream& operator << (std::ostream &os, string_view s)
    {
        for(char c : s)
          os << c;
        return os;
    }

    private:
    const_iterator i_ptr;
    size_type i_len;
};
using wstring_view = const std::wstring&;
#endif



using std::to_string;
template<typename Tp, 
        typename = EnableIf_t<std::is_convertible<Tp, std::string>::value>>
std::string to_string(Tp &&x)
{
    return std::forward<Tp>(x);
}
void stringFormatHelper(std::ostringstream&) {}
template<typename Tp, typename ...Args>
void stringFormatHelper(std::ostringstream& result, 
                          const Tp &first, const Args& ...args)
{
    result << first;
    stringFormatHelper(result, args...);
}
template<typename ...Args>
std::string stringFormat(const Args& ...args)
{
    std::ostringstream result;
    stringFormatHelper(result, args...);
    return result.str();
}



template<typename Tp>
Tp convert(string_view);
// #if ACHIBULUP__Cpp17_later
// template<> int convert<int>(string_view str)
// {
//     int res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// template<> unsigned int convert<unsigned int>(string_view str)
// {
//     unsigned int res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// template<> short convert<short>(string_view str)
// {
//     short res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// template<> unsigned short convert<unsigned short>(string_view str)
// {
//     unsigned short res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// template<> long convert<long>(string_view str)
// {
//     long res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// template<> unsigned long convert<unsigned long>(string_view str)
// {
//     unsigned long res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// template<> long long convert<long long>(string_view str)
// {
//     long long res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// template<> unsigned long long convert<unsigned long long>(string_view str)
// {
//     unsigned long long res = 0;
//     std::from_chars(str.data(), str.data() + str.size(), res);
//     return res;
// }
// #else
// template<> int 
// convert<int>(string_view str)
// {
//     return std::stoll(str);
// }
// template<> unsigned int 
// convert<unsigned int>(string_view str)
// {
//     return std::stoull(str);
// }
// template<> short 
// convert<short>(string_view str)
// {
//     return std::stoll(str);
// }
// template<> unsigned short 
// convert<unsigned short>(string_view str)
// {
//     return std::stoull(str);
// }
// template<> long 
// convert<long>(string_view str)
// {
//     return std::stoll(str);
// }
// template<> unsigned long 
// convert<unsigned long>(string_view str)
// {
//     return std::stoull(str);
// }
// template<> long long 
// convert<long long>(string_view str)
// {
//     return std::stoll(str);
// }
// template<> unsigned long long 
// convert<unsigned long long>(string_view str)
// {
//     return std::stoull(str);
// }
// #endif


namespace n_Utils
{
    template<typename Tp>
    struct is_basic 
    : std::integral_constant<bool, std::is_scalar<Tp>::value> {};
    template<> struct is_basic<std::string> : std::true_type {}; 

    template<typename Tp>
    struct is_basic<const Tp> : is_basic<Tp> {};
}


/** 
 * Simulating a C#-like property
 * Type : the underlying value type
 * Class : the class containing this property
 * usage : use this as public class data member
 * Getters and Setters are public
 * The containing class can access its private members such as constructors and setters
 
  Usage example : 

  Class Class
  {
    public:
      AutoProperty<int, Class> int_property;
  }
*/
template<typename Type, class Class, 
    typename = EnableIf_t<n_Utils::is_basic<Type>::value>>
class AutoProperty
{
  private:
    constexpr AutoProperty() noexcept : value() {}
    constexpr AutoProperty(Type value) noexcept : value(value) {}

    constexpr AutoProperty(const AutoProperty &) noexcept = default;

    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator = (const AutoProperty &) & noexcept = default;

  public:  

    ACHIBULUP__constexpr_fun14
    friend void swap(AutoProperty &a, AutoProperty &b) noexcept
    {
        a.swap(b);
    }

    friend ACHIBULUP__constexpr_fun14 Type Move(AutoProperty &src) noexcept
    {
        Type res = src();
        src = {};
        return res;
    }
    friend ACHIBULUP__constexpr_fun14 Type Move(AutoProperty &&src) noexcept
    {
        return Move(src);
    }
    
    constexpr Type operator () () const noexcept
    {
        return this->value;
    }

    constexpr const Type* operator -> () const noexcept
    {
        return &this->value;
    }

    ACHIBULUP__constexpr_fun14
    AutoProperty& operator = (Type value) & noexcept
    {
        this->value = value;
        return *this;
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
    AutoProperty& operator += (Type value) & noexcept
    {
        this->value += value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator -= (Type value) & noexcept
    {
        this->value -= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator *= (Type value) & noexcept
    {
        this->value *= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator /= (Type value) & noexcept
    {
        this->value /= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator %= (Type value) & noexcept
    {
        this->value %= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator &= (Type value) & noexcept
    {
        this->value &= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator |= (Type value) & noexcept
    {
        this->value |= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator ^= (Type value) & noexcept
    {
        this->value ^= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator >>= (Type value) & noexcept
    {
        this->value >>= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator <<= (Type value) & noexcept
    {
        this->value <<= value;
        return *this;
    }

  private:
    Type value;
    
    ACHIBULUP__constexpr_fun14
    void swap(AutoProperty &b) noexcept
    {
        using std::swap;
        swap(this->value, b.value);
    }

    friend Class;
};


/** 
 * A specialization of AutoProperty for std::string
*/
template<class Class>
class AutoProperty<std::string, Class>
{
  private:
    AutoProperty() noexcept : value() {}
    AutoProperty(std::string str) noexcept : value(std::move(str)) {}

    AutoProperty(const AutoProperty&) = default;
    AutoProperty(AutoProperty&&) noexcept = default;

    AutoProperty& operator = (const AutoProperty &) & = default;
    AutoProperty& operator = (AutoProperty &&) & noexcept = default;

  public:

    friend void swap(AutoProperty &a, AutoProperty &b) noexcept
    {
        a.swap(b);
    }

    std::string operator () () const &
    {
        return this->value;
    }
    std::string operator () () && noexcept
    {
        return std::move(this->value);
    }

    char operator [] (size_t index) const
    {
        return this->value[index];
    }

    const std::string* operator -> () const noexcept
    {
        return &(this->value);
    }

    friend std::string to_string(const AutoProperty &p)
    {
        return p();
    }
    friend std::string to_string(AutoProperty &&p) noexcept
    {
        return std::move(p)();
    }

    operator string_view () const noexcept
    {
        return {this->value.data(), this->value.size()};
    } 

    friend std::string operator + (const AutoProperty &lhs, 
                                   const AutoProperty &rhs)
    {
        return lhs.value + rhs.value;
    }

    template<typename Str>
    friend std::string operator + (const AutoProperty &lhs, const Str &rhs)
    {
        return lhs.value + std::forward<Str>(rhs);
    }
    template<typename Str>
    friend std::string operator + (const Str &lhs, const AutoProperty &rhs)
    {
        return std::forward<Str>(lhs) + rhs.value;
    }

    friend std::ostream& operator << (std::ostream &os, const AutoProperty &x)
    {
        return os << x->data();
    }

    template<typename Str>
    auto operator = (Str &&rhs) & 
    -> decltype(std::declval<std::string&>() = std::declval<Str>(), *this)
    {
        this->value = std::forward<Str>(rhs);
        return *this;
    }
    AutoProperty& operator = (std::string &&str) & noexcept
    {
        this->value = std::move(str);
        return *this;
    }

    template<typename Str>
    AutoProperty& operator += (Str &&rhs) 
    {
        this->value += std::forward<Str>(rhs);
        return *this;
    }
    
    template<typename Functor>
    void visit(Functor &&functor)
    {
        functor(this->value);
    }
    template<typename Functor>
    void visit(Functor &&functor) const
    {
        functor(this->value);
    }

  private:
    std::string& operator * () noexcept
    {
        return this->value;
    }
    const std::string& operator * () const noexcept
    {
        return this->value;
    }

    void swap(AutoProperty &b) noexcept
    {
        using std::swap;
        swap(this->value, b.value);
    }

    std::string value;

    friend Class;
};

/** 
 * A specialization of AutoProperty for constant types
 * Simulating a C#-like read-only property
 * Type : the underlying value type
 * Class : the class containing this property
 * usage : use this as public class data member
 * Only getters(in the form of implicit conversion) are public
 * The containing class can access its private members such as constructors and setters
 
  Usage example : 

  Class Class
  {
    public:
      AutoProperty<const int, Class> int_property;
  }
*/
template<typename Type, class Class>
class AutoProperty<const Type, Class>
{
  private:
    constexpr AutoProperty() noexcept : value() {}
    constexpr AutoProperty(Type value) noexcept : value(value) {}

    constexpr AutoProperty(const AutoProperty&) noexcept = default;

    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator = (const AutoProperty &) & noexcept = default;

    ACHIBULUP__constexpr_fun14
    void swap(AutoProperty &b) noexcept
    {
        std::swap(this->value, b.value);
    }

    class MoveReference
    {
        constexpr MoveReference(Type &ref) noexcept : ref(ref) {}
        ACHIBULUP__constexpr_fun14 Type get() const noexcept
        {
            Type res = ref;
            ref = {};
            return res;
        }
        Type &ref;
        friend AutoProperty;
    };

    constexpr MoveReference Move(AutoProperty &src) noexcept
    {
        return MoveReference{src.value};
    }
    constexpr MoveReference Move(AutoProperty &&src) noexcept
    {
        return Move(src);
    }

    ACHIBULUP__constexpr_fun14 
    AutoProperty(MoveReference value) noexcept : value(value.get()) {}
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator = (MoveReference value) & noexcept
    {
        this->value = value.get();
        return *this;
    }


  public:
    constexpr Type operator () () const noexcept
    {
        return this->value;
    }

    constexpr const Type* operator -> () const noexcept
    {
        return &this->value;
    }


  private:
    ACHIBULUP__constexpr_fun14
    AutoProperty& operator = (Type value) & noexcept
    {
        this->value = value;
        return *this;
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
    AutoProperty& operator += (Type value) & noexcept
    {
        this->value += value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator -= (Type value) & noexcept
    {
        this->value -= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator *= (Type value) & noexcept
    {
        this->value *= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator /= (Type value) & noexcept
    {
        this->value /= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator %= (Type value) & noexcept
    {
        this->value %= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator &= (Type value) & noexcept
    {
        this->value &= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator |= (Type value) & noexcept
    {
        this->value |= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator ^= (Type value) & noexcept
    {
        this->value ^= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator >>= (Type value) & noexcept
    {
        this->value >>= value;
        return *this;
    }
    ACHIBULUP__constexpr_fun14 
    AutoProperty& operator <<= (Type value) & noexcept
    {
        this->value <<= value;
        return *this;
    }

    Type value;

    friend Class;
};


template<class Class>
class AutoProperty<const std::string, Class>
{
  private:
    AutoProperty() noexcept : value() {}
    AutoProperty(std::string str) noexcept : value(std::move(str)) {}

    AutoProperty(const AutoProperty&) = default;
    AutoProperty(AutoProperty&&) noexcept = default;

    AutoProperty& operator = (const AutoProperty &) & = default;
    AutoProperty& operator = (AutoProperty &&) & noexcept = default;

    void swap(AutoProperty &b) noexcept
    {
        using std::swap;
        swap(this->value, b.value);
    }

  public:
    std::string operator () () const &
    {
        return this->value;
    }
    std::string operator () () && noexcept
    {
        return std::move(this->value);
    }

    char operator [] (size_t index) const
    {
        return this->value[index];
    }

    const std::string* operator -> () const noexcept
    {
        return &(this->value);
    }

    friend std::string to_string(const AutoProperty &p)
    {
        return p();
    }
    friend std::string to_string(AutoProperty &&p) noexcept
    {
        return std::move(p)();
    }

    operator string_view () const noexcept
    {
        return {this->value.data(), this->value.size()};
    } 

    friend std::string operator + (const AutoProperty &lhs, 
                                   const AutoProperty &rhs)
    {
        return lhs.value + rhs.value;
    }

    template<typename Str>
    friend std::string operator + (const AutoProperty &lhs, const Str &rhs)
    {
        return lhs.value + std::forward<Str>(rhs);
    }
    template<typename Str>
    friend std::string operator + (const Str &lhs, const AutoProperty &rhs)
    {
        return std::forward<Str>(lhs) + rhs.value;
    }

    friend std::ostream& operator << (std::ostream &os, 
                                      const AutoProperty &x)
    {
        return os << x->data();
    }

  private:
    template<typename Str>
    auto operator = (Str &&rhs) & 
    -> decltype(std::declval<std::string&>() = std::declval<Str>(), *this)
    {
        this->value = std::forward<Str>(rhs);
        return *this;
    }
    AutoProperty& operator = (std::string &&str) & noexcept
    {
        this->value = std::move(str);
        return *this;
    }

    template<typename Str>
    AutoProperty& operator += (Str &&rhs) 
    {
        this->value += std::forward<Str>(rhs);
        return *this;
    }

    std::string& operator * () noexcept
    {
        return this->value;
    }
    const std::string& operator * () const noexcept
    {
        return this->value;
    }

    template<typename Functor>
    void visit(Functor &&functor)
    {
        functor(this->value);
    }
    template<typename Functor>
    void visit(Functor &&functor) const
    {
        functor(this->value);
    }

    std::string value;

    friend Class;
};


template<typename Type, class Class>
using ReadOnlyProperty = AutoProperty<const Type, Class>;

} //namespace Achibulup

#endif  //COMMON_UTILS_H_INCLUDED