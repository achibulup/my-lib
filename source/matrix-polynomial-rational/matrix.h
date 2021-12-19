#include <vector>
#include <iterator>
#include <algorithm>
#include <utility>
#include <array>
#include <type_traits>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include "math_common.h"
template<typename Cont1, typename Cont2>
class DuoIterator
{
  private:
    using Itr1 = decltype(std::begin(std::declval<Cont1&>()));
    using Itr2 = decltype(std::begin(std::declval<Cont2&>()));

  public:
    struct reference
    {
        decltype(*std::declval<Itr1>()) first;
        decltype(*std::declval<Itr2>()) second;
    };

    DuoIterator() = default;
    DuoIterator(Itr1 i1, Itr2 i2) : itr1(i1), itr2(i2) {}

    reference operator * () const
    {
        return {*itr1, *itr2};
    }

    bool operator == (const DuoIterator &rhs) const
    {
        return itr1 == rhs.itr1 || itr2 == rhs.itr2;
    }
    bool operator != (const DuoIterator &rhs) const
    {
        return !(*this == rhs);
    }

    DuoIterator& operator ++ () &
    {
        ++itr1;
        ++itr2;
        return *this;
    }
    DuoIterator operator ++ (int) &
    {
        DuoIterator res = *this;
        ++*this;
        return res;
    }

  private:
    Itr1 itr1;
    Itr2 itr2;
};
template<typename Cont1, typename Cont2>
class DuoView
{
  public:
    DuoView(Cont1 &&c1, Cont2 &&c2) : cont1(c1), cont2(c2) {}

    using iterator = DuoIterator<Cont1, Cont2>;

    iterator begin() const
    {
        return {std::begin(cont1), std::begin(cont2)};
    }
    iterator end() const
    {
        return {std::end(cont1), std::end(cont2)};
    }

  private:
    Cont1 cont1;
    Cont2 cont2;
};
template<typename Cont1, typename Cont2>
DuoView<Cont1, Cont2> view(Cont1 &&cont1, Cont2 &&cont2)
{
    return {std::forward<Cont1>(cont1),
            std::forward<Cont2>(cont2)};
}

struct MatrixIndex
{
    int i, j;
};
template<typename Tp>
class Matrix
{
  public:
    using Element = Tp;
    Matrix() : mRow(), m_col(), m_data() {}
    Matrix(int m, int n) : Matrix() { resize(m, n); }

    Matrix(std::initializer_list<std::initializer_list<Tp>> il)
    : Matrix(il.size(), il.begin()->size(), il) {}

    Matrix(int m, int n, std::initializer_list<std::initializer_list<Tp>> il)
    : Matrix(m, n)
    {
        int initRows = std::min<int>(m, il.size());
        auto row = begin(il);
        for(int i = 1; i <= initRows; ++i, ++row) {
          for(auto &&it : view(getRow(i), row))
            it.first = it.second;
        }
    }

    Matrix& operator = (Literal0) &
    {
        for(auto &elem : m_data)
          elem = 0;
        return *this;
    } 
    Matrix& operator = (Literal1) &
    {
        if (rows() != cols()) 
          throw std::invalid_argument("matrix is not square");
        return *this = identity(rows);
    }


    Matrix& resize(int m, int n) &
    {
        mRow = m, m_col = n;
        m_data.resize(m * n);
        return *this;
    }
    Matrix&& resize(int m, int n) &&
    {
        return std::move(resize(m, n));
    }

    static Matrix identity(int N)
    {
        Matrix res(N, N);
        for(int i = 1; i <= N; ++i)
          res[{i, i}] = lit1;
        return res;
    }

    int rows() const { return mRow; }
    int cols() const { return m_col; }

    decltype(auto) operator [] (MatrixIndex idx)
    {
        return m_data.at(getIndex(idx.i, idx.j));
    }
    decltype(auto) operator [] (MatrixIndex idx) const
    {
        return m_data.at(getIndex(idx.i, idx.j));
    }

    explicit operator bool () const
    {
        for(const Tp& val : m_data)
          if (val) return true;
        return false;
    }

