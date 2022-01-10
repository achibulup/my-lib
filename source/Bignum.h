#ifndef BIGNUM_H_INCLUDED
#define BIGNUM_H_INCLUDED

//Big integer handling
//the two class Int and uInt provide support for big integer arithmetics
//Int can handle signed integer while uInt also supports bitwise operations for unsigned integer


#include <cmath> //log2
#include <string> //input
#include <limits> //unsigned long long max
#include <memory> //unique_ptr
#include <cstring> //memset, memcpy
#include <cstdint> //int types
#include <iostream> //in-out put
#include <algorithm> //max, min
#include <stdexcept> // exception
#include "common_utils.h"

namespace Achibulup{



namespace n_Int{

constexpr int ten = 10;


void throw_division_by_zero_exception()
{
    throw std::domain_error("division by zero");
}
void throw_unsigned_integer_underflow_exception()
{
    throw std::underflow_error("unsigned integer underflow");
}

using size_type = size_t;
using base_type = std::uint_least32_t;
using calc_type = std::uint_fast32_t;
using wcalc_type = std::uint_fast64_t;
using const_pointer = const base_type*;
using pointer = base_type*;

template<typename Tp>
using isIntegral_t = EnableIf_t<std::is_integral<Tp>::value>*;

class IntData
{
  public:
    using const_pointer = n_Int::const_pointer;
    using pointer = n_Int::pointer;

    IntData() noexcept : m_data(), size() {}
    explicit IntData(size_type cap) 
    : m_data(newArray(cap)), size(cap) {}

    IntData(IntData &&mov) noexcept
    : m_data(Move(mov.m_data)), size(Move(mov.size)) {}
    IntData& operator = (IntData &&mov) & noexcept
    {
        swap(*this, mov);
        return *this;
    }

    friend void swap(IntData &a, IntData &b) noexcept
    {
        doSwap(a, b);
    }


    base_type& operator[] (size_type i)
    {
        return this->m_data[i];
    }
    base_type operator[] (size_type i) const
    {
        return this->m_data[i];
    }


    const_pointer cbegin() const
    {
        return this->m_data.get();
    }
    const_pointer begin() const
    {
        return this->cbegin();
    }
    pointer begin()
    {
        return this->m_data.get();
    }


    void reset(size_type new_cap)
    {
        if(new_cap > this->size())
          *this = IntData(new_cap);
    }
    void reset()
    {
        IntData tmp;
        swap(tmp, *this);
    }


  private:
    static void doSwap(IntData &a, IntData &b) noexcept
    {
        using std::swap;
        swap(a.m_data, b.m_data);
        a.size.swap(b.size);
    }


    static pointer newArray(size_type len)
    { 
        return len ? new base_type[len] : pointer();
    }


    std::unique_ptr<base_type[]> m_data;

  public:
    ReadOnlyProperty<size_type, IntData> size;

};

template<typename Tp, typename Tp1>
constexpr Tp1 ceilDiv(const Tp &lhs, const Tp1 &rhs)
{
    return (lhs + rhs - 1) / rhs;
}

} //namespace n_Int

struct uIntDivResult;

///big unsigned integer
class uInt
{
  public:
    uInt() noexcept : m_data(), size(0)  {}

    ///when you want to reuse resources you might not want to implement assignment in term of constructor
    uInt(std::uintmax_t val) : uInt() { *this = val; }

    uInt(const uInt &cpy) : uInt(cpy.size(), 0) { *this = cpy; }

    uInt(uInt &&mov) noexcept 
    : m_data(Move(mov.m_data)), size(Move(mov.size)) {}

    uInt& operator = (std::uintmax_t val) &
    {
        static constexpr int max_size 
          = n_Int::ceilDiv(sizeof(std::uintmax_t) * CHAR_BIT, k_BaseBinDigit);
        if (val != 0)
          this->reset(max_size);
        this->clear();
        while(val > 0){
          this->unsafe_push_back(val % k_Base);
          val /= k_Base;
        }
        return *this;
    }

    uInt& operator = (const uInt &cpy) &
    {
        if (this != &cpy) {
          this->reset(cpy.size());
          unsafe_copy_data(*this, cpy);
        }
        return *this;
    }
    uInt& operator = (uInt &&mov) & noexcept
    {
        if (this != &mov) {
          this->clear();
          if (mov) swap(*this, mov);
        }
        return *this;
    }

    friend void swap(uInt &a, uInt &b) noexcept
    {
        doSwap(a, b);
    }
    
    ///downcasts to smaller primitive integers
    template<typename intg, n_Int::isIntegral_t<intg> = nullptr>
    explicit operator intg() const noexcept
    {
        return down_cast(*this);
    }

    explicit operator bool() const
    {
        return this->size();
    }


    friend bool operator == (const uInt &l, const uInt &r)
    { return SegView(l) == SegView(r); }
    friend bool operator != (const uInt &l, const uInt &r)
    { return !(l == r); }
    friend bool operator < (const uInt &l, const uInt &r)
    { return SegView(l) < SegView(r); }
    friend bool operator > (const uInt &l, const uInt &r)
    { return r < l; }
    friend bool operator <= (const uInt &l, const uInt &r)
    { return !(r < l); }
    friend bool operator >= (const uInt &l, const uInt &r)
    { return !(l < r); }
    
    uInt& operator ++() &
    {
        for(size_type i = 0; i < this->size(); ++i) {
          if ((*this)[i] == k_Base - 1)
            (*this)[i] = 0;
          else {
            ++(*this)[i];
            return *this;
          }
        }
        if (this->size() == this->capacity())
          this->increment_safe_reserve();
        this->unsafe_push_back(1);
        return *this;
    }
    uInt operator ++ (int) &
    {
        uInt res = *this;
        ++(*this);
        return res;
    }

    uInt& operator --() &
    {
        if (!*this) n_Int::throw_unsigned_integer_underflow_exception();
        for(size_type i = 0; i < this->size(); ++i) {
          if ((*this)[i] == 0)
            (*this)[i] = k_Base - 1;
          else {
            --(*this)[i];
            if (i == this->size() - 1 && (*this)[i] == 0)
              this->pop_back();
            return *this;
          }
        }
        return *this;
    }
    uInt operator -- (int) &
    {
        uInt res = *this;
        --(*this);
        return res;
    }


    friend uInt operator + (const uInt &lhs, const uInt &rhs)
    {
        return plus(lhs, rhs);
    }
    friend uInt operator + (const uInt &lhs, uInt &&rhs)
    {
        if (&lhs == &rhs) return lhs + rhs;
        return plus(lhs, std::move(rhs));
    }
    friend uInt operator + (uInt &&lhs, const uInt &rhs)
    {
        return rhs + std::move(lhs);
    }
    friend uInt operator + (uInt &&lhs, uInt &&rhs)
    {
        if (&lhs == &rhs && lhs.capacity() >= max_sum_size(lhs, lhs)) {
          unsafe_add(lhs, lhs, &lhs);
          return std::move(lhs);
        }
        if (lhs.capacity() < rhs.capacity()) 
          return plus(lhs, std::move(rhs));
        return plus(rhs, std::move(lhs));
    }

