#ifndef ITERATOR_WRAPPER_HPP_INCLUDED
#define ITERATOR_WRAPPER_HPP_INCLUDED


#include "common_utils.hpp"
#include <iterator>



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
///and you can override any behavior you want, creating iterator adapters

///usage example: assume you want to write a move iterator adapter for std::vector<int>::iterator
///class MoveIterator : public IteratorWrapper<std::vector<int>::iterator>
///{
///    using Base = IteratorWrapper<std::vector<int>::iterator>;
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


template<typename Base>
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

    explicit constexpr IteratorWrapper(Argument<Base> it) 
    noexcept(noexcept(Base(std::move(it)))) : m_base(std::move(it)) {}

    template<typename OtherBase, 
             EnableIf_t<std::is_convertible<OtherBase, Base>::value> = 0>
    explicit constexpr 
    IteratorWrapper(const IteratorWrapper<OtherBase> &cast) 
    noexcept(noexcept(Base(cast.base()))) : m_base(cast.base()) {}

    template<typename OtherBase, 
             EnableIf_t<std::is_convertible<OtherBase, Base>::value> = 0>
    explicit constexpr 
    IteratorWrapper(const IteratorWrapper<OtherBase> &&cast) 
    noexcept(noexcept(Base(std::move(cast).base()))) 
    : m_base(std::move(cast).base()) {}


  public:
    ACHIBULUP__constexpr_fun14 base_type& base() & noexcept
    {
        return this->m_base;
    }
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
    noexcept(noexcept(*(this->m_base + i)))
    {
        return *(this->m_base + i);
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

  protected:

    Base m_base;
};


template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
constexpr bool operator == (const Iter &lhs, const Iter &rhs) noexcept
{ 
    return lhs.base() == rhs.base(); 
}
template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
constexpr bool operator != (const Iter &lhs, const Iter &rhs) noexcept
{ 
    return lhs.base() != rhs.base(); 
}


template<typename Iter, 
         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator ++ (Iter &iter)
noexcept(noexcept(iter.advance()))
{
    iter.advance();
    return iter;
}
template<typename Iter, 
        EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator ++ (Iter &iter, int)
noexcept(noexcept(Iter{++iter}))
{
    Iter store = iter;
    ++iter;
    return store;
}

template<typename Iter, 
        EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator -- (Iter &iter)
noexcept(noexcept(iter.recede()))
{
    iter.recede();
    return iter;
}
template<typename Iter, 
        EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& operator -- (Iter &iter, int)
noexcept(noexcept(Iter{--iter}))
{
    Iter store = iter;
    --iter;
    return store;
}




template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
constexpr bool operator < (const Iter &lhs, const Iter &rhs) noexcept
{ 
    return lhs.base() < rhs.base(); 
}
template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
constexpr bool operator > (const Iter &lhs, const Iter &rhs) noexcept
{ 
    return rhs < lhs; 
}
template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
constexpr bool operator <= (const Iter &lhs, const Iter &rhs) noexcept
{ 
    return !(rhs < lhs); 
}
template<typename Iter,
         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
constexpr bool operator >= (const Iter &lhs, const Iter &rhs) noexcept
{ 
    return !(lhs < rhs); 
}


template<typename Iter, 
        EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& 
operator += (Iter &iter, typename Iter::difference_type dist) 
noexcept(noexcept(iter.advance(dist)))
{
    iter.advance(dist);
    return iter;
}
template<typename Iter, 
        EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
ACHIBULUP__constexpr_fun14 Iter& 
operator -= (Iter &iter, typename Iter::difference_type dist) 
noexcept(noexcept(iter.recede(dist)))
{
    iter.recede(dist);
    return iter;
}


template<typename Iter, typename Type = typename std::decay<Iter>::type,
        EnableIf_t<IsDerivedFrom<Type, IteratorWrapper>()> = 0>
constexpr Iter
operator + (Iter &&iter, typename Iter::difference_type dist)
noexcept(noexcept(Iter{std::forward<Iter>(iter += dist)}))
{
    auto ret = std::forward<Iter>(iter);
    iter += dist;
    return iter;
}
// template<typename Iter, 
//         EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
// constexpr Iter
// operator + (typename Iter::difference_type dist, Iter iter)
// noexcept(noexcept(Iter{std::move(iter += dist)}))
// {
//     iter += dist;
//     return iter;
// }

template<typename Iter, typename Type = typename std::decay<Iter>::type,
        EnableIf_t<IsDerivedFrom<Type, IteratorWrapper>()> = 0>
constexpr Iter
operator - (Iter &&iter, typename Iter::difference_type dist)
noexcept(noexcept(Iter{std::forward<Iter>(iter += dist)}))
{
    auto ret = std::forward<Iter>(iter);
    iter -= dist;
    return iter;
}

template<typename Iter, 
        EnableIf_t<IsDerivedFrom<Iter, IteratorWrapper>()> = 0>
constexpr typename Iter::difference_type
operator - (const Iter &lhs, const Iter &rhs)
noexcept(noexcept(lhs.base() - rhs.base()))
{
    return lhs.base() - rhs.base();
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
class BasicIterator : public IteratorWrapper<Base>
{
    using Impl = IteratorWrapper<Base>;

  public:
    constexpr BasicIterator() = default;
    constexpr explicit BasicIterator(Argument<Base> it) 
    noexcept(noexcept(Impl(std::move(it)))) : Impl(std::move(it)) {}


    template<typename OtherBase,
            EnableIf_t<std::is_convertible<OtherBase, Base>::value> = 0>
    constexpr BasicIterator(const BasicIterator<OtherBase, Container> &cast)
    noexcept(noexcept(BasicIterator{cast.base()})) 
    : BasicIterator(cast.base()) {}


    
    friend constexpr bool operator == (const BasicIterator &lhs, 
                                       const BasicIterator &rhs) noexcept
    { 
        return lhs.base() == rhs.base(); 
    }
    friend constexpr bool operator != (const BasicIterator &lhs, 
                                const BasicIterator &rhs) noexcept
    { 
        return !(lhs == rhs); 
    }
    friend constexpr bool operator < (const BasicIterator &lhs, 
                                      const BasicIterator &rhs) noexcept
    { 
        return lhs.base() < rhs.base(); 
    }
    friend constexpr bool operator > (const BasicIterator &lhs, 
                                      const BasicIterator &rhs) noexcept
    { 
        return rhs < lhs; 
    }
    friend constexpr bool operator <= (const BasicIterator &lhs, 
                                       const BasicIterator &rhs) noexcept
    { 
        return !(rhs < lhs); 
    }
    friend constexpr bool operator >= (const BasicIterator &lhs, 
                                       const BasicIterator &rhs) noexcept
    { 
        return !(lhs < rhs); 
    }

    friend constexpr typename Impl::difference_type
    operator - (const BasicIterator &lhs, const BasicIterator &rhs)
    noexcept(noexcept(lhs.base() - rhs.base()))
    { 
        return lhs.base() - rhs.base(); 
    }
};
template<typename Tp, typename Container>
using iterator_wrapper = BasicIterator<Tp, Container>;

}


#endif // ITERATOR_WRAPPER_HPP_INCLUDED