    friend Matrix operator + (Matrix mtr)
    {
        return mtr;
    }
    friend Matrix operator - (Matrix mtr)
    {
        for(auto &elem : mtr.m_data)
          elem = -elem;
        return mtr;
    }

    void swapRow(int row1, int row2) &
    {
        using std::swap;
        swap(getRow(row1), getRow(row2));
    }
    void mulRow(int row, Tp val) &
    {
        for(Tp &elem : getRow(row))
          elem *= val;
    }
    void divRow(int row, Tp val) &
    {
        for(Tp &elem : getRow(row))
          elem /= val;
    }
    void addRow(int dest_row, int src_row) &
    {
        for(auto &&elem : view(getRow(dest_row), getRow(src_row)))
          elem.first += elem.second;
    }
    void addRow(int dest_row, int src_row, Tp mult) &
    {
        for(auto &&elem : view(getRow(dest_row), getRow(src_row)))
          elem.first += elem.second * mult;
    }
    void subRow(int dest_row, int src_row) &
    {
        for(auto &&elem : view(getRow(dest_row), getRow(src_row)))
          elem.first -= elem.second;
    }
    void subRow(int dest_row, int src_row, Tp mult) &
    {
        for(auto &&elem : view(getRow(dest_row), getRow(src_row)))
          elem.first -= elem.second * mult;
    }


    void swapCol(int col1, int col2) &
    {
        using std::swap;
        swap(getCol(col1), getCol(col2));
    }
    void mulCol(int col, Tp val) &
    {
        for(Tp &elem : getCol(col))
          elem *= val;
    }
    void divCol(int col, Tp val) &
    {
        for(Tp &elem : getCol(col))
          elem /= val;
    }
    void addCol(int dest_col, int src_col) &
    {
        for(auto &&elem : view(getCol(dest_col), getCol(src_col)))
          elem.first += elem.second;
    }
    void addCol(int dest_col, int src_col, Tp mult) &
    {
        for(auto &&elem : view(getCol(dest_col), getCol(src_col)))
          elem.first += elem.second * mult;
    }
    void subCol(int dest_col, int src_col) &
    {
        for(auto &&elem : view(getCol(dest_col), getCol(src_col)))
          elem.first -= elem.second;
    }
    void subCol(int dest_col, int src_col, Tp mult) &
    {
        for(auto &&elem : view(getCol(dest_col), getCol(src_col)))
          elem.first -= elem.second * mult;
    }
    

    friend Matrix operator * (Tp mul, const Matrix &mt)
    {
        Matrix res(mt.rows(), mt.cols());
        mulTo(mul, mt, &res);
        return res;
    }
    friend Matrix operator * (const Matrix &mt, Tp mul)
    {
        Matrix res(mt.rows(), mt.cols());
        mulTo(mt, mul, &res);
        return res;
    }
    friend Matrix operator + (const Matrix &lhs, const Matrix &rhs)
    {
        if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
          throw std::invalid_argument("mismatching Matrix size");
        Matrix res(lhs.rows(), lhs.cols());
        addTo(lhs, rhs, &res);
        return res;
    }
    friend Matrix operator - (const Matrix &lhs, const Matrix &rhs)
    {
        if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
          throw std::invalid_argument("mismatching Matrix size");
        Matrix res(lhs.rows(), lhs.cols());
        subTo(lhs, rhs, &res);
        return res;
    }
    friend Matrix operator * (const Matrix &lhs, const Matrix &rhs)
    {
        if (lhs.cols() != rhs.rows())
          throw std::invalid_argument("mismatching Matrix size");
        Matrix res(lhs.rows(), rhs.cols());
        mulTo(lhs, rhs, &res);
        return res;
    }
    friend Matrix& operator += (Matrix &lhs, const Matrix &rhs)
    {
        addTo(lhs, rhs, &lhs);
        return lhs;
    }
    friend Matrix& operator -= (Matrix &lhs, const Matrix &rhs)
    {
        subTo(lhs, rhs, &lhs);
        return lhs;
    }
    friend Matrix& operator *= (Matrix &lhs, const Matrix &rhs)
    {
        return lhs = lhs * rhs;
    }

    class Row
    {
      public:
        class iterator
        {
          public:
            iterator(Matrix &mtr, int row, int col)
            : data(&mtr), m(row), n(col) {}

