#ifndef BIGNUM_H_INCLUDED
#define BIGNUM_H_INCLUDED

//Big integer handling
//the two class Int and uInt provide support for big integer arithmetics
//Int can handle signed integer while uInt also supports bitwise operations for unsigned integer


#include <cmath> //log2
#include <string> //input
#include <limits> //unsigned long long max
#include <cstring> //memset, memcpy
#include <cstdint> //int types
#include <iostream> //in-out put
#include <algorithm> //max, min
#include <stdexcept> // exception
#include "common_utils.h"
#if ACHIBULUP__Cpp17_later
#include <string_view>
#endif //ACHIBULUP__Cpp17_later

namespace Achibulup{



namespace n_Int_helper{

constexpr int ten = 10;


void throw_division_by_zero_exception()
{
    throw std::domain_error("division by zero");
};
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
    using const_pointer = n_Int_helper::const_pointer;
    using pointer = n_Int_helper::pointer;

    IntData() noexcept : i_data(), capacity() {}
    explicit IntData(size_type cap) 
    : i_data(new_array(cap)), capacity(cap) {}

    IntData(IntData &&mov) noexcept
    : i_data(Move(mov.i_data)), capacity(Move(mov.capacity)) {}
    IntData& operator = (IntData &&mov) & noexcept
    {
        swap(*this, mov);
        return *this;
    }

    IntData(const IntData &cpy) = delete;
    IntData& operator = (const IntData&) & = delete;

    ~IntData()
    {
        delete_array(this->i_data);
    }

    friend void swap(IntData &a, IntData &b) noexcept
    {
        IntData::swap(a, b);
    }


    base_type& operator[] (size_type i)
    {
        return this->i_data[i];
    }
    base_type operator[] (size_type i) const
    {
        return this->i_data[i];
    }


    const_pointer cbegin() const
    {
        return this->i_data;
    }
    const_pointer begin() const
    {
        return this->cbegin();
    }
    pointer begin()
    {
        return this->i_data;
    }


    void reset(size_type new_cap)
    {
        if(new_cap > this->capacity)
          *this = IntData(new_cap);
    }


    void zero_init(size_type start, size_type len)
    {
        if (len != 0)
          memset(this->i_data + start, 0, len * sizeof(base_type));
    }


  private:
    static void swap(IntData &a, IntData &b) noexcept
    {
        using std::swap;
        swap(a.i_data, b.i_data);
        swap(a.capacity.value, b.capacity.value);
    }


    static pointer new_array(size_type len)
    { 
        return len ? new base_type[len] : pointer()/*nullptr*/;
    }
    static void delete_array(pointer ptr)
    {
        delete[] ptr;
    }



    ///i for implement
    pointer i_data;

  public:
    ReadOnlyProperty<size_type, IntData> capacity;

};

class bit_proxy
{
  public:
    bit_proxy(base_type &obj, int pos) noexcept 
    : i_ptr(&obj), i_pos() {}

    bit_proxy(const bit_proxy&) noexcept = default;

    const bit_proxy& operator = (const bit_proxy &b) const noexcept
    { 
        return *this = static_cast<bool>(b);
    }

    const bit_proxy& operator = (bool b) const noexcept
    {
        *this->i_ptr &= ~this->mask(); 
        return *this |= b;
    }

    operator bool() const noexcept
    {
        return *this->i_ptr & this->mask();
    }

    const bit_proxy& operator &= (bool b) const noexcept
    {
        *this->i_ptr &= ~this->mask() | this->mask(b);
        return *this;
    }
    const bit_proxy& operator |= (bool b) const noexcept
    {
        *this->i_ptr |= this->mask(b);
        return *this;
    }
    const bit_proxy& operator ^= (bool b) const noexcept
    {
        *this->i_ptr ^= this->mask(b);
        return *this;
    }

  private:
    pointer i_ptr;
    int i_pos;

    base_type mask(bool b = true) const noexcept
    {
        return static_cast<base_type>(b) << this->i_pos;
    }
};

} //namespace n_Int_helper

struct uIntDivResult;

///big unsigned integer
class uInt
{
  public:
    uInt() noexcept : i_data(), i_size(0)  {}

    ///when you want to reuse resources you might not want to implement assignment in term of constructor
    uInt(std::uintmax_t val) : uInt()
    { *this = val; }

    uInt(uInt &&mov) noexcept 
    : i_data(Move(mov.i_data)), i_size(Move(mov.i_size)) {}

    uInt(const uInt &cpy) : uInt(cpy.i_size, 0)
    { *this = cpy; }