    friend uInt operator - (const uInt &lhs, const uInt &rhs)
    {
        return minus(lhs, rhs);
    }
    friend uInt operator - (const uInt &lhs, uInt &&rhs)
    {
        if (&lhs == &rhs) return uInt();
        return minus(lhs, rhs);
    }
    friend uInt operator - (uInt &&lhs, const uInt &rhs)
    {
        if (&lhs == &rhs) return uInt();
        return minus(std::move(lhs), rhs);
    }
    friend uInt operator - (uInt &&lhs, uInt &&rhs)
    {
        return std::move(lhs) - rhs;
    }

    friend uInt operator * (const uInt &lhs, const uInt &rhs)
    {
        return product(rhs, lhs);
    }
    
    ///have to be defined outside because of incomplete type issue
    friend uInt operator / (const uInt&, const uInt&);
    friend uInt operator % (const uInt&, const uInt&);
    friend uIntDivResult div(const uInt&, const uInt&);


 
    friend bool getbit(const uInt &x, size_t pos) noexcept
    {
        auto dec = decompose(pos);
        if (dec.unit >= x.size()) return false;
        return x[dec.unit] & (static_cast<base_type>(1u) << dec.digit);
    }

    friend uInt operator & (const uInt &lhs, const uInt &rhs)
    {
        uInt res(max_and_size(lhs, rhs), 0);
        unsafe_and(lhs, rhs, &res);
        return res;
    }
    friend uInt operator & (const uInt &lhs, uInt &&rhs)
    {
        unsafe_and(lhs, rhs, &rhs);
        return std::move(rhs);
    }
    friend uInt operator & (uInt &&lhs, const uInt &rhs)
    {
        return rhs & std::move(lhs); 
    }
    friend uInt operator & (uInt &&lhs, uInt &&rhs)
    {
        return rhs & std::move(lhs);
    }
    friend uInt operator | (const uInt &lhs, const uInt &rhs)
    {
        uInt res(max_or_size(lhs, rhs), 0);
        unsafe_or(lhs, rhs, &res);
        return res;
    }
    friend uInt operator | (const uInt &lhs, uInt &&rhs)
    {
        unsafe_or(lhs, rhs, &rhs);
        return std::move(rhs);
    }
    friend uInt operator | (uInt &&lhs, const uInt &rhs)
    {
        return rhs | std::move(lhs); 
    }
    friend uInt operator | (uInt &&lhs, uInt &&rhs)
    {
        if(lhs.capacity() < rhs.capacity())
          return lhs | std::move(rhs);
        return rhs | std::move(lhs);
    }
    friend uInt operator ^ (const uInt &lhs, const uInt &rhs)
    {
        uInt res(max_xor_size(lhs, rhs), 0);
        unsafe_xor(lhs, rhs, &res);
        return res;
    }
    friend uInt operator ^ (const uInt &lhs, uInt &&rhs)
    {
        unsafe_xor(lhs, rhs, &rhs);
        return std::move(rhs);
    }
    friend uInt operator ^ (uInt &&lhs, const uInt &rhs)
    {
        return rhs ^ std::move(lhs); 
    }
    friend uInt operator ^ (uInt &&lhs, uInt &&rhs)
    {
        if(lhs.capacity() < rhs.capacity())
          return lhs ^ std::move(rhs);
        return rhs ^ std::move(lhs);
    }
    friend uInt operator >> (const uInt &lhs, int rhs)
    {
        uInt res(max_shr_size(lhs, rhs), 0);
        unsafe_shr(lhs, rhs, &res);
        return res;
    }
    friend uInt operator >> (uInt &&lhs, int rhs)
    {
        unsafe_shr(lhs, rhs, &lhs);
        return std::move(lhs);
    }
    friend uInt operator << (const uInt &lhs, int rhs)
    {
        uInt res(max_shl_size(lhs, rhs), 0);
        unsafe_shl(lhs, rhs, &res);
        return res;
    }
    friend uInt operator << (uInt &&lhs, int rhs)
    {
        if(lhs.capacity() < max_shl_size(lhs, rhs))
          return lhs << rhs;
        unsafe_shl(lhs, rhs, &lhs);
        return std::move(lhs);
    }


    uInt& operator += (const uInt &rhs) &
    { 
        if(&rhs == this) return *this += std::move(*this);
        return *this = std::move(*this) + rhs; 
    }
    uInt& operator += (uInt &&rhs) &
    { return *this = std::move(*this) + std::move(rhs); }

    uInt& operator -= (const uInt &rhs) &
    { return *this = std::move(*this) - rhs; }

    uInt& operator *= (const uInt &rhs) &
    { return *this = *this * rhs; }
    uInt& operator /= (const uInt &rhs) &
    { return *this = *this / rhs; }
    uInt& operator %= (const uInt &rhs) &
    { return *this = *this % rhs; }

    uInt& operator &= (const uInt &rhs) &
    { return *this = std::move(*this) & rhs; }
    uInt& operator &= (uInt &&rhs) &
    { return *this = std::move(*this) & std::move(rhs); }
    uInt& operator |= (const uInt &rhs) &
    { return *this = std::move(*this) | rhs; }
    uInt& operator |= (uInt &&rhs) &
    { return *this = std::move(*this) | std::move(rhs); }
    uInt& operator ^= (const uInt &rhs) &
    { return *this = std::move(*this) ^ rhs; }
    uInt& operator ^= (uInt &&rhs) &
    { return *this = std::move(*this) ^ std::move(rhs); }
    uInt& operator >>= (int rhs) &
    { return *this = std::move(*this) >> rhs; }
    uInt& operator <<= (int rhs) &
    { return *this = std::move(*this) << rhs; }



    uInt& parse(string_view strv) &
    {
        const char *str = strv.data();
        size_type len = strv.size();
        while(len > 0 && *str == '0')
        { ++str; --len; }
        if (len == 0) 
        { this->clear(); return *this; }

        size_type estimate_size = 
         std::ceil((std::log2(n_Int::ten) / k_BaseBinDigit + .00001) * len);
        estimate_size += 2;
        this->reset(estimate_size);
        this->clear();

        size_type start = (len - 1) % k_ioDecDigit + 1;
        unsafe_add(*this, str_to_base(str, start), this);
        while(start < len){
          unsafe_small_mult(*this, k_ioUnit, this);
          unsafe_add(*this, str_to_base(str + start, k_ioDecDigit), this);
          start += k_ioDecDigit;
        }
        return *this;
    }
    uInt&& parse(string_view strv) &&
    {
        return std::move(this->parse(strv));
    }