            Tp& operator * () const
            {
                return Row(*data, m)[n];
            }
            friend bool operator == (iterator l, iterator r)
            {
                return l.data == r.data && l.m == r.m && l.n == r.n;
            }
            friend bool operator != (iterator l, iterator r)
            {
                return !(l == r);
            }
            iterator& operator ++ () &
            {
                ++n;
                return *this;
            }
            iterator operator ++ (int) &
            {
                iterator res = *this;
                ++*this;
                return res;
            }

          private:
            Matrix *data;
            int m, n;
        };

        Row(Matrix &mtr, int row)
        : data(&mtr), m(row) {}

        friend void swap(Row a, Row b)
        {
            using std::begin;
            using std::end;
            std::swap_ranges(begin(a), end(a), begin(b));
        }

        int size() const
        {
            return data->cols();
        }
        Tp& operator [] (int n) const
        {
            return (*data)[{m, n}];
        }
        iterator begin() const
        {
            return iterator(*data, m, 1);
        }
        iterator end() const
        {
            return iterator(*data, m, size() + 1);
        }

      private:
        Matrix *const data;
        const int m;
    };

    class Col
    {
      public:
        class iterator
        {
          public:
            iterator(Matrix &mtr, int row, int col)
            : data(&mtr), m(row), n(col) {}

            Tp& operator * () const
            {
                return Col(*data, n)[m];
            }
            friend bool operator == (iterator l, iterator r)
            {
                return l.data == r.data && l.m == r.m && l.n == r.n;
            }
            friend bool operator != (iterator l, iterator r)
            {
                return !(l == r);
            }
            iterator& operator ++ () &
            {
                ++m;
                return *this;
            }
            iterator operator ++ (int) &
            {
                iterator res = *this;
                ++*this;
                return res;
            }

          private:
            Matrix *data;
            int m, n;
        };

        Col(Matrix &mtr, int col)
        : data(&mtr), n(col) {}

        friend void swap(Col a, Col b)
        {
            using std::begin;
            using std::end;
            std::swap_ranges(begin(a), end(a), begin(b));
        }

        int size() const
        {
            return data->rows();
        }
        Tp& operator [] (int m) const
        {
            return (*data)[{m, n}];
        }
        iterator begin() const
        {
            return iterator(*data, 1, n);
        }
        iterator end() const
        {
            return iterator(*data, size() + 1, n);
        }

      private:
        Matrix *const data;
        const int n;
    };

    Row getRow(int i)
    {
        return Row(*this, i);
    }
    Col getCol(int i)
    {
        return Col(*this, i);
    }

  private:    
    static void mulTo(const Matrix &mt, Tp mul, Matrix *res)
    {
        int m = mt.rows(), n = mt.cols();
        for(int i = 1; i <= m; ++i)
        for(int j = 1; j <= n; ++j)
          (*res)[{i, j}] = mt[{i, j}] * mul;
    }
    static void mulTo(Tp mul, const Matrix &mt, Matrix *res)
    {
        int m = mt.rows(), n = mt.cols();
        for(int i = 1; i <= m; ++i)
        for(int j = 1; j <= n; ++j)
          (*res)[{i, j}] = mul * mt[{i, j}];
    }
    static void addTo(const Matrix &lhs, const Matrix &rhs, Matrix *res)
    {
        int m = lhs.rows(), n = lhs.cols();
        for(int i = 1; i <= m; ++i)
        for(int j = 1; j <= n; ++j)
          (*res)[{i, j}] = lhs[{i, j}] + rhs[{i, j}];
    }
    static void subTo(const Matrix &lhs, const Matrix &rhs, Matrix *res)
    {
        int m = lhs.rows(), n = lhs.cols();
        for(int i = 1; i <= m; ++i)
        for(int j = 1; j <= n; ++j)
          (*res)[{i, j}] = lhs[{i, j}] - rhs[{i, j}];
    }
    static void mulTo(const Matrix &lhs, const Matrix &rhs, Matrix *res)
    {
        int m = lhs.rows(), n = lhs.cols(), p = rhs.cols();
        for(int i = 1; i <= m; ++i)
        for(int j = 1; j <= p; ++j){
          Tp sum{};
          for(int k = 1; k <= n; ++k)
            sum += lhs[{i, k}] * rhs[{k, j}];
          (*res)[{i, j}] = sum;
        }
    }


