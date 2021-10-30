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


template<typename Tp>
inline constexpr Tp* Launder(Tp *ptr) noexcept
{
#if ACHIBULUP__Cpp17_later
    return std::launder(ptr);
#else
    return ptr;
#endif //ACHIBULUP__Cpp17_later
}


template<typename Tp>
class RelaxedPtr
{
    using voidptr = void*;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Tp;
    using difference_type = std::ptrdiff_t;
    using reference = Tp&;
    using pointer = Tp*;


    RelaxedPtr() noexcept = default;

    constexpr RelaxedPtr(pointer ptr) noexcept 
    : i_ptr(static_cast<alias_pointer>(static_cast<voidptr>(ptr))) {}

    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator = (const RelaxedPtr&) & noexcept = default;

    constexpr reference operator * () const noexcept
    {
        return *(this->get());
    }
    constexpr pointer operator -> () const noexcept
    {
        return this->get();
    }
    constexpr reference operator [] (difference_type offset) const noexcept
    {
        return *(*this + offset);
    }

    constexpr Tp* get() const noexcept
    {
        return Launder(static_cast<Tp*>(static_cast<voidptr>(this->i_ptr)));
    }

    constexpr explicit operator Tp* () const noexcept
    {
        return this->get();
    }
    constexpr operator voidptr () const noexcept
    {
        return this->i_ptr;
    }

