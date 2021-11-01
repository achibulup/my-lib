#ifndef ACHIBULUP_ANY_H_INCLUDED
#define ACHIBULUP_ANY_H_INCLUDED
#include <typeinfo>
#include <stdexcept>
#include "common_utils.h"
#include "objectManagement.h"

namespace Achibulup{

class bad_copy : std::exception
{
    const char* what () const noexcept override
    {
        return "bad_copy";
    }
};
class bad_compare : std::exception
{
    const char* what () const noexcept override
    {
        return "bad_compare";
    }
};



template<typename Tp>
using decay_t = typename std::decay<Tp>::type;
template<typename Tp>
using remove_const_ref_t = typename std::remove_const<typename std::remove_reference<Tp>::type>::type;
template<typename Tp>
using remove_const_ptr_ref_t = typename std::remove_const<typename std::remove_pointer<typename std::remove_reference<Tp>::type>::type>::type;





namespace n_Any_helper{

using CopyCtor_t = void(*)(RelaxedPtr<void>, RelaxedPtr<const void>);
using EqualComparator_t = bool(*)(RelaxedPtr<const void>, RelaxedPtr<const void>);
using Destructor_t = void(*)(RelaxedPtr<void>);


template<typename Tp>
void CopyCtor(RelaxedPtr<void> dest, RelaxedPtr<const void> source)
{
    new (dest) Tp(*static_cast<const Tp*>(source));
}
template<typename Tp>
bool EqualComparator(RelaxedPtr<const void> l, RelaxedPtr<const void> r)
{
    return *static_cast<const Tp*>(l) == *static_cast<const Tp*>(r);
}
template<typename Tp>
void Destructor(RelaxedPtr<void> ptr) noexcept
{
    using type = Tp;
    static_cast<type*>(ptr)->~type();
}


template<typename Tp
    , EnableIf_t<std::is_convertible<const Tp&, Tp>::value>* = nullptr>
constexpr CopyCtor_t GetCopyCtor() noexcept
{
    return CopyCtor<Tp>;
}
template<typename Tp, typename = void
    , EnableIf_t<!std::is_convertible<const Tp&, Tp>::value>* = nullptr>
constexpr CopyCtor_t GetCopyCtor() noexcept
{
    return nullptr;
}

template<typename Tp
    , typename = decltype(std::declval<const Tp&>() == std::declval<const Tp&>())>
constexpr EqualComparator_t GetEqualComparatorHelper(int) noexcept
{
    return EqualComparator<Tp>;
}
template<typename Tp, typename Tp2>
constexpr EqualComparator_t GetEqualComparatorHelper(Tp2) noexcept
{
    return nullptr;
}
template<typename Tp>
constexpr EqualComparator_t GetEqualComparator() noexcept
{
    return GetEqualComparatorHelper<Tp>(0);
}

template<typename Tp>
constexpr Destructor_t GetDestructor() noexcept
{
    return Destructor<Tp>;
}


struct AnyTypeManagerStructure
{
    size_t size;
    CopyCtor_t CopyCtor;
    EqualComparator_t EqualComparator;
    Destructor_t Destructor;
    const std::type_info *typeinfo;
};
template<typename Tp>
struct AnyTypeManager
{
    static constexpr AnyTypeManagerStructure manager{
        ssizeof<Tp>(),
        GetCopyCtor<Tp>(),
        GetEqualComparator<Tp>(),
        GetDestructor<Tp>(),
        &typeid(Tp)
    };
};
template<typename Tp>
const AnyTypeManagerStructure AnyTypeManager<Tp>::manager;


class AnyBuffer
{
  public:
    AnyBuffer() noexcept : i_ptr(), capacity(0) {}

    AnyBuffer(size_t len) : i_ptr(new_buffer(len)), capacity(len) {}

    AnyBuffer(AnyBuffer &&mov) noexcept 
    : i_ptr(Move(mov.i_ptr)), capacity(Move(mov.capacity)) {}

    ~AnyBuffer()
    {
        delete_buffer(i_ptr);
    }

    friend void swap(AnyBuffer &a, AnyBuffer &b) noexcept
    {
        AnyBuffer::swap(a, b);
    }

  private:
    using pointer = RelaxedPtr<void>;
    pointer i_ptr;

  public:
    ReadOnlyProperty<size_t, AnyBuffer> capacity;

    void reset(size_t new_cap)
    {
        if (new_cap > this->capacity){
            AnyBuffer tmp(new_cap);
            swap(*this, tmp);
        }
    }

    void operator = (AnyBuffer) = delete;

    void* Get_buffer() const noexcept
    {
        return this->i_ptr;
    }
    pointer data() const noexcept
    {
        return this->i_ptr;
    }

