#ifndef REF_H_INCLUDED
#define REF_H_INCLUDED
#include "common_utils.h"
#include <type_traits>
#include <memory>


///simulate variable reference semantics like in java or c#
///similar to std::shared_ptr
///constructor arguments are forwarded to the constructor of the referenced object

namespace Achibulup
{

namespace n_Ref
{


class ErasedRefInstance
{
  protected:
    ErasedRefInstance() : m_ref_count(0) {}
    virtual ~ErasedRefInstance() = default;

  public:
    void incRef() 
    {
        ++this->m_ref_count;
    }
    void decRef() 
    {
        --this->m_ref_count;
        if (this->m_ref_count == 0)
          delete this;
    }

    friend void deleteRefInstance(const ErasedRefInstance*);
    friend void destroyRefInstance(const ErasedRefInstance*);

    size_t m_ref_count;
};


void deleteRefInstance(const ErasedRefInstance *instance)
{
    delete instance;
}
void destroyRefInstance(const ErasedRefInstance *instance)
{
    instance->~ErasedRefInstance();
}


template<typename Tp>
class RefPtr;

template<typename Tp>
class RefInstance : public ErasedRefInstance
{
  public:
    template<typename ...Args>
#if ACHIBULUP__Cpp17_later
    [[nodiscard]]
#endif
    RefInstance(Args&& ...args)
    : ErasedRefInstance(), m_value(std::forward<Args>(args)...){}

    Tp* get() noexcept
    {
        return std::addressof(this->m_value);
    }

  private:
    ~RefInstance() override = default;

    Tp m_value;
};


template<typename Tp>
class RefPtr
{
  public:
    constexpr RefPtr() noexcept : m_value_ptr(), m_instance_ptr() {}

    RefPtr(const RefPtr &other) noexcept
    : RefPtr(other.m_value_ptr, other.m_instance_ptr) {}
    RefPtr(RefPtr &&other) noexcept
    : RefPtr() 
    { 
        swap(*this, other); 
    }

    RefPtr& operator = (RefPtr other) & noexcept
    {
        swap(*this, other);
        return *this;
    }

    ~RefPtr()
    {
        if (*this) this->m_instance_ptr->decRef();
    }

    friend void swap(RefPtr &a, RefPtr &b) noexcept
    {
        using std::swap;
        swap(a.m_value_ptr, b.m_value_ptr);
        swap(a.m_instance_ptr, b.m_instance_ptr);
    }

    RefPtr(RefInstance<Tp> *instance) noexcept
    : RefPtr(instance->get(), instance) {}

    template<typename Up, EnableIf_t<std::is_convertible<Up*, Tp*>::value> = 0>
    RefPtr(const RefPtr<Up> &other) noexcept
    : RefPtr(other.get(), other.manager()) {}

    Tp* get() const noexcept
    {
        return this->m_value_ptr;
    }

    ErasedRefInstance* manager() const noexcept
    {
        return this->m_instance_ptr;
    }

    constexpr explicit operator bool () const noexcept
    {
        return this->m_value_ptr;
    }

    friend constexpr 
    bool operator == (const RefPtr &lhs, const RefPtr &rhs) noexcept
    {
        return lhs.m_value_ptr == rhs.m_value_ptr;
    }
    friend constexpr 
    bool operator != (const RefPtr &lhs, const RefPtr &rhs) noexcept
    {
        return !(lhs == rhs);
    }

  private:
    RefPtr(Tp *value, ErasedRefInstance *instance) noexcept
    : m_value_ptr(value), m_instance_ptr(instance) 
    {
        if (this->m_instance_ptr)
          this->m_instance_ptr->incRef();
    }


