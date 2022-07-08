#ifndef POLYMORPHIC_STACK_HPP_INCLUDED
#define POLYMORPHIC_STACK_HPP_INCLUDED
#include "LIFOMemoryResource.hpp"
#include <type_traits>

namespace Achibulup 
{

/// \brief A stack whose elements are polymorphic subclasses of \a Base
/// used for game state management and stuffs
template<typename Base>
class PolymorphicStack
{
  public:
    using value_type = Base;
    using size_type = size_t;

    PolymorphicStack() noexcept = default;
    PolymorphicStack(const PolymorphicStack&) = delete;
    PolymorphicStack(PolymorphicStack &&other) noexcept
    : PolymorphicStack()
    {
        this->swap(other);
    }
    void operator = (const PolymorphicStack&) = delete;
    PolymorphicStack& operator = (PolymorphicStack &&other) noexcept
    {
        if (this != &other) {
          this->swap(other);
          other.clear();
        }
        return *this;
    }
    ~PolymorphicStack()
    {
        this->clear();
    }

    void swap(PolymorphicStack& other) noexcept
    {
        using std::swap;
        swap(this->m_host, other.m_host);
        swap(this->m_stack, other.m_stack);
    }

    size_type size() const noexcept
    {
        return this->m_stack.size();
    }

    bool empty() const noexcept
    {
        return this->m_stack.empty();
    } 

    Base* push(const Base &element)
    {
        return this->pushBack(element);
    }
    Base* push(Base &&element)
    {
        return this->pushBack(std::move(element));
    }

    template<typename Tp, typename Elem = std::decay_t<Tp>>
    Elem* push(Tp &&element)
    {
        return this->pushBack(static_cast<Tp&&>(element));
    }
    template<typename Elem = Base, typename ...Args>
    Elem* emplace(Args&& ...args)
    {
        return this->emplaceBack<Elem>(static_cast<Args&&>(args)...);
    }

    void pop()
    {
        this->popBack();
    }

    Base& top()
    {
        return this->back();
    }
    const Base& top() const
    {
        return this->back();
    }

    Base* pushBack(const Base &element)
    {
        return this->emplaceBack(element);
    }
    Base* pushBack(Base &&element)
    {
        return this->emplaceBack(std::move(element));
    }

    template<typename Tp, typename Elem = std::decay_t<Tp>>
    Elem* pushBack(Tp &&element)
    {
        return this->emplaceBack<Elem>(std::forward<Tp>(element));
    }
    template<typename Elem = Base, typename ...Args>
    Elem* emplaceBack(Args&& ...args)
    {
        static_assert(std::is_convertible<Elem*, Base*>(),
                      "element must be a subclass of Base");
        static_assert(std::is_same<Elem, Base>() 
                   || std::has_virtual_destructor<Base>(),
                      "Base must have virtual destructor to have polymorphic elements");
        void *mem = this->m_host.allocate(sizeof(Elem), alignof(Elem));
        Elem *ptr = new (mem) Elem(static_cast<Args&&>(args)...);
        this->m_stack.push_back(ptr);
        return ptr;
    }
    
    void popBack()
    {
        Achibulup::destroy(this->m_stack.back());
        this->m_host.deallocateLast();
        this->m_stack.pop_back();
    }

    Base& operator [] (size_type index)
    {
        return *this->m_stack[index];
    }
    const Base& operator [] (size_type index) const
    {
        return *this->m_stack[index];
    }

    Base& at(size_type index)
    {
        return *this->m_stack.at(index);
    }
    const Base& at(size_type index) const
    {
        return *this->m_stack.at(index);
    }

    Base& back()
    { 
        return *this->m_stack.back();
    }
    const Base& back() const
    {
        return *this->m_stack.back();
    }

    void clear() noexcept
    {
        while (!this->empty())
          this->pop();
    }

    void shrinkToFit() noexcept
    {
        this->m_host.shrinkToFit();
        this->m_stack.shrink_to_fit();
    }


  private:
    LIFOMemoryResource m_host;
    std::vector<Base*> m_stack;
};

} // namespace Achibulup


#endif // POLYMORPHIC_STACK_HPP_INCLUDED  