    int getIndex(int i, int j) const
    {
        return (i - 1) * cols() + (j - 1);
    }


    int mRow, m_col;
    std::vector<Tp> m_data;
};
template<typename matrix>
using MatrixElem = typename std::remove_const_t<
                            std::remove_reference_t<matrix>>::Element;



template<typename Tp, int N>
class BoundMatrices
{
  private:
    using pointer = Matrix<Tp>*;
    using Data = std::array<pointer, N>;

  public:
    using Element = typename Matrix<Tp>::Element;
    BoundMatrices() = default;
    BoundMatrices(const Data &mtrs) : m_mtrs(mtrs) {}

    Matrix<Tp>& get(int idx) const
    {
        return *m_mtrs.at(idx);
    }

    int rows() const
    {
        return m_mtrs[0]->rows();
    }
    int cols() const
    {
        return m_mtrs[0]->cols();
    }

    decltype(auto) operator [] (const MatrixIndex &idx) const
    {
        return (*m_mtrs[0])[idx];
    }
    void swapRow(int row1, int row2) const
    {
        for(pointer ptr : m_mtrs)
          ptr->swapRow(row1, row2);
    }

    void mulRow(int row, Tp val) const
    {
        for(pointer ptr : m_mtrs)
          ptr->mulRow(row, val);
    }
    void divRow(int row, Tp val) const
    {
        for(pointer ptr : m_mtrs)
          ptr->divRow(row, val);
    }

    void addRow(int dest_row, int src_row) const
    {
        for(pointer ptr : m_mtrs)
          ptr->addRow(dest_row, src_row);
    }
    void addRow(int dest_row, int src_row, Tp mult) const
    {
        for(pointer ptr : m_mtrs)
          ptr->addRow(dest_row, src_row, mult);
    }
    void subRow(int dest_row, int src_row) const
    {
        for(pointer ptr : m_mtrs)
          ptr->subRow(dest_row, src_row);
    }
    void subRow(int dest_row, int src_row, Tp mult) const
    {
        for(pointer ptr : m_mtrs)
          ptr->subRow(dest_row, src_row, mult);
    }

  private:
    Data m_mtrs;
};
template<typename Tp, int N>
class CBoundMatrices
{
  private:
    using pointer = const Matrix<Tp>*;
    using Data = std::array<pointer, N>;

  public:
    using Element = typename Matrix<Tp>::Element;

    CBoundMatrices() = default;
    CBoundMatrices(const Data &mtrs) : m_mtrs(mtrs) {}
    CBoundMatrices(BoundMatrices<Tp, N> cpy)
    {
        for(int i = 0; i < N; ++i)
          this->m_mtrs.at(i) = &cpy.get(i);
    }

    const Matrix<Tp>& get(int idx) const
    {
        return *m_mtrs.at(idx);
    }

    int rows() const
    {
        return m_mtrs[0]->rows();
    }

  private:
    Data m_mtrs;
};

template<typename Tp, typename ...Mtrs>
BoundMatrices<Tp, sizeof...(Mtrs)+ 1>
bind(Matrix<Tp> &first, Mtrs& ...others)
{
    return {{&first, &others...}};
}

template<typename Tp, typename ...Mtrs>
CBoundMatrices<Tp, sizeof...(Mtrs)+ 1>
cbind(const Matrix<Tp> &first, const Mtrs& ...others)
{
    return {{&first, &others...}};
}

template<typename Tp>
Matrix<Tp> transpose(const Matrix<Tp> &mtr)
{
    int m = mtr.rows(), n = mtr.cols();
    Matrix<Tp> res(n, m);
    for(int i = 1; i <= m; ++i)
    for(int j = 1; j <= n; ++j)
      res[{j, i}] = mtr[{i, j}];
    return res;
}

