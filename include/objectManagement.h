#ifndef OBJMANAGE_H_INCLUDED
#define OBJMANAGE_H_INCLUDED

#include "common_utils.h"
#if ACHIBULUP__Cpp17_later
#include <cstddef>
#endif // ACHIBULUP__Cpp17_later

namespace Achibulup{
#if ACHIBULUP__Cpp17_later
using Byte = std::byte;
#else
using Byte = unsigned char;
#endif


struct ByteSpan
{
    Byte *ptr;
    size_t size;

    ByteSpan() noexcept : ptr(nullptr), size(0) {}
    ByteSpan(void *ptr, size_t size) noexcept 
    : ptr(static_cast<Byte*>(ptr)), size(size) {}
};


template<typename Tp>
inline constexpr Tp* Launder(Tp *ptr) noexcept
{
#if ACHIBULUP__Cpp17_later
    return std::launder(ptr);
#else
    return ptr;
#endif //ACHIBULUP__Cpp17_later
}

#if ACHIBULUP__Cpp17
template<typename Tp>
class RelaxedPtr
{
    using voidptr = void*;
    using alias_pointer = Byte*;

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
        return std::launder(static_cast<Tp*>(static_cast<voidptr>(this->i_ptr)));
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
    alias_pointer i_ptr;
};
template<typename Tp>
class RelaxedPtr<const Tp>
{
    using voidptr = const void*;
    using alias_pointer = const Byte*;

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
        return std::launder(static_cast<const Tp*>
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
    alias_pointer i_ptr;
};

template<>
class RelaxedPtr<void>
{
    using alias_pointer = void*;

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
        return std::launder(static_cast<Tp*>(this->i_ptr));
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
    alias_pointer i_ptr;
};

template<>
class RelaxedPtr<const void>
{
    using alias_pointer = const void*;

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
        return std::launder(static_cast<Tp*>(this->i_ptr));
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
    alias_pointer i_ptr;
};

#else //ACHIBULUP_Cpp17

template<typename Tp>
using RelaxedPtr = Tp*;

#endif //ACHIBULUP_Cpp17

template<typename Tp>
RelaxedPtr<Tp> newBuffer(size_t s)
{
    if(s < 0) throw std::bad_alloc();
    return s ? static_cast<Tp*>(static_cast<void*>(new Byte[s * ssizeof<Tp>()])) 
             : nullptr;
}
inline RelaxedPtr<void> newBuffer(size_t s)
{
    return newBuffer<Byte>(s);
}
template<typename Tp>
void deleteBuffer(RelaxedPtr<Tp> ptr)
{
    delete[] static_cast<const Byte*>
        (static_cast<const void*>(ptr));
}



template<typename Tp, bool = std::is_trivially_destructible<Tp>::value>
struct Destructor
{
    static void destroy(RelaxedPtr<Tp>) noexcept {}
    static void destroy(RelaxedPtr<Tp>, size_t) noexcept {}
};
template<typename Tp>
struct Destructor<Tp, false>
{
    static void destroy(RelaxedPtr<Tp> pos) noexcept
    {
        pos->~Tp();
    }
    static void destroy(RelaxedPtr<Tp> pos, size_t num) noexcept
    {
        pos += num;
        for(size_t i = 0; i < num; ++i)
          (--pos)->~Tp();
    }
};

#if ACHIBULUP_Cpp17
template<typename Tp>
void destroy(RelaxedPtr<Tp> pos) noexcept
{
    Destructor<Tp>::destroy(pos);
}
template<typename Tp>
void destroy(RelaxedPtr<Tp> pos, size_t num) noexcept
{
    Destructor<Tp>::destroy(pos, num);
}
#endif //ACHIBULUP_Cpp17

template<typename Tp>
void destroy(Tp *pos) noexcept
{
    Destructor<Tp>::destroy(pos);
}
template<typename Tp>
void destroy(Tp *pos, size_t num) noexcept
{
    Destructor<Tp>::destroy(pos, num);
}


template<typename Tp, bool = std::is_trivially_destructible<Tp>::value>
struct Constructor
{
    struct exception_safety{RelaxedPtr<Tp> first, last;};


