#ifndef BIGNUM_HPP_INCLUDED
#define BIGNUM_HPP_INCLUDED

//Big integer handling
//the two class Int and uInt provide support for big integer arithmetics
//Int can handle signed integer while uInt also supports bitwise operations for unsigned integer


#include "common_utils.h"
#include <cmath> //log2
#include <string> //input
#include <limits> //unsigned long long max
#include <memory> //unique_ptr
#include <cstring> //memset, memcpy
#include <cstdint> //int types
#include <iostream> //in-out put
#include <algorithm> //max, min
#include <stdexcept> // exception

namespace Achibulup{



namespace n_Int{

constexpr int ten = 10;

inline void throw_division_by_zero_exception()
{
    throw std::domain_error("division by ZERO");
}
inline void throw_unsigned_integer_underflow_exception()
{
    throw std::underflow_error("unsigned integer underflow");
}

using size_type = size_t;
using base_type = std::uint_least32_t;
using calc_type = std::uint_fast32_t;
using wcalc_type = std::uint_fast64_t;
using const_pointer = const base_type*;
using pointer = base_type*;


static constexpr int k_BaseBinDigit = 30;
static constexpr base_type k_Base = 1u << k_BaseBinDigit;
static constexpr wcalc_type 
k_SmallDivisorLimit = std::numeric_limits<wcalc_type>::max() / k_Base;

static constexpr int k_ioDecDigit = 9;
static constexpr base_type k_ioUnit = 1000000000;

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


    const_pointer cdata() const
    {
        return this->m_data.get();
    }
    const_pointer data() const
    {
        return this->cdata();
    }
    pointer data()
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


class SegView
{
  public:
    using size_type = size_type;
    using const_pointer = IntData::const_pointer;

    SegView() = delete;
    SegView(const SegView&) = default;
  
    SegView(const_pointer start, size_type len) noexcept
    : data(start), size(len) {}

    SegView(const base_type &b) noexcept : data(&b), size(!!b) {}

    SegView(SegView s, size_type start) noexcept
    : data(s.cdata() + start), size(s.size() - start) {}
    SegView(SegView s, size_type start, size_type len) noexcept
    : data(s.cdata() + start), size(len) {}
    

    base_type operator[] (size_type i) const
    {
        return this->cdata()[i];
    }

    const_pointer cdata() const
    {
        return this->data();
    }
    ReadOnlyProperty<const_pointer, SegView> data;
    ReadOnlyProperty<size_type, SegView> size;


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
    

    ///most significant digit
    wcalc_type msd(size_type start) const;

    size_type digitCount() const noexcept;

}; //SegView

} //namespace n_Int

struct uIntDivResult;

///big unsigned integer
class uInt
{
  public:
    uInt() noexcept : m_data(), size(0)  {}

    ///when you want to reuse resources you might not want to implement assignment in term of constructor
    uInt(const uInt &cpy) : uInt(cpy.size(), 0) { *this = cpy; }

    uInt(uInt &&mov) noexcept 
    : m_data(Move(mov.m_data)), size(Move(mov.size)) {}

    uInt& operator = (const uInt &cpy) &;
    uInt& operator = (uInt &&mov) & noexcept;
    

    uInt(std::uintmax_t val) : uInt() { *this = val; }
    uInt& operator = (std::uintmax_t val) &;


    friend void swap(uInt &a, uInt &b) noexcept
    {
        a.swap(b);
    }

    void swap(uInt &other) noexcept
    {
        using std::swap;
        swap(this->m_data, other.m_data);
        swap(this->size.value, other.size.value);
    }
    
    ///downcasts to smaller primitive integers
    template<typename intg, n_Int::isIntegral_t<intg> = nullptr>
    explicit operator intg() const noexcept
    {
        return downCast(*this);
    }

    explicit operator bool() const
    {
        return this->size() != 0;
    }


