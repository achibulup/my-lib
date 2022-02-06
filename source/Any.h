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

using CopyCtor = void(*)(RelaxedPtr<void>, RelaxedPtr<const void>);
using Destructor = void(*)(RelaxedPtr<const void>);
using PointerThrow = void(*)(RelaxedPtr<void>);
using StringFormatter = void(*)(std::ostream&, RelaxedPtr<const void>);
using EqualComparator = bool(*)(RelaxedPtr<const void>, RelaxedPtr<const void>);




template<typename Tp, 
    EnableIf_t<std::is_convertible<const Tp&, Tp>::value>* = nullptr>
constexpr CopyCtor getCopyCtorHelper(int) noexcept
{
    return +[](RelaxedPtr<void> dest, RelaxedPtr<const void> source) {
        new (dest) Tp(*static_cast<const Tp*>(source));
    };
}
template<typename Tp, typename Tp2>
constexpr CopyCtor getCopyCtorHelper(Tp2) noexcept
{
    return nullptr;
}
template<typename Tp>
constexpr CopyCtor getCopyCtor() noexcept
{
    return getCopyCtorHelper<Tp>(0);
}

template<typename Tp>
constexpr Destructor getDestructor() noexcept
{
    return +[](RelaxedPtr<const void> ptr) {
        using Type = Tp;
        static_cast<const Type*>(ptr)->~Type();
    };
}

template<typename Tp>
constexpr PointerThrow getPointerThrow() noexcept
{
    return +[](RelaxedPtr<void> ptr) {
        throw static_cast<Tp*>(ptr);
    };
}

template<typename Tp, 
    typename = decltype(std::declval<const Tp&>() == std::declval<const Tp&>())>
constexpr EqualComparator getEqualComparatorHelper(int) noexcept
{
    return +[](RelaxedPtr<const void> lhs, RelaxedPtr<const void> rhs) -> bool {
        return *static_cast<const Tp*>(lhs) == *static_cast<const Tp*>(rhs);
    };
}
template<typename Tp, typename Tp2>
constexpr EqualComparator getEqualComparatorHelper(Tp2) noexcept
{
    return nullptr;
}
template<typename Tp>
constexpr EqualComparator getEqualComparator() noexcept
{
    return getEqualComparatorHelper<Tp>(0);
}

template<typename Tp, typename = decltype(
            std::declval<std::ostream&>() << std::declval<const Tp&>())>
constexpr StringFormatter getStringFormatterHelper(int) noexcept
{
    return +[](std::ostream &os, RelaxedPtr<const void> obj) {
        os << *static_cast<const Tp*>(obj);
    };
}
template<typename Tp, typename Tp2>
constexpr StringFormatter getStringFormatterHelper(Tp2) noexcept
{
    return +[](std::ostream &os, RelaxedPtr<const void>) {
        os << typeid(Tp).name();
    };
}
template<typename Tp>
constexpr StringFormatter getStringFormatter() noexcept
{
    return getStringFormatterHelper<Tp>(0);
}



struct AnyTypeManagerStructure
{
    size_t size;
    CopyCtor copyCtor;
    Destructor destruct;
    const std::type_info *typeinfo;
    PointerThrow throwPointer;
    EqualComparator equalCompare;
    StringFormatter formatTo;
};
template<typename Tp>
struct AnyTypeManager
{
    static constexpr AnyTypeManagerStructure manager{
        ssizeof<Tp>(),
        getCopyCtor<Tp>(),
        getDestructor<Tp>(),
        &typeid(Tp),
        getPointerThrow<Tp>(),
        getEqualComparator<Tp>(),
        getStringFormatter<Tp>()
    };
};
template<typename Tp>
const AnyTypeManagerStructure AnyTypeManager<Tp>::manager;



} //namespace n_Any_helper


class Any
{
  public:
    Any() noexcept : m_buffer(), m_manager() {}