    friend std::string to_string(uInt x)
    {
        if (!x) return "0";
        std::string res;
        while(x){
          calc_type unit = unsafe_small_divide(x, k_ioUnit, &x);
          if (x) 
            for(int i = 0; i < k_ioDecDigit; ++i){
              res.push_back('0' + unit % n_Int::ten);
              unit /= n_Int::ten;
            }
          else
            while(unit){
              res.push_back('0' + unit % n_Int::ten);
              unit /= n_Int::ten;
            }
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    
    friend std::istream& operator >> (std::istream &is, uInt &x) 
    {
        std::string input;
        is >> input;
        x.parse(input);
        return is;
    }

    friend std::ostream& operator << (std::ostream &os, const uInt &x)
    {
        return os << to_string(x);
    }

  private:
    using size_type = n_Int::size_type;
    using base_type = n_Int::base_type;
    using calc_type = n_Int::calc_type;
    using wcalc_type = n_Int::wcalc_type;
    using const_pointer = n_Int::IntData::const_pointer;
    using pointer = n_Int::IntData::pointer;

    static constexpr int k_BaseBinDigit = 30;
    static constexpr base_type k_Base = 1u << k_BaseBinDigit;
    static constexpr wcalc_type 
    k_SmallDivisorLimit = std::numeric_limits<wcalc_type>::max() / k_Base;

    static constexpr int k_ioDecDigit = 9;
    static constexpr base_type k_ioUnit = 1000000000;


    class SegView
    {
      public:
        using size_type = size_type;
        using const_pointer = n_Int::IntData::const_pointer;

        SegView() = delete;
      
        SegView(const uInt &s, size_type start = 0) noexcept
        : begin(s.begin() + start), size(s.size() - start) {}
        SegView(const uInt &s, size_type start, size_type len) noexcept
        : begin(s.begin() + start), size(len) {}

        SegView(const base_type &b) noexcept : begin(&b), size(!!b) {}

        SegView(SegView s, size_type start) noexcept
        : begin(s.cbegin() + start), size(s.size() - start) {}
        SegView(SegView s, size_type start, size_type len) noexcept
        : begin(s.cbegin() + start), size(len) {}
        

        base_type operator[] (size_type i) const
        {
            return this->cbegin()[i];
        }

        const_pointer cbegin() const
        {
            return this->begin();
        }
        const ReadOnlyProperty<const_pointer, SegView> begin;
        const ReadOnlyProperty<size_type, SegView> size;


        friend bool operator == (SegView lhs, SegView rhs)
        {
            if (lhs.size() != rhs.size()) return false;
            for(size_type i = lhs.size(); i-- > 0;)
              if (lhs[i] != rhs[i]) return false;
            return true;
        }
        friend bool operator != (SegView lhs, SegView rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator < (SegView lhs, SegView rhs)
        {
            if (lhs.size() != rhs.size()) 
              return lhs.size() < rhs.size();
            for(size_type i = lhs.size(); i-- > 0;)
              if (lhs[i] != rhs[i]) 
                return lhs[i] < rhs[i];
            return false;
        }
        friend bool operator > (SegView lhs, SegView rhs)
        {
            return rhs < lhs;
        }
        friend bool operator <= (SegView lhs, SegView rhs)
        {
            return !(rhs < lhs);
        }
        friend bool operator >= (SegView lhs, SegView rhs)
        {
            return !(lhs < rhs);
        }
        

        wcalc_type msd(size_type start) const
        {
            size_type start_unit = start / k_BaseBinDigit;
            if(this->size() <= start_unit) return 0;
            wcalc_type res = 0;
            size_type i = this->size();
            while(i--)
            {
              if(i * k_BaseBinDigit <= start){
                calc_type tmp = (*this)[i];
                int skip = start - i * k_BaseBinDigit;
                int remaining_digit = k_BaseBinDigit - skip;
                res = (res << remaining_digit) + (tmp >> skip);
                break;
              }
              res = res * k_Base + (*this)[i];
            }
            return res;
        }

        size_type digit_count() const
        {
            if(this->size() == 0) return 0;
            size_type res = k_BaseBinDigit * (this->size() - 1);
            for(calc_type tmp = (*this)[this->size() - 1]; tmp > 0; tmp >>= 1)
              ++res;
            return res;
        }
        

      public:
    }; //SegView

    struct decompose_result
    {
        size_type unit;
        int digit;
    };
    static constexpr decompose_result decompose(size_t pos) noexcept
    {
        return {size_type(pos / k_BaseBinDigit), int(pos % k_BaseBinDigit)};
    }


    uInt(size_type cap, size_type s) : m_data(cap), size(s) {}

    uInt(uInt &cpy, size_type cap) : m_data(cap), size(cpy.size()) 
    {
        *this = cpy;
    }

    static void doSwap(uInt &a, uInt &b) noexcept
    {
        using std::swap;
        swap(a.m_data, b.m_data);
        swap(a.size.value, b.size.value);
    }
    
    static std::uintmax_t down_cast(const uInt &x) noexcept
    {
        std::uintmax_t res = 0;
        for(size_type i = x.size(); i-- > 0;)
          res = res * k_Base + x[i];
        return res;
    }

    static base_type str_to_base(const char *str, size_type len)
    {
        calc_type res = 0;
        for(size_type i = 0; i < len; ++i)
          res = res * n_Int::ten + (str[i] - '0');
        return res;
    }

    
    static uInt plus(SegView lhs, SegView rhs)
    { 
        uInt res(max_sum_size(lhs, rhs), 0);
        unsafe_add(lhs, rhs, &res);
        return res;
    }
    static uInt plus(SegView lhs, uInt &&rhs)
    {
        if (rhs.capacity() < max_sum_size(lhs, rhs))
          return plus(lhs, rhs);
        unsafe_add(lhs, rhs, &rhs);
        return std::move(rhs);
    }
    
    static uInt minus(SegView lhs, SegView rhs)
    {
        uInt res(max_dif_size(lhs, rhs), 0);
        unsafe_subtract(lhs, rhs, &res);
        return res;
    }
    static uInt minus(SegView lhs, uInt &&rhs)
    {
        if (rhs.capacity() < max_dif_size(lhs, rhs))
          return minus(lhs, rhs);
        unsafe_subtract(lhs, rhs, &rhs);
        return std::move(rhs);
    }
    static uInt minus(uInt &&lhs, SegView rhs)
    {
        unsafe_subtract(lhs, rhs, &lhs);
        return std::move(lhs);
    }

    ///ordered means lhs.size >= rhs.size
    static uInt product(SegView lhs, SegView rhs)
    {
        if(lhs.size() < rhs.size()) return product(rhs, lhs);
        // std::cout << indent << lhs.size() << ' ' << rhs.size() << '\n';
        // autoindent aut;
        if (exceed_threshold(lhs.size(), rhs.size()))
          return big_ordered_product(lhs, rhs);
        return small_ordered_product(lhs, rhs);
    }

    static bool exceed_threshold(size_type size1, size_type size2)
    {
        constexpr size_type threshold = 400;
        return !(threshold / size2 >= size2);
    }

    /// Karatsuba algorithm
    static uInt big_ordered_product(SegView lhs, SegView rhs)
    {
        uInt res(max_prod_size(lhs, rhs), 0);
        size_type half = (lhs.size() + 1) / 2;

        if (rhs.size() <= half) {
          uInt res1 = product(SegView(lhs, 0, half), rhs);
          uInt res2 = product(SegView(lhs, half), rhs);
          unsafe_copy_data(res, res1);
          res.size = res.capacity();
          res.zero_fill(res1.size(), res.size() - res1.size());
          for(size_type i = 0; i < res2.size(); ++i)
            if ((res[i + half] += res2[i]) >= k_Base) { 
              res[i + half] -= k_Base;
              ++res[i + half + 1];
            }
          res.trim_zero();
        }

        else {
          SegView lhalf1(lhs, 0, half), lhalf2(lhs, half);
          SegView rhalf1(rhs, 0, half), rhalf2(rhs, half);
          uInt suml = plus(lhalf1, lhalf2), sumr = plus(rhalf1, rhalf2);
          uInt res1 = product(lhalf1, rhalf1);
          uInt res2 = product(suml, sumr);
          uInt res3 = product(lhalf2, rhalf2);
          unsafe_subtract(res2, res1, &res2);
          unsafe_subtract(res2, res3, &res2);

          res.size = res.capacity();
          calc_type tmp = 0, carry = 0;
          for(size_type i = 0; i < res.size(); ++i){
            tmp = carry;
            carry = 0;
            if(i < res1.size()) 
              tmp += res1[i];
            if(half <= i && i < half + res2.size())
              tmp += res2[i - half];
            if(half * 2 <= i && i < half * 2 + res3.size())
              tmp += res3[i - half * 2];
            while (tmp >= k_Base){
              tmp -= k_Base;
              ++carry;
            }
            res[i] = tmp;
          }
          res.trim_zero();
        }

        return res;
    }

    /// schoolbook algorithm
    static uInt small_ordered_product(SegView lhs, SegView rhs)
    {
        uInt res(max_prod_size(lhs, rhs), lhs.size());
        res.zero_fill(0, lhs.size());
        for(size_type i = 0; i < rhs.size(); ++i){
          wcalc_type tmp = 0;
          wcalc_type cur_digit = rhs[i];
          for(size_type j = 0; j < lhs.size(); ++j){
            tmp += lhs[j] * cur_digit + res[j + i];
            res[j + i] = tmp & (k_Base - 1);
            tmp >>= k_BaseBinDigit;
          }
          res.unsafe_push_back(tmp);
        }
        res.trim_zero();
        return res;
    }

    static base_type small_div(SegView lhs, const uInt &rhs, uInt *rem)
    {
        size_type rhs_dc = rhs.digit_count();
        wcalc_type lhs_msd = lhs.msd(rhs_dc - k_BaseBinDigit);
        wcalc_type rhs_msd = rhs.msd(rhs_dc - k_BaseBinDigit);
        ///dividing the most significant digits is a good approximation
        calc_type quotient =
             std::min<calc_type>(k_Base - 1, (lhs_msd + 1) / rhs_msd);
        uInt remainder;
        if(rem) swap(remainder, *rem);
        remainder.reset(rhs.size() + 1);
        unsafe_small_mult(rhs, quotient, &remainder);
        while(remainder > lhs){
          --quotient;
          unsafe_subtract(remainder, rhs, &remainder);
        }
        unsafe_subtract(lhs, remainder, &remainder);
        if(rem) swap(remainder, *rem);
        return quotient;
    }
    static uInt small_division(SegView lhs, wcalc_type rhs)
    {
        uInt res(max_div_quo_size(lhs, rhs), 0);
        unsafe_small_divide(lhs, rhs, &res);
        return res;
    }
    static uInt small_modulo(SegView lhs, wcalc_type rhs)
    {
        wcalc_type rem = 0;
        for(size_type i = lhs.size(); i-- > 0;)
          rem = ((rem << k_BaseBinDigit) + lhs[i]) % rhs;
        return rem;
    }


    ///unsafe functions assume capacity is sufficient
    //res may point to lhs or rhs
    static void unsafe_add(SegView lhs, SegView rhs, uInt *res)
    {
        if(lhs.size() < rhs.size()) return unsafe_add(rhs, lhs, res);
        calc_type tmp = 0, carry = 0;
        for(size_type i = 0; i < lhs.size(); ++i) {
          tmp = lhs[i] + carry;
          if (i < rhs.size()) tmp += rhs[i];
          carry = tmp >> k_BaseBinDigit;
          tmp &= k_Base - 1;
          (*res)[i] = tmp;
        }
        (*res)[lhs.size()] = carry;
        res->size = lhs.size() + carry;
    }
    static void unsafe_subtract(SegView lhs, SegView rhs, uInt *res)
    {
        if (lhs.size() < rhs.size())
          n_Int::throw_unsigned_integer_underflow_exception();
        calc_type tmp = 0, carry = 0;
        for(size_type i = 0; i < lhs.size(); ++i) {
          tmp = k_Base + lhs[i] - carry;
          if (i < rhs.size()) tmp -= rhs[i];
          carry = !(tmp >> k_BaseBinDigit);
          tmp &= k_Base - 1;
          (*res)[i] = tmp;
        }
        if (carry)
          n_Int::throw_unsigned_integer_underflow_exception();
        res->size = lhs.size();
        res->trim_zero();
    }
    static void unsafe_small_mult(SegView lhs, calc_type rhs, uInt *res)
    {
        wcalc_type tmp = 0, r = rhs;
        for(size_type i = 0; i < lhs.size(); ++i){
          tmp += r * lhs[i];
          (*res)[i] = tmp & (k_Base - 1);
          tmp >>= k_BaseBinDigit;
        }
        res->size = lhs.size();
        if (tmp != 0) res->unsafe_push_back(tmp);
        res->trim_zero();
    }
    static wcalc_type 
    unsafe_small_divide(SegView lhs, wcalc_type rhs, uInt *quo)
    {
        wcalc_type rem = 0;
        for(size_type i = lhs.size(); i-- > 0;){
          rem = (rem << k_BaseBinDigit) + lhs[i];
          rem = rem - rhs * ((*quo)[i] = rem / rhs);
        }
        quo->size = lhs.size();
        quo->trim_zero();
        return rem;
    }
    static void unsafe_and(const uInt &lhs, const uInt &rhs, uInt *res)
    {
        if(lhs.size() > rhs.size()) return unsafe_and(rhs, lhs, res);
        for(size_type i = 0; i < lhs.size(); ++i)
          (*res)[i] = lhs[i] & rhs[i]; 
        res->size = lhs.size();
        res->trim_zero();
    }
    static void unsafe_or(const uInt &lhs, const uInt &rhs, uInt *res)
    {
        if(lhs.size() < rhs.size()) return unsafe_or(rhs, lhs, res);
        for(size_type i = 0; i < lhs.size(); ++i)
          (*res)[i] = lhs[i] | (i < rhs.size() ? rhs[i] : 0); 
        res->size = lhs.size();
    }
    static void unsafe_xor(const uInt &lhs, const uInt &rhs, uInt *res)
    {
        if(lhs.size() < rhs.size()) return unsafe_xor(rhs, lhs, res);
        for(size_type i = 0; i < lhs.size(); ++i)
          (*res)[i] = lhs[i] ^ (i < rhs.size() ? rhs[i] : 0); 
        res->size = lhs.size();
        res->trim_zero();
    }
    static void unsafe_shr(const uInt &lhs, size_t rhs, uInt *res)
    {
        if (rhs < 0) throw std::invalid_argument("invalid argument");
        auto dec = decompose(rhs);
        if(lhs.size() < dec.unit) {}
        else if (dec.digit == 0) {
          if(dec.unit || (lhs.cbegin() != res->cbegin()))
            std::copy_n(lhs.cbegin() + dec.unit, 
                        lhs.size() - dec.unit, res->begin());
        }
        else {
          (*res)[0] = lhs[dec.unit] >> dec.digit;
          for(size_type i = dec.unit + 1; i < lhs.size(); ++i){
            (*res)[i - dec.unit - 1] 
             |= (lhs[i] << (k_BaseBinDigit - dec.digit)) & (k_Base - 1);
            (*res)[i - dec.unit] = lhs[i] >> dec.digit;
          }
        }
        res->size = max_shr_size(lhs, rhs);
        res->trim_zero();
    }
    static void unsafe_shl(const uInt &lhs, size_t rhs, uInt *res)
    {
        if (rhs < 0) throw std::invalid_argument("invalid argument");
        auto dec = decompose(rhs);
        res->zero_fill(0, dec.unit);
        if (dec.digit == 0) {
          if(dec.unit || (lhs.cbegin() != res->cbegin()))
            std::copy_n(lhs.cbegin(), 
                        lhs.size(), res->begin() + dec.unit);
        }
        else {
          (*res)[lhs.size() + dec.unit] = 0;
          for(size_type i = lhs.size(); i-- > 0;){
            (*res)[i + dec.unit + 1] |=
              lhs[i] >> (k_BaseBinDigit - dec.digit);
            (*res)[i + dec.unit] = 
              (lhs[i] << dec.digit) & (k_Base - 1);
          }
        }
        res->size = max_shl_size(lhs, rhs);
        res->trim_zero();
    }



    static size_type max_sum_size(SegView lhs, SegView rhs)
    {
        return std::max(lhs.size(), rhs.size()) + 1;
    }
    static size_type max_dif_size(SegView lhs, SegView rhs)
    {
        return lhs.size();
    }
    static size_type max_prod_size(SegView lhs, SegView rhs)
    {
        return lhs.size() + rhs.size();
    }
    static size_type max_div_quo_size(SegView lhs, wcalc_type rhs)
    {
        return lhs.size();
    }
    static size_type max_and_size(const uInt &lhs, const uInt &rhs)
    {
        return std::min(lhs.size(), rhs.size());
    }
    static size_type max_or_size(const uInt &lhs, const uInt &rhs)
    {
        return std::max(lhs.size(), rhs.size());
    }
    static size_type max_xor_size(const uInt &lhs, const uInt &rhs)
    {
        return std::max(lhs.size(), rhs.size());
    }
    static size_type max_shr_size(const uInt &lhs, size_t rhs)
    {
        return std::max<base_type>(lhs.size() - rhs / k_BaseBinDigit, 0);
    }
    static size_type max_shl_size(const uInt &lhs, size_t rhs)
    {
        return lhs.size() + (rhs + k_BaseBinDigit - 1) / k_BaseBinDigit;
    }
    





    ///most significant digits
    wcalc_type msd(size_type start) const
    {
        return SegView(*this).msd(start);
    }

    size_type digit_count() const
    {
        return SegView(*this).digit_count();
    }


    void trim_zero()
    {
        while(this->size() > 0 && this->back() == 0)
          this->pop_back();
    }
    bool is_small_divisor() const
    {
        return this->size() <= 2 && down_cast(*this) < k_SmallDivisorLimit;
    }

    static void copy_and_shift(uInt &dest, SegView src)
    {
        std::copy_n(src.cbegin(), src.size(), dest.begin() + 1);
        dest.size = src.size() + 1;
    }
    static void unsafe_copy_data(uInt &dest, SegView src)
    {
        std::copy_n(src.cbegin(), src.size(), dest.begin());
        dest.size = src.size();
    }

    void zero_fill(size_type start, size_type len)
    {
        if (len != 0)
          memset(this->begin() + start, 0, len * sizeof(base_type));
    }

    base_type& operator [] (size_type idx) &
    {
        return this->m_data[idx];
    }
    base_type operator [] (size_type idx) const &
    {
        return this->m_data[idx];
    }

    base_type back() const
    {
        return (*this)[this->size() - 1];
    }
  
    void unsafe_push_back(base_type c)
    {
        (*this)[this->size++] = c;
    }
    void pop_back()
    {
        --this->size;
    }

    pointer begin() &
    {
        return this->m_data.begin();
    }
    const_pointer cbegin() const &
    {
        return this->m_data.cbegin();
    }
    const_pointer begin() const &
    {
        return this->cbegin();
    }


    ///reserve(capacity + 1) 
    ///reverse the effect of ++ if an exception is thrown
    void increment_safe_reserve()
    {
        try{this->reserve(this->capacity() + 1);}
        catch(...) {
          for(size_type i = 0; i < this->size(); ++i)
            (*this)[i] = k_Base - 1;
          throw;
        }
    }


    void clear()
    {
        this->size = 0;
    }
    void reset(size_type cap)
    {
        this->m_data.reset(cap);
        this->clear();
    }
    void reserve(size_type cap)
    {
        if (this->capacity() < cap)
          *this = uInt(*this, cap);
    }
    size_type capacity() const
    {
        return this->m_data.size();
    }



    n_Int::IntData m_data;
    ReadOnlyProperty<size_type, uInt> size;
};


struct uIntDivResult
{
    uInt quo, rem;
};

inline uInt operator / (const uInt &divident, const uInt &divisor)
{
    if (divisor.size() == 0) 
      n_Int::throw_division_by_zero_exception();
    if (divident < divisor) 
      return uInt();
    if (divisor.is_small_divisor())
      return uInt::small_division(divident, uInt::down_cast(divisor));
    return div(divident, divisor).quo;
}
inline uInt operator % (const uInt &divident, const uInt &divisor)
{
    if (divisor.size() == 0) 
      n_Int::throw_division_by_zero_exception();
    if (divident < divisor) 
      return divident;
    if (divisor.is_small_divisor())
      return uInt::small_modulo(divident, uInt::down_cast(divisor));
    return div(divident, divisor).rem;
}
inline uIntDivResult div(const uInt &divident, const uInt &divisor)
{
    if (divisor.size() == 0) 
      n_Int::throw_division_by_zero_exception();
    if (divident < divisor) 
      return uIntDivResult{uInt(), divident};
    if (divisor.is_small_divisor())
      return uIntDivResult{
          uInt::small_division(divident, uInt::down_cast(divisor)),
          uInt::small_modulo(divident, uInt::down_cast(divisor))};

    /*else*/ /*school-book algorithm*/
    uInt::size_type quotient_max_size = divident.size() - divisor.size() + 1;
    uIntDivResult res = {uInt{quotient_max_size, quotient_max_size},
                      uInt{divisor.size() + 1, divisor.size() - 1}};
    uInt cur_divident(divisor.size() + 1, 0);
    /// copy <divisor.digitcount - 1> divident's most significant digits to cur remainder
    std::copy_n(divident.cbegin() + quotient_max_size, 
                divisor.size() - 1, res.rem.begin());

    for(uInt::size_type i = quotient_max_size; i-- > 0;) {
      uInt::copy_and_shift(cur_divident, res.rem);
      cur_divident[0] = divident[i];
      res.quo[i] = 
          uInt::small_div(cur_divident, divisor, &res.rem);
    }
    res.quo.trim_zero();
    return res;
}


template<> uInt convert<uInt>(string_view str)
{
    uInt res;
    res.parse(str);
    return res;
}


template<char ...cs>
const uInt& operator "" _ulll()
{
    static constexpr char str[]{cs..., '\0'};
    static const uInt x = convert<uInt>(str);
    return x;
}



struct IntDivResult;

///big signed integer
class Int
{
  public:
    Int() noexcept = default;

    template<typename intg, n_Int::isIntegral_t<intg> = nullptr>
    Int(intg val) : Int()
    {
        *this = val;
    }

    Int(uInt uns) : Int{positive, std::move(uns)} {}

    Int(Int&&) noexcept = default;
    Int(const Int&) = default;

    template<typename intg, n_Int::isIntegral_t<intg> = nullptr>
    Int& operator = (intg val) &
    {
        sign_type new_sign = positive;
        if (val < 0) {
          new_sign = negative;
          val = -val;
        }
        if (val == 0) this->sign = zero;
        else {
          this->m_abs = val;
          this->sign = new_sign;
        }
        return *this;
    }

    Int& operator = (Int&&) & noexcept = default;
    Int& operator = (const Int&) & = default;

    friend void swap(Int &a, Int &b) noexcept
    {
        doSwap(a, b);
    }


    template<typename intg, n_Int::isIntegral_t<intg> = nullptr>
    explicit operator intg() const noexcept
    {
        return static_cast<intg>(this->m_abs) * this->sign;
    }

    explicit operator bool() const noexcept
    {
        return this->sign;
    }

    using sign_type = int;
    static constexpr sign_type zero = 0;
    static constexpr sign_type positive = 1;
    static constexpr sign_type negative = -positive;

    ReadOnlyProperty<sign_type, Int> sign;


    friend bool operator == (const Int &lhs, const Int &rhs)
    {
        return lhs.sign() == rhs.sign() && lhs.m_abs == rhs.m_abs;
    }
    friend bool operator != (const Int &lhs, const Int &rhs)
    {
        return !(lhs == rhs);
    }
    friend bool operator < (const Int &lhs, const Int &rhs)
    {
        if (&lhs == &rhs) return false;
        if (lhs.sign() != rhs.sign()) return lhs.sign() < rhs.sign();
        if (lhs.sign == positive) return lhs.m_abs < rhs.m_abs;
        if (lhs.sign == negative) return rhs.m_abs < lhs.m_abs;
        return false;
    }
    friend bool operator > (const Int &lhs, const Int &rhs)
    {
        return rhs < lhs;
    }
    friend bool operator <= (const Int &lhs, const Int &rhs)
    {
        return !(rhs < lhs);
    }
    friend bool operator >= (const Int &lhs, const Int &rhs)
    {
        return !(lhs < rhs);
    }


    friend Int& negate(Int &x)
    {
        x.doNegate();
        return x;
    }



    friend Int operator + (Int x)
    {
        return x;
    }
    friend Int operator - (Int x)
    {
        negate(x);
        return x;
    }


    Int& operator ++ () &
    {
        if (this->sign == negative) {
          --this->m_abs;
          if (!this->m_abs) 
            this->sign = zero;
        }else {
          ++this->m_abs;
          this->sign = positive;
        }
        return *this;
    }
    Int operator ++ (int) &
    {
        Int res = *this;
        ++(*this);
        return res;
    }
    Int& operator -- () &
    {
        if (this->sign == positive) {
          --this->m_abs;
          if (!this->m_abs) 
            this->sign = zero;
        }else {
          ++this->m_abs;
          this->sign = negative;
        }
        return *this;
    }
    Int operator -- (int) &
    {
        Int res = *this;
        --(*this);
        return res;
    }



    friend Int operator + (const Int &lhs, const Int &rhs)
    {
        if(lhs.sign() * rhs.sign() != negative)
          return Int{(lhs.sign() ? lhs.sign() : rhs.sign()), 
                      lhs.m_abs + rhs.m_abs};
        if (lhs.m_abs < rhs.m_abs)
          return Int{rhs.sign(), rhs.m_abs - lhs.m_abs}; 
        else
          return Int{lhs.sign(), lhs.m_abs - rhs.m_abs};
    }
    friend Int operator + (const Int &lhs, Int &&rhs)
    {
        if(lhs.sign() * rhs.sign() != negative)
          return Int{(lhs.sign() ? lhs.sign() : rhs.sign()), 
                      lhs.m_abs + std::move(rhs.m_abs)};
        if (lhs.m_abs < rhs.m_abs)
          return Int{rhs.sign(), std::move(rhs.m_abs) - lhs.m_abs}; 
        else
          return Int{lhs.sign(), lhs.m_abs - std::move(rhs.m_abs)};
    }
    friend Int operator + (Int &&lhs, const Int &rhs)
    {
        return rhs + std::move(lhs);
    }
    friend Int operator + (Int &&lhs, Int &&rhs)
    {
        if(lhs.sign() * rhs.sign() != negative)
          return Int{(lhs.sign() ? lhs.sign() : rhs.sign()), 
                      std::move(lhs.m_abs) + std::move(rhs.m_abs)};
        if (lhs.m_abs < rhs.m_abs)
          return Int{rhs.sign(), std::move(rhs.m_abs) - std::move(lhs.m_abs)}; 
        else
          return Int{lhs.sign(), std::move(lhs.m_abs) - std::move(rhs.m_abs)};
    }

    friend Int operator - (const Int &lhs, const Int &rhs)
    {
        if(lhs.sign() * -rhs.sign() != negative)
          return Int{(lhs.sign() ? +lhs.sign() : -rhs.sign()), 
                      lhs.m_abs + rhs.m_abs};
        if (lhs.m_abs < rhs.m_abs)
          return Int{-rhs.sign(), rhs.m_abs - lhs.m_abs}; 
        else
          return Int{+lhs.sign(), lhs.m_abs - rhs.m_abs};
    }
    friend Int operator - (const Int &lhs, Int &&rhs)
    {
        return lhs + -std::move(rhs);
    }
    friend Int operator - (Int &&lhs, const Int &rhs)
    {
        return -(rhs + -std::move(lhs));
    }
    friend Int operator - (Int &&lhs, Int &&rhs)
    {
        return std::move(lhs) + -std::move(rhs);
    }

    friend Int operator * (const Int &lhs, const Int &rhs)
    {
        return Int{lhs.sign() * rhs.sign(), lhs.m_abs * rhs.m_abs};
    }
    friend Int operator / (const Int &lhs, const Int &rhs)
    {
        return Int{lhs.sign() * rhs.sign(), lhs.m_abs / rhs.m_abs};
    }
    friend Int operator % (const Int &lhs, const Int &rhs)
    {
        return Int{lhs.sign() * rhs.sign(), lhs.m_abs % rhs.m_abs};
    }
    friend IntDivResult div(const Int&, const Int&);


    Int& parse(string_view strv) &
    {
        const char *str = strv.data();
        auto len = strv.size();
        sign_type new_sign = positive;
        const char* en = str + len;
        if (*str == '-') {
          new_sign = -new_sign;
          ++str;
        }
        this->m_abs.parse(string_view(str, en - str));
        if (!this->m_abs) this->sign = zero;
        else this->sign = new_sign;
        return *this;
    }
    Int&& parse(string_view strv) &&
    {
        return std::move(this->parse(strv));
    }

    friend std::string to_string(const Int &x)
    { 
        std::ostringstream format;
        format << x;
        return format.str();
    }


    friend std::istream& operator >> (std::istream &is, Int &x) 
    {
        std::string input;
        is >> input;
        x.parse(input);
        return is;
    }

    friend std::ostream& operator << (std::ostream &os, const Int &x)
    {
        if (x.sign() == negative) os << '-';
        os << x.m_abs;
        return os;
    }


  private:
    Int(sign_type si, uInt b) noexcept : sign(si * !!b), m_abs(Move(b)) {}

    
    static void doSwap(Int &a, Int &b) noexcept
    {
        using std::swap;
        a.sign.swap(b.sign);
        swap(a.m_abs, b.m_abs);
    }


    void doNegate() &
    {
        this->sign = -this->sign();
    }


    uInt m_abs;
};


struct IntDivResult
{
    Int quo, rem;
};

IntDivResult div(const Int &lhs, const Int &rhs)
{
    auto res = div(lhs.m_abs, rhs.m_abs);
    return {Int{lhs.sign() * rhs.sign(), std::move(res.quo)},
            Int{lhs.sign() * rhs.sign(), std::move(res.rem)}};
}


template<> Int convert<Int>(string_view str)
{
    Int res;
    res.parse(str);
    return res;
}


template<char ...cs>
const Int& operator "" _lll()
{
    static constexpr char str[]{cs..., '\0'};
    static const Int x = convert<Int>(str);
    return x;
}






















#if ACHIBULUP__Cpp14_later
class uint128_t
{
  public:
    uint128_t() noexcept = default;

    template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
    constexpr uint128_t(const Tp &val) : low((val < 0) ? -val : val), high((val < 0) ? -1 : 0) {}
    constexpr uint128_t(uint64_t _sc, uint64_t _fi) : low(_sc), high(_fi) {}

    template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
    explicit constexpr operator Tp() const
    {
        return low;
    }
    explicit constexpr operator bool () const
    {
        return high || low;
    }

    friend std::istream& operator >> (std::istream &is, uint128_t &x)
    {
        return x.extractFrom(is);
    }
    friend std::ostream& operator << (std::ostream &os, const uint128_t &x)
    {
        return x.insertTo(os);
    }


    uint64_t low, high;

    static constexpr uint64_t k_MaskLow = (1ull << 32) - 1;
  private:
    std::istream& extractFrom(std::istream&);
    std::ostream& insertTo(std::ostream&) const;





};
inline constexpr bool operator == (const uint128_t &l, const uint128_t &r)
{
    return l.high == r.high && l.low == r.low;
}
inline constexpr bool operator != (const uint128_t &l, const uint128_t &r)
{   return !(l == r); }
inline constexpr bool operator < (const uint128_t &l, const uint128_t &r)
{
    return l.high < r.high || (l.high == r.high && l.low < r.low);
}
inline constexpr bool operator > (const uint128_t &l, const uint128_t &r)
{   return r < l; }
inline constexpr bool operator <= (const uint128_t &l, const uint128_t &r)
{   return !(r < l); }
inline constexpr bool operator >= (const uint128_t &l, const uint128_t &r)
{   return !(l < r); }

inline constexpr uint128_t operator & (const uint128_t &l, const uint128_t &r)
{
    return {l.low & r.low, l.high & r.high};
}
inline constexpr uint128_t operator | (const uint128_t &l, const uint128_t &r)
{
    return {l.low | r.low, l.high | r.high};
}
inline constexpr uint128_t operator ^ (const uint128_t &l, const uint128_t &r)
{
    return {l.low ^ r.low, l.high ^ r.high};
}

inline constexpr uint128_t operator << (const uint128_t &x, const uint128_t &sh)
{
    return (!sh.high && sh.low < 64) ?
    uint128_t{x.low << sh.low, (x.high << sh.low) ^ (x.low >> (64 - sh.low))}
  : uint128_t{0, x.low << (sh.low - 64)};
}
inline constexpr uint128_t operator >> (const uint128_t &x, const uint128_t &sh)
{
    return (!sh.high && sh.low < 64) ?
    uint128_t{(x.low >> sh.low) ^ (x.high << (64 - sh.low)), x.high >> sh.low}
  : uint128_t{x.high >> (sh.low - 64), 0};
}
inline constexpr uint128_t operator ~ (const uint128_t &x)
{
    return uint128_t{~x.low, ~x.high};
}

inline constexpr uint128_t& operator &= (uint128_t &x, const uint128_t &v)
{   return x = x & v; }
inline constexpr uint128_t& operator |= (uint128_t &x, const uint128_t &v)
{   return x = x | v; }
inline constexpr uint128_t& operator ^= (uint128_t &x, const uint128_t &v)
{   return x = x ^ v; }
inline constexpr uint128_t& operator <<= (uint128_t &x, const uint128_t &sh)
{   return x = x << sh; }
inline constexpr uint128_t& operator >>= (uint128_t &x, const uint128_t &sh)
{   return x = x >> sh; }

inline constexpr int count_leading_zero(const uint64_t &x)
{
    return x ? (64 - int(std::log2(x)) - 1) : 64;
}
inline constexpr int count_leading_zero(const uint128_t &x)
{
    return x.high ? count_leading_zero(x.high) : (64 + count_leading_zero(x.low));
}


inline constexpr uint128_t operator + (uint128_t x)
{
    return x;
}
inline constexpr uint128_t operator - (const uint128_t &x)
{
    return {~x.low + 1, ~x.high + !x.low};
}

inline constexpr uint128_t& operator ++(uint128_t &x)
{
    ++x.low;
    x.high += !x.low;
    return x;
}
inline constexpr uint128_t operator ++(uint128_t &x, int)
{
    uint128_t res = x;
    ++x;
    return res;
}
inline constexpr uint128_t& operator --(uint128_t &x)
{
    --x.low;
    x.high -= !~x.low;
    return x;
}
inline constexpr uint128_t operator --(uint128_t &x, int)
{
    uint128_t res = x;
    --x;
    return res;
}

inline constexpr uint128_t operator + (const uint128_t &l, const uint128_t &r)
{
    return {l.low + r.low, l.high + r.high + (l.low > ~r.low)};
}
inline constexpr uint128_t operator - (const uint128_t &l, const uint128_t &r)
{
    return {l.low - r.low, l.high - r.high - (l.low < r.low)};
}

inline constexpr uint128_t& operator += (uint128_t &x, const uint128_t &d)
{   return x = x + d; }
inline constexpr uint128_t& operator -= (uint128_t &x, const uint128_t &d)
{   return x = x - d; }


inline constexpr uint128_t operator * (const uint128_t &l, const uint128_t &r)
{
    uint128_t res{static_cast<uint32_t>(l.low) * (r.low & uint128_t::k_MaskLow),
      l.high * r.low + l.low * r.high + static_cast<uint32_t>(l.low >> 32) * (r.low >> 32)};
    uint64_t v1 = static_cast<uint32_t>(l.low) * (r.low >> 32);
    uint64_t v2 = static_cast<uint32_t>(r.low) * (l.low >> 32);
    res.high += (v1 >> 32) + (v2 >> 32);
    v1 = (res.low >> 32) + static_cast<uint32_t>(v1) + static_cast<uint32_t>(v2);
    res.high += v1 >> 32;
    res.low = (res.low & uint128_t::k_MaskLow) + ((v1 & uint128_t::k_MaskLow) << 32);
    return res;
}
inline constexpr uint128_t operator / (uint128_t l, uint128_t r)
{
    uint128_t res{};
    if (l < r);
    else if (!l.high) {
      if (!r.high) res.low = l.low / r.low;
    }
    else if (!r.high && r.low < (1ull << 32)) {
      uint32_t div = static_cast<uint32_t>(r.low);
      res.high = l.high / div;
      unsigned long long temp = (static_cast<uint64_t>(l.high % div) << 32) + (l.low >> 32);
      res.low = ((temp / div) << 32) + ((static_cast<uint64_t>(temp % div) << 32) + (l.low & uint128_t::k_MaskLow)) / div;
    }
    else {
      unsigned shift = count_leading_zero(r);
      r <<= shift;
      if (shift >= 64) {
        unsigned shift2 = shift - 64;
        do{
          if (l >= r) {
            res.high |= (1ull << shift2);
            l -= r;
          }
          r >>= 1;
        }while(shift2--);
        shift = 63;
      }
      do{
        if (l >= r) {
          res.high |= (1ull << shift);
          l -= r;
        }
        r >>= 1;
      }while(shift--);
    }
    return res;
}
inline constexpr uint128_t operator % (uint128_t l, uint128_t r)
{
    uint128_t res{};
    if (l < r) res = l;
    else if (!l.high) res.low = l.low % r.low;
    else if (r.low < (1ull << 32)) {
      uint32_t div = static_cast<uint32_t>(r.low);
      res.low = ((((((l.high % div) << 32) + (l.low >> 32)) % div) << 32) + (l.low & uint128_t::k_MaskLow)) % div;
    }
    else {
      unsigned shift = count_leading_zero(r);
      res = l;
      r <<= shift;
      do{
        if (res >= r) res -= r;
        r >>= 1;
      }while(shift--);
    }
    return res;
}

inline constexpr uint128_t& operator *= (uint128_t &x, const uint128_t &d)
{   return x = x * d; }
inline constexpr uint128_t& operator /= (uint128_t &x, const uint128_t &d)
{   return x = x / d; }
inline constexpr uint128_t& operator %= (uint128_t &x, const uint128_t &d)
{   return x = x % d; }



template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator == (const uint128_t &l, Tp r)
{
    return !l.high && l.low == r;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator == (Tp l, const uint128_t &r)
{   return r == l; }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator != (const uint128_t &l, Tp r)
{   return !(l == r); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator != (Tp l, const uint128_t &r)
{   return !(r == l); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator < (const uint128_t &l, Tp r)
{
    return !l.high && l.low < r;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator < (Tp l, const uint128_t &r)
{
    return r.high || l < r.low;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator > (const uint128_t &l, Tp r)
{  return r < l; }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator > (Tp l, const uint128_t &r)
{  return r < l; }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator <= (const uint128_t &l, Tp r)
{  return !(r < l); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator <= (Tp l, const uint128_t &r)
{  return !(r < l); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator >= (const uint128_t &l, Tp r)
{  return !(l < r); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator >= (Tp l, const uint128_t &r)
{  return !(l < r); }



template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp operator << (Tp x, const uint128_t &sh)
{ 
    return sh.high ? (x << (64 + !sh.high)) : (x << sh.low);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp operator >> (Tp x, const uint128_t &sh)
{ 
    return sh.high ? (x >> (64 + !sh.high)) : (x >> sh.low);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp& operator &= (Tp &x, const uint128_t &v)
{ 
    return x = x & v.low;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp& operator <<= (Tp &x, const uint128_t &sh)
{ 
    return x = x << sh;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp& operator >>= (Tp &x, const uint128_t &sh)
{ 
    return x = x >> sh;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp& operator /= (Tp &x, const uint128_t &v)
{ 
    return (v.high) ? (x = 0) : (x /= v.low);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp& operator %= (Tp &x, const uint128_t &v)
{ 
    return (v.high) ? x : (x %= v.low);
}


std::istream& uint128_t::extractFrom(std::istream &is)
{
    std::string istr;
    is >> istr;
    uint128_t &x = *this;
    x = {};
    uint64_t temp1, temp2;
    for(std::string::size_type i = 0, len = istr.size(); i < len; ++i) {
      temp1 = (x.low & uint128_t::k_MaskLow) * n_Int::ten;
      temp2 = (temp1 >> 32) + ((x.low >> 32) * n_Int::ten);
      x.low = (temp1 & uint128_t::k_MaskLow) + (temp2 << 32);
      x.high = x.high * n_Int::ten + (temp2 >> 32);
      x.low += istr[i] - '0';
    }
    return is;
}

std::ostream& uint128_t::insertTo(std::ostream &os) const
{
    uint128_t x = *this;
    char ostr[40];
    if (!x) return os << '0';
    int first = 40;
    while(x) {
      ostr[--first] = int(x % n_Int::ten) + '0';
      x /= n_Int::ten;
    }
    os << (ostr + first);
    return os;
}


#endif // __cplusplus
} //namespace Achibulup
#endif //BIGNUM_H_INCLUDED