    uInt& operator = (std::uintmax_t val) &
    {
        if (val != 0)
          this->i_data.reset(5);
        this->clear();
        while(val > 0){
          this->unsafe_push_back(val % k_Base);
          val /= k_Base;
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
    uInt& operator = (const uInt &cpy) &
    {
        if (this != &cpy) {
          this->i_data.reset(cpy.i_size);
          unsafe_copy_data(*this, cpy);
        }
        return *this;
    }

    friend void swap(uInt &a, uInt &b) noexcept
    {
        using std::swap;
        swap(a.i_data, b.i_data);
        swap(a.i_size, b.i_size);
    }
    
    ///downcasts to smaller primitive integers
    template<typename intg, n_Int_helper::isIntegral_t<intg> = nullptr>
    explicit operator intg() const noexcept
    {
        return down_cast(*this);
    }

    explicit operator bool() const
    {
        return this->i_size;
    }

    void parse(str_arg str) &
    {
        this->parse(str.data(), str.size());
    }
    void parse(const char *str) &
    {
        this->parse(str, strlen(str));
    }
    void parse(const char *str, size_t len) &
    {
        while(len > 0 && *str == '0')
        { ++str; --len; }
        if (len == 0) 
        { this->clear(); return; }

        size_type estimate_size = 
         std::ceil((std::log2(n_Int_helper::ten) / k_BaseBinDigit + .00001) * len);
        estimate_size += 2;
        this->i_data.reset(estimate_size);
        this->clear();

        size_type start = (len - 1) % k_ioDecDigit + 1;
        unsafe_add(*this, str_to_base(str, start), this);
        while(start < len){
          unsafe_small_mult(*this, k_ioUnit, this);
          unsafe_add(*this, str_to_base(str + start, k_ioDecDigit), this);
          start += k_ioDecDigit;
        }

    }

    friend std::string to_string(uInt x)
    {
        if (!x) return "0";
        std::string res;
        while(x){
          calc_type unit = unsafe_small_divide(x, k_ioUnit, &x);
          if (x) 
            for(int i = 0; i < k_ioDecDigit; ++i){
              res.push_back('0' + unit % n_Int_helper::ten);
              unit /= n_Int_helper::ten;
            }
          else
            while(unit){
              res.push_back('0' + unit % n_Int_helper::ten);
              unit /= n_Int_helper::ten;
            }
        }
        std::reverse(res.begin(), res.end());
        return res;
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
    
    friend uInt operator + (uInt);
    
    uInt& operator ++() &
    {
        for(size_type i = 0; i < this->i_size; ++i) {
          if (this->i_data[i] == k_Base - 1)
            this->i_data[i] = 0;
          else {
            ++(this->i_data[i]);
            return *this;
          }
        }
        if (this->i_size == this->i_data.capacity)
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
        if (!*this) n_Int_helper::throw_unsigned_integer_underflow_exception();
        for(size_type i = 0; i < this->i_size; ++i) {
          if (this->i_data[i] == 0)
            this->i_data[i] = k_Base - 1;
          else {
            --(this->i_data[i]);
            if (i == this->i_size - 1 && this->i_data[i] == 0)
              --(this->i_size);
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
        if (&lhs == &rhs && lhs.i_data.capacity >= max_sum_size(lhs, lhs)) {
          unsafe_add(lhs, lhs, &lhs);
          return std::move(lhs);
        }
        if (lhs.i_data.capacity < rhs.i_data.capacity) 
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
        if (dec.unit >= x.i_size) return false;
        return x.i_data[dec.unit] & (static_cast<base_type>(1u) << dec.digit);
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
        if(lhs.i_data.capacity < rhs.i_data.capacity)
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
        if(lhs.i_data.capacity < rhs.i_data.capacity)
          return lhs ^ std::move(rhs);
        return rhs ^ std::move(lhs);
    }
    friend uInt operator >> (const uInt &lhs, size_t rhs)
    {
        uInt res(max_shr_size(lhs, rhs), 0);
        unsafe_shr(lhs, rhs, &res);
        return res;
    }
    friend uInt operator >> (uInt &&lhs, size_t rhs)
    {
        unsafe_shr(lhs, rhs, &lhs);
        return std::move(lhs);
    }
    friend uInt operator << (const uInt &lhs, size_t rhs)
    {
        uInt res(max_shl_size(lhs, rhs), 0);
        unsafe_shl(lhs, rhs, &res);
        return res;
    }
    friend uInt operator << (uInt &&lhs, size_t rhs)
    {
        if(lhs.i_data.capacity < max_shl_size(lhs, rhs))
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

  private:
    using size_type = n_Int_helper::size_type;
    using base_type = n_Int_helper::base_type;
    using calc_type = n_Int_helper::calc_type;
    using wcalc_type = n_Int_helper::wcalc_type;

    static constexpr int k_BaseBinDigit = 30;
    static constexpr base_type k_Base = 1u << k_BaseBinDigit;
    static constexpr wcalc_type 
    k_SmallDivisorLimit = std::numeric_limits<wcalc_type>::max() / k_Base;

    static constexpr int k_ioDecDigit = 9;
    static constexpr base_type k_ioUnit = 1000000000;


    class SegView
    {
        using size_type = n_Int_helper::size_type;

      public:
        SegView() = delete;
      
        SegView(const uInt &s, size_type start = 0) noexcept
        : i_start(s.i_data.begin() + start), size(s.i_size - start) {}
        SegView(const uInt &s, size_type start, size_type len) noexcept
        : i_start(s.i_data.begin() + start), size(len) {}

        SegView(const base_type &b) noexcept : i_start(&b), size(!!b) {}

        SegView(SegView s, size_type start) noexcept
        : i_start(s.i_start + start), size(s.size - start) {}
        SegView(SegView s, size_type start, size_type len) noexcept
        : i_start(s.i_start + start), size(len) {}

        base_type operator[] (size_type i) const
        {
            return this->i_start[i];
        }


        n_Int_helper::IntData::const_pointer cbegin() const
        {
            return this->i_start;
        }
        auto begin() const -> decltype(this->cbegin())
        {
            return this->cbegin();
        }


        friend bool operator == (SegView lhs, SegView rhs)
        {
            if (lhs.size != rhs.size) return false;
            for(size_type i = lhs.size; i-- > 0;)
              if (lhs[i] != rhs[i]) return false;
            return true;
        }
        friend bool operator != (SegView lhs, SegView rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator < (SegView lhs, SegView rhs)
        {
            if (lhs.size != rhs.size) 
              return lhs.size < rhs.size;
            for(size_type i = lhs.size; i-- > 0;)
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
            if(this->size <= start_unit) return 0;
            wcalc_type res = 0;
            size_type i = this->size;
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
            if(this->size == 0) return 0;
            size_type res = k_BaseBinDigit * (this->size - 1);
            for(calc_type tmp = (*this)[this->size - 1]; tmp > 0; tmp >>= 1)
              ++res;
            return res;
        }
        
      private:
        const n_Int_helper::IntData::const_pointer i_start;

      public:
        const size_type size;
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


    uInt(size_type cap, size_type s) : i_data(cap), i_size(s) {}

    uInt(uInt &cpy, size_type cap) : i_data(cap), i_size(cpy.i_size) 
    {
        *this = cpy;
    }

    
    static std::uintmax_t down_cast(const uInt &x) noexcept
    {
        std::uintmax_t res = 0;
        for(size_type i = x.i_size; i-- > 0;)
          res = res * k_Base + x.i_data[i];
        return res;
    }

    static base_type str_to_base(const char *str, size_type len)
    {
        calc_type res = 0;
        for(size_type i = 0; i < len; ++i)
          res = res * n_Int_helper::ten + (str[i] - '0');
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
        if (rhs.i_data.capacity < max_sum_size(lhs, rhs))
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
        if (rhs.i_data.capacity < max_dif_size(lhs, rhs))
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
        if(lhs.size < rhs.size) return product(rhs, lhs);
        // std::cout << indent << lhs.size << ' ' << rhs.size << '\n';
        // autoindent aut;
        if (exceed_threshold(lhs.size, rhs.size))
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
        size_type half = (lhs.size + 1) / 2;

        if (rhs.size <= half) {
          uInt res1 = product(SegView(lhs, 0, half), rhs);
          uInt res2 = product(SegView(lhs, half), rhs);
          unsafe_copy_data(res, res1);
          res.i_size = res.i_data.capacity;
          res.i_data.zero_init(res1.i_size, res.i_data.capacity - res1.i_size);
          for(size_type i = 0; i < res2.i_size; ++i)
            if ((res.i_data[i + half] += res2.i_data[i]) >= k_Base) { 
              res.i_data[i + half] -= k_Base;
              ++res.i_data[i + half + 1];
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

          res.i_size = res.i_data.capacity;
          calc_type tmp = 0, carry = 0;
          for(size_type i = 0; i < res.i_size; ++i){
            tmp = carry;
            carry = 0;
            if(i < res1.i_size) 
              tmp += res1.i_data[i];
            if(half <= i && i < half + res2.i_size)
              tmp += res2.i_data[i - half];
            if(half * 2 <= i && i < half * 2 + res3.i_size)
              tmp += res3.i_data[i - half * 2];
            while (tmp >= k_Base){
              tmp -= k_Base;
              ++carry;
            }
            res.i_data[i] = tmp;
          }
          res.trim_zero();
        }

        return res;
    }

    /// schoolbook algorithm
    static uInt small_ordered_product(SegView lhs, SegView rhs)
    {
        uInt res(max_prod_size(lhs, rhs), lhs.size);
        res.i_data.zero_init(0, lhs.size);
        for(size_type i = 0; i < rhs.size; ++i){
          wcalc_type tmp = 0;
          wcalc_type cur_digit = rhs[i];
          for(size_type j = 0; j < lhs.size; ++j){
            tmp += lhs[j] * cur_digit + res.i_data[j + i];
            res.i_data[j + i] = tmp & (k_Base - 1);
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
        remainder.i_data.reset(rhs.i_size + 1);
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
        for(size_type i = lhs.size; i-- > 0;)
          rem = ((rem << k_BaseBinDigit) + lhs[i]) % rhs;
        return rem;
    }


    ///unsafe functions assume capacity is sufficient
    //res may point to lhs or rhs
    static void unsafe_add(SegView lhs, SegView rhs, uInt *res)
    {
        if(lhs.size < rhs.size) return unsafe_add(rhs, lhs, res);
        calc_type tmp = 0, carry = 0;
        for(size_type i = 0; i < lhs.size; ++i) {
          tmp = lhs[i] + carry;
          if (i < rhs.size) tmp += rhs[i];
          carry = tmp >> k_BaseBinDigit;
          tmp &= k_Base - 1;
          res->i_data[i] = tmp;
        }
        res->i_data[lhs.size] = carry;
        res->i_size = lhs.size + carry;
    }
    static void unsafe_subtract(SegView lhs, SegView rhs, uInt *res)
    {
        if (lhs.size < rhs.size)
          n_Int_helper::throw_unsigned_integer_underflow_exception();
        calc_type tmp = 0, carry = 0;
        for(size_type i = 0; i < lhs.size; ++i) {
          tmp = k_Base + lhs[i] - carry;
          if (i < rhs.size) tmp -= rhs[i];
          carry = !(tmp >> k_BaseBinDigit);
          tmp &= k_Base - 1;
          res->i_data[i] = tmp;
        }
        if (carry)
          n_Int_helper::throw_unsigned_integer_underflow_exception();
        res->i_size = lhs.size;
        res->trim_zero();
    }
    static void unsafe_small_mult(SegView lhs, calc_type rhs, uInt *res)
    {
        wcalc_type tmp = 0, r = rhs;
        for(size_type i = 0; i < lhs.size; ++i){
          tmp += r * lhs[i];
          res->i_data[i] = tmp & (k_Base - 1);
          tmp >>= k_BaseBinDigit;
        }
        res->i_size = lhs.size;
        if (tmp != 0) res->unsafe_push_back(tmp);
        res->trim_zero();
    }
    static wcalc_type 
    unsafe_small_divide(SegView lhs, wcalc_type rhs, uInt *quo)
    {
        wcalc_type rem = 0;
        for(size_type i = lhs.size; i-- > 0;){
          rem = (rem << k_BaseBinDigit) + lhs[i];
          rem = rem - rhs * (quo->i_data[i] = rem / rhs);
        }
        quo->i_size = lhs.size;
        quo->trim_zero();
        return rem;
    }
    static void unsafe_and(const uInt &lhs, const uInt &rhs, uInt *res)
    {
        if(lhs.i_size > rhs.i_size) return unsafe_and(rhs, lhs, res);
        for(size_type i = 0; i < lhs.i_size; ++i)
          res->i_data[i] = lhs.i_data[i] & rhs.i_data[i]; 
        res->i_size = lhs.i_size;
        res->trim_zero();
    }
    static void unsafe_or(const uInt &lhs, const uInt &rhs, uInt *res)
    {
        if(lhs.i_size < rhs.i_size) return unsafe_or(rhs, lhs, res);
        for(size_type i = 0; i < lhs.i_size; ++i)
          res->i_data[i] = lhs.i_data[i] | (i < rhs.i_size ? rhs.i_data[i] : 0); 
        res->i_size = lhs.i_size;
    }
    static void unsafe_xor(const uInt &lhs, const uInt &rhs, uInt *res)
    {
        if(lhs.i_size < rhs.i_size) return unsafe_xor(rhs, lhs, res);
        for(size_type i = 0; i < lhs.i_size; ++i)
          res->i_data[i] = lhs.i_data[i] ^ (i < rhs.i_size ? rhs.i_data[i] : 0); 
        res->i_size = lhs.i_size;
        res->trim_zero();
    }
    static void unsafe_shr(const uInt &lhs, size_t rhs, uInt *res)
    {
        if (rhs < 0) throw std::invalid_argument("invalid argument");
        auto dec = decompose(rhs);
        if(lhs.i_size < dec.unit) {}
        else if (dec.digit == 0) {
          if(dec.unit || (lhs.i_data.cbegin() != res->i_data.begin()))
            std::copy_n(lhs.i_data.cbegin() + dec.unit, 
                        lhs.i_size - dec.unit, res->i_data.begin());
        }
        else {
          res->i_data[0] = lhs.i_data[dec.unit] >> dec.digit;
          for(size_type i = dec.unit + 1; i < lhs.i_size; ++i){
            res->i_data[i - dec.unit - 1] 
             |= (lhs.i_data[i] << (k_BaseBinDigit - dec.digit)) & (k_Base - 1);
            res->i_data[i - dec.unit] = lhs.i_data[i] >> dec.digit;
          }
        }
        res->i_size = max_shr_size(lhs, rhs);
        res->trim_zero();
    }
    static void unsafe_shl(const uInt &lhs, size_t rhs, uInt *res)
    {
        if (rhs < 0) throw std::invalid_argument("invalid argument");
        auto dec = decompose(rhs);
        res->i_data.zero_init(0, dec.unit);
        if (dec.digit == 0) {
          if(dec.unit || (lhs.i_data.cbegin() != res->i_data.begin()))
            std::copy_n(lhs.i_data.cbegin(), 
                        lhs.i_size, res->i_data.begin() + dec.unit);
        }
        else {
          res->i_data[lhs.i_size + dec.unit + 1] = 0;
          for(size_type i = lhs.i_size; i-- > 0;){
            res->i_data[i + dec.unit + 1] |=
              lhs.i_data[i] >> (k_BaseBinDigit - dec.digit);
            res->i_data[i + dec.unit] = 
              (lhs.i_data[i] << dec.digit) & (k_Base - 1);
          }
        }
        res->i_size = max_shl_size(lhs, rhs);
        res->trim_zero();
    }



    static size_type max_sum_size(SegView lhs, SegView rhs)
    {
        return std::max(lhs.size, rhs.size) + 1;
    }
    static size_type max_dif_size(SegView lhs, SegView rhs)
    {
        return lhs.size;
    }
    static size_type max_prod_size(SegView lhs, SegView rhs)
    {
        return lhs.size + rhs.size;
    }
    static size_type max_div_quo_size(SegView lhs, wcalc_type rhs)
    {
        return lhs.size;
    }
    static size_type max_and_size(const uInt &lhs, const uInt &rhs)
    {
        return std::min(lhs.i_size, rhs.i_size);
    }
    static size_type max_or_size(const uInt &lhs, const uInt &rhs)
    {
        return std::max(lhs.i_size, rhs.i_size);
    }
    static size_type max_xor_size(const uInt &lhs, const uInt &rhs)
    {
        return std::max(lhs.i_size, rhs.i_size);
    }
    static size_type max_shr_size(const uInt &lhs, size_t rhs)
    {
        return std::max<base_type>(lhs.i_size - rhs / k_BaseBinDigit, 0);
    }
    static size_type max_shl_size(const uInt &lhs, size_t rhs)
    {
        return lhs.i_size + (rhs + k_BaseBinDigit - 1) / k_BaseBinDigit;
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
        while(this->i_size > 0 && this->back() == 0)
          --this->i_size;
    }
    bool is_small_divisor() const
    {
        return this->i_size <= 2 && down_cast(*this) < k_SmallDivisorLimit;
    }
    static void copy_and_shift(uInt &dest, SegView src)
    {
        std::copy_n(src.cbegin(), src.size, dest.i_data.begin() + 1);
        dest.i_size = src.size + 1;
    }
    static void unsafe_copy_data(uInt &dest, SegView src)
    {
        std::copy_n(src.cbegin(), src.size, dest.i_data.begin());
        dest.i_size = src.size;
    }


    base_type back() const
    {
        return this->i_data[this->i_size - 1];
    }
  
    void unsafe_push_back(base_type c)
    {
        this->i_data[this->i_size++] = c;
    }


    ///reserve(capacity + 1) 
    ///rewind the effect of ++ if exception occurs
    void increment_safe_reserve()
    {
        try{this->reserve(this->i_data.capacity + 1);}
        catch(...) {
          for(size_type i = 0; i < this->i_size; ++i)
            this->i_data[i] = k_Base - 1;
          throw;
        }
    }


    void clear()
    {
        this->i_size = 0;
    }

    void reserve(size_type cap)
    {
        if (this->i_data.capacity < cap)
          *this = uInt(*this, cap);
    }
  



    n_Int_helper::IntData i_data;
    size_type i_size;
};


inline uInt operator + (uInt x)
{
    return x;
}


struct uIntDivResult
{
    uInt quo, rem;
};

inline uInt operator / (const uInt &divident, const uInt &divisor)
{
    if (divisor.i_size == 0) 
      n_Int_helper::throw_division_by_zero_exception();
    if (divident < divisor) 
      return uInt();
    if (divisor.is_small_divisor())
      return uInt::small_division(divident, uInt::down_cast(divisor));
    return div(divident, divisor).quo;
}
inline uInt operator % (const uInt &divident, const uInt &divisor)
{
    if (divisor.i_size == 0) 
      n_Int_helper::throw_division_by_zero_exception();
    if (divident < divisor) 
      return divident;
    if (divisor.is_small_divisor())
      return uInt::small_modulo(divident, uInt::down_cast(divisor));
    return div(divident, divisor).rem;
}
inline uIntDivResult div(const uInt &divident, const uInt &divisor)
{
    if (divisor.i_size == 0) 
      n_Int_helper::throw_division_by_zero_exception();
    if (divident < divisor) 
      return uIntDivResult{uInt(), divident};
    if (divisor.is_small_divisor())
      return uIntDivResult{
          uInt::small_division(divident, uInt::down_cast(divisor)),
          uInt::small_modulo(divident, uInt::down_cast(divisor))};

    /*else*/ /*school-book algorithm*/
    uInt::size_type quotient_max_size = divident.i_size - divisor.i_size + 1;
    uIntDivResult res = {uInt{quotient_max_size, quotient_max_size},
                      uInt{divisor.i_size + 1, divisor.i_size - 1}};
    uInt cur_divident(divisor.i_size + 1, 0);
    /// copy <divisor.digitcount - 1> divident's most significant digits to cur remainder
    std::copy_n(divident.i_data.cbegin() + quotient_max_size, 
                divisor.i_size - 1, res.rem.i_data.begin());

    for(uInt::size_type i = quotient_max_size; i-- > 0;) {
      uInt::copy_and_shift(cur_divident, res.rem);
      cur_divident.i_data[0] = divident.i_data[i];
      res.quo.i_data[i] = 
          uInt::small_div(cur_divident, divisor, &res.rem);
    }
    res.quo.trim_zero();
    return res;
}


template<> uInt convert<uInt>(str_arg str)
{
    uInt res;
    res.parse(str);
    return res;
}

template<typename istr>
auto operator >> (istr &is, uInt &x) 
-> decltype(is >> std::declval<std::string&>())
{
    std::string input;
    auto &&chain = is >> input;
    x.parse(input);
    return std::forward<decltype(chain)>(chain);
}

template<typename ostr>
auto operator << (ostr &os, const uInt &x)
-> decltype(os << to_string(x))
{
    return os << to_string(x);
}



struct IntDivResult;

///big signed integer
class Int
{
  public:
    Int() noexcept = default;

    template<typename intg, n_Int_helper::isIntegral_t<intg> = nullptr>
    Int(intg val) : Int()
    {
        *this = val;
    }
    
    Int(uInt uns) : Int{positive, std::move(uns)} {}

    Int(Int&&) noexcept = default;
    Int(const Int&) = default;

    template<typename intg, n_Int_helper::isIntegral_t<intg> = nullptr>
    Int& operator = (intg val) &
    {
        sign_type new_sign = positive;
        if (val < 0) {
          new_sign = negative;
          val = -val;
        }
        if (val == 0) this->sign = zero;
        else {
          this->i_abs = val;
          this->sign = new_sign;
        }
        return *this;
    }

    Int& operator = (Int&&) & noexcept = default;
    Int& operator = (const Int&) & = default;

    ///have to call member func because of friend-ing issue
    friend void swap(Int &a, Int &b) noexcept
    {
        Int::swap(a, b);
    }


    using sign_type = int;
    static constexpr sign_type zero = 0;
    static constexpr sign_type positive = 1;
    static constexpr sign_type negative = -positive;

    ReadOnlyProperty<sign_type, Int> sign;


    template<typename intg, n_Int_helper::isIntegral_t<intg> = nullptr>
    explicit operator intg() const noexcept
    {
        return static_cast<intg>(this->i_abs) * this->sign;
    }

    explicit operator bool() const noexcept
    {
        return this->sign;
    }


    void parse(str_arg str) &
    {
        this->parse(str.data(), str.size());
    }
    void parse(const char *str) &
    {
        this->parse(str, strlen(str));
    }
    void parse(const char *str, size_t len) &
    {
        sign_type new_sign = positive;
        const char* en = str + len;
        if (*str == '-') {
          new_sign = -new_sign;
          ++str;
        }
        this->i_abs.parse(str, en - str);
        if (!this->i_abs) this->sign = zero;
        else this->sign = new_sign;
    }

    friend std::string to_string(const Int &x)
    { 
        if (x.sign == negative) return '-' + to_string(x.i_abs);
        return to_string(x.i_abs);
    }


    friend bool operator == (const Int &lhs, const Int &rhs)
    {
        return lhs.sign == rhs.sign && lhs.i_abs == rhs.i_abs;
    }
    friend bool operator != (const Int &lhs, const Int &rhs)
    {
        return !(lhs == rhs);
    }
    friend bool operator < (const Int &lhs, const Int &rhs)
    {
        if (&lhs == &rhs) return false;
        if (lhs.sign != rhs.sign) return lhs.sign < rhs.sign;
        if (lhs.sign == positive) return lhs.i_abs < rhs.i_abs;
        if (lhs.sign == negative) return rhs.i_abs < lhs.i_abs;
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


    friend void negate(Int &x)
    {
        x.negate();
    }


    friend Int operator + (Int);
    friend Int operator - (Int);


    Int& operator ++ () &
    {
        if (this->sign == negative) {
          --this->i_abs;
          if (!this->i_abs) 
            this->sign = zero;
        }else {
          ++this->i_abs;
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
          --this->i_abs;
          if (!this->i_abs) 
            this->sign = zero;
        }else {
          ++this->i_abs;
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
        if(lhs.sign * rhs.sign != negative)
          return Int{(lhs.sign ? lhs.sign : rhs.sign), 
                      lhs.i_abs + rhs.i_abs};
        if (lhs.i_abs < rhs.i_abs)
          return Int{rhs.sign, rhs.i_abs - lhs.i_abs}; 
        else
          return Int{lhs.sign, lhs.i_abs - rhs.i_abs};
    }
    friend Int operator + (const Int &lhs, Int &&rhs)
    {
        if(lhs.sign * rhs.sign != negative)
          return Int{(lhs.sign ? lhs.sign : rhs.sign), 
                      lhs.i_abs + std::move(rhs.i_abs)};
        if (lhs.i_abs < rhs.i_abs)
          return Int{rhs.sign, std::move(rhs.i_abs) - lhs.i_abs}; 
        else
          return Int{lhs.sign, lhs.i_abs - std::move(rhs.i_abs)};
    }
    friend Int operator + (Int &&lhs, const Int &rhs)
    {
        return rhs + std::move(lhs);
    }
    friend Int operator + (Int &&lhs, Int &&rhs)
    {
        if(lhs.sign * rhs.sign != negative)
          return Int{(lhs.sign ? lhs.sign : rhs.sign), 
                      std::move(lhs.i_abs) + std::move(rhs.i_abs)};
        if (lhs.i_abs < rhs.i_abs)
          return Int{rhs.sign, std::move(rhs.i_abs) - std::move(lhs.i_abs)}; 
        else
          return Int{lhs.sign, std::move(lhs.i_abs) - std::move(rhs.i_abs)};
    }

    friend Int operator - (const Int &lhs, const Int &rhs)
    {
        if(lhs.sign * -rhs.sign != negative)
          return Int{(lhs.sign ? +lhs.sign : -rhs.sign), 
                      lhs.i_abs + rhs.i_abs};
        if (lhs.i_abs < rhs.i_abs)
          return Int{-rhs.sign, rhs.i_abs - lhs.i_abs}; 
        else
          return Int{+lhs.sign, lhs.i_abs - rhs.i_abs};
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
        return Int{lhs.sign * rhs.sign, lhs.i_abs * rhs.i_abs};
    }
    friend Int operator / (const Int &lhs, const Int &rhs)
    {
        return Int{lhs.sign * rhs.sign, lhs.i_abs / rhs.i_abs};
    }
    friend Int operator % (const Int &lhs, const Int &rhs)
    {
        return Int{lhs.sign * rhs.sign, lhs.i_abs % rhs.i_abs};
    }
    friend IntDivResult div(const Int&, const Int&);


  private:
    Int(sign_type si, uInt b) noexcept : sign(si * !!b), i_abs(Move(b)) {}

    
    static void swap(Int &a, Int &b) noexcept
    {
        using std::swap;
        swap(a.sign.value, b.sign.value);
        swap(a.i_abs, b.i_abs);
    }


    void negate() &
    {
        this->sign.value = -this->sign.value;
    }


    uInt i_abs;
};


inline Int operator + (Int x)
{
    return x;
}
inline Int operator - (Int x)
{
    negate(x);
    return x;
}

struct IntDivResult
{
    Int quo, rem;
};

IntDivResult div(const Int &lhs, const Int &rhs)
{
    auto res = div(lhs.i_abs, rhs.i_abs);
    return {Int{lhs.sign * rhs.sign, std::move(res.quo)},
            Int{lhs.sign * rhs.sign, std::move(res.rem)}};
}



template<> Int convert<Int>(str_arg str)
{
    Int res;
    res.parse(str);
    return res;
}

template<typename istr>
auto operator >> (istr &is, Int &x) 
-> decltype(is >> std::declval<std::string&>())
{
    std::string input;
    decltype(is >> input) chain = is >> input;
    x.parse(input);
    return chain;
}

template<typename ostr>
auto operator << (ostr &os, const Int &x) 
-> decltype(os << to_string(x))
{
    return os << to_string(x);
}






















#if ACHIBULUP__Cpp14_later
struct uint128_t
{
    uint64_t sc, fi;
    uint128_t() noexcept = default;

    template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
    constexpr uint128_t(const Tp &val) : sc((val < 0) ? -val : val), fi((val < 0) ? ~0ull : 0) {}
    constexpr uint128_t(uint64_t _sc, uint64_t _fi) : sc(_sc), fi(_fi) {}

    constexpr uint128_t(const uint128_t&) = default;
    constexpr uint128_t& operator = (const uint128_t&) & = default;

    template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
    explicit constexpr operator Tp() const
    {
        return sc;
    }
    explicit constexpr operator bool () const
    {
        return fi || sc;
    }


    static constexpr uint64_t mask_second = (1ull << 32) - 1;
};
inline constexpr bool operator == (const uint128_t &l, const uint128_t &r)
{
    return l.fi == r.fi && l.sc == r.sc;
}
inline constexpr bool operator != (const uint128_t &l, const uint128_t &r)
{   return !(l == r); }
inline constexpr bool operator < (const uint128_t &l, const uint128_t &r)
{
    return l.fi < r.fi || (l.fi == r.fi && l.sc < r.sc);
}
inline constexpr bool operator > (const uint128_t &l, const uint128_t &r)
{   return r < l; }
inline constexpr bool operator <= (const uint128_t &l, const uint128_t &r)
{   return !(r < l); }
inline constexpr bool operator >= (const uint128_t &l, const uint128_t &r)
{   return !(l < r); }

inline constexpr uint128_t operator & (const uint128_t &l, const uint128_t &r)
{
    return {l.sc & r.sc, l.fi & r.fi};
}
inline constexpr uint128_t operator | (const uint128_t &l, const uint128_t &r)
{
    return {l.sc | r.sc, l.fi | r.fi};
}
inline constexpr uint128_t operator ^ (const uint128_t &l, const uint128_t &r)
{
    return {l.sc ^ r.sc, l.fi ^ r.fi};
}

inline constexpr uint128_t operator << (const uint128_t &x, const uint128_t &sh)
{
    return (!sh.fi && sh.sc < 64) ?
    uint128_t{x.sc << sh.sc, (x.fi << sh.sc) ^ (x.sc >> (64 - sh.sc))}
  : uint128_t{0, x.sc << (sh.sc - 64)};
}
inline constexpr uint128_t operator >> (const uint128_t &x, const uint128_t &sh)
{
    return (!sh.fi && sh.sc < 64) ?
    uint128_t{(x.sc >> sh.sc) ^ (x.fi << (64 - sh.sc)), x.fi >> sh.sc}
  : uint128_t{x.fi >> (sh.sc - 64), 0};
}
inline constexpr uint128_t operator ~ (const uint128_t &x)
{
    return uint128_t{~x.sc, ~x.fi};
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
    return x.fi ? count_leading_zero(x.fi) : (64 + count_leading_zero(x.sc));
}


inline constexpr uint128_t operator + (uint128_t x)
{
    return x;
}
inline constexpr uint128_t operator - (const uint128_t &x)
{
    return {~x.sc + 1, ~x.fi + !x.sc};
}

inline constexpr uint128_t& operator ++(uint128_t &x)
{
    ++x.sc;
    x.fi += !x.sc;
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
    --x.sc;
    x.fi -= !~x.sc;
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
    return {l.sc + r.sc, l.fi + r.fi + (l.sc > ~r.sc)};
}
inline constexpr uint128_t operator - (const uint128_t &l, const uint128_t &r)
{
    return {l.sc - r.sc, l.fi - r.fi - (l.sc < r.sc)};
}

inline constexpr uint128_t& operator += (uint128_t &x, const uint128_t &d)
{   return x = x + d; }
inline constexpr uint128_t& operator -= (uint128_t &x, const uint128_t &d)
{   return x = x - d; }


inline constexpr uint128_t operator * (const uint128_t &l, const uint128_t &r)
{
    uint128_t res{static_cast<uint32_t>(l.sc) * (r.sc & uint128_t::mask_second),
      l.fi * r.sc + l.sc * r.fi + static_cast<uint32_t>(l.sc >> 32) * (r.sc >> 32)};
    uint64_t v1 = static_cast<uint32_t>(l.sc) * (r.sc >> 32);
    uint64_t v2 = static_cast<uint32_t>(r.sc) * (l.sc >> 32);
    res.fi += (v1 >> 32) + (v2 >> 32);
    v1 = (res.sc >> 32) + static_cast<uint32_t>(v1) + static_cast<uint32_t>(v2);
    res.fi += v1 >> 32;
    res.sc = (res.sc & uint128_t::mask_second) + ((v1 & uint128_t::mask_second) << 32);
    return res;
}
inline constexpr uint128_t operator / (uint128_t l, uint128_t r)
{
    uint128_t res{};
    if (l < r);
    else if (!l.fi) {
      if (!r.fi) res.sc = l.sc / r.sc;
    }
    else if (!r.fi && r.sc < (1ull << 32)) {
      uint32_t div = static_cast<uint32_t>(r.sc);
      res.fi = l.fi / div;
      unsigned long long temp = (static_cast<uint64_t>(l.fi % div) << 32) + (l.sc >> 32);
      res.sc = ((temp / div) << 32) + ((static_cast<uint64_t>(temp % div) << 32) + (l.sc & uint128_t::mask_second)) / div;
    }
    else {
      unsigned shift = count_leading_zero(r);
      r <<= shift;
      if (shift >= 64) {
        unsigned shift2 = shift - 64;
        do{
          if (l >= r) {
            res.fi |= (1ull << shift2);
            l -= r;
          }
          r >>= 1;
        }while(shift2--);
        shift = 63;
      }
      do{
        if (l >= r) {
          res.fi |= (1ull << shift);
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
    else if (!l.fi) res.sc = l.sc % r.sc;
    else if (r.sc < (1ull << 32)) {
      uint32_t div = static_cast<uint32_t>(r.sc);
      res.sc = ((((((l.fi % div) << 32) + (l.sc >> 32)) % div) << 32) + (l.sc & uint128_t::mask_second)) % div;
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
    return !l.fi && l.sc == r;
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
    return !l.fi && l.sc < r;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr bool operator < (Tp l, const uint128_t &r)
{
    return r.fi || l < r.sc;
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
    return sh.fi ? (x << (64 + !sh.fi)) : (x << sh.sc);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp operator >> (Tp x, const uint128_t &sh)
{ 
    return sh.fi ? (x >> (64 + !sh.fi)) : (x >> sh.sc);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp& operator &= (Tp &x, const uint128_t &v)
{ 
    return x = x & v.sc;
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
    return (v.fi) ? (x = 0) : (x /= v.sc);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
inline constexpr Tp& operator %= (Tp &x, const uint128_t &v)
{ 
    return (v.fi) ? x : (x %= v.sc);
}





inline std::istream& operator >> (std::istream &is, uint128_t &x)
{
    std::string istr;
    x = {};
    is >> istr;
    uint64_t temp1, temp2;
    for(std::string::size_type i = 0, len = istr.size(); i < len; ++i) {
      temp1 = (x.sc & uint128_t::mask_second) * n_Int_helper::ten;
      temp2 = (temp1 >> 32) + ((x.sc >> 32) * n_Int_helper::ten);
      x.sc = (temp1 & uint128_t::mask_second) + (temp2 << 32);
      x.fi = x.fi * n_Int_helper::ten + (temp2 >> 32);
      x.sc += istr[i] - '0';
    }
    return is;
}
inline std::ostream& operator << (std::ostream &os, uint128_t x)
{
    char ostr[40];
    if (!x) return os << '0';
    int first = 40;
    while(x) {
      ostr[--first] = int(x % n_Int_helper::ten) + '0';
      x /= n_Int_helper::ten;
    }
    return os << (ostr + first);
}
#endif // __cplusplus
} //namespace Achibulup
#endif //BIGNUM_H_INCLUDED