    template<typename Tp0,
        EnableIf_t<!std::is_same<decay_t<Tp0>, Any>::value>* = nullptr>
    Any(Tp0 &&val) : Any()
    {
        using Tp = decay_t<Tp0>;
        this->unsafe_emplace<Tp>(std::forward<Tp0>(val));
    }
    Any(const Any &cpy) : m_buffer(cpy.m_manager ? cpy.m_manager->size : 0), 
                          m_manager(cpy.m_manager)
    {
        if (cpy.m_manager){
          if (!cpy.m_manager->copyCtor) throw bad_copy();
          cpy.m_manager->copyCtor(this->m_buffer.buffer(), 
                                  cpy.m_buffer.buffer());
        }
    }

    Any(Any &&mov) noexcept
    : m_buffer(Move(mov.m_buffer)), m_manager(Move(mov.m_manager)) {}


    Any& operator = (const Any &cpy) &
    {
        Any tmp(cpy);
        swap(*this, tmp);
        return *this;
    }

    Any& operator = (Any &&mov) & noexcept
    {
        this->reset();
        if (!mov.empty())
          swap(*this, mov);
        return *this;
    }
    
    ~Any() noexcept
    {
        this->reset();
    }
    
    friend void swap(Any &a, Any &b) noexcept
    {
        using std::swap;
        swap(a.m_buffer, b.m_buffer);
        swap(a.m_manager, b.m_manager);
    }


    template<typename Tp, 
            bool variadic = true,
            EnableIf_t<std::is_same<Tp, decay_t<Tp>>::value>* = nullptr,
            EnableIf_t<!std::is_same<Tp, Any>::value>* = nullptr, 
            typename ...Args>
    void emplace(Args&& ...args) &
    {
        if (this->m_buffer.size() >= ssizeof<Tp>() 
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
         && noexcept(*(this->unguardedCastPtr<Tp>()) = std::forward<Tp0>(val)) )
          *(this->unguardedCastPtr<Tp>()) = std::forward<Tp0>(val);
        else this->emplace<Tp>(std::forward<Tp0>(val));
        return *(this->unguardedCastPtr<Tp>());
    }

    
    static const Any null;

    
    const std::type_info& type() const noexcept
    {
        if (this->empty()) return typeid(void);
        return *(this->m_manager->typeinfo);
    }

    
    template<typename Tp, 
        EnableIf_t<std::is_same<Tp, decay_t<Tp>>::value>* = nullptr>
    bool isSame() const noexcept
    {
        return std::is_same<Tp, Any>::value
        || this->m_manager == &n_Any_helper::AnyTypeManager<Tp>::manager;
    }
    template<typename Tp, 
        EnableIf_t<std::is_same<Tp, decay_t<Tp>>::value>* = nullptr>
    bool is() const noexcept
    {
        if ((*this).isSame<Tp>()) return true;
        return static_cast<bool>(tryCastPtr<Tp>());
    }


    
    template<typename Tp>
    Tp cast() &
    {
        return this->refCast<remove_const_ref_t<Tp>>();
    }
    template<typename Tp>
    Tp cast() const &
    {
        return this->refCast<remove_const_ref_t<Tp>>();
    }
    template<typename Tp>
    Tp cast() &&
    {
        return std::move(this->refCast<remove_const_ref_t<Tp>>());
    }
    template<typename Tp>
    Tp cast() const &&
    {
        return std::move(this->refCast<remove_const_ref_t<Tp>>());
    }
    // template<typename Tp>
    // friend Tp AnyCast(Any *any) noexcept
    // {
    //     return any->ptrCast<remove_const_ptr_ref_t<Tp>>();
    // }
    // template<typename Tp>
    // friend Tp AnyCast(const Any *any) noexcept
    // {
    //     return any->ptrCast<remove_const_ptr_ref_t<Tp>>();
    // }


