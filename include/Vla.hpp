#ifndef VLA_HPP_INCLUDED
#define VLA_HPP_INCLUDED
#include "Iterator_wrapper.h"
#include "common_utils.hpp"
#include "objectManagement.hpp"
#include <new>
#include <array>
#include <iterator>
#include <exception>
#include <type_traits>
#include <initializer_list>
namespace Achibulup
{

template<typename Tp, std::size_t dim>
class Vla;
template<typename Tp, std::size_t dim>
class VlaLproxy;
template<typename Tp, std::size_t dim>
class VlaCLproxy;
template<typename Tp, std::size_t dim>
class VlaRproxy;
template<typename Tp, std::size_t dim>
class VlaCRproxy;
template<typename Tp, std::size_t dim>
class VlaBase;
template<typename Tp, std::size_t dim>
class VlaPtr;
template<typename Tp, std::size_t dim>
class VlaCPtr;


namespace n_Vla
{
    using size_type = size_t;
    using dim_type = std::size_t;
    inline void maximize(size_type &var, size_type val)
    {
        if (var < val) var = val;
    }
    inline void throw_Vla_out_of_range(size_type idx, size_type sz)
    {
        throw std::out_of_range(stringFormat(
            "Vla indexing out of range : i (which is ",idx,"), "
            "is not in range [0, size()) (which is [0, ",sz,"))"));
    };
    template<typename Tp, std::size_t dim>
    class VlaProxy;
    template<typename Tp, std::size_t dim>
    class VlaCProxy;
    class VlaKey
    {
        constexpr VlaKey () noexcept {}
        constexpr VlaKey (const VlaKey &) noexcept = default;

        template<typename, std::size_t>
        friend class Achibulup::Vla;
        template<typename, std::size_t>
        friend class Achibulup::VlaLproxy;
        template<typename, std::size_t>
        friend class Achibulup::VlaCLproxy;
        template<typename, std::size_t>
        friend class Achibulup::VlaRproxy;
        template<typename, std::size_t>
        friend class Achibulup::VlaCRproxy;
        template<typename, std::size_t>
        friend class Achibulup::VlaPtr;
        template<typename, std::size_t>
        friend class Achibulup::VlaCPtr;
        template<typename, std::size_t>
        friend class Achibulup::n_Vla::VlaProxy;
        template<typename, std::size_t>
        friend class Achibulup::n_Vla::VlaCProxy;
    };
}




template<typename Tp, std::size_t dim>
class VlaBase
{
  public:
    using size_type = n_Vla::size_type;

  protected:
    using dim_type = n_Vla::dim_type;
    using offset_ptr_t = const size_type*;
    using const_data_ptr_t = RelaxedPtr<const Tp>;
    using data_ptr_t = RelaxedPtr<Tp>;

    VlaBase() noexcept = default;

    VlaBase(const VlaBase&) noexcept = default;
    VlaBase& operator = (const VlaBase&) = delete;
    
    VlaBase(offset_ptr_t offset, data_ptr_t data) 
    : m_offset(offset), m_data(data) {}

  public:
    friend bool operator == (const VlaBase &l, const VlaBase &r)
    {
        for(dim_type i = 0; i <= dim; ++i)
          if (l.m_offset[i] != r.m_offset[i]) return false;
        const_data_ptr_t curl = l.m_data, curr = r.m_data;
        for(size_type i = 0, cnt = l.m_offset[dim]; i < cnt; ++i)
          if (*(curl++) != *(curr++)) return false;
        return true;
    }
    friend bool operator != (const VlaBase &l, const VlaBase &r)
    {
        return !(l == r);
    }

    template<dim_type d = 0>
    size_type size() const
    {
        return size(dim - d);
    }

    bool empty() const
    {
        return !this->m_offset[dim];
    }

  protected:
    size_type size(size_type i_d) const
    {
        return this->m_offset[i_d] / this->m_offset[i_d - 1];
    }

    void range_check(dim_type i_d, size_type i) const
    {
        if (i >= this->size(i_d) || i < 0) 
          n_Vla::throw_Vla_out_of_range(i, this->size(i_d));
    }
    void range_check(size_type i) const
    {
        return range_check(dim, i);
    }

    data_ptr_t index(size_type i) const
    {
        return this->m_data + this->m_offset[dim - 1] * i;
    }
    data_ptr_t index(dim_type d, size_type i) const
    {
        return this->m_data + this->m_offset[d - 1] * i;
    }


    offset_ptr_t m_offset;
    data_ptr_t m_data;
};

namespace n_Vla
{

template<typename Tp, std::size_t dim>
class VlaProxy : protected VlaBase<Tp, dim>
{
  protected:
    using Base = VlaBase<Tp, dim>;
    using typename Base::size_type;
    using typename Base::dim_type;
    using typename Base::offset_ptr_t;
    using typename Base::const_data_ptr_t;
    using typename Base::data_ptr_t;

    
    using value_type                 =    typename Vla<Tp, dim>::value_type;
    using reference                  =    typename Vla<Tp, dim>::reference;
    using const_pointer              =    typename Vla<Tp, dim>::const_pointer;
    using pointer                    =    typename Vla<Tp, dim>::pointer;
    using const_iterator             =    typename Vla<Tp, dim>::const_iterator;
    using iterator                   =    typename Vla<Tp, dim>::iterator;
    using const_reverse_iterator     =    typename Vla<Tp, dim>::const_reverse_iterator;
    using reverse_iterator           =    typename Vla<Tp, dim>::reverse_iterator;


    VlaProxy() = delete;
    
    VlaProxy(const VlaProxy&) noexcept = default;
    VlaProxy& operator = (const VlaProxy&) = delete;

    VlaProxy(const Base& sub) noexcept : Base(sub) {}

    using Base::Base;

  public:
    reference operator [] (size_type i) const &
    {
        return reference(this->m_offset, this->index(i), n_Vla::VlaKey{});
    }

    reference at(size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }



    const_iterator cbegin() const &
    {
        return const_iterator(this->m_offset, this->m_data, n_Vla::VlaKey{});
    }
    iterator begin() const &
    {
        return iterator(this->m_offset, this->m_data, n_Vla::VlaKey{});
    }

    const_iterator cend() const &
    {
        return const_iterator(this->m_offset, 
                              this->m_data + this->m_offset[dim],
                              n_Vla::VlaKey{});
    }
    iterator end() const &
    {
        return iterator(this->m_offset, this->m_data + this->m_offset[dim], 
                        n_Vla::VlaKey{});
    }