  private:
    static void swap(AnyBuffer &a, AnyBuffer &b) noexcept
    {
        using std::swap;
        swap(a.i_ptr, b.i_ptr);
        swap(a.capacity.value, b.capacity.value);
    }
};

} //namespace n_Any_helper



class Any
{
  public:
    Any() noexcept : i_buffer(), i_manager() {}

    template<typename Tp0,
        EnableIf_t<!std::is_same<decay_t<Tp0>, Any>::value>* = nullptr>
    Any(Tp0 &&val) : Any()
    {
        using Tp = decay_t<Tp0>;
        this->unsafe_emplace<Tp>(std::forward<Tp0>(val));
    }

    Any(Any &&mov) noexcept
    : i_buffer(Move(mov.i_buffer)), i_manager(Move(mov.i_manager)) {}

    Any(const Any &cpy) : i_buffer(cpy.i_manager ? cpy.i_manager->size : 0), 
            i_manager(cpy.i_manager)
    {
        if (cpy.i_manager){
          if (cpy.i_manager->CopyCtor) throw bad_copy();
          cpy.i_manager->CopyCtor(this->i_buffer.Get_buffer(), 
                                  cpy.i_buffer.Get_buffer());
        }
    }

    Any& operator = (Any &&mov) & noexcept
    {
        this->clear();
        if (!mov.empty())
          swap(*this, mov);
        return *this;
    }

    Any& operator = (const Any &cpy) &
    {
        Any tmp(cpy);
        swap(*this, tmp);
        return *this;
    }
    
    ~Any() noexcept
    {
        this->clear();
    }
    
    friend void swap(Any &a, Any &b) noexcept
    {
        using std::swap;
        swap(a.i_buffer, b.i_buffer);
        swap(a.i_manager, b.i_manager);
    }


    template<typename Tp, 
            bool variadic = true,
            EnableIf_t<std::is_same<Tp, decay_t<Tp>>::value>* = nullptr,
            EnableIf_t<!std::is_same<Tp, Any>::value>* = nullptr, 
            typename ...Args>
    void emplace(Args&& ...args) &
    {
        if (this->i_buffer.capacity >= ssizeof<Tp>() 
        && noexcept(this->construct<Tp>(std::forward<Args>(args)...)))
          this->unsafe_emplace<Tp>(std::forward<Args>(args)...);
        else {
          Any tmp;
          tmp.unsafe_emplace<Tp>(std::forward<Args>(args)...);
          swap(*this, tmp);
        }
    }

    template<typename Tp,
            EnableIf_t<!std::is_same<decay_t<Tp>, Any>::value>* = nullptr>
    void emplace(Tp &&val) &
    {
        this->emplace<decay_t<Tp>, true>(std::forward<Tp>(val));
    }
    template<typename Tp,
            EnableIf_t<std::is_same<Tp, decay_t<Tp>>::value>* = nullptr,
            EnableIf_t<!std::is_same<Tp, Any>::value>* = nullptr>
    void emplace(Typeof<Tp> &&val) &
    {
        this->emplace<Tp, true>(std::forward<Tp>(val));
    }


    template<typename Tp0,  
            EnableIf_t<!std::is_same<decay_t<Tp0>, Any>::value>* = nullptr,
            typename Tp = decay_t<Tp0>>
    Tp& operator = (Tp0 &&val) &
    {
        if ( this->is<Tp>() 
         && noexcept(this->unguarded_cast<Tp>() = std::forward<Tp0>(val)) )
          this->unguarded_cast<Tp>() = std::forward<Tp0>(val);
        else this->emplace<Tp>(std::forward<Tp0>(val));
        return this->unguarded_cast<Tp>();
    }

    
    static const Any null;


    explicit operator bool () const noexcept
    {
        return !this->empty();
    }


    friend bool operator == (const Any &l, const Any &r)
    {
        if (l.i_manager != r.i_manager) return false;
        if (l.empty()) return true;
        if (!l.i_manager->EqualComparator) throw bad_compare();
        return l.i_manager->EqualComparator
            (l.i_buffer.data(), r.i_buffer.data());
    }
    friend bool operator != (const Any &l, const Any &r)
    {
        return !(l == r);
    }

    
    const std::type_info& type() const noexcept
    {
        if (this->empty()) return typeid(void);
        return *(this->i_manager->typeinfo);
    }

    
    template<typename Tp, 
        EnableIf_t<std::is_same<Tp, decay_t<Tp>>::value>* = nullptr>
    bool is() const noexcept
    {
        return std::is_same<Tp, Any>::value
        || this->i_manager == &n_Any_helper::AnyTypeManager<Tp>::manager;
    }


    template<typename Tp>
    explicit operator Tp () &
    {
        return this->ref_cast<remove_const_ref_t<Tp>>();
    }
    template<typename Tp>
    explicit operator Tp& () &
    {
        return this->ref_cast<remove_const_ref_t<Tp>>();
    }

