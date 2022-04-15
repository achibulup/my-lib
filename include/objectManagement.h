#ifndef OBJMANAGE_H_INCLUDED
#define OBJMANAGE_H_INCLUDED

#include "common_utils.h"

namespace Achibulup{
#if ACHIBULUP__Cpp17_later
using std::byte;
#else
using byte = unsigned char;
#endif


struct ByteSpan
{
    byte *ptr;
    std::size_t size;

    ByteSpan() noexcept : ptr(nullptr), size(0) {}
    ByteSpan(void *ptr, std::size_t size) noexcept 
    : ptr(static_cast<byte*>(ptr)), size(size) {}
};


#if ACHIBULUP_Cpp17_prior
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

#else //ACHIBULUP_Cpp17_prior

template<typename Tp>
using RelaxedPtr = Tp*;

#endif //ACHIBULUP_Cpp17_prior

template<typename Tp>
inline RelaxedPtr<Tp> newBuffer(size_t s)
{
    if(s < 0) throw std::bad_alloc();
    return s ? static_cast<Tp*>(static_cast<void*>(new byte[s * ssizeof<Tp>()])) 
             : nullptr;
}
inline RelaxedPtr<void> newBuffer(size_t s)
{
    return newBuffer<byte>(s);
}
template<typename Tp>
inline void deleteBuffer(RelaxedPtr<Tp> ptr)
{
    delete[] static_cast<const byte*>
        (static_cast<const void*>(ptr));
}



template<typename Tp, bool = std::is_trivially_destructible<Tp>::value>
struct destructor
{
    static void destroy(RelaxedPtr<Tp>) noexcept {}
    static void destroy(RelaxedPtr<Tp>, size_t) noexcept {}
};
template<typename Tp>
struct destructor<Tp, false>
{
    static void destroy(RelaxedPtr<Tp> pos) noexcept
    {
        destroy(pos, 1);
    }
    static void destroy(RelaxedPtr<Tp> pos, size_t num) noexcept
    {
        using Type = Tp;
        pos += num;
        for(size_t i = 0; i < num; ++i)
          (--pos)->~Type();
    }
};



template<typename Tp, bool = std::is_trivially_destructible<Tp>::value>
struct constructor
{
    struct exception_safety{RelaxedPtr<Tp> first, last;};


    template<typename ...Args>
    static void construct(RelaxedPtr<Tp> pos, Args&& ...args)
    noexcept(noexcept(Tp(std::forward<Args>(args)...)))
    {
        new(pos) Tp(std::forward<Args>(args)...);
    }
    template<typename ...Args>
    static void brace_construct(RelaxedPtr<Tp> pos, Args&& ...args)
    noexcept(noexcept(Tp{std::forward<Args>(args)...}))
    {
        new(pos) Tp{std::forward<Args>(args)...};
    }

    template<typename ...Args>
    static void construct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    noexcept(noexcept(Tp(args...)))
    {
        for(size_t i = 0; i < num; ++i)
          new(pos++) Tp(args...);
    }
    template<typename ...Args>
    static void brace_construct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    noexcept(noexcept(Tp{args...}))
    {
        for(size_t i = 0; i < num; ++i)
          new(pos++) Tp{args...};
    }

    static void copy_construct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<const Tp> source)
    noexcept(std::is_nothrow_copy_constructible<Tp>::value)
    {
        for(size_t i = 0; i < num; ++i){
          new(pos++) Tp(*source);
          ++source;
        }
    }
    static void move_construct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<Tp> source)
    noexcept(std::is_nothrow_move_constructible<Tp>::value)
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
    noexcept(noexcept(Tp(std::forward<Args>(args)...)))
    {
        new(pos) Tp(std::forward<Args>(args)...);
    }
    template<typename ...Args>
    static void brace_construct(RelaxedPtr<Tp> pos, Args&& ...args)
    noexcept(noexcept(Tp{std::forward<Args>(args)...}))
    {
        new(pos) Tp{std::forward<Args>(args)...};
    }

    template<typename ...Args>
    static void construct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    noexcept(noexcept(Tp(args...)))
    {
        exception_safety p{pos, pos};
        for(size_t i = 0; i < num; ++i)
          new(p.last++) Tp(args...);
        p.last = p.first;
    }
    template<typename ...Args>
    static void brace_construct(std::size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    noexcept(noexcept(Tp{args...}))
    {
        exception_safety p{pos, pos};
        for(std::size_t i = 0; i < num; ++i)
          new(p.last++) Tp{args...};
        p.last = p.first;
    }

    static void copy_construct(std::size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<const Tp> source)
    noexcept(std::is_nothrow_copy_constructible<Tp>::value)
    {
        exception_safety p{pos, pos};
        for(std::size_t i = 0; i < num; ++i){
          new(p.last++) Tp(*source);
          ++source;
        }
        p.last = p.first;
    }
    static void move_construct(std::size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<Tp> source)
    noexcept(std::is_nothrow_move_constructible<Tp>::value)
    {
        exception_safety p{pos, pos};
        for(std::size_t i = 0; i < num; ++i){
          new(p.last++) Tp(static_cast<Tp&&>(*source));
          ++source;
        }
        p.last = p.first;
    }
};


class Buffer
{
  public:
    using pointer = RelaxedPtr<void>;

    Buffer() noexcept : size(0), m_buffer(){}

    Buffer(size_t len) : size(len), m_buffer(newBuffer(len)) {}

    Buffer(Buffer &&mov) noexcept 
    : size(Move(mov.size)), m_buffer(Move(mov.m_buffer)) {}
    
    void operator = (Buffer) = delete;

    ~Buffer()
    {
        this->reset();
    }

    friend void swap(Buffer &a, Buffer &b) noexcept
    {
        a.doSwap(b);
    }

    ReadOnlyProperty<size_t, Buffer> size;

    void reset(size_t new_cap)
    {
        if (new_cap > this->size){
            Buffer tmp(new_cap);
            swap(*this, tmp);
        }
    }
    void reset()
    {
        deleteBuffer(this->m_buffer);
        this->size = 0;
    }


    void* buffer() const noexcept
    {
        return this->m_buffer;
    }
    pointer data() const noexcept
    {
        return this->m_buffer;
    }

  private:
    void doSwap(Buffer &b) noexcept
    {
        using std::swap;
        swap(this->m_buffer, b.m_buffer);
        this->size.swap(b.size);
    }


    pointer m_buffer;
};

} //namespace Achibulup

#endif  //OBJMANAGE_H_INCLUDED