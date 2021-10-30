#ifndef VLA_H_INCLUDED
#define VLA_H_INCLUDED
#include <new>
#include <array>
#include <iterator>
#include <exception>
#include <type_traits>
#include <initializer_list>
#include "Iterator_wrapper.h"
#include "common_utils.h"
namespace Achibulup
{

template<typename Tp, std::size_t dim>
class Vla_base;
template<typename Tp, std::size_t dim>
class Vla;
template<typename Tp, std::size_t dim>
class Vla_lproxy;
template<typename Tp, std::size_t dim>
class Vla_clproxy;
template<typename Tp, std::size_t dim>
class Vla_rproxy;
template<typename Tp, std::size_t dim>
class Vla_crproxy;
template<typename Tp, std::size_t dim>
class Vla_ptr;
template<typename Tp, std::size_t dim>
class const_Vla_ptr;


namespace n_Vla_common
{
    using size_type = size_t;
    using dim_type = std::size_t;
    inline void maximize(size_type &var, size_type val)
    {
        if (var < val) var = val;
    }
    inline void throw_Vla_out_of_range(size_type idx, size_type sz)
    {
        throw std::out_of_range(std::string("Vla indexing out of range : i (which is ") + std::to_string(idx)
                    + "), is not in range [0, size()) (which is [0, " + std::to_string(sz) + "))");
    };
}




template<typename Tp, std::size_t dim>
class Vla_base
{
  public:
    using size_type = n_Vla_common::size_type;

  protected:
    using dim_type = n_Vla_common::dim_type;
    using offset_ptr_t = const size_type*;
    using const_data_ptr_t = RelaxedPtr<const Tp>;
    using data_ptr_t = RelaxedPtr<Tp>;

    Vla_base() noexcept = default;
    Vla_base(offset_ptr_t offset, data_ptr_t data) 
    : i_offset(offset), i_data(data) {}

    Vla_base(const Vla_base&) noexcept = default;
    Vla_base& operator = (const Vla_base&) = delete;

  public:
    friend bool operator == (const Vla_base &l, const Vla_base &r)
    {
        for(dim_type i = 0; i <= dim; ++i)
          if (l.i_offset[i] != r.i_offset[i]) return false;
        const_data_ptr_t curl = l.i_data, curr = r.i_data;
        for(size_type i = 0, cnt = l.i_offset[dim]; i < cnt; ++i)
          if (*(curl++) != *(curr++)) return false;
        return true;
    }
    friend bool operator != (const Vla_base &l, const Vla_base &r)
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
        return !this->i_offset[dim];
    }

  protected:
    size_type size(size_type i_d) const
    {
        return this->i_offset[i_d] / this->i_offset[i_d - 1];
    }

    void range_check(dim_type i_d, size_type i) const
    {
        if (i >= this->size(i_d) || i < 0) 
          n_Vla_common::throw_Vla_out_of_range(i, this->size(i_d));
    }
    void range_check(size_type i) const
    {
        return range_check(dim, i);
    }

    data_ptr_t index(size_type i) const
    {
        return this->i_data + this->i_offset[dim - 1] * i;
    }
    data_ptr_t index(dim_type d, size_type i) const
    {
        return this->i_data + this->i_offset[d - 1] * i;
    }


    offset_ptr_t i_offset;
    data_ptr_t i_data;
};



template<typename Tp, std::size_t dim>
class Vla_proxy : protected Vla_base<Tp, dim>
{
  protected:
    using Base = Vla_base<Tp, dim>;
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


    Vla_proxy() = delete;
    
    Vla_proxy(const Base& sub) noexcept : Base(sub) {}
    using Base::Base;
    Vla_proxy(const Vla_proxy&) noexcept = default;

    Vla_proxy& operator = (const Vla_proxy&) = delete;

  public:
    reference operator [] (size_type i) const &
    {
        return reference(this->i_offset, this->index(i));
    }

    reference at(size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }



    const_iterator cbegin() const &
    {
        return const_iterator(const_pointer(this->i_offset, this->i_data));
    }
    iterator begin() const &
    {
        return iterator(pointer(this->i_offset, this->i_data));
    }

