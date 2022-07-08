#ifndef SINGLEALLOCATOR_HPP_INCLUDED
#define SINGLEALLOCATOR_HPP_INCLUDED

#include "LIFOMemoryResource.hpp"
#include <memory>

namespace Achibulup
{

class SingleAllocatorResource
{
  public:
    /// for each instance of this type, @a n and @a alignment have to be the same in every call to allocate()
    void* allocate(size_t n, size_t alignment = alignof(std::max_align_t))
    {
        if (n == 0) return {};
        if (!m_reuse.empty()) {
          void* result = this->m_reuse.back();
          this->m_reuse.pop_back();
          return result;
        }
        return this->m_resource.allocate(n, alignment);
    }

    void deallocate(void* ptr)
    {
        if (ptr) this->m_reuse.push_back(ptr);
    }
  
  private:
    LIFOMemoryResource m_resource;
    std::vector<void*> m_reuse;
};

class SingleDeallocator
{
  public:
    constexpr SingleDeallocator(SingleAllocatorResource *resource = {}) noexcept
    : m_resource(resource) {}

    void operator()(void* ptr) noexcept
    {
        if (this->m_resource)
          this->m_resource->deallocate(ptr);
    }

    explicit constexpr operator bool () const noexcept
    {
        return this->m_resource;
    }

  private:
    SingleAllocatorResource *m_resource;
};

template<typename Tp>
class SingleAllocator
{
  public:
    using value_type = Tp;
    using is_always_equal = std::true_type;

    static Tp* allocate(std::size_t n)
    {
        if (n > 1) throw std::runtime_error("SingleAllocator can only allocate one object at a time");
        if (n == 0) return {};
        return allocate();
    }

    static void deallocate(void* ptr, std::size_t n)
    {
        if (n > 1) throw std::runtime_error("SingleAllocator can only deallocate one object at a time");
        if (n == 0) return;
        deallocate(ptr);
    }

    static Tp* allocate()
    {
        return static_cast<Tp*>(
            s_sharedResource.allocate(sizeof(Tp), alignof(Tp)));
    }

    static void deallocate(Tp* ptr)
    {
        s_sharedResource.deallocate(ptr);
    }

    static SingleDeallocator getDeallocator() noexcept
    {
        return SingleDeallocator(&s_sharedResource);
    }

  private:
    static SingleAllocatorResource s_sharedResource;
};
template<typename Tp>
SingleAllocatorResource SingleAllocator<Tp>::s_sharedResource;

class SADeleter
{
  public:
    constexpr SADeleter() noexcept : m_derived(), m_deal() {}
    explicit constexpr SADeleter(std::nullptr_t) noexcept : SADeleter() {}

    template<typename Tp>
    explicit constexpr SADeleter(Tp *ptr) noexcept 
    : m_derived(ptr), m_deal(SingleAllocator<Tp>::getDeallocator()) {}

    ACHIBULUP__constexpr_fun14 SADeleter(SADeleter &&other) noexcept
    : m_derived(other.m_derived), m_deal(other.m_deal)
    {
        other.m_derived = {};
        other.m_deal = {};
    }

    ACHIBULUP__constexpr_fun14 SADeleter& 
    operator = (SADeleter &&other) noexcept
    {
        if (this != &other) {
          this->m_derived = other.m_derived;
          this->m_deal = other.m_deal;
          other.m_derived = {};
          other.m_deal = {};
        }
        return *this;
    }

    void swap(SADeleter &other) noexcept
    {
        using std::swap;
        swap(this->m_derived, other.m_derived);
        swap(this->m_deal, other.m_deal);
    }

    void operator () (const void*) noexcept
    {
        if (this->m_deal) {
          this->m_deal(this->m_derived);
          this->m_derived = {};
          this->m_deal = {};
        }
    }

    void* getPtr() const noexcept
    {
        return this->m_derived;
    }

  private:
    void *m_derived;
    SingleDeallocator m_deal;
};




template<typename Tp>
using SAUniquePtr = std::unique_ptr<Tp, SADeleter>;

template<typename Tp, typename ...Args>
SAUniquePtr<Tp> makeSAUnique(Args&& ...args)
{
    Tp *result_ptr = new (SingleAllocator<Tp>::allocate()) 
                         Tp(std::forward<Args>(args)...);
    return SAUniquePtr<Tp>(result_ptr, SADeleter(result_ptr));
}

} // namespace Achibulup

#endif // SINGLEALLOCATOR_HPP_INCLUDED