    const_reverse_iterator crbegin() const &
    {
        return const_reverse_iterator(this->end());
    }
    reverse_iterator rbegin() const &
    {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator crend() const &
    {
        return const_reverse_iterator(this->begin());
    }
    reverse_iterator rend() const &
    {
        return reverse_iterator(this->begin());
    }

};


template<typename Tp>
class VlaProxy<Tp, 1> : protected VlaBase<Tp, 1>
{
  protected:
    using Base = VlaBase<Tp, 1>;
    using typename Base::size_type;
    using typename Base::dim_type;
    using typename Base::offset_ptr_t;
    using typename Base::const_data_ptr_t;
    using typename Base::data_ptr_t;

    
    using value_type                 =    typename Vla<Tp, 1>::value_type;
    using reference                  =    typename Vla<Tp, 1>::reference;
    using const_pointer              =    typename Vla<Tp, 1>::const_pointer;
    using pointer                    =    typename Vla<Tp, 1>::pointer;
    using const_iterator             =    typename Vla<Tp, 1>::const_iterator;
    using iterator                   =    typename Vla<Tp, 1>::iterator;
    using const_reverse_iterator     =    typename Vla<Tp, 1>::const_reverse_iterator;
    using reverse_iterator           =    typename Vla<Tp, 1>::reverse_iterator;


    VlaProxy() = delete;
    
    VlaProxy(const VlaProxy&) noexcept = default;
    VlaProxy& operator = (const VlaProxy&) = delete;
    
    VlaProxy(const Base& sub) noexcept : Base(sub) {}

    using Base::Base;

  public:
    reference operator [] (size_type i) const &
    {
        return this->m_data[i];
    }

    reference at (size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }


    const_iterator cbegin() const &
    {
        return const_iterator(this->m_data);
    }
    iterator begin() const &
    {
        return iterator(this->m_data);
    }

    const_iterator cend() const &
    {
        return const_iterator(this->m_data + this->m_offset[1]);
    }
    iterator end() const &
    {
        return iterator(this->m_data + this->m_offset[1]);
    }

    const_reverse_iterator crbegin() const &
    {
        return const_reverse_iterator(this->end());
    }
    reverse_iterator rbegin() const &
    {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator crend() const &
    {
        return const_reverse_iterator(this->begin());
    }
    reverse_iterator rend() const &
    {
        return reverse_iterator(this->begin());
    }
};

template<typename Tp>
class VlaProxy<Tp, 0>;

template<typename Tp, std::size_t dim>
class VlaCProxy : protected VlaBase<Tp, dim>
{
  protected:
    using Base = VlaBase<Tp, dim>;
    using typename Base::size_type;
    using typename Base::dim_type;
    using typename Base::offset_ptr_t;
    using typename Base::const_data_ptr_t;
    using typename Base::data_ptr_t;

    using value_type                 =    typename Vla<Tp, dim>::value_type;
    using const_reference            =    typename Vla<Tp, dim>::const_reference;
    using reference                  =    const_reference;
    using const_pointer              =    typename Vla<Tp, dim>::const_pointer;
    using pointer                    =    const_pointer;
    using const_iterator             =    typename Vla<Tp, dim>::const_iterator;
    using iterator                   =    const_iterator;
    using const_reverse_iterator     =    typename Vla<Tp, dim>::const_reverse_iterator;
    using reverse_iterator           =    const_reverse_iterator;

    VlaCProxy() = delete;
    
    VlaCProxy(const VlaCProxy&) noexcept = default;
    VlaCProxy& operator = (const VlaCProxy&) = delete;

    VlaCProxy(const Base& sub) noexcept : Base(sub) {}

    using Base::Base;

  public:
    const_reference operator [] (size_type i) const &
    {
        return const_reference(this->m_offset, this->index(i), n_Vla::VlaKey{});
    }

    const_reference at (size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }



    const_iterator cbegin() const &
    {
        return const_iterator(this->m_offset, this->m_data, n_Vla::VlaKey{});
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }

    const_iterator cend() const &
    {
        return const_iterator(this->m_offset, 
                              this->m_data + this->m_offset[dim], 
                              n_Vla::VlaKey{});
    }
    const_iterator end() const &
    {
        return this->cend();
    }

    const_reverse_iterator crbegin() const &
    {
        return const_reverse_iterator(this->end());
    }
    const_reverse_iterator rbegin() const &
    {
        return this->crbegin();
    }

    const_reverse_iterator crend() const &
    {
        return const_reverse_iterator(this->begin());
    }
    const_reverse_iterator rend() const &
    {
        return this->crend();
    }
};

template<typename Tp>
class VlaCProxy<Tp, 1> : protected VlaBase<Tp, 1>
{
  protected:
    using Base = VlaBase<Tp, 1>;
    using typename Base::size_type;
    using typename Base::dim_type;
    using typename Base::offset_ptr_t;
    using typename Base::const_data_ptr_t;
    using typename Base::data_ptr_t;

    using value_type                 =    typename Vla<Tp, 1>::value_type;
    using const_reference            =    typename Vla<Tp, 1>::const_reference;
    using reference                  =    const_reference;
    using const_pointer              =    typename Vla<Tp, 1>::const_pointer;
    using pointer                    =    const_pointer;
    using const_iterator             =    typename Vla<Tp, 1>::const_iterator;
    using iterator                   =    const_iterator;
    using const_reverse_iterator     =    typename Vla<Tp, 1>::const_reverse_iterator;
    using reverse_iterator           =    const_reverse_iterator;


    VlaCProxy() = delete;
    
    VlaCProxy(const VlaCProxy&) noexcept = default;
    VlaCProxy& operator = (const VlaCProxy&) = delete;

    VlaCProxy(const Base& sub) noexcept : Base(sub) {}

    using Base::Base;

  public:
    const_reference operator [] (size_type i) const &
    {
        return this->m_data[i];
    }

    const_reference at (size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }


    const_iterator cbegin() const &
    {
        return const_iterator(this->m_data);
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }

    const_iterator cend() const &
    {
        return const_iterator(this->m_data + this->m_offset[1]);
    }
    const_iterator end() const &
    {
        return this->cend();
    }

    const_reverse_iterator crbegin() const &
    {
        return const_reverse_iterator(this->end());
    }
    const_reverse_iterator rbegin() const &
    {
        return this->crbegin();
    }