template<typename Tp>
bool operator == (const Matrix<Tp> &lhs, const Matrix<Tp> &rhs) noexcept
{
    if(lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
      return false;
    int m = lhs.rows(), n = lhs.cols();
    for(int i = 1; i <= m; ++i)
    for(int j = 1; j <= n; ++j)
      if(lhs[{i, j}] != rhs[{i, j}])
        return false;
    return true;
}
template<typename Tp>
bool operator != (const Matrix<Tp> &lhs, const Matrix<Tp> &rhs) noexcept
{
    return !(lhs == rhs);
}

template<typename Tp>
bool operator == (const Matrix<Tp> &l, Literal0)
{
    return static_cast<bool>(l);
}
template<typename Tp>
bool operator == (Literal0, const Matrix<Tp> &r)
{
    return r == 0;
}
template<typename Tp>
bool operator != (const Matrix<Tp> &l, Literal0)
{
    return !(l == 0);
}
template<typename Tp>
bool operator != (Literal0, const Matrix<Tp> &r)
{
    return r != 0;
}
template<typename Tp>
bool operator == (const Matrix<Tp> &l, Literal1)
{
    return l.rows() == l.rows() && l == l.identity(l.rows());
}
template<typename Tp>
bool operator == (Literal1, const Matrix<Tp> &r)
{
    return r == lit1;
}
template<typename Tp>
bool operator != (const Matrix<Tp> &l, Literal1)
{
    return !(l == lit1);
}
template<typename Tp>
bool operator != (Literal1, const Matrix<Tp> &r)
{
    return r != lit1;
}





template<typename Tp,
  typename elem_type = typename std::remove_reference_t<Tp>::Element>
elem_type toRowEchelon(Tp &&mtr)
{
    elem_type det_mul = lit1;
    int m = mtr.rows(), n = mtr.cols();
    for(int i = 1, j = 1; i <= m && j <= n; ++j){
      if (!mtr[{i, j}]) {
        for(int k = i + 1; k <= m; ++k){
          if (mtr[{k, j}]) {
            mtr.swapRow(i, k);
            det_mul = -det_mul;
            break;
          }
        }
      }

      if (mtr[{i, j}]) {
        for(int k = i + 1; k <= m; ++k)
          if (mtr[{k, j}])
            mtr.addRow(k, i, -mtr[{k, j}] / mtr[{i, j}]);
        ++i;
      }
      else det_mul = 0;
    }
    return det_mul;
}
template<typename Tp,
  typename elem_type = typename std::remove_reference_t<Tp>::Element>
elem_type toReducedRowEchelon(Tp &&mtr)
{
    elem_type det_mul = toRowEchelon(mtr);
    int m = mtr.rows(), n = mtr.cols();
    for(int i = 1, j = 1; i <= m && j <= n; ++j)
      if (mtr[{i, j}]) {
        det_mul /= mtr[{i, j}];
        mtr.divRow(i, mtr[{i, j}]);
        for(int k = 1; k < i; ++k)
          if (mtr[{k, j}])
            mtr.addRow(k, i, -mtr[{k, j}]);
        ++i;
      }
    return det_mul;
}


template<typename Tp,
  typename elem_type = typename std::remove_reference_t<Tp>::Element>
elem_type toColEchelon(Tp &&mtr)
{
    elem_type det_mul = lit1;
    int m = mtr.rows(), n = mtr.cols();
    for(int i = 1, j = 1; i <= m && j <= n; ++i){
      if (!mtr[{i, j}]) {
        for(int k = j + 1; k <= n; ++k){
          if (mtr[{i, k}]) {
            mtr.swapCol(j, k);
            det_mul = -det_mul;
            break;
          }
        }
      }

      if (mtr[{i, j}]) {
        for(int k = j + 1; k <= n; ++k)
          if (mtr[{i, k}])
            mtr.addCol(k, j, -mtr[{i, k}] / mtr[{i, j}]);
        ++j;
      }
      else det_mul = 0;
    }
    return det_mul;
}
template<typename Tp,
  typename elem_type = typename std::remove_reference_t<Tp>::Element>
elem_type toReducedColEchelon(Tp &&mtr)
{
    elem_type det_mul = toColEchelon(mtr);
    int m = mtr.rows(), n = mtr.cols();
    for(int i = 1, j = 1; i <= m && j <= n; ++i)
      if (mtr[{i, j}]) {
        det_mul /= mtr[{i, j}];
        mtr.divCol(j, mtr[{i, j}]);
        for(int k = 1; k < j; ++k)
          if (mtr[{i, k}])
            mtr.addCol(k, j, -mtr[{i, k}]);
        ++j;
      }
    return det_mul;
}


template<typename Tp>
Tp det(Matrix<Tp> mtr)
{
    if (mtr.rows() != mtr.cols())
      throw std::invalid_argument("Matrix is not square");
    int n = mtr.rows();
    Tp res = toRowEchelon(mtr);
    for(int i = 1; i <= n; ++i)
      res *= mtr[{i, i}];
    return res;
}

template<typename Tp>
int rank(Matrix<Tp> mtr)
{
    toRowEchelon(mtr);
    int res = 0;
    for(int i = 1, j = 1; i <= mtr.rows() && j <= mtr.cols();)
    {
      if (mtr[{i, j}]) {
        ++res;
        ++i;
      }
      ++j;
    }
    return res;
}

template<typename Tp>
Matrix<Tp> inverse(Matrix<Tp> mtr)
{
    if (mtr.rows() != mtr.cols())
      throw std::invalid_argument("Matrix is not square");
    int n = mtr.rows();
    Matrix<Tp> res = Matrix<Tp>::identity(n);
    auto bmtr = bind(mtr, res);
    if (!toReducedRowEchelon(bmtr))
      throw std::logic_error("Matrix not invertible");
    return res;
}
template<typename Tp>
Matrix<Tp> pow(Matrix<Tp> base, std::uintmax_t exp)
{
    // if (exp < 0) return pow(inverse(base), -exp);
    if (base.rows() != base.cols())
      throw std::invalid_argument("Matrix is not square");
    int n = base.rows();
    if (exp == 0) return Matrix<Tp>::identity(n);
    while (exp % 2 == 0) {
      exp /= 2;
      base *= base;
    }
    Matrix<Tp> res = base;
    while(exp /= 2){
      base *= base;
      if (exp % 2 == 1)
        res *= base;
    }
    return res;
}

template<typename Tp>
int toStrLen(Tp x)
{
    std::stringstream ss;
    if (std::is_floating_point<Tp>())
      ss << std::setprecision(2) << std::fixed;
    ss << x;
    return ss.str().size();
}
template<typename Tp, int N>
void print(CBoundMatrices<Tp, N> mtr)
{
    if (std::is_floating_point<Tp>())
      std::cout << std::setprecision(1) << std::fixed;
    int m = mtr.rows();
    std::array<int, N> n;
    for(int i = 0; i < N; ++i)
      n[i] = mtr.get(i).cols();

    std::array<std::vector<std::vector<int>>, N> lens;
    std::array<std::vector<int>, N> max_len;
    for(int k = 0; k < N; ++k){
      lens[k].resize(m + 1);
      max_len[k].assign(n[k] + 1, 0);
      for(int i = 1; i <= m; ++i){
        lens[k].at(i).resize(n[k] + 1);
        for(int j = 1; j <= n[k]; ++j){
          lens[k].at(i).at(j) = toStrLen(mtr.get(k)[{i, j}]);
          max_len[k].at(j) = std::max(max_len[k].at(j), lens[k].at(i).at(j));
        }
      }
    }

    for(int i = 1; i <= m; ++i){
      std::cout << "|\332\300["[(i == 1) + 2 * (i == m)] << ' ';
      for(int k = 0; k < N; ++k)
      for(int j = 1; j <= n[k]; ++j){
        for(int cnt = max_len[k].at(j) - lens[k].at(i).at(j); cnt --> 0;)
          std::cout << ' ';
        std::cout << mtr.get(k)[{i, j}];
        if (j < n[k]) std::cout << ", ";
        else if (k + 1 < N) std::cout << " | ";
        else std::cout << ' ' << "|\277\331]"[(i == 1) + 2 * (i == m)];
      }
      std::cout << '\n';
    }
}
template<typename Tp, int N>
void print(BoundMatrices<Tp, N> mtr)
{
    print(CBoundMatrices<Tp, N>(mtr));
}
template<typename Tp>
void print(const Matrix<Tp> &mtr)
{
    print(cbind(mtr));
}