    template<typename ...Args>
    static void construct(RelaxedPtr<Tp> pos, Args&& ...args)
    noexcept(noexcept(Tp(std::forward<Args>(args)...)))
    {
        new(pos) Tp(std::forward<Args>(args)...);
    }
    template<typename ...Args>
    static void braceConstruct(RelaxedPtr<Tp> pos, Args&& ...args)
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
    static void braceConstruct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    noexcept(noexcept(Tp{args...}))
    {
        for(size_t i = 0; i < num; ++i)
          new(pos++) Tp{args...};
    }

    static void copyConstruct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<const Tp> source)
    noexcept(std::is_nothrow_copy_constructible<Tp>::value)
    {
        for(size_t i = 0; i < num; ++i){
          new(pos++) Tp(*source);
          ++source;
        }
    }
    static void moveConstruct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<Tp> source)
    noexcept(std::is_nothrow_move_constructible<Tp>::value)
    {
        for(size_t i = 0; i < num; ++i){
          new(pos++) Tp(static_cast<Tp&&>(*source));
          ++source;
        }
    }
};

template<typename Tp>
struct Constructor<Tp, false>
{
    struct exception_safety
    {
        RelaxedPtr<Tp> first, last;
        ~exception_safety()
        {
            while(last != first)
              Destructor<Tp>::destroy(--last);
        }
    };


    template<typename ...Args>
    static void construct(RelaxedPtr<Tp> pos, Args&& ...args)
    noexcept(noexcept(Tp(std::forward<Args>(args)...)))
    {
        new(pos) Tp(std::forward<Args>(args)...);
    }
    template<typename ...Args>
    static void braceConstruct(RelaxedPtr<Tp> pos, Args&& ...args)
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
    static void braceConstruct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
    noexcept(noexcept(Tp{args...}))
    {
        exception_safety p{pos, pos};
        for(size_t i = 0; i < num; ++i)
          new(p.last++) Tp{args...};
        p.last = p.first;
    }

    static void copyConstruct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<const Tp> source)
    noexcept(std::is_nothrow_copy_constructible<Tp>::value)
    {
        exception_safety p{pos, pos};
        for(size_t i = 0; i < num; ++i){
          new(p.last++) Tp(*source);
          ++source;
        }
        p.last = p.first;
    }
    static void moveConstruct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<Tp> source)
    noexcept(std::is_nothrow_move_constructible<Tp>::value)
    {
        exception_safety p{pos, pos};
        for(size_t i = 0; i < num; ++i){
          new(p.last++) Tp(static_cast<Tp&&>(*source));
          ++source;
        }
        p.last = p.first;
    }
};

#if ACHIBULUP_Cpp17
template<typename Tp, typename ...Args>
void construct(RelaxedPtr<Tp> pos, Args&& ...args)
noexcept(noexcept(Constructor<Tp>::construct(pos, std::forward<Args>(args)...)))
{
    Constructor<Tp>::construct(pos, std::forward<Args>(args)...);
}
template<typename Tp, typename ...Args>
void braceConstruct(RelaxedPtr<Tp> pos, Args&& ...args)
noexcept(noexcept(Constructor<Tp>::braceConstruct(pos, std::forward<Args>(args)...)))
{
    Constructor<Tp>::braceConstruct(pos, std::forward<Args>(args)...);
}
template<typename Tp, typename ...Args>
void construct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
noexcept(noexcept(Constructor<Tp>::construct(num, pos, args...)))
{
    Constructor<Tp>::construct(num, pos, args...);
}
template<typename Tp, typename ...Args>
void braceConstruct(size_t num, RelaxedPtr<Tp> pos, const Args& ...args)
noexcept(noexcept(Constructor<Tp>::braceConstruct(num, pos, args...)))
{
    Constructor<Tp>::braceConstruct(num, pos, args...);
}
template<typename Tp, typename ...Args>
void copyConstruct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<const Tp> source)
noexcept(noexcept(Constructor<Tp>::copyConstruct(num, pos, source)))
{
    Constructor<Tp>::copyConstruct(num, pos, source);
}
template<typename Tp, typename ...Args>
void moveConstruct(size_t num, RelaxedPtr<Tp> pos, RelaxedPtr<Tp> source)
noexcept(noexcept(Constructor<Tp>::moveConstruct(num, pos, source)))
{
    Constructor<Tp>::moveConstruct(num, pos, source);
}
#endif // ACHIBULUP_Cpp17