    const_iterator cend() const &
    {
        return const_iterator(const_pointer(this->i_offset, this->i_data + this->i_offset[dim]));
    }
    iterator end() const &
    {
        return iterator(pointer(this->i_offset, this->i_data + this->i_offset[dim]));
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
class Vla_proxy<Tp, 1> : protected Vla_base<Tp, 1>
{
  protected:
    using Base = Vla_base<Tp, 1>;
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


    Vla_proxy() = delete;
    
    Vla_proxy(const Base& sub) noexcept : Base(sub) {}
    using Base::Base;
    Vla_proxy(const Vla_proxy&) noexcept = default;

    Vla_proxy& operator = (const Vla_proxy&) = delete;

  public:
    reference operator [] (size_type i) const &
    {
        return this->i_data[i];
    }

    reference at (size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }


    const_iterator cbegin() const &
    {
        return const_iterator(this->i_data);
    }
    iterator begin() const &
    {
        return iterator(this->i_data);
    }

    const_iterator cend() const &
    {
        return const_iterator(this->i_data + this->i_offset[1]);
    }
    iterator end() const &
    {
        return iterator(this->i_data + this->i_offset[1]);
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
class Vla_proxy<Tp, 0>;

template<typename Tp, std::size_t dim>
class Vla_cproxy : protected Vla_base<Tp, dim>
{
  protected:
    using Base = Vla_base<Tp, dim>;
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

    Vla_cproxy() = delete;
    
    Vla_cproxy(const Base& sub) noexcept : Base(sub) {}
    using Base::Base;
    Vla_cproxy(const Vla_cproxy&) noexcept = default;

    Vla_cproxy& operator = (const Vla_cproxy&) = delete;

  public:
    const_reference operator [] (size_type i) const &
    {
        return const_reference(this->i_offset, this->index(i));
    }

    const_reference at (size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }



    const_iterator cbegin() const &
    {
        return const_iterator(const_pointer(this->i_offset, this->i_data));
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }

    const_iterator cend() const &
    {
        return const_iterator(const_pointer(this->i_offset, this->i_data + this->i_offset[dim]));
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
class Vla_cproxy<Tp, 1> : protected Vla_base<Tp, 1>
{
  protected:
    using Base = Vla_base<Tp, 1>;
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


    Vla_cproxy() = delete;
    
    Vla_cproxy(const Base& sub) noexcept : Base(sub) {}
    using Base::Base;
    Vla_cproxy(const Vla_cproxy&) noexcept = default;

    Vla_cproxy& operator = (const Vla_cproxy&) = delete;


  public:
    const_reference operator [] (size_type i) const &
    {
        return this->i_data[i];
    }

    const_reference at (size_type i) const &
    {
        this->range_check(i);
        return (*this)[i];
    }


    const_iterator cbegin() const &
    {
        return const_iterator(this->i_data);
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }

    const_iterator cend() const &
    {
        return const_iterator(this->i_data + this->i_offset[1]);
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
class Vla_cproxy<Tp, 0>;



template<typename Tp, std::size_t dim>
class Vla_lproxy : private Vla_proxy<Tp, dim>
{
    using pBase = Vla_proxy<Tp, dim>;
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

    using typename pBase::Vla_base;
    using typename pBase::Base;

  private:
    using pBase::pBase;

    friend Vla_lproxy Vla<Tp, dim + 1>::operator[](size_type)&;
    friend Vla_lproxy Vla_proxy<Tp, dim + 1>::operator[](size_type)const&;
    friend Vla_lproxy Vla_ptr<Tp, dim>::operator*()const;

  public:
    Vla_lproxy(const Vla_rproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    Vla_lproxy(const Vla_rproxy<Tp, dim> &&) = delete;


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
class Vla_lproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class Vla_clproxy : private Vla_cproxy<Tp, dim>
{
    using pBase = Vla_cproxy<Tp, dim>;
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

    using typename pBase::Vla_base;
    using typename pBase::Base;

  private:
    using pBase::pBase;


    friend Vla_clproxy Vla<Tp, dim + 1>::operator[](size_type)const&;
    friend Vla_clproxy Vla_cproxy<Tp, dim + 1>::operator[](size_type)const&;
    friend Vla_clproxy const_Vla_ptr<Tp, dim>::operator*()const;

  public:
    Vla_clproxy(const Vla_lproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    Vla_clproxy(const Vla_crproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    Vla_clproxy(const Vla_crproxy<Tp, dim> &&) = delete;
    Vla_clproxy(const Vla_rproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    Vla_clproxy(const Vla_rproxy<Tp, dim> &&) = delete;

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
class Vla_clproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class Vla_rproxy : private Vla_proxy<Tp, dim>
{
    using pBase = Vla_proxy<Tp, dim>;
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

    using typename pBase::Vla_base;
    using typename pBase::Base;

  private:
    using pBase::pBase;

    friend class Vla<Tp, dim + 1>;

  public:
    explicit Vla_rproxy(const Vla_lproxy<Tp, dim> &cast) noexcept 
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
class Vla_rproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class Vla_crproxy : private Vla_cproxy<Tp, dim>
{
    using pBase = Vla_cproxy<Tp, dim>;
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

    using typename pBase::Vla_base;
    using typename pBase::Base;

  private:
    using pBase::pBase;

    friend class Vla<Tp, dim + 1>;

  public:
    Vla_crproxy(const Vla_rproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    explicit Vla_crproxy(const Vla_clproxy<Tp, dim> &cast) noexcept 
    : pBase(static_cast<const Base&>(cast)) {}
    explicit Vla_crproxy(const Vla_lproxy<Tp, dim> &cast) noexcept 
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
class Vla_rproxy<tp, 0>;


template<typename Tp, std::size_t dim>
class Vla_ptr
{
    using dim_type = std::size_t;
    using size_type          =  typename Vla<Tp, dim>::size_type;

  public:
    using iterator_category  =  std::random_access_iterator_tag;
    using difference_type    =  size_type;
    using value_type         =  Vla_base<Tp, dim>;
    using reference          =  Vla_lproxy<Tp, dim>;
    using pointer = struct{
        reference ref;
        reference* operator -> () &&
        {
            return &(this->ref);
        }
    };

  private:
    const size_type *i_offset;
    RelaxedPtr<Tp> i_data;

    Vla_ptr(const size_type *ofs, RelaxedPtr<Tp> p) : i_offset(ofs), i_data(p) {}

    friend class Vla<Tp, dim + 1>;
    friend class Vla_proxy<Tp, dim + 1>;
    friend class const_Vla_ptr<Tp, dim>;

  public:
    Vla_ptr() noexcept = default;
    Vla_ptr(const Vla_ptr&) noexcept = default;
    Vla_ptr& operator = (const Vla_ptr&) & noexcept = default;


    reference operator * () const
    {
        return reference(this->i_offset, this->i_data);
    }

    pointer operator -> () const
    {
        return pointer{*(*this)};
    }

    Vla_ptr& operator ++ () &
    {
        this->i_data += this->i_offset[dim];
        return *this;
    }
    Vla_ptr operator ++ (int) &
    {
        Vla_ptr store = *this;
        ++(*this);
        return store;
    }


    Vla_ptr& operator -- () &
    {
        this->i_data -= this->i_offset[dim];
        return *this;
    }
    Vla_ptr operator -- (int) &
    {
        Vla_ptr store = *this;
        --(*this);
        return store;
    }


    reference operator [] (difference_type i) const
    {
        return *((*this) + i);
    }

    friend Vla_ptr operator + (const Vla_ptr &Ptr, difference_type i)
    {
        return Vla_ptr(Ptr.i_offset, Ptr.ptr + Ptr.i_offset[dim] * i);
    }
    friend Vla_ptr operator + (difference_type i, const Vla_ptr &Ptr)
    {
        return Vla_ptr(Ptr.i_offset, Ptr.ptr + Ptr.i_offset[dim] * i);
    }
    friend Vla_ptr operator - (const Vla_ptr &Ptr, difference_type i)
    {
        return Vla_ptr(Ptr.i_offset, Ptr.ptr - Ptr.i_offset[dim] * i);
    }
    friend difference_type operator - (const Vla_ptr &Ptr1, const Vla_ptr &Ptr2)
    {
        return (Ptr1.ptr - Ptr2.ptr) / Ptr1.i_offset[dim];
    }

    Vla_ptr& operator += (difference_type i) &
    {
        this->i_data += this->i_offset[dim] * i;
        return *this;
    }
    Vla_ptr& operator -= (difference_type i) &
    {
        this->i_data -= this->i_offset[dim] * i;
        return *this;
    }

    friend bool operator == (const Vla_ptr &l, const Vla_ptr &r)
    {  return l.ptr == r.ptr;  }
    friend bool operator != (const Vla_ptr &l, const Vla_ptr &r)
    {  return l.ptr != r.ptr;  }
    friend bool operator > (const Vla_ptr &l, const Vla_ptr &r)
    {  return l.ptr > r.ptr;  }
    friend bool operator >= (const Vla_ptr &l, const Vla_ptr &r)
    {  return l.ptr >= r.ptr;  }
    friend bool operator < (const Vla_ptr &l, const Vla_ptr &r)
    {  return l.ptr < r.ptr;  }
    friend bool operator <= (const Vla_ptr &l, const Vla_ptr &r)
    {  return l.ptr <= r.ptr;  }
};


template<typename Tp>
class Vla_ptr<Tp, 0>;



template<typename Tp, std::size_t dim>
class const_Vla_ptr
{
    using dim_type = std::size_t;
    using size_type          =  typename Vla<Tp, dim>::size_type;

  public:
    using iterator_category  =  std::random_access_iterator_tag;
    using difference_type    =  size_type;
    using value_type         =  Vla_base<Tp, dim>;
    using const_reference    =  Vla_clproxy<Tp, dim>;
    using reference          =  const_reference;
    using const_pointer = struct{
        const_reference ref;
        const_reference* operator -> () &&
        {
            return &(this->ref);
        }
    };
    using pointer = const_pointer;

  private:
    const size_type *i_offset;
    RelaxedPtr<Tp> i_data;

    const_Vla_ptr(const size_type *ofs, RelaxedPtr<Tp> p) : i_offset(ofs), i_data(p) {}

    friend class Vla<Tp, dim + 1>;
    friend class Vla_proxy<Tp, dim + 1>;
    friend class Vla_cproxy<Tp, dim + 1>;

  public:
    const_Vla_ptr() noexcept = default;
    const_Vla_ptr(const const_Vla_ptr&) noexcept = default;
    const_Vla_ptr& operator = (const const_Vla_ptr&) & noexcept = default;

    const_Vla_ptr(const Vla_ptr<Tp, dim>& cast) : i_offset(cast.i_offset), i_data(cast.i_data) {}


    const_reference operator * () const
    {
        return const_reference(this->i_offset, this->i_data);
    }

    pointer operator -> () const
    {
        return pointer{*(*this)};
    }

    const_Vla_ptr& operator ++ () &
    {
        this->i_data += this->i_offset[dim];
        return *this;
    }
    const_Vla_ptr operator ++ (int) &
    {
        const_Vla_ptr store = *this;
        ++(*this);
        return store;
    }


    const_Vla_ptr& operator -- () &
    {
        this->i_data -= this->i_offset[dim];
        return *this;
    }
    const_Vla_ptr operator -- (int) &
    {
        const_Vla_ptr store = *this;
        --(*this);
        return store;
    }


    reference operator [] (size_type i) const
    {
        return *(*this + i);
    }

    friend const_Vla_ptr operator + (const const_Vla_ptr &Ptr, difference_type i)
    {
        return const_Vla_ptr(Ptr.i_offset, Ptr.ptr + Ptr.i_offset[dim] * i);
    }
    friend const_Vla_ptr operator + (difference_type i, const const_Vla_ptr &Ptr)
    {
        return const_Vla_ptr(Ptr.i_offset, Ptr.ptr + Ptr.i_offset[dim] * i);
    }
    friend const_Vla_ptr operator - (const const_Vla_ptr &Ptr, difference_type i)
    {
        return const_Vla_ptr(Ptr.i_offset, Ptr.ptr - Ptr.i_offset[dim] * i);
    }
    friend difference_type operator - (const const_Vla_ptr &Ptr1, const const_Vla_ptr &Ptr2)
    {
        return (Ptr1.ptr - Ptr2.ptr) / Ptr1.i_offset[dim];
    }

    const_Vla_ptr& operator += (difference_type i) &
    {
        this->i_data += this->i_offset[dim] * i;
        return *this;
    }
    const_Vla_ptr& operator -= (difference_type i) &
    {
        this->i_data -= this->i_offset[dim] * i;
        return *this;
    }

    friend bool operator == (const const_Vla_ptr &l, const const_Vla_ptr &r)
    {  return l.ptr == r.ptr;  }
    friend bool operator !=(const const_Vla_ptr &l, const const_Vla_ptr &r)
    {  return l.ptr != r.ptr;  }
    friend bool operator > (const const_Vla_ptr &l, const const_Vla_ptr &r)
    {  return l.ptr > r.ptr;  }
    friend bool operator >= (const const_Vla_ptr &l, const const_Vla_ptr &r)
    {  return l.ptr >= r.ptr;  }
    friend bool operator < (const const_Vla_ptr &l, const const_Vla_ptr &r)
    {  return l.ptr < r.ptr;  }
    friend bool operator <= (const const_Vla_ptr &l, const const_Vla_ptr &r)
    {  return l.ptr <= r.ptr;  }
};


template<typename Tp>
class const_Vla_ptr<Tp, 0>;



#if ACHIBULUP__Cpp17_later

template<std::size_t dim, std::size_t...seq>
size_t Vla_index_helper(const size_t(&idx)[dim], const size_t *i_offset
                            , std::integer_sequence<std::size_t, seq...>)
{
    return (... + (idx[seq] * i_offset[dim - 1 - seq]));
}

#endif // __cplusplus

template<typename Tp, std::size_t dim>
struct VlaArgHelper;

template<typename Tp, std::size_t dim>
struct Vla_initializer
{
    std::array<size_t, dim> dimensions;
    std::initializer_list<Vla_initializer<Tp, dim - 1>> i_data;
    Vla_initializer(std::initializer_list<Vla_initializer<Tp, dim - 1>> il) 
    : dimensions{}, i_data(il)
    {
        dimensions[dim - 1] = i_data.size();
        for(auto &sub : i_data)
        for(std::size_t i = 0; i < dim - 1; ++i)
          n_Vla_common::maximize(dimensions[i], sub.dimensions[i]);
    }
};
template<typename Tp>
struct Vla_initializer<Tp, 1>
{
    std::array<size_t, 1> dimensions;
    std::initializer_list<Tp> i_data;
    Vla_initializer(std::initializer_list<Tp> il) 
    : dimensions{static_cast<size_t>(il.size())}, i_data(il) {}
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
          VlaListInitHelper<Tp, dim - 1>::construct(pos, sub.i_data, dimensions);
          pos += dimensions[dim - 1];
        }
        constructor<Tp>::construct(en - pos, pos);
    }
};
template<typename Tp, std::size_t dim>
struct VlaListInitHelper<Tp, dim, false>
{
    using initializer_list = std::initializer_list<Vla_initializer<Tp, dim - 1>>;

    static void construct(RelaxedPtr<Tp> pos, initializer_list init_list, const size_t *dimensions)
    {
        typename constructor<Tp>::exception_safety p{pos, pos};
        RelaxedPtr<Tp> en = pos + dimensions[dim];
        for(auto &sub : init_list){
          VlaListInitHelper<Tp, dim - 1>::construct(p.last, sub.i_data, dimensions);
          p.last += dimensions[dim - 1];
        }
        constructor<Tp>::construct(en - p.last, p.last);
        p.last = p.first;
    }
};
template<typename Tp>
struct VlaListInitHelper<Tp, 1, true>
{
    using initializer_list = std::initializer_list<Tp>;

    static void construct(RelaxedPtr<Tp> pos, initializer_list init_list, const size_t *dimensions)
    {
        constructor<Tp>::copy_construct(init_list.size(), pos, init_list.begin());
        constructor<Tp>::construct(dimensions[1] - init_list.size(), pos + init_list.size());
    }
};
template<typename Tp>
struct VlaListInitHelper<Tp, 1, false>
{
    using initializer_list = std::initializer_list<Tp>;

    static void construct(RelaxedPtr<Tp> pos, initializer_list init_list, const size_t *dimensions)
    {
        typename constructor<Tp>::exception_safety p{pos, pos};
        constructor<Tp>::copy_construct(init_list.size(), pos, init_list.begin());
        p.last += init_list.size();
        constructor<Tp>::construct(dimensions[1] - init_list.size(), pos + init_list.size());
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
    using size_list_t = std::array<typename Vla_base<Tp, dim>::size_type, dim>;
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
class Vla_impl : protected Vla_base<Tp, dim>
{
  protected:
    using Base = Vla_base<Tp, dim>;
    using typename Base::size_type;
    using typename Base::dim_type;
    using typename Base::offset_ptr_t;
    using typename Base::const_data_ptr_t;
    using typename Base::data_ptr_t;
    
    using size_list_t       = typename VlaArgHelper<Tp, dim>::size_list_t;
    using value_initializer = typename VlaArgHelper<Tp, dim>::value_initializer;
    using initializer_list  = typename VlaArgHelper<Tp, dim>::initializer_list;

    Vla_impl() noexcept : i_offset_base{1}
    {
        this->init();
    }

    Vla_impl(const size_list_t &size_list) 
    : i_offset_base(calc_offset(size_list))
    {
        this->init(this->i_offset_base[dim]);
    }
    Vla_impl(const value_initializer &init) 
    : i_offset_base(calc_offset(init))
    {
        this->init(this->i_offset_base[dim], init.value);
    }
    Vla_impl(initializer_list init_list)
    : i_offset_base(calc_offset(init_list))
    {
        this->init(this->i_offset_base[dim], init_list);
    }

    Vla_impl(Vla_impl &&mov) noexcept : Vla_impl()
    {
        swap(*this, mov);
    }
    Vla_impl(const Vla_impl &cpy) : i_offset_base(cpy.i_offset_base)
    {
        this->copy(cpy.i_data, cpy.i_offset[dim]);
    }

    Vla_impl& operator = (Vla_impl other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    friend void swap(Vla_impl &a, Vla_impl &b) noexcept
    {
        using std::swap;
        swap(a.i_data, b.i_data);
        swap(a.i_offset_base, b.i_offset_base);
    }

    ~Vla_impl()
    {
        if (this->i_data != data_ptr_t()){
            destructor<Tp>::destroy(this->i_data, this->i_offset[dim]);
            delete_buffer(this->i_data);
        }
    }

  public:
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    Tp& operator [] (const size_type (&i)[Nm]) &
    {
        return this->i_data[this->get_index(i)];
    }
    template<std::size_t Nm, EnableIf_t<Nm == dim>* = nullptr>
    const Tp& operator [] (const size_type (&i)[Nm]) const &
    {
        return this->i_data[this->get_index(i)];
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
            delete_buffer(ptr);
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
            n_Vla_common::maximize(res[i], val.dimensions[i - 1]);
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
        this->i_offset = this->i_offset_base.data();
    }

    void init() &
    {
        set_offset_ptr();
        this->i_data = {};
    }
    void init(size_type len) &
    {
        set_offset_ptr();
        safe_arr safe{new_buffer<Tp>(len)};
        this->i_data = safe.ptr;
        constructor<Tp>::construct(len, this->i_data);
        safe.ptr = nullptr;
    }
    void init(size_type len, const Tp &value) &
    {
        set_offset_ptr();
        safe_arr safe{new_buffer<Tp>(len)};
        this->i_data = safe.ptr;
        constructor<Tp>::construct(len, this->i_data, value);
        safe.ptr = nullptr;
    }
    void init(size_type len, initializer_list init_list) &
    {
        set_offset_ptr();
        safe_arr safe{new_buffer<Tp>(len)};
        this->i_data = safe.ptr;
        VlaListInitHelper<Tp, dim>::construct(this->i_data, init_list, this->i_offset);
        safe.ptr = nullptr;
    }

    void copy(const_data_ptr_t cpy, size_type len) &
    {
        set_offset_ptr();
        safe_arr safe{new_buffer<Tp>(len)};
        this->i_data = safe.ptr;
        constructor<Tp>::copy_construct(len, this->i_data, cpy);
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
        return Vla_index_helper(i, this->i_offset, 
                                std::make_index_sequence<dim>());
    }
#else
    size_type get_index(const size_type(&i)[dim])
    {
        size_type res = 0;
        for(dim_type d = dim; d != 0; --d)
          res += i[dim - d] * this->i_offset[d - 1];
        return res;
    }
#endif // __cplusplus



    offset_arr_t i_offset_base;
};
template<typename Tp>
struct Vla_impl<Tp, 0>;



template<typename Tp, std::size_t dim>
class Vla : private Vla_impl<Tp, dim>
{
  private:
    using Impl = Vla_impl<Tp, dim>;
    using typename Impl::Base;
    using typename Base::dim_type;
    using ArgsBase = VlaArgHelper<Tp, dim>;

  public:
    using typename Base::size_type;
    using value_type                 =    Vla_base<Tp, dim - 1>;
    using const_reference            =    Vla_clproxy<Tp, dim - 1>;
    using reference                  =    Vla_lproxy<Tp, dim - 1>;
    using const_rvalue               =    Vla_crproxy<Tp, dim - 1>;
    using rvalue                     =    Vla_rproxy<Tp, dim - 1>;
    using const_pointer              =    const_Vla_ptr<Tp, dim - 1>;
    using pointer                    =    Vla_ptr<Tp, dim - 1>;
    using const_iterator             =    const_pointer;
    using iterator                   =    pointer;
    using const_reverse_iterator     =    std::reverse_iterator<const_iterator>;
    using reverse_iterator           =    std::reverse_iterator<iterator>;

    using typename Impl::Vla_base;

    using size_list_t                = typename ArgsBase::size_list_t;
    using value_initializer          = typename ArgsBase::value_initializer;
    using initializer_list           = typename ArgsBase::initializer_list;
    



    Vla() = default;

    template<typename ...bound, typename = typename VlaArgHelper<Tp, dim>
                                 ::template IsSizeListArgs<bound...>>
    Vla(bound&&... s) : Impl(size_list_t{static_cast<size_type>(s)...}) {}

    template<typename ...Tps, typename = typename VlaArgHelper<Tp, dim>
                    ::template IsValInitzerArgs<Tps...>, typename = void>
    Vla(Tps&&... s) 
    : Impl(ArgsBase::make_initializer(std::forward<Tps>(s)...)) {}

    Vla(initializer_list il) : Impl(il) {}

    Vla(Vla&&) noexcept = default;
    Vla(const Vla&) = default;

    Vla& operator = (Vla&&) & noexcept = default;
    Vla& operator = (const Vla&) & = default;

    friend void swap(Vla &a, Vla &b) noexcept
    {
        swap(static_cast<Impl&>(a), static_cast<Impl&>(b));
    }



    using Base::size;
    using Base::empty;

    using Impl::operator[];
    using Impl::operator();
    using Impl::at;


    reference operator [] (size_type i) &
    {
        return reference(this->i_offset, this->index(i));
    }
    const_reference operator [] (size_type i) const &
    {
        return const_reference(this->i_offset, this->index(i));
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
        return const_iterator(const_pointer(this->i_offset, this->i_data));
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }
    iterator begin() &
    {
        return iterator(pointer(this->i_offset, this->i_data));
    }

    const_iterator cend() const &
    {
        return const_iterator(const_pointer(this->i_offset, this->i_data + this->i_offset[dim]));
    }
    const_iterator end() const &
    {
        return this->cend();
    }
    iterator end() &
    {
        return iterator(pointer(this->i_offset, this->i_data + this->i_offset[dim]));
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
class Vla<Tp, 1> : private Vla_impl<Tp, 1>
{
  private:
    using Impl = Vla_impl<Tp, 1>;
    using typename Impl::Base;
    using typename Base::dim_type;

    static constexpr dim_type dim = 1;

    using ArgsBase = VlaArgHelper<Tp, dim>;

  public:
    using typename Impl::Vla_base;
    using typename Base::size_type;
    using value_type                 =    Tp;
    using const_reference            =    const Tp&;
    using reference                  =    Tp&;
    using const_pointer              =    RelaxedPtr<const Tp>;
    using pointer                    =    RelaxedPtr<Tp>;
    using const_rvalue               =    const Tp&&;
    using rvalue                     =    Tp&&;
    using const_iterator             =    Iterator_wrapper<const_pointer, Vla>;
    using iterator                   =    Iterator_wrapper<pointer, Vla>;
    using const_reverse_iterator     =    std::reverse_iterator<const_iterator>;
    using reverse_iterator           =    std::reverse_iterator<iterator>;

    using size_list_t                = typename ArgsBase::size_list_t;
    using value_initializer          = typename ArgsBase::value_initializer;
    using initializer_list           = typename ArgsBase::initializer_list;



    Vla() = default;

    Vla(size_type s) : Impl(size_list_t{s}) {}

    Vla(size_type s, const Tp &value) 
    : Impl(VlaArgHelper<Tp, dim>::make_initializer(s, value)) {}

    Vla(initializer_list init_list) : Impl(init_list) {}

    Vla(Vla&&) noexcept = default;
    Vla(const Vla&) = default;

    Vla& operator = (Vla&&) & noexcept = default;
    Vla& operator = (const Vla&) & = default;

    friend void swap(Vla &a, Vla &b) noexcept
    {
        return swap(static_cast<Impl&>(a), static_cast<Impl&>(b));
    }


    using Base::size;
    using Base::empty;

    using Impl::operator[];
    using Impl::operator();
    using Impl::at;


    reference operator [] (size_type i) &
    {
        return this->i_data[i];
    }
    const_reference operator [] (size_type i) const &
    {
        return this->i_data[i];
    }
    rvalue operator [] (size_type i) &&
    {
        return static_cast<rvalue>(this->i_data[i]);
    }
    const_rvalue operator [] (size_type i) const &&
    {
        return static_cast<const_rvalue>(this->i_data[i]);
    }


    reference operator () (size_type i) &
    {
        return this->i_data[i];
    }
    const_reference operator () (size_type i) const &
    {
        return this->i_data[i];
    }
    rvalue operator () (size_type i) &&
    {
        return static_cast<rvalue>(this->i_data[i]);
    }
    const_rvalue operator () (size_type i) const &&
    {
        return static_cast<const_rvalue>(this->i_data[i]);
    }


    reference at (size_type i) &
    {
        this->range_check(i);
        return this->i_data[i];
    }
    const_reference at (size_type i) const &
    {
        this->range_check(i);
        return this->i_data[i];
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
        return const_iterator(this->i_data);
    }
    const_iterator begin() const &
    {
        return this->cbegin();
    }
    iterator begin() &
    {
        return iterator(this->i_data);
    }

    const_iterator cend() const &
    {
        return const_iterator(this->i_data + this->i_offset[dim]);
    }
    const_iterator end() const &
    {
        return this->cend();
    }
    iterator end() &
    {
        return iterator(this->i_data + this->i_offset[dim]);
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
#endif // VLA_H_INCLUDED
