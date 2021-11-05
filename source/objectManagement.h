#ifndef OBJMANAGE_H_INCLUDED
#define OBJMANAGE_H_INCLUDED

#include "common_utils.h"

namespace Achibulup{
#if ACHIBULUP__Cpp17_later
using std::byte;
#else
using byte = unsigned char;
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
    return s ? static_cast<Tp*>(static_cast<void*>(new byte[s * ssizeof<Tp>()])) 
             : nullptr;
}
inline RelaxedPtr<void> new_buffer(size_t s)
{
    return new_buffer<byte>(s);
}
template<typename Tp>
inline void delete_buffer(RelaxedPtr<Tp> ptr)
{
    delete[] static_cast<copy_cvref_t<Tp, byte>*>
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



} //namespace Achibulup

#endif  //OBJMANAGE_H_INCLUDED