    const_reverse_iterator crend() const &
    {
        return const_reverse_iterator(this->begin());
    }
    const_reverse_iterator rend() const &
    {
        return this->crend();
    }
};

template<typename Tp>
class VlaCProxy<Tp, 0>;

}


template<typename Tp, std::size_t dim>
class VlaLproxy : private n_Vla::VlaProxy<Tp, dim>
{
    using pBase = n_Vla::VlaProxy<Tp, dim>;
    using typename pBase::dim_type;

  public:
    using typename pBase::size_type;
    using typename pBase::value_type;
    using typename pBase::reference;
    using typename pBase::const_pointer;
    using typename pBase::pointer;
    using typename pBase::const_iterator;
    using typename pBase::iterator;
    using typename pBase::const_reverse_iterator;
    using typename pBase::reverse_iterator;

    using typename pBase::VlaBase;
    using typename pBase::Base;

  public:
    VlaLproxy(typename Base::offset_ptr_t offset, 
              typename Base::data_ptr_t ptr, n_Vla::VlaKey) noexcept
    : pBase(offset, ptr) {}

    VlaLproxy(const VlaRproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    VlaLproxy(const VlaRproxy<Tp, dim> &&) = delete;


    using Base::size;
    using Base::empty;

    using pBase::operator[];
    using pBase::at;

    using pBase::cbegin;
    using pBase::begin;
    using pBase::cend;
    using pBase::end;
    using pBase::crbegin;
    using pBase::rbegin;
    using pBase::crend;
    using pBase::rend;
};

template<typename tp>
class VlaLproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class VlaCLproxy : private n_Vla::VlaCProxy<Tp, dim>
{
    using pBase = n_Vla::VlaCProxy<Tp, dim>;
    using typename pBase::dim_type;

  public:
    using typename pBase::size_type;
    using typename pBase::value_type;
    using typename pBase::const_reference;
    using typename pBase::reference;
    using typename pBase::const_pointer;
    using typename pBase::pointer;
    using typename pBase::const_iterator;
    using typename pBase::iterator;
    using typename pBase::const_reverse_iterator;
    using typename pBase::reverse_iterator;

    using typename pBase::VlaBase;
    using typename pBase::Base;

  public:
    VlaCLproxy(typename Base::offset_ptr_t offset, 
               typename Base::data_ptr_t ptr, n_Vla::VlaKey) noexcept
    : pBase(offset, ptr) {}