    friend bool operator == (const uInt &l, const uInt &r)
    { return doEqual(l, r); }
    friend bool operator != (const uInt &l, const uInt &r)
    { return !(l == r); }
    friend bool operator < (const uInt &l, const uInt &r)
    { return doLess(l, r); }
    friend bool operator > (const uInt &l, const uInt &r)
    { return r < l; }
    friend bool operator <= (const uInt &l, const uInt &r)
    { return !(r < l); }
    friend bool operator >= (const uInt &l, const uInt &r)
    { return !(l < r); }
    
    uInt& operator ++() &;
    uInt operator ++ (int) &
    {
        uInt res = *this;
        ++(*this);
        return res;
    }

    uInt& operator --() &;
    uInt operator -- (int) &
    {
        uInt res = *this;
        --(*this);
        return res;
    }


    friend uInt operator + (const uInt &lhs, const uInt &rhs)
    {
        return doAdd(lhs, rhs);
    }
    friend uInt operator + (const uInt &lhs, uInt &&rhs)
    {
        return doAdd(lhs, std::move(rhs));
    }
    friend uInt operator + (uInt &&lhs, const uInt &rhs)
    {
        return rhs + std::move(lhs);
    }
    friend uInt operator + (uInt &&lhs, uInt &&rhs)
    {
        return doAdd(std::move(lhs), std::move(rhs));
    }

    friend uInt operator - (const uInt &lhs, const uInt &rhs)
    {
        return doSub(lhs, rhs);
    }
    friend uInt operator - (const uInt &lhs, uInt &&rhs)
    {
        return doSub(lhs, std::move(rhs));
    }
    friend uInt operator - (uInt &&lhs, const uInt &rhs)
    {
        return doSub(std::move(lhs), rhs);
    }
    friend uInt operator - (uInt &&lhs, uInt &&rhs)
    {
        return doSub(std::move(lhs), std::move(rhs));
    }

    friend uInt operator * (const uInt &lhs, const uInt &rhs)
    {
        return doMul(rhs, lhs);
    }
    
    ///have to be defined outside because of incomplete type issue
    friend uInt operator / (const uInt& lhs, const uInt &rhs)
    {
        return doDiv(lhs, rhs);
    }
    friend uInt operator % (const uInt &lhs, const uInt &rhs)
    {
        return doMod(lhs, rhs);
    }
    friend uIntDivResult divMod(const uInt&, const uInt&);


 
    bool getbit(size_t pos) const noexcept;