    template<typename Tp>
    explicit operator Tp () const &
    {
        return this->ref_cast<remove_const_ref_t<Tp>>();
    }
    template<typename Tp>
    explicit operator Tp& () const &
    {
        return this->ref_cast<remove_const_ref_t<Tp>>();
    }

    template<typename Tp>
    explicit operator Tp () &&
    {
        return std::move(this->ref_cast<remove_const_ref_t<Tp>>());
    }
    template<typename Tp>
    explicit operator Tp&& () &&
    {
        return std::move(this->ref_cast<remove_const_ref_t<Tp>>());
    }
    template<typename Tp>
    explicit operator const Tp& () &&
    {
        return std::move(this->ref_cast<remove_const_ref_t<Tp>>());
    }

    template<typename Tp>
    explicit operator Tp () const &&
    {
        return std::move(this->ref_cast<remove_const_ref_t<Tp>>());
    }
    template<typename Tp>
    explicit operator Tp&& () const &&
    {
        return std::move(this->ref_cast<remove_const_ref_t<Tp>>());
    }
    template<typename Tp>
    explicit operator const Tp& () const &&
    {
        return std::move(this->ref_cast<remove_const_ref_t<Tp>>());
    }

    template<typename Tp>
    explicit operator Tp&& () & = delete;
    template<typename Tp>
    explicit operator Tp&& () const & = delete;
    template<typename Tp>
    explicit operator Tp& () && = delete;
    template<typename Tp>
    explicit operator Tp& () const && = delete;


    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    Tp& ref_cast() &
    {
        if (!this->is<Tp>()) throw std::bad_cast();
        return this->unguarded_cast<Tp>();
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp& ref_cast() const &
    {
        if (!this->is<Tp>()) throw std::bad_cast();
        return this->unguarded_cast<Tp>();
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    Tp&& ref_cast() &&
    {
        return std::move(this->ref_cast<Tp>());
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp&& ref_cast() const &&
    {
        return std::move(this->ref_cast<Tp>());
    }

    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    Tp* ptr_cast() & noexcept
    {
        if (!this->is<Tp>()) return nullptr;
        return static_cast<Tp*>(this->i_buffer.data());
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp* ptr_cast() const & noexcept
    {
        if (!this->is<Tp>()) return nullptr;
        return static_cast<const Tp*>(this->i_buffer.data());
    }

    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp* ptr_cast() const && noexcept = delete;



  private:
    using manager_type = const n_Any_helper::AnyTypeManagerStructure*;
    using buffer_type = n_Any_helper::AnyBuffer;

    bool empty() const noexcept
    {
        return !this->i_manager;
    }

    void clear() & noexcept
    {
        if (!this->empty()) {
          this->i_manager->Destructor(i_buffer.data());
          this->clear_manager();
        }
    }

    template<typename Tp>
    void set_manager() & noexcept
    {
        this->i_manager = &n_Any_helper::template AnyTypeManager<Tp>::manager;
    }
    void clear_manager() & noexcept
    {
        this->i_manager = {};
    }


    template<typename Tp, typename ...Args>
    void construct(Args&& ...args) noexcept(noexcept(Tp(std::forward<Args>(args)...)))
    {
        new (this->i_buffer.Get_buffer()) Tp(std::forward<Args>(args)...);
    }


    /// doesn't have strong exception safety guarantee
    template<typename Tp, typename ...Args>
    void unsafe_emplace(Args&& ...args)
    {
        this->clear();
        this->i_buffer.reset(ssizeof<Tp>());
        this->construct<Tp>(std::forward<Args>(args)...);
        this->set_manager<Tp>();
    }



    template<typename Tp>
    Tp& unguarded_cast() & noexcept
    {
        return *static_cast<Tp*>(i_buffer.data());
    }
    template<typename Tp>
    const Tp& unguarded_cast() const & noexcept
    {
        return *static_cast<const Tp*>(i_buffer.data());
    }



    buffer_type i_buffer;
    size_t i_current_size;
    manager_type i_manager;
};
const Any Any::null;




template<typename Tp>
inline Tp Any_cast(Any &cont)
{
    return static_cast<Tp>(cont);
}
template<typename Tp>
inline Tp Any_cast(const Any &cont)
{
    return static_cast<Tp>(cont);
}
template<typename Tp>
inline Tp Any_cast(Any &&cont)
{
    return static_cast<Tp>(std::move(cont));
}
template<typename Tp>
inline Tp Any_cast(const Any &&cont)
{
    return static_cast<Tp>(std::move(cont));
}
template<typename Tp>
inline Tp Any_cast(Any *cont) noexcept
{
    return cont->ptr_cast<remove_const_ptr_ref_t<Tp>>();
}
template<typename Tp>
inline Tp Any_cast(const Any *cont) noexcept
{
    return cont->ptr_cast<remove_const_ptr_ref_t<Tp>>();
}

} // namespace Achibulup
#endif // ACHIBULUP_ANY_H_INCLUDED