    VlaCLproxy(const VlaLproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    VlaCLproxy(const VlaCRproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    VlaCLproxy(const VlaCRproxy<Tp, dim> &&) = delete;
    VlaCLproxy(const VlaRproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    VlaCLproxy(const VlaRproxy<Tp, dim> &&) = delete;

    using pBase::size;
    using Base::empty;

    using pBase::operator[];
    using pBase::at;

    using pBase::cbegin;
    using pBase::begin;
    using pBase::cend;
    using pBase::end;
    using pBase::crbegin;
    using pBase::rbegin;
    using pBase::crend;
    using pBase::rend;
};

template<typename tp>
class VlaCLproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class VlaRproxy : private n_Vla::VlaProxy<Tp, dim>
{
    using pBase = n_Vla::VlaProxy<Tp, dim>;
    using typename pBase::dim_type;

  public:
    using typename pBase::size_type;
    using typename pBase::value_type;
    using typename pBase::reference;
    using typename pBase::const_pointer;
    using typename pBase::pointer;
    using typename pBase::const_iterator;
    using typename pBase::iterator;
    using typename pBase::const_reverse_iterator;
    using typename pBase::reverse_iterator;

    using rvalue = typename Vla<Tp, dim>::rvalue;

    using typename pBase::VlaBase;
    using typename pBase::Base;

  public:
    explicit VlaRproxy(const VlaLproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}


    using pBase::size;
    using Base::empty;

    using pBase::operator[];
    using pBase::at;

    using pBase::cbegin;
    using pBase::begin;
    using pBase::cend;
    using pBase::end;
    using pBase::crbegin;
    using pBase::rbegin;
    using pBase::crend;
    using pBase::rend;


    rvalue operator [] (size_type i) const &&
    {
        return static_cast<rvalue>((*this)[i]);
    }

    rvalue at (size_type i) const &&
    {
        return static_cast<rvalue>(this->at(i));
    }

    
    const_iterator cbegin() const && = delete;
    const_iterator begin() const && = delete;
    const_iterator cend() const && = delete;
    const_iterator end() const && = delete;
    const_reverse_iterator crbegin() const && = delete;
    const_reverse_iterator rbegin() const && = delete;
    const_reverse_iterator crend() const && = delete;
    const_reverse_iterator rend() const && = delete;
};

template<typename tp>
class VlaRproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class VlaCRproxy : private n_Vla::VlaCProxy<Tp, dim>
{
    using pBase = n_Vla::VlaCProxy<Tp, dim>;
    using typename pBase::dim_type;

  public:
    using typename pBase::size_type;
    using typename pBase::value_type;
    using typename pBase::const_reference;
    using typename pBase::reference;
    using typename pBase::const_pointer;
    using typename pBase::pointer;
    using typename pBase::const_iterator;
    using typename pBase::iterator;
    using typename pBase::const_reverse_iterator;
    using typename pBase::reverse_iterator;

    using const_rvalue = typename Vla<Tp, dim>::const_rvalue;
    using rvalue = const_rvalue;

    using typename pBase::VlaBase;
    using typename pBase::Base;

  public:
    VlaCRproxy(const VlaRproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    explicit VlaCRproxy(const VlaCLproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    explicit VlaCRproxy(const VlaLproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}


    using pBase::size;

    using pBase::operator[];
    using pBase::at;

    using pBase::cbegin;
    using pBase::begin;
    using pBase::cend;
    using pBase::end;
    using pBase::crbegin;
    using pBase::rbegin;
    using pBase::crend;
    using pBase::rend;


    
    const_rvalue operator [] (size_type i) const &&
    {
        return static_cast<const_rvalue>((*this)[i]);
    }

    const_rvalue at (size_type i) const &&
    {
        return static_cast<const_rvalue>(this->at(i));
    }

    
    const_iterator cbegin() const && = delete;
    const_iterator begin() const && = delete;
    const_iterator cend() const && = delete;
    const_iterator end() const && = delete;
    const_reverse_iterator crbegin() const && = delete;
    const_reverse_iterator rbegin() const && = delete;
    const_reverse_iterator crend() const && = delete;
    const_reverse_iterator rend() const && = delete;
};

template<typename tp>
class VlaRproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class VlaPtr
{
    using dim_type           =  std::size_t;
    using size_type          =  typename Vla<Tp, dim>::size_type;

  public:
    using iterator_category  =  std::random_access_iterator_tag;
    using difference_type    =  size_type;
    using value_type         =  VlaBase<Tp, dim>;
    using reference          =  VlaLproxy<Tp, dim>;
    using pointer = struct{
        reference ref;
        reference* operator -> () &&
        {
            return &(this->ref);
        }
    };

    VlaPtr() noexcept = default;
    VlaPtr& operator = (const VlaPtr&) & noexcept = default;

    VlaPtr(decltype(nullptr)) noexcept : VlaPtr() {}
    
    VlaPtr(const size_type *ofs, RelaxedPtr<Tp> ptr, n_Vla::VlaKey) noexcept
    : VlaPtr(ofs, ptr) {}


    reference operator * () const
    {
        return reference(this->m_offset, this->m_data, n_Vla::VlaKey{});
    }

    pointer operator -> () const
    {
        return pointer{*(*this)};
    }

    VlaPtr& operator ++ () &
    {
        this->m_data += this->m_offset[dim];
        return *this;
    }
    VlaPtr operator ++ (int) &
    {
        VlaPtr store = *this;
        ++(*this);
        return store;
    }


    VlaPtr& operator -- () &
    {
        this->m_data -= this->m_offset[dim];
        return *this;
    }
    VlaPtr operator -- (int) &
    {
        VlaPtr store = *this;
        --(*this);
        return store;
    }


    reference operator [] (difference_type i) const
    {
        return *((*this) + i);
    }

    friend VlaPtr operator + (const VlaPtr &Ptr, difference_type i)
    {
        return VlaPtr(Ptr.m_offset, Ptr.ptr + Ptr.m_offset[dim] * i);
    }
    friend VlaPtr operator + (difference_type i, const VlaPtr &Ptr)
    {
        return VlaPtr(Ptr.m_offset, Ptr.ptr + Ptr.m_offset[dim] * i);
    }
    friend VlaPtr operator - (const VlaPtr &Ptr, difference_type i)
    {
        return VlaPtr(Ptr.m_offset, Ptr.ptr - Ptr.m_offset[dim] * i);
    }
    friend difference_type operator - (const VlaPtr &Ptr1, const VlaPtr &Ptr2)
    {
        return (Ptr1.ptr - Ptr2.ptr) / Ptr1.m_offset[dim];
    }

    VlaPtr& operator += (difference_type i) &
    {
        this->m_data += this->m_offset[dim] * i;
        return *this;
    }
    VlaPtr& operator -= (difference_type i) &
    {
        this->m_data -= this->m_offset[dim] * i;
        return *this;
    }

    friend bool operator == (const VlaPtr &l, const VlaPtr &r) noexcept
    {  return l.ptr == r.ptr;  }
    friend bool operator != (const VlaPtr &l, const VlaPtr &r) noexcept
    {  return l.ptr != r.ptr;  }
    friend bool operator > (const VlaPtr &l, const VlaPtr &r) noexcept
    {  return l.ptr > r.ptr;  }
    friend bool operator >= (const VlaPtr &l, const VlaPtr &r) noexcept
    {  return l.ptr >= r.ptr;  }
    friend bool operator < (const VlaPtr &l, const VlaPtr &r) noexcept
    {  return l.ptr < r.ptr;  }
    friend bool operator <= (const VlaPtr &l, const VlaPtr &r) noexcept
    {  return l.ptr <= r.ptr;  }

  private:
    VlaPtr(const size_type *ofs, RelaxedPtr<Tp> ptr) noexcept
    : m_offset(ofs), m_data(ptr) {}

    const size_type *m_offset;
    RelaxedPtr<Tp> m_data;
};


template<typename Tp>
class VlaPtr<Tp, 0>;



template<typename Tp, std::size_t dim>
class VlaCPtr
{
    using dim_type           =  std::size_t;
    using size_type          =  typename Vla<Tp, dim>::size_type;

  public:
    using iterator_category  =  std::random_access_iterator_tag;
    using difference_type    =  size_type;
    using value_type         =  VlaBase<Tp, dim>;
    using const_reference    =  VlaCLproxy<Tp, dim>;
    using reference          =  const_reference;
    using const_pointer = struct{
        const_reference ref;
        const_reference* operator -> () &&
        {
            return &(this->ref);
        }
    };
    using pointer = const_pointer;


    VlaCPtr() noexcept = default;
    VlaCPtr& operator = (const VlaCPtr&) & noexcept = default;

    VlaCPtr(decltype(nullptr)) noexcept : VlaCPtr() {}

    VlaCPtr(const size_type *ofs, RelaxedPtr<Tp> ptr, n_Vla::VlaKey) noexcept
    : VlaCPtr(ofs, ptr) {}

    VlaCPtr(const VlaPtr<Tp, dim>& cast) noexcept
    : m_offset(cast.m_offset), m_data(cast.m_data) {}


    const_reference operator * () const
    {
        return const_reference(this->m_offset, this->m_data, n_Vla::VlaKey{});
    }

    pointer operator -> () const
    {
        return pointer{*(*this)};
    }

    VlaCPtr& operator ++ () &
    {
        this->m_data += this->m_offset[dim];
        return *this;
    }
    VlaCPtr operator ++ (int) &
    {
        VlaCPtr store = *this;
        ++(*this);
        return store;
    }


    VlaCPtr& operator -- () &
    {
        this->m_data -= this->m_offset[dim];
        return *this;
    }
    VlaCPtr operator -- (int) &
    {
        VlaCPtr store = *this;
        --(*this);
        return store;
    }


    reference operator [] (size_type i) const
    {
        return *(*this + i);
    }

    friend VlaCPtr operator + (const VlaCPtr &Ptr, difference_type i)
    {
        return VlaCPtr(Ptr.m_offset, Ptr.ptr + Ptr.m_offset[dim] * i);
    }
    friend VlaCPtr operator + (difference_type i, const VlaCPtr &Ptr)
    {
        return VlaCPtr(Ptr.m_offset, Ptr.ptr + Ptr.m_offset[dim] * i);
    }
    friend VlaCPtr operator - (const VlaCPtr &Ptr, difference_type i)
    {
        return VlaCPtr(Ptr.m_offset, Ptr.ptr - Ptr.m_offset[dim] * i);
    }
    friend difference_type operator - (const VlaCPtr &Ptr1, const VlaCPtr &Ptr2)
    {
        return (Ptr1.ptr - Ptr2.ptr) / Ptr1.m_offset[dim];
    }

    VlaCPtr& operator += (difference_type i) &
    {
        this->m_data += this->m_offset[dim] * i;
        return *this;
    }
    VlaCPtr& operator -= (difference_type i) &
    {
        this->m_data -= this->m_offset[dim] * i;
        return *this;
    }

    friend bool operator == (const VlaCPtr &l, const VlaCPtr &r) noexcept
    {  return l.ptr == r.ptr;  }
    friend bool operator !=(const VlaCPtr &l, const VlaCPtr &r) noexcept
    {  return l.ptr != r.ptr;  }
    friend bool operator > (const VlaCPtr &l, const VlaCPtr &r) noexcept
    {  return l.ptr > r.ptr;  }
    friend bool operator >= (const VlaCPtr &l, const VlaCPtr &r) noexcept
    {  return l.ptr >= r.ptr;  }
    friend bool operator < (const VlaCPtr &l, const VlaCPtr &r) noexcept
    {  return l.ptr < r.ptr;  }
    friend bool operator <= (const VlaCPtr &l, const VlaCPtr &r) noexcept
    {  return l.ptr <= r.ptr;  }

  private:
    VlaCPtr(const size_type *ofs, RelaxedPtr<Tp> ptr) noexcept
    : m_offset(ofs), m_data(ptr) {}

    const size_type *m_offset;
    RelaxedPtr<Tp> m_data;
};


template<typename Tp>
class VlaCPtr<Tp, 0>;



#if ACHIBULUP__Cpp17_later

template<std::size_t dim, std::size_t...seq>
size_t Vla_index_helper(const size_t(&idx)[dim], const size_t *m_offset
                            , std::integer_sequence<std::size_t, seq...>)
{
    return (... + (idx[seq] * m_offset[dim - 1 - seq]));
}

#endif // __cplusplus

template<typename Tp, std::size_t dim>
struct VlaArgHelper;

template<typename Tp, std::size_t dim>
struct Vla_initializer
{
    std::array<size_t, dim> dimensions;
    std::initializer_list<Vla_initializer<Tp, dim - 1>> m_data;
    Vla_initializer(std::initializer_list<Vla_initializer<Tp, dim - 1>> il) 
    : dimensions{}, m_data(il)
    {
        dimensions[dim - 1] = m_data.size();
        for(auto &sub : m_data)
        for(std::size_t i = 0; i < dim - 1; ++i)
          n_Vla::maximize(dimensions[i], sub.dimensions[i]);
    }
};
template<typename Tp>
struct Vla_initializer<Tp, 1>
{
    std::array<size_t, 1> dimensions;
    std::initializer_list<Tp> m_data;
    Vla_initializer(std::initializer_list<Tp> il) 
    : dimensions{static_cast<size_t>(il.size())}, m_data(il) {}
};
template<typename Tp>
struct Vla_initializer<Tp, 0>;


template<typename Tp, std::size_t dim, bool = std::is_trivially_destructible<Tp>::value>
struct VlaListInitHelper
{
    using initializer_list = std::initializer_list<Vla_initializer<Tp, dim - 1>>;

    static void construct(RelaxedPtr<Tp> pos, initializer_list init_list, const size_t *dimensions)
    {
        RelaxedPtr<Tp> en = pos + dimensions[dim];
        for(auto &sub : init_list){
          VlaListInitHelper<Tp, dim - 1>::construct(pos, sub.m_data, dimensions);
          pos += dimensions[dim - 1];
        }
        Achibulup::construct(en - pos, pos);
    }
};
template<typename Tp, std::size_t dim>
struct VlaListInitHelper<Tp, dim, false>
{
    using initializer_list = std::initializer_list<Vla_initializer<Tp, dim - 1>>;

    static void construct(RelaxedPtr<Tp> pos, initializer_list init_list, const size_t *dimensions)
    {
        typename Constructor<Tp>::exception_safety p{pos, pos};
        RelaxedPtr<Tp> en = pos + dimensions[dim];
        for(auto &sub : init_list){
          VlaListInitHelper<Tp, dim - 1>::construct(p.last, sub.m_data, dimensions);
          p.last += dimensions[dim - 1];
        }
        Achibulup::construct(en - p.last, p.last);
        p.last = p.first;
    }
};
template<typename Tp>
struct VlaListInitHelper<Tp, 1, true>
{
    using initializer_list = std::initializer_list<Tp>;

    static void construct(RelaxedPtr<Tp> pos, initializer_list init_list, const size_t *dimensions)
    {
        Achibulup::copyConstruct(init_list.size(), pos, init_list.begin());
        Achibulup::construct(dimensions[1] - init_list.size(), pos + init_list.size());
    }
};
template<typename Tp>
struct VlaListInitHelper<Tp, 1, false>
{
    using initializer_list = std::initializer_list<Tp>;

    static void construct(RelaxedPtr<Tp> pos, initializer_list init_list, const size_t *dimensions)
    {
        typename Constructor<Tp>::exception_safety p{pos, pos};
        Achibulup::copyConstruct(init_list.size(), pos, init_list.begin());
        p.last += init_list.size();
        Achibulup::construct(dimensions[1] - init_list.size(), pos + init_list.size());
        p.last = p.first;
    }
};
template<typename Tp, bool b>
struct VlaListInitHelper<Tp, 0, b>;


template<typename, typename, typename>
struct ChangeRetAddArg;
template<typename Ret, typename ...Args, typename NewRet, typename Add>
struct ChangeRetAddArg<Ret(Args...), NewRet, Add>
{
    using type = NewRet(Add, Args...);
};

template<typename Tp, std::size_t dim>
struct VlaArgHelper
{
    using size_list_t = std::array<typename VlaBase<Tp, dim>::size_type, dim>;
    struct value_initializer
    {
        size_list_t size_list;
        const Tp &value;
    };
    using initializer_list = typename VlaListInitHelper<Tp, dim>::initializer_list;

  private:
    template<typename ...Args>
    static constexpr value_initializer make_initializer_helper(Args ...args)
    {
        return {args...};
    }

  public:
    using TakeSizeListArgs = typename ChangeRetAddArg<
        typename VlaArgHelper<Tp, dim - 1>::TakeSizeListArgs, 
        value_initializer, size_t
    >::type;
    using TakeValInitzerArgs = typename ChangeRetAddArg<
        typename VlaArgHelper<Tp, dim - 1>::TakeValInitzerArgs, 
        value_initializer, size_t
    >::type;


    static constexpr TakeValInitzerArgs 
    &make_initializer = make_initializer_helper;


    template<typename ...bound>
    using IsSizeListArgs = decltype(
        std::declval<TakeSizeListArgs>()(std::declval<bound>()...));
    template<typename ...Tps>
    using IsValInitzerArgs = decltype(
        std::declval<TakeValInitzerArgs>()(std::declval<Tps>()...));

};
template<typename Tp>
struct VlaArgHelper<Tp, 0>
{
    using TakeSizeListArgs = void();
    using TakeValInitzerArgs = void(const Tp&);
};


template<typename Tp, std::size_t dim>
class VlaImpl : protected VlaBase<Tp, dim>
{
  protected:
    using Base = VlaBase<Tp, dim>;
    using typename Base::size_type;
    using typename Base::dim_type;
    using typename Base::offset_ptr_t;
    using typename Base::const_data_ptr_t;
    using typename Base::data_ptr_t;
    
    using size_list_t       = typename VlaArgHelper<Tp, dim>::size_list_t;
    using value_initializer = typename VlaArgHelper<Tp, dim>::value_initializer;
    using initializer_list  = typename VlaArgHelper<Tp, dim>::initializer_list;

    VlaImpl() noexcept : i_offset_base{1}
    {
        this->init();
    }

    VlaImpl(const VlaImpl &cpy) : i_offset_base(cpy.i_offset_base)
    {
        this->copy(cpy.m_data, cpy.m_offset[dim]);
    }
    VlaImpl(VlaImpl &&mov) noexcept : VlaImpl()
    {
        swap(*this, mov);
    }

    VlaImpl& operator = (VlaImpl other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    friend void swap(VlaImpl &a, VlaImpl &b) noexcept
    {
        using std::swap;
        swap(a.m_data, b.m_data);
        swap(a.i_offset_base, b.i_offset_base);
    }

    ~VlaImpl()
    {
        if (this->m_data != data_ptr_t()){
            Achibulup::destroy(this->m_data, this->m_offset[dim]);
            deleteBuffer(this->m_data);
        }
    }


    VlaImpl(const size_list_t &size_list) 
    : i_offset_base(calc_offset(size_list))
    {
        this->init(this->i_offset_base[dim]);
    }
    VlaImpl(const value_initializer &init) 
    : i_offset_base(calc_offset(init))
    {
        this->init(this->i_offset_base[dim], init.value);
    }
    VlaImpl(initializer_list init_list)
    : i_offset_base(calc_offset(init_list))
    {
        this->init(this->i_offset_base[dim], init_list);
    }


  public:
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    Tp& operator [] (const size_type (&i)[Nm]) &
    {
        return this->m_data[this->get_index(i)];
    }
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    const Tp& operator [] (const size_type (&i)[Nm]) const &
    {
        return this->m_data[this->get_index(i)];
    }
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    Tp&& operator [] (const size_type (&i)[Nm]) &&
    {
        return std::move((*this)[i]);
    }
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    const Tp&& operator [] (const size_type (&i)[Nm]) const &&
    {
        return std::move((*this)[i]);
    }

    template<typename ...indices, typename = typename VlaArgHelper<Tp, dim>
                                     ::template IsSizeListArgs<indices...>>
    Tp& operator () (indices&& ...is) &
    {
        return (*this)[{static_cast<size_type>(is)...}];
    }
    template<typename ...indices, typename = typename VlaArgHelper<Tp, dim>
                                    ::template IsSizeListArgs<indices...>>
    const Tp& operator () (indices&& ...is) const &
    {
        return (*this)[{static_cast<size_type>(is)...}];
    }
    template<typename ...indices, typename = typename VlaArgHelper<Tp, dim>
                                    ::template IsSizeListArgs<indices...>>
    Tp&& operator () (indices&& ...is) &&
    {
        return std::move((*this)[{static_cast<size_type>(is)...}]);
    }
    template<typename ...indices, typename = typename VlaArgHelper<Tp, dim>
                                    ::template IsSizeListArgs<indices...>>
    const Tp&& operator () (indices&& ...is) const &&
    {
        return std::move((*this)[{static_cast<size_type>(is)...}]);
    }


    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    Tp& at(const size_type (&i)[Nm]) &
    {
        this->range_check(i);
        return (*this)[i];
    }
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    const Tp& at(const size_type (&i)[Nm]) const &
    {
        this->range_check(i);
        return (*this)[i];
    }
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    Tp&& at(const size_type (&i)[Nm]) &&
    {
        return std::move(this->at(i));
    }
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    const Tp&& at(const size_type (&i)[Nm]) const &&
    {
        return std::move(this->at(i));
    }


    template<typename ...indices, typename = typename VlaArgHelper<Tp, dim>
                                    ::template IsSizeListArgs<indices...>>
    Tp& at(indices&& ...is) &
    {
        return this->at({static_cast<size_type>(is)...});
    }
    template<typename ...indices, typename = typename VlaArgHelper<Tp, dim>
                                    ::template IsSizeListArgs<indices...>>
    const Tp& at(indices&& ...is) const &
    {
        return this->at({static_cast<size_type>(is)...});
    }
    template<typename ...indices, 
        typename = typename VlaArgHelper<Tp, dim>::template IsSizeListArgs<indices...>>
    Tp&& at(indices&& ...is) &&
    {
        return std::move(this->at({static_cast<size_type>(is)...}));
    }
    template<typename ...indices, typename = typename VlaArgHelper<Tp, dim>
                                    ::template IsSizeListArgs<indices...>>
    const Tp&& at(indices&& ...is) const &&
    {
        return std::move(this->at({static_cast<size_type>(is)...}));
    }


  private:
    using offset_arr_t = std::array<size_type, dim + 1>;

    struct safe_arr
    {
        data_ptr_t ptr;
        ~safe_arr()
        {
            deleteBuffer(ptr);
        }
    };

    static offset_arr_t calc_offset(const size_list_t &size_list)
    {
        offset_arr_t res;
        res[0] = 1;
        for(dim_type i = 1; i <= dim; ++i){
          if (size_list[dim - i] <= 0)
             throw std::bad_alloc();
          res[i] = res[i - 1] * size_list[dim - i];
        }
        return res;
    }
    static offset_arr_t calc_offset(const value_initializer &val_init)
    {
        return calc_offset(val_init.size_list);
    }
    static offset_arr_t calc_offset(initializer_list init_list)
    {
        offset_arr_t res{1};
        for(auto &val : init_list)
          for(std::size_t i = 1; i < dim; ++i)
            n_Vla::maximize(res[i], val.dimensions[i - 1]);
        res[dim] = init_list.size();
        for(std::size_t i = 0; i < dim; ++i){
          if (res[i + 1] <= 0)
             throw std::bad_alloc();
          res[i + 1] *= res[i];
        }
        return res;
    }

    void set_offset_ptr() &
    {
        this->m_offset = this->i_offset_base.data();
    }

    void init() &
    {
        set_offset_ptr();
        this->m_data = {};
    }
    void init(size_type len) &
    {
        set_offset_ptr();
        safe_arr safe{newBuffer<Tp>(len)};
        this->m_data = safe.ptr;
        Achibulup::construct(len, this->m_data);
        safe.ptr = nullptr;
    }
    void init(size_type len, const Tp &value) &
    {
        set_offset_ptr();
        safe_arr safe{newBuffer<Tp>(len)};
        this->m_data = safe.ptr;
        Achibulup::construct(len, this->m_data, value);
        safe.ptr = nullptr;
    }
    void init(size_type len, initializer_list init_list) &
    {
        set_offset_ptr();
        safe_arr safe{newBuffer<Tp>(len)};
        this->m_data = safe.ptr;
        VlaListInitHelper<Tp, dim>::construct(this->m_data, init_list, this->m_offset);
        safe.ptr = nullptr;
    }

    void copy(const_data_ptr_t cpy, size_type len) &
    {
        set_offset_ptr();
        safe_arr safe{newBuffer<Tp>(len)};
        this->m_data = safe.ptr;
        Achibulup::copyConstruct(len, this->m_data, cpy);
        safe.ptr = nullptr;
    }


    void range_check(const size_type (&i)[dim])
    {
        for(dim_type d = dim; d != 0; --d)
          Base::range_check(d, i[dim - d]);
    }
#if ACHIBULUP__Cpp17_later
    size_type get_index(const size_type(&i)[dim])
    {
        return Vla_index_helper(i, this->m_offset, 
                                std::make_index_sequence<dim>());
    }
#else
    size_type get_index(const size_type(&i)[dim])
    {
        size_type res = 0;
        for(dim_type d = dim; d != 0; --d)
          res += i[dim - d] * this->m_offset[d - 1];
        return res;
    }
#endif // __cplusplus



    offset_arr_t i_offset_base;
};
template<typename Tp>
struct VlaImpl<Tp, 0>;



template<typename Tp, std::size_t dim>
class Vla : private VlaImpl<Tp, dim>
{
  private:
    using Impl = VlaImpl<Tp, dim>;
    using typename Impl::Base;
    using typename Base::dim_type;
    using ArgsBase = VlaArgHelper<Tp, dim>;

  public:
    using value_type                 =    VlaBase<Tp, dim - 1>;
    using reference                  =    VlaLproxy<Tp, dim - 1>;
    using const_reference            =    VlaCLproxy<Tp, dim - 1>;
    using rvalue                     =    VlaRproxy<Tp, dim - 1>;
    using const_rvalue               =    VlaCRproxy<Tp, dim - 1>;
    using pointer                    =    VlaPtr<Tp, dim - 1>;
    using const_pointer              =    VlaCPtr<Tp, dim - 1>;
    using iterator                   =    pointer;
    using const_iterator             =    const_pointer;
    using reverse_iterator           =    std::reverse_iterator<iterator>;
    using const_reverse_iterator     =    std::reverse_iterator<const_iterator>;
    
    using typename Impl::VlaBase;
    using typename Base::size_type;

    using size_list_t                =    typename ArgsBase::size_list_t;
    using value_initializer          =    typename ArgsBase::value_initializer;
    using initializer_list           =    typename ArgsBase::initializer_list;
    



    Vla() = default;
    Vla(const Vla&) = default;
    Vla(Vla&&) noexcept = default;

    Vla& operator = (const Vla&) & = default;
    Vla& operator = (Vla&&) & noexcept = default;

    friend void swap(Vla &a, Vla &b) noexcept
    {
        swap(static_cast<Impl&>(a), static_cast<Impl&>(b));
    }

    template<typename ...bound, typename = typename VlaArgHelper<Tp, dim>
                                 ::template IsSizeListArgs<bound...>>
    Vla(bound&&... s) : Impl(size_list_t{static_cast<size_type>(s)...}) {}

    template<typename ...Tps, typename = typename VlaArgHelper<Tp, dim>
                    ::template IsValInitzerArgs<Tps...>, typename = void>
    Vla(Tps&&... s) 
    : Impl(ArgsBase::make_initializer(std::forward<Tps>(s)...)) {}

    Vla(initializer_list il) : Impl(il) {}




    using Base::size;
    using Base::empty;

    using Impl::operator[];
    using Impl::operator();
    using Impl::at;


    reference operator [] (size_type i) &
    {
        return reference(this->m_offset, this->index(i), n_Vla::VlaKey{});
    }
    const_reference operator [] (size_type i) const &
    {
        return const_reference(this->m_offset, this->index(i), n_Vla::VlaKey{});
    }
    rvalue operator [] (size_type i) &&
    {
        return static_cast<rvalue>((*this)[i]);
    }
    const_rvalue operator [] (size_type i) const &&
    {
        return static_cast<const_rvalue>((*this)[i]);
    }

    reference at(size_type i) &
    {
        this->range_check(i);
        return (*this)[i];
    }
    const_reference at(size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }
    rvalue at(size_type i) &&
    {
        return static_cast<rvalue>(this->at(i));
    }
    const_rvalue at(size_type i) const &&
    {
        return static_cast<const_rvalue>(this->at(i));
    }


    const_iterator cbegin() const &
    {
        return const_iterator(this->m_offset, this->m_data, n_Vla::VlaKey{});
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }
    iterator begin() &
    {
        return iterator(this->m_offset, this->m_data);
    }

    const_iterator cend() const &
    {
        return const_iterator(this->m_offset, 
                              this->m_data + this->m_offset[dim],
                              n_Vla::VlaKey{});
    }
    const_iterator end() const &
    {
        return this->cend();
    }
    iterator end() &
    {
        return iterator(this->m_offset, this->m_data + this->m_offset[dim],
                        n_Vla::VlaKey{});
    }

    const_reverse_iterator crbegin() const &
    {
        return const_reverse_iterator(this->end());
    }
    const_reverse_iterator rbegin() const &
    {
        return this->crbegin();
    }
    reverse_iterator rbegin() &
    {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator crend() const &
    {
        return const_reverse_iterator(this->begin());
    }
    const_reverse_iterator rend() const &
    {
        return this->crend();
    }
    reverse_iterator rend() &
    {
        return reverse_iterator(this->begin());
    }

    const_iterator cbegin() const && = delete;
    const_iterator begin() const && = delete;
    const_iterator cend() const && = delete;
    const_iterator end() const && = delete;
    const_reverse_iterator crbegin() const && = delete;
    const_reverse_iterator rbegin() const && = delete;
    const_reverse_iterator crend() const && = delete;
    const_reverse_iterator rend() const && = delete;
};


template<typename Tp>
class Vla<Tp, 1> : private VlaImpl<Tp, 1>
{
  private:
    using Impl = VlaImpl<Tp, 1>;
    using typename Impl::Base;
    using typename Base::dim_type;

    static constexpr dim_type dim = 1;

    using ArgsBase = VlaArgHelper<Tp, dim>;

  public:
    using value_type                 =    Tp;
    using const_reference            =    const Tp&;
    using reference                  =    Tp&;
    using const_pointer              =    RelaxedPtr<const Tp>;
    using pointer                    =    RelaxedPtr<Tp>;
    using const_rvalue               =    const Tp&&;
    using rvalue                     =    Tp&&;
    using const_iterator             =    BasicIterator<const_pointer, Vla>;
    using iterator                   =    BasicIterator<pointer, Vla>;
    using const_reverse_iterator     =    std::reverse_iterator<const_iterator>;
    using reverse_iterator           =    std::reverse_iterator<iterator>;

    using typename Impl::VlaBase;
    using typename Base::size_type;

    using size_list_t                =    typename ArgsBase::size_list_t;
    using value_initializer          =    typename ArgsBase::value_initializer;
    using initializer_list           =    typename ArgsBase::initializer_list;



    Vla() = default;
    Vla(const Vla&) = default;
    Vla(Vla&&) noexcept = default;

    Vla& operator = (const Vla&) & = default;
    Vla& operator = (Vla&&) & noexcept = default;

    friend void swap(Vla &a, Vla &b) noexcept
    {
        return swap(static_cast<Impl&>(a), static_cast<Impl&>(b));
    }

    Vla(size_type s) : Impl(size_list_t{s}) {}

    Vla(size_type s, const Tp &value) 
    : Impl(VlaArgHelper<Tp, dim>::make_initializer(s, value)) {}

    Vla(initializer_list init_list) : Impl(init_list) {}



    using Base::size;
    using Base::empty;

    using Impl::operator[];
    using Impl::operator();
    using Impl::at;


    reference operator [] (size_type i) &
    {
        return this->m_data[i];
    }
    const_reference operator [] (size_type i) const &
    {
        return this->m_data[i];
    }
    rvalue operator [] (size_type i) &&
    {
        return static_cast<rvalue>(this->m_data[i]);
    }
    const_rvalue operator [] (size_type i) const &&
    {
        return static_cast<const_rvalue>(this->m_data[i]);
    }


    reference operator () (size_type i) &
    {
        return this->m_data[i];
    }
    const_reference operator () (size_type i) const &
    {
        return this->m_data[i];
    }
    rvalue operator () (size_type i) &&
    {
        return static_cast<rvalue>(this->m_data[i]);
    }
    const_rvalue operator () (size_type i) const &&
    {
        return static_cast<const_rvalue>(this->m_data[i]);
    }


    reference at (size_type i) &
    {
        this->range_check(i);
        return this->m_data[i];
    }
    const_reference at (size_type i) const &
    {
        this->range_check(i);
        return this->m_data[i];
    }
    rvalue at (size_type i) &&
    {
        return static_cast<rvalue>(this->at(i));
    }
    const_rvalue at (size_type i) const &&
    {
        return static_cast<const_rvalue>(this->at(i));
    }



    const_iterator cbegin() const &
    {
        return const_iterator(this->m_data);
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }
    iterator begin() &
    {
        return iterator(this->m_data);
    }

    const_iterator cend() const &
    {
        return const_iterator(this->m_data + this->m_offset[dim]);
    }
    const_iterator end() const &
    {
        return this->cend();
    }
    iterator end() &
    {
        return iterator(this->m_data + this->m_offset[dim]);
    }

    const_reverse_iterator crbegin() const &
    {
        return const_reverse_iterator(this->end());
    }
    const_reverse_iterator rbegin() const &
    {
        return this->crbegin();
    }
    reverse_iterator rbegin() &
    {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator crend() const &
    {
        return const_reverse_iterator(this->begin());
    }
    const_reverse_iterator rend() const &
    {
        return this->crend();
    }
    reverse_iterator rend() &
    {
        return reverse_iterator(this->begin());
    }

    const_iterator cbegin() const && = delete;
    const_iterator begin() const && = delete;
    const_iterator cend() const && = delete;
    const_iterator end() const && = delete;
    const_reverse_iterator crbegin() const && = delete;
    const_reverse_iterator rbegin() const && = delete;
    const_reverse_iterator crend() const && = delete;
    const_reverse_iterator rend() const && = delete;
};

template<typename tp>
class Vla<tp, 0>;




template<std::size_t d, typename Tp>
inline auto size(const Tp &V) -> decltype(V.template size<d>())
{
    return V.template size<d>();
}

template<typename Tp>
inline auto size(const Tp &V) -> decltype(V.size())
{
    return V.size();
}

template<std::size_t d = 0, typename Tp>
inline bool empty(const Tp &V)
{
    return !size(V);
}

}
#endif // VLA_HPP_INCLUDED
