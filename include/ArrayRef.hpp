#ifndef ARRAYREF_HPP_INCLUDED
#define ARRAYREF_HPP_INCLUDED
#include <type_traits>

namespace Achibulup
{

template<typename Tp>
using Array2dAccessor = Tp& (*)(const void*, std::ptrdiff_t, std::ptrdiff_t);

template<typename Array2d, typename Elem>
auto accessArray2d(const void *arr_ptr, std::ptrdiff_t row, std::ptrdiff_t col)  
-> decltype(static_cast<Elem&>(std::declval<Array2d&>()[row][col]))
{
    /// the pointed to array may be non-const
    auto &arr = *const_cast<Array2d*>(static_cast<const Array2d*>(arr_ptr));
    return arr[row][col];
}

template<typename Array2d, typename Elem>
constexpr Array2dAccessor<Elem> getArray2dAccessor() noexcept
{
    return &accessArray2d<typename std::remove_reference<Array2d>::type, Elem>;
}



template<typename Tp>
class Array2dRef
{
  public:
    Array2dRef(const Array2dRef &other) noexcept = default;

    template<typename Array2d>
    Array2dRef(Array2d &&arr) noexcept : 
    m_array_ptr(&arr), m_accessor(getArray2dAccessor<Array2d, Tp>()) {}

    void operator = (const Array2dRef &other) = delete;

    Tp& access(std::ptrdiff_t row, std::ptrdiff_t col) const
    {
        return this->m_accessor(this->m_array_ptr, row, col);
    }
    
    class IndexProxy
    {
      public:
        IndexProxy(const Array2dRef &arr, std::ptrdiff_t row) noexcept
        : m_arr(arr), m_row(row) {}

        Tp& operator [] (std::ptrdiff_t col) const
        {
            return this->m_arr.access(this->m_row, col);
        }

      private:
        const Array2dRef &m_arr;
        std::ptrdiff_t m_row;
    };

    IndexProxy operator [] (std::ptrdiff_t row) const
    {
        return IndexProxy(*this, row);
    }

  private:
    const void *m_array_ptr;
    Array2dAccessor<Tp> m_accessor;
};

}

#endif // ARRAYREF_HPP_INCLUDED