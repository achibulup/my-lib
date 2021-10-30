#ifndef ITERATOR_WRAPPER_H_INCLUDED
#define ITERATOR_WRAPPER_H_INCLUDED
#include<iterator>
namespace Achibulup
{

    template<typename uiter, typename container_type>
    class Iterator_wrapper
    {
        uiter i_base;

    public:
        using iterator_category       =   typename std::iterator_traits<uiter>::iterator_category;
        using value_type              =   typename std::iterator_traits<uiter>::value_type;
        using difference_type         =   typename std::iterator_traits<uiter>::difference_type;
        using reference               =   typename std::iterator_traits<uiter>::reference;
        using pointer                 =   typename std::iterator_traits<uiter>::pointer;

        Iterator_wrapper() = default;
        Iterator_wrapper(const uiter &it) : i_base(it) {}
        Iterator_wrapper(uiter &&it) : i_base(static_cast<uiter&&>(it)) {}
        template<typename other_iter>
        Iterator_wrapper(const Iterator_wrapper<other_iter, container_type> &cast) : i_base(cast.base()) {}

        reference operator * () const
        {
            return *(this->i_base);
        }
        const uiter& operator -> () const
        {
            return this->i_base;
        }
        reference operator [] (difference_type i) const
        {
            return *(*this + i);
        }

        friend bool operator == (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        {
            return iterl.i_base == iterr.i_base;
        }
        friend bool operator != (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        {
            return iterl.i_base != iterr.i_base;
        }
        friend bool operator > (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        {
            return iterl.i_base > iterr.i_base;
        }
        friend bool operator >= (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        {
            return iterl.i_base >= iterr.i_base;
        }
        friend bool operator < (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        {
            return iterl.i_base < iterr.i_base;
        }
        friend bool operator <= (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        {
            return iterl.i_base <= iterr.i_base;
        }

        Iterator_wrapper& operator ++ ()
        {
            ++(this->i_base);
            return *this;
        }
        Iterator_wrapper operator ++ (int)
        {
            Iterator_wrapper store = *this;
            ++(*this);
            return store;
        }
        const uiter& base() const
        {
            return i_base;
        }

        Iterator_wrapper& operator -- ()
        {
            --(this->i_base);
            return *this;
        }
        Iterator_wrapper operator -- (int)
        {
            Iterator_wrapper store = *this;
            --(*this);
            return store;
        }

        Iterator_wrapper& operator += (difference_type d)
        {
            this->i_base += d;
            return *this;
        }
        Iterator_wrapper& operator -= (difference_type d)
        {
            this->i_base -= d;
            return *this;
        }
        friend Iterator_wrapper operator + (const Iterator_wrapper &iter, difference_type d)
        {
            return Iterator_wrapper(iter.i_base + d);
        }
        friend Iterator_wrapper operator + (difference_type d, const Iterator_wrapper &iter)
        {
            return Iterator_wrapper(d + iter.i_base);
        }
        friend Iterator_wrapper operator - (const Iterator_wrapper &iter, difference_type d)
        {
            return Iterator_wrapper(iter.i_base - d);
        }
        friend difference_type operator - (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        {
            return iterl.i_base - iterr.i_base;
        }
    };
}


#endif // ITERATOR_WRAPPER_H_INCLUDED