    friend constexpr bool operator == (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr != r.i_ptr;
    }
    friend constexpr bool operator != (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l == r);
    }
    friend constexpr bool operator < (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr < r.i_ptr;
    }
    friend constexpr bool operator > (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return r < l;
    }
    friend constexpr bool operator <= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(r < l);
    }
    friend constexpr bool operator >= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l < r);
    }

    ACHIBULUP__constexpr_fun14 RelaxedPtr& operator ++() & noexcept
    {
        this->i_ptr += ssizeof<Tp>();
        return *this;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr operator ++(int) & noexcept
    {
        RelaxedPtr res = *this;
        ++(*this);
        return res;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr& operator --() & noexcept
    {
        this->i_ptr -= ssizeof<Tp>();
        return *this;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr& operator --(int) & noexcept
    {
        RelaxedPtr res = *this;
        --(*this);
        return res;
    }

    friend constexpr RelaxedPtr
    operator + (RelaxedPtr lhs, difference_type rhs) noexcept
    {
        return static_cast<pointer>(static_cast<voidptr>
            (lhs.i_ptr + rhs * ssizeof<Tp>()));
    }
    friend constexpr RelaxedPtr 
    operator + (difference_type lhs, RelaxedPtr rhs) noexcept
    {
        return rhs + lhs;
    }
    friend constexpr RelaxedPtr 
    operator - (RelaxedPtr lhs, difference_type rhs) noexcept
    {
        return static_cast<pointer>(static_cast<voidptr>
            (lhs.i_ptr - rhs * ssizeof<Tp>()));
    }
    friend constexpr difference_type 
    operator - (RelaxedPtr lhs, RelaxedPtr rhs) noexcept
    {
        return (lhs.i_ptr - rhs.i_ptr) / ssizeof<Tp>();
    }

    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator += (difference_type val) & noexcept
    {
        return *this = *this + val;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator -= (difference_type val) & noexcept
    {
        return *this = *this - val;
    }

  private:
    using alias_pointer = byte*;
    alias_pointer i_ptr;
};
template<typename Tp>
class RelaxedPtr<const Tp>
{
    using voidptr = const void*;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Tp;
    using difference_type = std::ptrdiff_t;
    using reference = const Tp&;
    using pointer = const Tp*;


    RelaxedPtr() noexcept = default;

    constexpr RelaxedPtr(pointer ptr) noexcept 
    : i_ptr(static_cast<alias_pointer>(static_cast<voidptr>(ptr))) {}

    constexpr RelaxedPtr(RelaxedPtr<Tp> ptr) noexcept 
    : i_ptr(static_cast<alias_pointer>(static_cast<void*>(ptr))) {}

    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator = (const RelaxedPtr&) & noexcept = default;

    constexpr reference operator * () const noexcept
    {
        return *(this->get());
    }
    constexpr pointer operator -> () const noexcept
    {
        return this->get();
    }
    constexpr reference operator [] (difference_type offset) const noexcept
    {
        return *(*this + offset);
    }

    constexpr const Tp* get() const noexcept
    {
        return Launder(static_cast<const Tp*>
            (static_cast<voidptr>(this->i_ptr)));
    }

    constexpr explicit operator const Tp* () const noexcept
    {
        return this->get();
    }
    constexpr operator voidptr () const noexcept
    {
        return this->i_ptr;
    }

    friend constexpr bool operator == (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr != r.i_ptr;
    }
    friend constexpr bool operator != (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l == r);
    }
    friend constexpr bool operator < (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr < r.i_ptr;
    }
    friend constexpr bool operator > (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return r < l;
    }
    friend constexpr bool operator <= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(r < l);
    }
    friend constexpr bool operator >= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l < r);
    }

    ACHIBULUP__constexpr_fun14 RelaxedPtr& operator ++() & noexcept
    {
        this->i_ptr += ssizeof<Tp>();
        return *this;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr operator ++(int) & noexcept
    {
        RelaxedPtr res = *this;
        ++(*this);
        return res;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr& operator --() & noexcept
    {
        this->i_ptr -= ssizeof<Tp>();
        return *this;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr& operator --(int) & noexcept
    {
        RelaxedPtr res = *this;
        --(*this);
        return res;
    }

    friend constexpr RelaxedPtr
    operator + (RelaxedPtr lhs, difference_type rhs) noexcept
    {
        return static_cast<pointer>(static_cast<voidptr>
            (lhs.i_ptr + rhs * ssizeof<Tp>()));
    }
    friend constexpr RelaxedPtr 
    operator + (difference_type lhs, RelaxedPtr rhs) noexcept
    {
        return rhs + lhs;
    }
    friend constexpr RelaxedPtr 
    operator - (RelaxedPtr lhs, difference_type rhs) noexcept
    {
        return static_cast<pointer>(static_cast<voidptr>
            (lhs.i_ptr - rhs * ssizeof<Tp>()));
    }
    friend constexpr difference_type 
    operator - (RelaxedPtr lhs, RelaxedPtr rhs) noexcept
    {
        return (lhs.i_ptr - rhs.i_ptr) / ssizeof<Tp>();
    }

    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator += (difference_type val) & noexcept
    {
        return *this = *this + val;
    }
    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator -= (difference_type val) & noexcept
    {
        return *this = *this - val;
    }

  private:
    using alias_pointer = const byte*;
    alias_pointer i_ptr;
};

template<>
class RelaxedPtr<void>
{
  public:
    RelaxedPtr() noexcept = default;

    constexpr RelaxedPtr(void* ptr) noexcept : i_ptr(ptr) {}

    template<typename Tp, 
        typename = decltype(static_cast<void*>(Typeof<Tp*>()))>
    constexpr RelaxedPtr(RelaxedPtr<Tp> ptr) noexcept : i_ptr(ptr) {}

    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator = (const RelaxedPtr&) & noexcept = default;

    template<typename Tp>
    constexpr Tp* get() const noexcept
    {
        return Launder(static_cast<Tp*>(this->i_ptr));
    }

    constexpr operator void* () const noexcept
    {
        return this->i_ptr;
    }
    template<typename Tp, 
        typename = EnableIf_t<!std::is_same<Tp, const void>::value>>
    constexpr explicit operator Tp* () const noexcept
    {
        return this->get<Tp>();
    }
    template<typename Tp>
    constexpr explicit operator RelaxedPtr<Tp> () const noexcept
    {
        return static_cast<Tp*>(this->i_ptr);
    }

    friend constexpr bool operator == (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr != r.i_ptr;
    }
    friend constexpr bool operator != (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l == r);
    }
    friend constexpr bool operator < (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr < r.i_ptr;
    }
    friend constexpr bool operator > (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return r < l;
    }
    friend constexpr bool operator <= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(r < l);
    }
    friend constexpr bool operator >= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l < r);
    }

  private:
    using alias_pointer = void*;
    alias_pointer i_ptr;
};

template<>
class RelaxedPtr<const void>
{
  public:
    RelaxedPtr() noexcept = default;

    constexpr RelaxedPtr(const void* ptr) noexcept : i_ptr(ptr) {}

    template<typename Tp>
    constexpr RelaxedPtr(RelaxedPtr<Tp> ptr) noexcept : i_ptr(ptr) {}

    ACHIBULUP__constexpr_fun14 RelaxedPtr& 
    operator = (const RelaxedPtr&) & noexcept = default;

    template<typename Tp, EnableIf_t<std::is_const<Tp>::value>* = nullptr>
    constexpr Tp* get() const noexcept
    {
        return Launder(static_cast<Tp*>(this->i_ptr));
    }

    constexpr operator const void* () const noexcept
    {
        return this->i_ptr;
    }
    template<typename Tp>
    constexpr explicit operator const Tp* () const noexcept
    {
        return this->get<const Tp>();
    }
    template<typename Tp>
    constexpr explicit operator RelaxedPtr<const Tp> () const noexcept
    {
        return static_cast<const Tp*>(this->i_ptr);
    }

