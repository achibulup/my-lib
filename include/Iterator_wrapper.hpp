#ifndef ITERATOR_WRAPPER_HPP_INCLUDED
#define ITERATOR_WRAPPER_HPP_INCLUDED
#include <iterator>
#include "common_utils.hpp"
namespace Achibulup
{
namespace n_Iterator
{
struct custom_iterator_tag{};
template<typename Iter>
typename std::iterator_traits<Iter>::iterator_category getIterCat(int);
template<typename Iter>
custom_iterator_tag getIterCat(...);
template<typename Iter>
typename std::iterator_traits<Iter>::reference getReference(int);
template<typename Iter>
void getReference(...);
template<typename Iter>
typename std::iterator_traits<Iter>::pointer getPointer(int);
template<typename Iter>
void* getPointer(...);
template<typename Iter>
typename std::iterator_traits<Iter>::difference_type getDiff(int);
template<typename Iter>
int getDiff(...);
template<typename Iter>
typename std::iterator_traits<Iter>::value_type getValue(int);
template<typename Iter>
void getValue(...);

template<typename Iter>
struct IterTraits
{
    using iterator_category       =   decltype(getIterCat<Iter>(0));
    using value_type              =   decltype(getValue<Iter>(0));
    using reference               =   decltype(getReference<Iter>(0));
    using pointer                 =   decltype(getPointer<Iter>(0));
    using difference_type         =   decltype(getDiff<Iter>(0));
};
} // namespace n_Iterator


///a template for writing iterator wrappers
///used for writing an iterator wrapper class that behave like the underlying iterator
///you can inherit from this template, using the curiously recurring template pattern
///then this class will behave like the underlying iterator
///and you can override any behavior you want, creating iterator adapters

///usage example: assume you want to write a move iterator adapter for std::vector<int>::iterator
///class MoveIterator : public IteratorWrapper<MoveIterator, std::vector<int>::iterator>
///{
///    using Base = IteratorWrapper<MoveIterator, std::vector<int>::iterator>;
///    make your own constructor
///    MoveIterator(std::vector<int>::iterator iter) : Base(iter) {}
///    override the dereference operator
///    int&& operator*() const noexcept 
///    {
///        return std::move(Base::operator*()); 
///    }
///    ...
///    all other behaviors are inherited from the underlying iterator
///};


template<typename Iter, typename Base>
class IteratorWrapper
{
  public:
    using iterator_category       =   typename n_Iterator::IterTraits<Base>::iterator_category;
    using value_type              =   typename n_Iterator::IterTraits<Base>::value_type;
    using reference               =   typename n_Iterator::IterTraits<Base>::reference;
    using pointer                 =   typename n_Iterator::IterTraits<Base>::pointer;
    using difference_type         =   typename n_Iterator::IterTraits<Base>::difference_type;
    using base_type               =   Base;

  protected:
    constexpr IteratorWrapper() noexcept(noexcept(Base())) = default;

    explicit constexpr IteratorWrapper(Base it) 
    noexcept(noexcept(Base{std::move(it)})) : m_base(std::move(it)) {}

    template<typename OtherIter, typename OtherBase, 
                EnableIf_t<std::is_convertible<OtherBase, Base>::value> = 0>
    explicit constexpr 
    IteratorWrapper(const IteratorWrapper<OtherIter, OtherBase> &cast) 
    noexcept(noexcept(Base{cast.base()})) : m_base(cast.base()) {}


  public:
    constexpr const base_type& base() const & noexcept
    {
        return this->m_base;
    }
    ACHIBULUP__constexpr_fun14 base_type&& base() && noexcept
    {
        return std::move(this->m_base);
    }

    ///input iter & forward iter requirements

    constexpr reference operator * () const noexcept
    {
        return *(this->m_base);
    }
    constexpr pointer operator -> () const noexcept
    {
        return this->m_base;
    }