template<typename Tp, typename ...Args>
void construct(Tp *pos, Args&& ...args)
noexcept(noexcept(Constructor<Tp>::construct(pos, std::forward<Args>(args)...)))
{
    Constructor<Tp>::construct(pos, std::forward<Args>(args)...);
}
template<typename Tp, typename ...Args>
void braceConstruct(Tp *pos, Args&& ...args)
noexcept(noexcept(Constructor<Tp>::braceConstruct(pos, std::forward<Args>(args)...)))
{
    Constructor<Tp>::braceConstruct(pos, std::forward<Args>(args)...);
}
template<typename Tp, typename ...Args>
void construct(size_t num, Tp *pos, const Args& ...args)
noexcept(noexcept(Constructor<Tp>::construct(num, pos, args...)))
{
    Constructor<Tp>::construct(num, pos, args...);
}
template<typename Tp, typename ...Args>
void braceConstruct(size_t num, Tp *pos, const Args& ...args)
noexcept(noexcept(Constructor<Tp>::braceConstruct(num, pos, args...)))
{
    Constructor<Tp>::braceConstruct(num, pos, args...);
}
template<typename Tp, typename ...Args>
void copyConstruct(size_t num, Tp *pos, const Tp *source)
noexcept(noexcept(Constructor<Tp>::copyConstruct(num, pos, source)))
{
    Constructor<Tp>::copyConstruct(num, pos, source);
}
template<typename Tp, typename ...Args>
void moveConstruct(size_t num, Tp *pos, Tp *source)
noexcept(noexcept(Constructor<Tp>::moveConstruct(num, pos, source)))
{
    Constructor<Tp>::moveConstruct(num, pos, source);
}



class Buffer
{
  public:
    using pointer = RelaxedPtr<Byte>;

    Buffer() noexcept : size(0), m_buffer() {}

    Buffer(size_t len) : size(len), m_buffer(pointer(newBuffer(len))) {}

    Buffer(Buffer &&mov) noexcept 
    : size(Move(mov.size)), m_buffer(Move(mov.m_buffer)) {}
    
    void operator = (Buffer) = delete;

    ~Buffer()
    {
        this->reset();
    }

    void swap(Buffer &b) noexcept
    {
        using std::swap;
        swap(this->m_buffer, b.m_buffer);
        this->size.swap(b.size);
    }

    friend void swap(Buffer &a, Buffer &b) noexcept
    {
        a.swap(b);
    }

    void reset(size_t new_cap)
    {
        if (new_cap > this->size()){
          Buffer tmp(new_cap);
          this->swap(tmp);
        }
    }
    void reset()
    {
        deleteBuffer(this->m_buffer);
        this->size = 0;
    }

    ByteSpan get() const noexcept
    {
        return ByteSpan{this->buffer(), this->size()};
    }

    pointer buffer() const noexcept
    {
        return this->m_buffer;
    }
    void* data() const noexcept
    {
        return this->m_buffer;
    }


    ReadOnlyProperty<size_t, Buffer> size;

  private:
    pointer m_buffer;
};

} //namespace Achibulup

#endif  //OBJMANAGE_H_INCLUDED