    friend uInt operator & (const uInt &lhs, const uInt &rhs)
    {
        return doAnd(lhs, rhs);
    }
    friend uInt operator & (const uInt &lhs, uInt &&rhs)
    {
        return doAnd(lhs, std::move(rhs));
    }
    friend uInt operator & (uInt &&lhs, const uInt &rhs)
    {
        return rhs & std::move(lhs); 
    }
    friend uInt operator & (uInt &&lhs, uInt &&rhs)
    {
        return doAnd(std::move(lhs), std::move(rhs));
    }
    friend uInt operator | (const uInt &lhs, const uInt &rhs)
    {
        return doOr(lhs, rhs);
    }
    friend uInt operator | (const uInt &lhs, uInt &&rhs)
    {
        return doOr(lhs, std::move(rhs));
    }
    friend uInt operator | (uInt &&lhs, const uInt &rhs)
    {
        return rhs | std::move(lhs); 
    }
    friend uInt operator | (uInt &&lhs, uInt &&rhs)
    {
        return doOr(std::move(lhs), std::move(rhs));
    }
    friend uInt operator ^ (const uInt &lhs, const uInt &rhs)
    {
        return doXor(lhs, rhs);
    }
    friend uInt operator ^ (const uInt &lhs, uInt &&rhs)
    {
        return doXor(lhs, std::move(rhs));
    }
    friend uInt operator ^ (uInt &&lhs, const uInt &rhs)
    {
        return rhs ^ std::move(lhs); 
    }
    friend uInt operator ^ (uInt &&lhs, uInt &&rhs)
    {
        return doXor(std::move(lhs), std::move(rhs));
    }
    friend uInt operator >> (const uInt &lhs, int rhs)
    {
        return doShr(lhs, rhs);
    }
    friend uInt operator >> (uInt &&lhs, int rhs)
    {
        return doShr(std::move(lhs), rhs);
    }
    friend uInt operator << (const uInt &lhs, int rhs)
    {
        return doShl(lhs, rhs);
    }
    friend uInt operator << (uInt &&lhs, int rhs)
    {
        return doShl(std::move(lhs), rhs);
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



    uInt& parse(string_view strv) &;
    uInt&& parse(string_view strv) &&
    {
        return std::move(this->parse(strv));
    }

    std::string toString() const &
    {
        return uInt(*this).toString();
    }
    std::string toString() &&;

    friend std::string to_string(uInt x)
    {
        return std::move(x).toString();
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

    // static constexpr int k_BaseBinDigit = n_Int::k_BaseBinDigit;
    // static constexpr base_type k_Base = n_Int::k_Base;
    // static constexpr wcalc_type k_SmallDivisorLimit = n_Int::k_SmallDivisorLimit;

    // static constexpr int k_ioDecDigit = n_Int::k_ioDecDigit;
    // static constexpr base_type k_ioUnit = n_Int::k_ioUnit;

    uInt(size_type cap, size_type s) : m_data(cap), size(s) {}

    uInt(uInt &cpy, size_type cap) : m_data(cap), size(cpy.size()) 
    {
        *this = cpy;
    }

    explicit uInt(n_Int::SegView view) : m_data(view.size())
    {
        copy(*this, view);
    }

    operator n_Int::SegView () const noexcept
    {
        return n_Int::SegView(this->data(), this->size());
    }


    static bool doEqual(n_Int::SegView l, n_Int::SegView r);
    static bool doLess(n_Int::SegView l, n_Int::SegView r);
    static uInt doAdd(n_Int::SegView l, n_Int::SegView r);
    static uInt doAdd(n_Int::SegView l, uInt &&r);
    static uInt doAdd(uInt &&l, uInt &&r);
    static uInt doSub(n_Int::SegView l, n_Int::SegView r);
    static uInt doSub(n_Int::SegView l, uInt &&r);
    static uInt doSub(uInt &&l, n_Int::SegView r);
    static uInt doSub(uInt &&l, uInt &&r);
    static uInt doMul(n_Int::SegView l, n_Int::SegView r);
    static uInt doDiv(n_Int::SegView l, const uInt &r);
    static uInt doMod(n_Int::SegView l, const uInt &r);
    static uIntDivResult doDivMod(n_Int::SegView l, const uInt &r);
    bool doGetBit(size_t pos) const noexcept;
    static uInt doAnd(const uInt &l, const uInt &r);
    static uInt doAnd(const uInt &l, uInt &&r);
    static uInt doAnd(uInt &&l, uInt &&r);
    static uInt doOr(const uInt &l, const uInt &r);
    static uInt doOr(const uInt &l, uInt &&r);
    static uInt doOr(uInt &&l, uInt &&r);
    static uInt doXor(const uInt &l, const uInt &r);
    static uInt doXor(const uInt &l, uInt &&r);
    static uInt doXor(uInt &&l, uInt &&r);
    static uInt doShl(const uInt &l, int r);
    static uInt doShl(uInt &&l, int r);
    static uInt doShr(const uInt &l, int r);
    static uInt doShr(uInt &&l, int r);

    static uInt smallOrderedProduct(n_Int::SegView l, n_Int::SegView r);
    static uInt bigOrderedProduct(n_Int::SegView l, n_Int::SegView r);
    static n_Int::base_type smallDivMod(n_Int::SegView l, const uInt &r, uInt *rem);
    static uInt smallDiv(n_Int::SegView l, n_Int::wcalc_type r);
    static uInt smallMod(n_Int::SegView l, n_Int::wcalc_type r);
    
    static std::uintmax_t downCast(const uInt &x) noexcept;
    static base_type strToBase(const char *str, size_type len);
    



    ///reserve(capacity + 1) 
    ///reverse the effect of ++ if an exception is thrown
    void incrementSafeReserve();



    ///most significant digits
    wcalc_type msd(size_type start) const
    {
        return n_Int::SegView(*this).msd(start);
    }

    size_type digitCount() const noexcept
    {
        return n_Int::SegView(*this).digitCount();
    }

    bool isSmallDivisor() const
    {
        return this->size() <= 2 
            && downCast(*this) < n_Int::k_SmallDivisorLimit;
    }

    static void copyAndShift(uInt &dest, n_Int::SegView src);
    static void copy(uInt &dest, n_Int::SegView src);

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
  
    void unsafePushBack(base_type c)
    {
        (*this)[this->size++] = c;
    }
    void popBack()
    {
        --this->size;
    }

    pointer data() &
    {
        return this->m_data.data();
    }
    const_pointer cdata() const &
    {
        return this->m_data.cdata();
    }
    const_pointer data() const &
    {
        return this->cdata();
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


template<> inline uInt convert<uInt>(string_view str)
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

    Int(uInt uns) : Int{POSITIVE, std::move(uns)} {}

    Int(Int&&) noexcept = default;
    Int(const Int&) = default;

    template<typename intg, n_Int::isIntegral_t<intg> = nullptr>
    Int& operator = (intg val) &
    {
        Sign new_sign = POSITIVE;
        if (val < 0) {
          new_sign = NEGATIVE;
          val = -val;
        }
        if (val == 0) this->sign = ZERO;
        else {
          this->m_abs = val;
          this->sign = new_sign;
        }
        return *this;
    }

    Int& operator = (Int&&) & noexcept = default;
    Int& operator = (const Int&) & = default;

    

    void swap(Int &b) noexcept
    {
        using std::swap;
        this->sign.swap(b.sign);
        swap(this->m_abs, b.m_abs);
    }
    friend void swap(Int &a, Int &b) noexcept
    {
        a.swap(b);
    }


    template<typename intg, n_Int::isIntegral_t<intg> = nullptr>
    explicit operator intg() const noexcept
    {
        return static_cast<intg>(this->m_abs) * this->sign();
    }

    explicit operator bool() const noexcept
    {
        return this->sign();
    }


    using Sign = int;
    static constexpr Sign ZERO = 0;
    static constexpr Sign POSITIVE = 1;
    static constexpr Sign NEGATIVE = -POSITIVE;


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
        if (lhs.sign() == POSITIVE) return lhs.m_abs < rhs.m_abs;
        if (lhs.sign() == NEGATIVE) return rhs.m_abs < lhs.m_abs;
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


    Int& negate()
    {
        this->sign = -this->sign();
        return *this;
    }

    friend Int abs(Int x)
    {
        if (x.sign() == NEGATIVE) x.negate();
        return x;
    }

    friend Int operator + (Int x)
    {
        return x;
    }
    friend Int operator - (Int x)
    {
        x.negate();
        return x;
    }


    Int& operator ++ () &
    {
        if (this->sign() == NEGATIVE) {
          --this->m_abs;
          if (!this->m_abs) 
            this->sign = ZERO;
        }else {
          ++this->m_abs;
          this->sign = POSITIVE;
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
        if (this->sign() == POSITIVE) {
          --this->m_abs;
          if (!this->m_abs) 
            this->sign = ZERO;
        }else {
          ++this->m_abs;
          this->sign = NEGATIVE;
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
        if(lhs.sign() * rhs.sign() != NEGATIVE)
          return Int{(lhs.sign() ? lhs.sign() : rhs.sign()), 
                      lhs.m_abs + rhs.m_abs};
        if (lhs.m_abs < rhs.m_abs)
          return Int{rhs.sign(), rhs.m_abs - lhs.m_abs}; 
        else
          return Int{lhs.sign(), lhs.m_abs - rhs.m_abs};
    }
    friend Int operator + (const Int &lhs, Int &&rhs)
    {
        if(lhs.sign() * rhs.sign() != NEGATIVE)
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
        if(lhs.sign() * rhs.sign() != NEGATIVE)
          return Int{(lhs.sign() ? lhs.sign() : rhs.sign()), 
                      std::move(lhs.m_abs) + std::move(rhs.m_abs)};
        if (lhs.m_abs < rhs.m_abs)
          return Int{rhs.sign(), std::move(rhs.m_abs) - std::move(lhs.m_abs)}; 
        else
          return Int{lhs.sign(), std::move(lhs.m_abs) - std::move(rhs.m_abs)};
    }

    friend Int operator - (const Int &lhs, const Int &rhs)
    {
        if(lhs.sign() * -rhs.sign() != NEGATIVE)
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
    friend IntDivResult divMod(const Int&, const Int&);


    Int& parse(string_view strv) &
    {
        const char *str = strv.data();
        auto len = strv.size();
        Sign new_sign = POSITIVE;
        const char* en = str + len;
        if (*str == '-') {
          new_sign = -new_sign;
          ++str;
        }
        this->m_abs.parse(string_view(str, en - str));
        if (!this->m_abs) this->sign = ZERO;
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
        if (x.sign() == NEGATIVE) os << '-';
        os << x.m_abs;
        return os;
    }


  private:
    Int(Sign si, uInt b) noexcept : sign(si * !!b), m_abs(Move(b)) {}


  public:
    ReadOnlyProperty<Sign, Int> sign;

  private:
    uInt m_abs;
};


struct IntDivResult
{
    Int quo, rem;
};

inline IntDivResult divMod(const Int &lhs, const Int &rhs)
{
    auto res = divMod(lhs.m_abs, rhs.m_abs);
    return {Int{lhs.sign() * rhs.sign(), std::move(res.quo)},
            Int{lhs.sign() * rhs.sign(), std::move(res.rem)}};
}


template<> inline Int convert<Int>(string_view str)
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
      uint32_t divMod = static_cast<uint32_t>(r.low);
      res.high = l.high / divMod;
      unsigned long long temp = (static_cast<uint64_t>(l.high % divMod) << 32) + (l.low >> 32);
      res.low = ((temp / divMod) << 32) + ((static_cast<uint64_t>(temp % divMod) << 32) + (l.low & uint128_t::k_MaskLow)) / divMod;
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
constexpr bool operator == (const uint128_t &l, Tp r)
{
    return !l.high && l.low == r;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator == (Tp l, const uint128_t &r)
{   return r == l; }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator != (const uint128_t &l, Tp r)
{   return !(l == r); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator != (Tp l, const uint128_t &r)
{   return !(r == l); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator < (const uint128_t &l, Tp r)
{
    return !l.high && l.low < r;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator < (Tp l, const uint128_t &r)
{
    return r.high || l < r.low;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator > (const uint128_t &l, Tp r)
{  return r < l; }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator > (Tp l, const uint128_t &r)
{  return r < l; }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator <= (const uint128_t &l, Tp r)
{  return !(r < l); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator <= (Tp l, const uint128_t &r)
{  return !(r < l); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator >= (const uint128_t &l, Tp r)
{  return !(l < r); }
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr bool operator >= (Tp l, const uint128_t &r)
{  return !(l < r); }



template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr Tp operator << (Tp x, const uint128_t &sh)
{ 
    return sh.high ? (x << (64 + !sh.high)) : (x << sh.low);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr Tp operator >> (Tp x, const uint128_t &sh)
{ 
    return sh.high ? (x >> (64 + !sh.high)) : (x >> sh.low);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr Tp& operator &= (Tp &x, const uint128_t &v)
{ 
    return x = x & v.low;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr Tp& operator <<= (Tp &x, const uint128_t &sh)
{ 
    return x = x << sh;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr Tp& operator >>= (Tp &x, const uint128_t &sh)
{ 
    return x = x >> sh;
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr Tp& operator /= (Tp &x, const uint128_t &v)
{ 
    return (v.high) ? (x = 0) : (x /= v.low);
}
template<typename Tp, typename = EnableIf_t<std::is_integral<Tp>::value>>
constexpr Tp& operator %= (Tp &x, const uint128_t &v)
{ 
    return (v.high) ? x : (x %= v.low);
}


inline std::istream& uint128_t::extractFrom(std::istream &is)
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

inline std::ostream& uint128_t::insertTo(std::ostream &os) const
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
#endif //BIGNUM_HPP_INCLUDED