    friend constexpr bool operator == (const IteratorWrapper &lhs, 
                                       const IteratorWrapper &rhs) noexcept
    { 
        return lhs.base() == rhs.base(); 
    }
    friend constexpr bool operator != (const IteratorWrapper &lhs, 
                                       const IteratorWrapper &rhs) noexcept
    { 
        return lhs.base() != rhs.base(); 
    }

    ACHIBULUP__constexpr_fun14 void advance() &
    noexcept(noexcept(++(this->m_base)))
    {
        ++(this->m_base);
    }
    
    ///bidirectional iter requirements  

    ACHIBULUP__constexpr_fun14 void recede() &
    noexcept(noexcept(--(this->m_base)))
    {
        --(this->m_base);
    }



    ///random access iter requirements
    constexpr reference operator [] (difference_type i) const
    noexcept(noexcept(this->m_base + i))
    {
        return *(*this + i);
    }

    friend bool constexpr operator > (const IteratorWrapper &lhs, 
                                      const IteratorWrapper &rhs) noexcept
    { 
        return lhs.base() > rhs.base(); 
    }
    friend constexpr bool operator >= (const IteratorWrapper &lhs, 
                                       const IteratorWrapper &rhs) noexcept
    { 
        return lhs.base() >= rhs.base();
    }
    friend constexpr bool operator < (const IteratorWrapper &lhs, 
                                      const IteratorWrapper &rhs) noexcept
    { 
        return lhs.base() < rhs.base(); 
    }
    friend constexpr bool operator <= (const IteratorWrapper &lhs, 
                                       const IteratorWrapper &rhs) noexcept
    { 
        return lhs.base() <= rhs.base();
    }

    ACHIBULUP__constexpr_fun14 void advance(difference_type dist) &
    noexcept(noexcept(this->m_base += dist))
    {
        this->m_base += dist;
    }
    ACHIBULUP__constexpr_fun14 void recede(difference_type dist) &
    noexcept(noexcept(this->m_base -= dist))
    {
        this->m_base -= dist;
    }

    friend constexpr difference_type 
    operator - (const IteratorWrapper &lhs, const IteratorWrapper &rhs)
    noexcept(noexcept(lhs.base() - rhs.base()))
    {
        return lhs.base() - rhs.base();
    }

  protected:

    Base m_base;
};

namespace n_Iterator
{
struct Fallback
{
    template<typename Tp>
    constexpr Fallback(const Tp*) noexcept {}
};
inline constexpr bool isIterHelper(Fallback) noexcept
{
    return false;
}
template<typename Iter, typename Base>
constexpr bool isIterHelper(IteratorWrapper<Iter, Base>*) noexcept
{
    return true;
}
template<typename Tp>
constexpr bool isIteratorWrapper() noexcept
{
    return isIterHelper(static_cast<Tp*>(nullptr));
}
} // namespace n_Iterator


template<typename Iter, 
         EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator ++ (Iter &iter)
noexcept(noexcept(iter.advance()))
{
    iter.advance();
    return iter;
}
template<typename Iter, 
        EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator ++ (Iter &iter, int)
noexcept(noexcept(Iter{++iter}))
{
    Iter store = iter;
    ++iter;
    return store;
}

template<typename Iter, 
        EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator -- (Iter &iter)
noexcept(noexcept(iter.recede()))
{
    iter.recede();
    return iter;
}
template<typename Iter, 
        EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator -- (Iter &iter, int)
noexcept(noexcept(Iter{--iter}))
{
    Iter store = iter;
    --iter;
    return store;
}

template<typename Iter, 
        EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& 
operator += (Iter &iter, typename Iter::difference_type dist) 
noexcept(noexcept(iter.advance(dist)))
{
    iter.advance(dist);
    return iter;
}
template<typename Iter, 
        EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& 
operator -= (Iter &iter, typename Iter::difference_type dist) 
noexcept(noexcept(iter.recede(dist)))
{
    iter.recede(dist);
    return iter;
}


template<typename Iter, 
        EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
constexpr Iter
operator + (Iter iter, typename Iter::difference_type dist)
noexcept(noexcept(Iter{std::move(iter += dist)}))
{
    iter += dist;
    return iter;
}
// template<typename Iter, 
//         EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
// constexpr Iter
// operator + (typename Iter::difference_type dist, Iter iter)
// noexcept(noexcept(iter))
// {
//     return iter + dist;
// }
template<typename Iter, 
        EnableIf_t<n_Iterator::isIteratorWrapper<Iter>()> = 0>
constexpr Iter
operator - (Iter iter, typename Iter::difference_type dist)
noexcept(noexcept(Iter{std::move(iter -= dist)}))
{
    iter -= dist;
    return iter;
}


/// a template for writing iterator wrapper for containers (usually wraps normal pointers)
/// all properties of the iterator are inherited from the base iterator
/// example: implementing std::vector<T>
/// template<typename T>
/// class vector
/// {
///     using pointer = T*; 
///     using iterator = BasicIterator<pointer, vector>;
///     using const_iterator = BasicIterator<const pointer, vector>;
///     ...
///};


template<typename Base, typename Container>
class BasicIterator : 
public IteratorWrapper<BasicIterator<Base, Container>, Base>
{
    using Impl = IteratorWrapper<BasicIterator<Base, Container>, Base>;

  public:
    constexpr BasicIterator() = default;
    constexpr explicit BasicIterator(Base it) 
    noexcept(noexcept(Base{std::move(it)})) : Impl(std::move(it)) {}


    template<typename OtherBase,
            EnableIf_t<std::is_convertible<OtherBase, Base>::value> = 0>
    constexpr BasicIterator(const BasicIterator<OtherBase, Container> &cast)
    noexcept(noexcept(BasicIterator{cast.base()})) 
    : BasicIterator(cast.base()) {}
};
template<typename Tp, typename Container>
using iterator_wrapper = BasicIterator<Tp, Container>;

template<typename BaseL, typename BaseR, typename Container>
constexpr bool operator == (const BasicIterator<BaseL, Container> &lhs, 
                            const BasicIterator<BaseR, Container> &rhs) noexcept
{ 
    return lhs.base() == rhs.base(); 
}
template<typename BaseL, typename BaseR, typename Container>
constexpr bool operator != (const BasicIterator<BaseL, Container> &lhs, 
                            const BasicIterator<BaseR, Container> &rhs) noexcept
{ 
    return lhs.base() != rhs.base(); 
}
template<typename BaseL, typename BaseR, typename Container>
constexpr bool operator < (const BasicIterator<BaseL, Container> &lhs, 
                            const BasicIterator<BaseR, Container> &rhs) noexcept
{ 
    return lhs.base() < rhs.base(); 
}
template<typename BaseL, typename BaseR, typename Container>
constexpr bool operator > (const BasicIterator<BaseL, Container> &lhs, 
                            const BasicIterator<BaseR, Container> &rhs) noexcept
{ 
    return lhs.base() > rhs.base(); 
}
template<typename BaseL, typename BaseR, typename Container>
constexpr bool operator <= (const BasicIterator<BaseL, Container> &lhs, 
                            const BasicIterator<BaseR, Container> &rhs) noexcept
{ 
    return lhs.base() <= rhs.base(); 
}
template<typename BaseL, typename BaseR, typename Container>
constexpr bool operator >= (const BasicIterator<BaseL, Container> &lhs, 
                            const BasicIterator<BaseR, Container> &rhs) noexcept
{ 
    return lhs.base() >= rhs.base(); 
}
template<typename BaseL, typename BaseR, typename Container,
        typename Diff = EnableIf_t<
            std::is_same<
                typename BasicIterator<BaseL, Container>::difference_type,
                typename BasicIterator<BaseR, Container>::difference_type
            >::value, 
            typename BasicIterator<BaseL, Container>::difference_type>>
constexpr Diff operator - (const BasicIterator<BaseL, Container> &lhs, 
                           const BasicIterator<BaseR, Container> &rhs)
noexcept(noexcept(Diff{lhs.base() - rhs.base()}))
{ 
    return lhs.base() - rhs.base(); 
}
}


#endif // ITERATOR_WRAPPER_HPP_INCLUDED