    template<typename Tp>
    explicit operator Tp& () &
    {
        return this->cast<Tp&>();
    }
    template<typename Tp>
    explicit operator const Tp& () const &
    {
        return this->cast<const Tp&>();
    }
    template<typename Tp>
    explicit operator Tp&& () &&
    {
        return std::move(this->cast<Tp&>());
    }
    template<typename Tp>
    explicit operator const Tp&& () const &&
    {
        return std::move(this->cast<const Tp&>());
    }


    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    Tp& refCast() &
    {
        Tp *ptr = this->ptrCast<Tp>();
        if (!ptr) throw std::bad_cast();
        return *ptr;
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp& refCast() const &
    {
        const Tp *ptr = this->ptrCast<Tp>();
        if (!ptr) throw std::bad_cast();
        return *ptr;
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    Tp&& refCast() &&
    {
        return std::move(this->refCast<Tp>());
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp&& refCast() const &&
    {
        return std::move(this->refCast<Tp>());
    }

    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    Tp* ptrCast() & noexcept
    {
        if (this->isSame<Tp>()) return unguardedCastPtr<Tp>();
        return tryCastPtr<Tp>();
    }
    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp* ptrCast() const & noexcept
    {
        if (this->isSame<Tp>()) return unguardedCastPtr<Tp>();
        return tryCastPtr<Tp>();
    }

    template<typename Tp, EnableIf_t<std::is_same<Tp, remove_const_ref_t<Tp>>::value>* = nullptr>
    const Tp* ptrCast() const && noexcept = delete;




    friend bool operator == (const Any &l, const Any &r)
    {
        if (l.m_manager != r.m_manager) return false;
        if (l.empty()) return true;
        if (!l.m_manager->equalCompare) throw bad_compare();
        return l.m_manager->equalCompare(l.m_buffer.data(), r.m_buffer.data());
    }
    friend bool operator != (const Any &l, const Any &r)
    {
        return !(l == r);
    }


    friend std::ostream& operator << (std::ostream &os, const Any &any)
    {
        if (!any.empty()) 
          any.m_manager->formatTo(os, any.m_buffer.data());
        return os;
    }

    friend std::string to_string(const Any &any)
    {
        std::ostringstream format;
        format << any;
        return format.str();
    } 

  private:
    using manager_type = const n_Any_helper::AnyTypeManagerStructure*;
    using buffer_type = Buffer;

    template<typename Tp>
    Tp* tryCastPtr() noexcept
    {
        if (*this == Any::null) return nullptr;
        try {
          this->m_manager->throwPointer(m_buffer.data());
        }
        catch (Tp *success) {
          return success;
        }
        catch (...) {}
        return nullptr;
    }
    template<typename Tp>
    const Tp* tryCastPtr() const noexcept
    {
        if (*this == Any::null) return nullptr;
        try {
          this->m_manager->throwPointer(m_buffer.data());
        }
        catch (const Tp *success) {
          return success;
        }
        catch (...) {}
        return nullptr;
    }

    

    template<typename Tp>
    Tp* unguardedCastPtr() noexcept
    {
        return static_cast<Tp*>(m_buffer.data());
    }
    template<typename Tp>
    const Tp* unguardedCastPtr() const noexcept
    {
        return static_cast<const Tp*>(m_buffer.data());
    }

    bool empty() const noexcept
    {
        return !this->m_manager;
    }

    void reset() & noexcept
    {
        if (!this->empty()) {
          this->m_manager->destruct(m_buffer.data());
          this->clear_manager();
        }
    }

    template<typename Tp>
    void set_manager() & noexcept
    {
        this->m_manager = &n_Any_helper::template AnyTypeManager<Tp>::manager;
    }
    void clear_manager() & noexcept
    {
        this->m_manager = {};
    }


    template<typename Tp, typename ...Args>
    void construct(Args&& ...args) noexcept(noexcept(Tp(std::forward<Args>(args)...)))
    {
        new (this->m_buffer.buffer()) Tp(std::forward<Args>(args)...);
    }


    /// doesn't have strong exception safety guarantee
    template<typename Tp, typename ...Args>
    void unsafe_emplace(Args&& ...args)
    {
        this->reset();
        this->m_buffer.reset(ssizeof<Tp>());
        this->construct<Tp>(std::forward<Args>(args)...);
        this->set_manager<Tp>();
    }





    buffer_type m_buffer;
    manager_type m_manager;
};
const Any Any::null;







} // namespace Achibulup
#endif // ACHIBULUP_ANY_H_INCLUDED