    Tp *m_value_ptr;
    ErasedRefInstance *m_instance_ptr;
};


template<typename Tp, typename ...Args>
RefPtr<Tp> makeInstance(Args&& ...args)
{
    return new RefInstance<Tp>(std::forward<Args>(args)...);
}


template<typename Arr, typename ...Args>
auto subHelper(Arr &&arr, Args&& ...args)
noexcept(noexcept(std::declval<Arr>().operator[](std::declval<Args>()...)))
-> decltype(std::declval<Arr>().operator[](std::declval<Args>()...))
{
    return std::forward<Arr>(arr).operator[](std::forward<Args>(args)...);
}
template<typename Tp, typename Arg>
Tp& subHelper(Tp *arr, Arg &&arg) noexcept
{
    return arr[arg];
}

}

struct NullRef
{
    friend constexpr bool operator == (NullRef, NullRef) {return true;}
    friend constexpr bool operator != (NullRef, NullRef) {return false;}
};
constexpr NullRef null_ref = {};


template<typename Tp>
class Ref;

template<typename>
struct is_Ref : std::integral_constant<bool, false> {};
template<typename Tp>
struct is_Ref<Ref<Tp>> : std::integral_constant<bool, true> {};
template<typename Tp>
struct is_Ref<Tp&> : is_Ref<Tp> {};
template<typename Tp>
struct is_Ref<Tp&&> : is_Ref<Tp> {};
template<typename Tp>
struct is_Ref<const Tp> : is_Ref<Tp> {};


template<typename Tp>
class Ref
{
  public:
    constexpr Ref(NullRef) noexcept : m_shared() {}

    Ref(const Ref &other) noexcept = default;
    Ref(Ref &&other) noexcept = default;
    
    Ref& operator = (const Ref &other) & = default;
    Ref& operator = (Ref &&other) & = default;

    friend void swap(Ref &a, Ref &b) noexcept
    {
        using std::swap;
        swap(a.m_shared, b.m_shared);
    }

    template<typename ...Args, EnableIf_t<
            (sizeof...(Args) != 1 || !is_Ref<Args...>::value)
         && std::is_constructible<Tp, Args...>::value> = 0>
    explicit Ref(Args&& ...args) 
    : m_shared(n_Ref::makeInstance<Tp>(std::forward<Args>(args)...)) {}
    explicit Ref(const Tp &value)
    : m_shared(n_Ref::makeInstance<Tp>(value)) {}
    explicit Ref(Tp &&value) 
    : m_shared(n_Ref::makeInstance<Tp>(std::move(value))) {}

    template<typename Up, EnableIf_t<std::is_convertible<Up*, Tp*>::value> = 0>
    Ref(const Ref<Up> &other) noexcept : m_shared(other.ptr()) {}

    Tp& instance() const noexcept
    {
        return *m_shared.get();
    }

    Tp* operator -> () const noexcept
    {
        return m_shared.get();
    }

    template<typename ...Args>
    auto operator () (Args&& ...args) const
    noexcept(noexcept(this->instance()(std::forward<Args>(args)...)))
    -> decltype(this->instance()(std::forward<Args>(args)...))
    {
        return this->instance()(std::forward<Args>(args)...);
    }

    template<typename ...Args>
    auto operator [] (Args&& ...args) const noexcept(noexcept(
        n_Ref::subHelper(this->instance(), std::declval<Args>()...)))
    -> decltype(n_Ref::subHelper(this->instance(), std::declval<Args>()...))
    {
        return n_Ref::subHelper(this->instance(), std::forward<Args>(args)...);
    }

    constexpr bool isNull() const noexcept
    {
        return !this->m_shared;
    }

    friend constexpr bool operator == (const Ref &lhs, const Ref &rhs) noexcept
    {
        return lhs.m_shared == rhs.m_shared;
    }
    friend constexpr bool operator != (const Ref &lhs, const Ref &rhs) noexcept
    {
        return lhs.m_shared != rhs.m_shared;
    }
    friend constexpr bool operator == (const Ref &lhs, NullRef) noexcept
    {
        return lhs.isNull();
    }
    friend constexpr bool operator == (NullRef, const Ref &rhs) noexcept
    {
        return rhs == null_ref;
    }
    friend constexpr bool operator != (const Ref &lhs, NullRef) noexcept
    {
        return !(lhs == null_ref);
    }
    friend constexpr bool operator != (NullRef, const Ref &rhs) noexcept
    {
        return rhs != null_ref;
    }
  
  private:
    using StoredType = typename std::remove_cv<Tp>::type;
  
  public:
    const n_Ref::RefPtr<StoredType>& ptr() const noexcept
    {
        return m_shared;
    }

  private:
    n_Ref::RefPtr<StoredType> m_shared;
};
}

#endif // REF_H_INCLUDED