    friend constexpr bool operator == (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr != r.i_ptr;
    }
    friend constexpr bool operator != (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l == r);
    }
    friend constexpr bool operator < (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return l.i_ptr < r.i_ptr;
    }
    friend constexpr bool operator > (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return r < l;
    }
    friend constexpr bool operator <= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(r < l);
    }
    friend constexpr bool operator >= (RelaxedPtr l, RelaxedPtr r) noexcept
    {
        return !(l < r);
    }

  private:
    using alias_pointer = const void*;
    alias_pointer i_ptr;
};


template<typename Tp>
inline RelaxedPtr<Tp> new_buffer(size_t s)
{
    if(s < 0) throw std::bad_alloc();
    return s ? static_cast<Tp*>(static_cast<copy_cvref_t<Tp, void>*>
                   (new byte[s * ssizeof<Tp>()])) 
             : nullptr;
}
inline RelaxedPtr<void> new_buffer(size_t s)
{
    if(s < 0) throw std::bad_alloc();
    return s ? new unsigned char[s] : nullptr;
}
template<typename Tp>
inline void delete_buffer(RelaxedPtr<Tp> ptr)
{
    delete[] static_cast<copy_cvref_t<Tp, unsigned char>*>
        (static_cast<copy_cvref_t<Tp, void>*>(ptr));
}



template<typename Tp, bool = std::is_trivially_destructible<Tp>::value>
struct destructor
{
    static void destroy(RelaxedPtr<Tp>, size_t = 1) {}
};
template<typename Tp>
struct destructor<Tp, false>
{
    static void destroy(RelaxedPtr<Tp> pos, size_t num = 1)
    {
        pos += num;
        for(size_t i = 0; i < num; ++i)
          delete --pos;
    }
};



template<typename Tp, bool = std::is_trivially_destructible<Tp>::value>
struct constructor
{
    struct exception_safety{RelaxedPtr<Tp> first, last;};


    template<typename ...Args>
    static void construct(RelaxedPtr<Tp> pos, Args&& ...args)
    {
        new(pos) Tp(std::forward<Args>(args)...);
    }
    template<typename ...Args>
    static void brace_construct(RelaxedPtr<Tp> pos, Args&& ...args)
    {
        new(pos) Tp{std::forward<Args>(args)...};
    }

    template<typename ...Args>
    static void construct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    {
        for(size_t i = 0; i < num; ++i)
          new(pos++) Tp(args...);
    }
    template<typename ...Args>
    static void brace_construct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    {
        for(size_t i = 0; i < num; ++i)
          new(pos++) Tp{args...};
    }

    static void copy_construct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<const Tp> source)
    {
        for(size_t i = 0; i < num; ++i){
          new(pos++) Tp(*source);
          ++source;
        }
    }
    static void move_construct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<Tp> source)
    {
        for(size_t i = 0; i < num; ++i){
          new(pos++) Tp(static_cast<Tp&&>(*source));
          ++source;
        }
    }
};

template<typename Tp>
struct constructor<Tp, false>
{
    struct exception_safety
    {
        RelaxedPtr<Tp> first, last;
        ~exception_safety()
        {
            while(last != first)
              destructor<Tp>::destroy(--last);
        }
    };


    template<typename ...Args>
    static void construct(RelaxedPtr<Tp> pos, Args&& ...args)
    {
        new(pos) Tp(std::forward<Args>(args)...);
    }
    template<typename ...Args>
    static void brace_construct(RelaxedPtr<Tp> pos, Args&& ...args)
    {
        new(pos) Tp{std::forward<Args>(args)...};
    }

    template<typename ...Args>
    static void construct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    {
        exception_safety p{pos, pos};
        for(size_t i = 0; i < num; ++i)
          new(p.last++) Tp(args...);
        p.last = p.first;
    }
    template<typename ...Args>
    static void brace_construct(std::size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    {
        exception_safety p{pos, pos};
        for(std::size_t i = 0; i < num; ++i)
          new(p.last++) Tp{args...};
        p.last = p.first;
    }

    static void copy_construct(std::size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<const Tp> source)
    {
        exception_safety p{pos, pos};
        for(std::size_t i = 0; i < num; ++i){
          new(p.last++) Tp(*source);
          ++source;
        }
        p.last = p.first;
    }
    static void move_construct(std::size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<Tp> source)
    {
        exception_safety p{pos, pos};
        for(std::size_t i = 0; i < num; ++i){
          new(p.last++) Tp(static_cast<Tp&&>(*source));
          ++source;
        }
        p.last = p.first;
    }
};



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