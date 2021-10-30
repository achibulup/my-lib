#ifndef ITERATOR_WRAPPER_H_INCLUDED
#define ITERATOR_WRAPPER_H_INCLUDED
#include<iterator>
namespace Achibulup
{

    template<typename Iter, typename Container>
    class Iterator_wrapper
    {
      public:
        using iterator_category       =   typename std::iterator_traits<Iter>::iterator_category;
        using value_type              =   typename std::iterator_traits<Iter>::value_type;
        using difference_type         =   typename std::iterator_traits<Iter>::difference_type;
        using reference               =   typename std::iterator_traits<Iter>::reference;
        using pointer                 =   typename std::iterator_traits<Iter>::pointer;

        Iterator_wrapper() = default;
        Iterator_wrapper(const Iter &it) : i_base(it) {}
        Iterator_wrapper(Iter &&it) : i_base(std::move(it)) {}

        template<typename otherIter>
        Iterator_wrapper(const Iterator_wrapper<otherIter, Container> &cast) 
        : i_base(cast.base()) {}

        const Iter& base() const
        {
            return i_base;
        }

        ///input iter & forward iter requirements

        reference operator * () const
        {
            return *(this->i_base);
        }
        const Iter& operator -> () const
        {
            return this->i_base;
        }

        friend bool operator == (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        { return iterl.i_base == iterr.i_base; }
        friend bool operator != (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        { return iterl.i_base != iterr.i_base; }

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

        ///bidirectional iter requirements
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

        ///random access iter requirements
        reference operator [] (difference_type i) const
        {
            return *(*this + i);
        }

        friend bool operator > (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        { return iterl.i_base > iterr.i_base; }
        friend bool operator >= (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        { return iterl.i_base >= iterr.i_base; }
        friend bool operator < (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        { return iterl.i_base < iterr.i_base; }
        friend bool operator <= (const Iterator_wrapper &iterl, const Iterator_wrapper &iterr)
        { return iterl.i_base <= iterr.i_base; }

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

      private:
        Iter i_base;
    };
}


#endif // ITERATOR_WRAPPER_H_INCLUDED
