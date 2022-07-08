#ifndef PMIterator_HPP_INCLUDED
#define PMIterator_HPP_INCLUDED

#include "Range.hpp"
#include <SFML/Achibulup_dependencies/SingleAllocator.hpp>
#include <iterator>
#include <type_traits>

namespace Achibulup
{

template<typename Tp>
class IInputIterator;
template<typename Tp>
class IOutputIterator;
template<typename Tp>
class IForwardIterator;
template<typename Tp>
class IBidirectionalIterator;
template<typename Tp>
class IRandomAccessIterator;

template<typename Tp>
class IInputIterator
{
  public:
    using iterator_category = std::input_iterator_tag;
    class End{};

    IInputIterator(bool is_end = false) noexcept : m_isEnd(is_end) {}

    virtual ~IInputIterator() = default;

    Tp operator * ()
    {
        return this->yield();
    }

    bool operator == (End) const noexcept
    {
        return this->isEnd();
    }
    bool operator != (End) const noexcept
    {
        return !(*this == End());
    }
  
    bool isEnd() const noexcept
    {
        return this->m_isEnd;
    }

    /// optain the value and advance the state of iterator, including updating the isEnd field
    virtual Tp yield() = 0;
    
  protected:
    void setIsEnd(bool is_end) noexcept
    {
        this->m_isEnd = is_end;
    }

    void markAsEnd() noexcept
    {
        this->setIsEnd(true);
    }

  private:
    bool m_isEnd;
};

template<typename Tp>
class IInputIterator<Tp&>
{
  public:
    using iterator_category = std::forward_iterator_tag;
    class End{};

    virtual ~IInputIterator() = default;

    Tp& operator * ()
    {
        return this->yield();
    }
    Tp* operator -> ()
    {
        return &this->yield();
    }

    bool operator == (End) const noexcept
    {
        return this->isEnd();
    }
    bool operator != (End) const noexcept
    {
        return !(*this == nullptr);
    }

    bool isEnd() const noexcept
    {
        return this->m_isEnd;
    }

    /// optain the value and advance the state of iterator, including updating the isEnd field
    virtual Tp& yield() = 0;
    
  protected:
    void setIsEnd(bool is_end) noexcept
    {
        this->m_isEnd = is_end;
    }

    void markAsEnd() noexcept
    {
        this->setIsEnd(true);
    }

  private:
    bool m_isEnd;
};


template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IInputIterator>::value> = 0>
Iter& operator ++ (Iter& iter)
{
    return iter;
}
template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IInputIterator>::value> = 0>
Iter operator ++ (Iter& iter, int)
{
    auto copy = iter;
    ++iter;
    return copy;
}

/// Polymorphic input iterator for range-based for loops
/// uses a mix of polymorphism and type erasure
template<typename Tp>
class PMIIterator
{
    using Iter = IInputIterator<Tp>;
    struct Data
    {
        SAUniquePtr<Iter> iter;
        void* getMostDerived() const noexcept
        {
            if (*this) return this->iter.get_deleter().getPtr();
            return {};
        }
        explicit operator bool () const noexcept
        {
            return !!this->iter;
        }
    };
    using Cloner = Data(*)(const void*);
    // template<typename Arg>
    // class Emplace
    // {
    //   public:
    //     Emplace(const Arg &arg) : m_arg(arg) {}
    //     operator Arg () const noexcept { return this->m_arg; }

    //   private:
    //     const Arg &m_arg;
    // }

  public:
    using iterator_category = typename Iter::iterator_category;
    using End = typename Iter::End;

    PMIIterator() noexcept = default;

    template<typename Derived>
    PMIIterator(Derived &&iter) noexcept
    : PMIIterator(makeSAUnique<std::decay_t<Derived>>(
          std::forward<Derived>(iter))) {}

    template<typename Derived>
    explicit PMIIterator(SAUniquePtr<Derived> &&iter) noexcept
    : m_data{std::move(iter)}, 
      m_cloner([](const void *ptr) -> Data
          {
              return {makeSAUnique<Derived>(*static_cast<const Derived*>(ptr))};
          }) {}

    PMIIterator(const PMIIterator &other)
    : m_data(other.clone()), m_cloner(other.m_cloner) {}

    PMIIterator(PMIIterator &&other) noexcept
    : m_data(std::move(other.m_data)),
      m_cloner(std::move(other.m_cloner)) 
    {
        other.m_cloner = {};
    }

    PMIIterator& operator = (PMIIterator other) &
    {
        this->swap(other);
        return *this;
    }

    void swap(PMIIterator &other) noexcept
    {
        using std::swap;
        swap(m_data, other.m_data);
        swap(m_cloner, other.m_cloner);
    }
    
    Tp operator * ()
    {
        return **this->m_data.iter;
    }

    PMIIterator& operator ++ () &
    {
        ++*this->m_data.iter;
        return *this;
    }
    [[deprecated]]
    PMIIterator operator ++ (int) &
    {
        auto copy = *this;
        ++*this;
        return copy;
    }

    bool isEnd() const noexcept
    {
        return !this->m_data.iter || this->m_data.iter->isEnd();
    }

    bool operator == (End) const noexcept
    {
        return this->isEnd();
    }

    bool operator != (End) const noexcept
    {
        return !(*this == End());
    }

    explicit operator bool () const noexcept
    {
        return static_cast<bool>(this->m_data);
    }

  private:
    Data clone() const
    {
        if (!*this) return {};
        return this->m_cloner(this->m_data.getMostDerived());
    }

    Data m_data = {};
    Cloner m_cloner = {};
};

template<typename Elem>
using PMRange = IterRange<PMIIterator<Elem>>;

} // namespace Achibulup

#endif // PMIterator_HPP_INCLUDED