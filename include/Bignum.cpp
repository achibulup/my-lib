#include "Bignum.hpp"

namespace Achibulup
{

namespace n_Int
{

template<typename Tp, typename Tp1>
constexpr Tp1 ceilDiv(const Tp &lhs, const Tp1 &rhs)
{
    return (lhs + rhs - 1) / rhs;
}

wcalc_type SegView::msd(size_type start) const
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

size_type SegView::digitCount() const noexcept
{
    if(this->size() == 0) return 0;
    size_type res = k_BaseBinDigit * (this->size() - 1);
    for(calc_type tmp = (*this)[this->size() - 1]; tmp > 0; tmp >>= 1)
      ++res;
    return res;
}
 
struct decompose_result
{
    size_type unit;
    int digit;
};

static constexpr decompose_result decompose(size_t pos) noexcept
{
    return {size_type(pos / k_BaseBinDigit), int(pos % k_BaseBinDigit)};
}


static size_type maxSumSize(size_type left_size, size_type right_size)
{
    return std::max(left_size, right_size) + 1;
}
static size_type maxDifSize(size_type left_size, size_type right_size)
{
    return left_size;
}
static size_type maxProdSize(size_type left_size, size_type right_size)
{
    return left_size + right_size;
}
static size_type maxDivSize(size_type left_size, wcalc_type rhs)
{
    return left_size;
}
static size_type maxAndSize(size_type left_size, size_type right_size)
{
    return std::min(left_size, right_size);
}
static size_type maxOrSize(size_type left_size, size_type right_size)
{
    return std::max(left_size, right_size);
}
static size_type maxXorSize(size_type left_size, size_type right_size)
{
    return std::max(left_size, right_size);
}
static size_type maxShrSize(size_type left_size, size_t rhs)
{
    return std::max<base_type>(left_size - rhs / k_BaseBinDigit, 0);
}
static size_type maxShlSize(size_type left_size, size_t rhs)
{
    return left_size + (rhs + k_BaseBinDigit - 1) / k_BaseBinDigit;
}


void zeroFill(pointer ptr, size_type len)
{
    if (len != 0)
      memset(ptr, 0, len * sizeof(base_type));
}
size_type trimZero(const_pointer digits, size_type size)
{
    while (size > 0 && digits[size - 1] == 0) --size;
    return size;
}

///unsafe functions assume capacity is sufficient
///these functions store the result to the array pointed by res and return the number of elements stored
///res may point to lhs or rhs and that's okay
static size_type unsafeAdd(SegView lhs, SegView rhs, pointer res)
{
    if(lhs.size() < rhs.size()) return unsafeAdd(rhs, lhs, res);
    calc_type tmp = 0, carry = 0;
    for (size_type i = 0; i < lhs.size(); ++i) {
      tmp = lhs[i] + carry;
      if (i < rhs.size()) tmp += rhs[i];
      carry = tmp >> k_BaseBinDigit;
      tmp &= k_Base - 1;
      res[i] = tmp;
    }
    res[lhs.size()] = carry;
    return lhs.size() + carry;
}
static size_type unsafeSub(SegView lhs, SegView rhs, pointer res)
{
    if (lhs.size() < rhs.size())
      n_Int::throw_unsigned_integer_underflow_exception();
    calc_type tmp = 0, carry = 0;
    for (size_type i = 0; i < lhs.size(); ++i) {
      tmp = k_Base + lhs[i] - carry;
      if (i < rhs.size()) tmp -= rhs[i];
      carry = !(tmp >> k_BaseBinDigit);
      tmp &= k_Base - 1;
      res[i] = tmp;
    }
    if (carry)
      n_Int::throw_unsigned_integer_underflow_exception();
    return trimZero(res, lhs.size());
}
static size_type unsafeSmallMult(SegView lhs, calc_type rhs, pointer res)
{
    if (rhs == 0) return 0;
    wcalc_type tmp = 0, r = rhs;
    for (size_type i = 0; i < lhs.size(); ++i) {
      tmp += r * lhs[i];
      res[i] = tmp & (k_Base - 1);
      tmp >>= k_BaseBinDigit;
    }
    if (tmp != 0) {
      res[lhs.size()] = tmp;
      return lhs.size() + 1;
    }
    return lhs.size();
}
static size_type
unsafeSmallDivMod(SegView lhs, wcalc_type rhs, 
                  pointer quo, wcalc_type *rem = nullptr)
{
    wcalc_type cur_rem = 0;
    for (size_type i = lhs.size(); i-- > 0;) {
      cur_rem = (cur_rem << k_BaseBinDigit) + lhs[i];
      cur_rem = cur_rem - rhs * (quo[i] = cur_rem / rhs);
    }
    if (rem) *rem = cur_rem;
    return trimZero(quo, lhs.size());
}
static size_type unsafeAnd(SegView lhs, SegView rhs, pointer res)
{
    if(lhs.size() > rhs.size()) return unsafeAnd(rhs, lhs, res);
    for (size_type i = 0; i < lhs.size(); ++i)
      res[i] = lhs[i] & rhs[i]; 
    return trimZero(res, lhs.size());
}
static size_type unsafeOr(SegView lhs, SegView rhs, pointer res)
{
    if(lhs.size() < rhs.size()) return unsafeOr(rhs, lhs, res);
    for (size_type i = 0; i < lhs.size(); ++i)
      res[i] = lhs[i] | (i < rhs.size() ? rhs[i] : 0); 
    return lhs.size();
}
static size_type unsafeXor(SegView lhs, SegView rhs, pointer res)
{
    if(lhs.size() < rhs.size()) return unsafeXor(rhs, lhs, res);
    for (size_type i = 0; i < lhs.size(); ++i)
      res[i] = lhs[i] ^ (i < rhs.size() ? rhs[i] : 0); 
    return trimZero(res, lhs.size());
}
static size_type unsafeShr(SegView lhs, size_t rhs, pointer res)
{
    if (rhs < 0) throw std::invalid_argument("invalid argument");
    auto dec = decompose(rhs);
    if(lhs.size() < dec.unit) {}
    else if (dec.digit == 0) {
      if(dec.unit || (lhs.cdata() != res))
        std::copy_n(lhs.cdata() + dec.unit, lhs.size() - dec.unit, res);
    }
    else {
      res[0] = lhs[dec.unit] >> dec.digit;
      for (size_type i = dec.unit + 1; i < lhs.size(); ++i) {
        res[i - dec.unit - 1] 
          |= (lhs[i] << (k_BaseBinDigit - dec.digit)) & (k_Base - 1);
        res[i - dec.unit] = lhs[i] >> dec.digit;
      }
    }
    return trimZero(res, maxShrSize(lhs.size(), rhs));
}
static size_type unsafeShl(SegView lhs, size_t rhs, pointer res)
{
    if (rhs < 0) throw std::invalid_argument("invalid argument");
    auto dec = decompose(rhs);
    zeroFill(res, dec.unit);
    if (dec.digit == 0) {
      if(dec.unit || (lhs.cdata() != res))
        std::copy_n(lhs.cdata(), lhs.size(), res + dec.unit);
    }
    else {
      res[lhs.size() + dec.unit] = 0;
      for (size_type i = lhs.size(); i-- > 0;) {
        res[i + dec.unit + 1] |=
          lhs[i] >> (k_BaseBinDigit - dec.digit);
        res[i + dec.unit] = (lhs[i] << dec.digit) & (k_Base - 1);
      }
    }
    return trimZero(res, maxShlSize(lhs.size(), rhs));
}


static bool exceedThreshold(size_type size1, size_type size2)
{
    constexpr size_type threshold = 400;
    return !(threshold / size2 >= size2);
}

} // namespace n_Int

using namespace n_Int;


/// Karatsuba algorithm
uInt uInt::bigOrderedProduct(SegView lhs, SegView rhs)
{
    uInt res(maxProdSize(lhs.size(), rhs.size()), 0);
    size_type half = (lhs.size() + 1) / 2;

    if (rhs.size() <= half) {
      uInt res1 = doMul(SegView(lhs, 0, half), rhs);
      uInt res2 = doMul(SegView(lhs, half), rhs);
      copy(res, res1);
      res.size = res.capacity();
      zeroFill(res.data() + res1.size(), res.size() - res1.size());
      for (size_type i = 0; i < res2.size(); ++i)
        if ((res[i + half] += res2[i]) >= k_Base) { 
          res[i + half] -= k_Base;
          ++res[i + half + 1];
        }
      res.size = n_Int::trimZero(res.data(), res.size());
    }

    else {
      SegView lhalf1(lhs, 0, half), lhalf2(lhs, half);
      SegView rhalf1(rhs, 0, half), rhalf2(rhs, half);
      uInt suml = doAdd(lhalf1, lhalf2), sumr = doAdd(rhalf1, rhalf2);
      uInt res1 = doMul(lhalf1, rhalf1);
      uInt res2 = doMul(suml, sumr);
      uInt res3 = doMul(lhalf2, rhalf2);
      res2.size = unsafeSub(res2, res1, res2.data());
      res2.size = unsafeSub(res2, res3, res2.data());

      res.size = res.capacity();
      calc_type tmp = 0, carry = 0;
      for (size_type i = 0; i < res.size(); ++i) {
        tmp = carry;
        carry = 0;
        if(i < res1.size()) 
          tmp += res1[i];
        if(half <= i && i < half + res2.size())
          tmp += res2[i - half];
        if(half * 2 <= i && i < half * 2 + res3.size())
          tmp += res3[i - half * 2];
        while (tmp >= k_Base) {
          tmp -= k_Base;
          ++carry;
        }
        res[i] = tmp;
      }
      res.size = n_Int::trimZero(res.data(), res.size());
    }

    return res;
}

/// schoolbook algorithm
uInt uInt::smallOrderedProduct(SegView lhs, SegView rhs)
{
    uInt res(maxProdSize(lhs.size(), rhs.size()), lhs.size());
    zeroFill(res.data(), lhs.size());
    for (size_type i = 0; i < rhs.size(); ++i) {
      wcalc_type tmp = 0;
      wcalc_type cur_digit = rhs[i];
      for (size_type j = 0; j < lhs.size(); ++j) {
        tmp += lhs[j] * cur_digit + res[j + i];
        res[j + i] = tmp & (k_Base - 1);
        tmp >>= k_BaseBinDigit;
      }
      res.unsafePushBack(tmp);
    }
    res.size = n_Int::trimZero(res.data(), res.size());
    return res;
}

base_type uInt::smallDivMod(SegView lhs, const uInt &rhs, uInt *rem)
{
    size_type rhs_dc = rhs.digitCount();
    wcalc_type lhs_msd = lhs.msd(rhs_dc - k_BaseBinDigit);
    wcalc_type rhs_msd = rhs.msd(rhs_dc - k_BaseBinDigit);
    ///dividing the most significant digits is a good approximation
    calc_type quotient =
          std::min<calc_type>(k_Base - 1, (lhs_msd + 1) / rhs_msd);
    uInt remainder;
    if(rem) remainder.swap(*rem);
    remainder.reset(rhs.size() + 1);
    remainder.size = unsafeSmallMult(rhs, quotient, remainder.data());
    while (remainder > lhs) {
      --quotient;
      remainder.size = unsafeSub(remainder, rhs, remainder.data());
    }
    remainder.size = unsafeSub(lhs, remainder, remainder.data());
    if(rem) remainder.swap(*rem);
    return quotient;
}
uInt uInt::smallDiv(SegView lhs, wcalc_type rhs)
{
    uInt res(maxDivSize(lhs.size(), rhs), 0);
    res.size = unsafeSmallDivMod(lhs, rhs, res.data());
    return res;
}
uInt uInt::smallMod(SegView lhs, wcalc_type rhs)
{
    wcalc_type rem = 0;
    for (size_type i = lhs.size(); i-- > 0;)
      rem = ((rem << k_BaseBinDigit) + lhs[i]) % rhs;
    return rem;
}


uInt& uInt::operator = (std::uintmax_t val) &
{
    static constexpr int max_size = ceilDiv(
        sizeof(std::uintmax_t) * CHAR_BIT, k_BaseBinDigit);
    if (val != 0)
      this->reset(max_size);
    this->clear();
    while (val > 0) {
      this->unsafePushBack(val % k_Base);
      val /= k_Base;
    }
    return *this;
}

uInt& uInt::operator = (const uInt &cpy) &
{
    if (this != &cpy) {
      this->reset(cpy.size());
      copy(*this, cpy);
    }
    return *this;
}
uInt& uInt::operator = (uInt &&mov) & noexcept
{
    if (this != &mov) {
      this->clear();
      if (mov) mov.swap(*this);
    }
    return *this;
}

uInt& uInt::operator ++() &
{
    for (size_type i = 0; i < this->size(); ++i) {
      if ((*this)[i] == k_Base - 1)
        (*this)[i] = 0;
      else {
        ++(*this)[i];
        return *this;
      }
    }
    if (this->size() == this->capacity())
      this->incrementSafeReserve();
    this->unsafePushBack(1);
    return *this;
}

uInt& uInt::operator --() &
{
    if (!*this) throw_unsigned_integer_underflow_exception();
    for (size_type i = 0; i < this->size(); ++i) {
      if ((*this)[i] == 0)
        (*this)[i] = k_Base - 1;
      else {
        --(*this)[i];
        if (i == this->size() - 1 && (*this)[i] == 0)
          this->popBack();
        return *this;
      }
    }
    return *this;
}

uInt uInt::doAdd(SegView lhs, SegView rhs)
{
    uInt res(maxSumSize(lhs.size(), rhs.size()), 0);
    res.size = unsafeAdd(lhs, rhs, res.data());
    return res;
}
uInt uInt::doAdd(SegView lhs, uInt &&rhs)
{
    if (lhs.data() == rhs.data()
     || rhs.capacity() < maxSumSize(lhs.size(), rhs.size()))
      return doAdd(lhs, rhs);
    rhs.size = unsafeAdd(lhs, rhs, rhs.data());
    return std::move(rhs);
}
uInt uInt::doAdd(uInt &&lhs, uInt &&rhs)
{
    if (lhs.data() == rhs.data() 
     && lhs.capacity() >= maxSumSize(lhs.size(), lhs.size())) {
      lhs.size = unsafeAdd(lhs, lhs, lhs.data());
      return std::move(lhs);
    }
    if (lhs.capacity() < rhs.capacity()) 
      return doAdd(lhs, std::move(rhs));
    return doAdd(rhs, std::move(lhs));
}

uInt uInt::doSub(SegView lhs, SegView rhs)
{
    uInt res(maxDifSize(lhs.size(), rhs.size()), 0);
    res.size = unsafeSub(lhs, rhs, res.data());
    return res;
}
uInt uInt::doSub(SegView lhs, uInt &&rhs)
{
    if (lhs.data() == rhs.data()) return uInt();
    if (rhs.capacity() < maxDifSize(lhs.size(), rhs.size()))
      return doSub(lhs, rhs);
    rhs.size = unsafeSub(lhs, rhs, rhs.data());
    return std::move(rhs);
}
uInt uInt::doSub(uInt &&lhs, SegView rhs)
{
    if (lhs.data() == rhs.data()) return uInt();
    lhs.size = unsafeSub(lhs, rhs, lhs.data());
    return std::move(lhs);
}
uInt uInt::doSub(uInt &&lhs, uInt &&rhs)
{
    if (lhs.capacity() < rhs.capacity())
      return doSub(lhs, std::move(rhs));
    return doSub(std::move(lhs), rhs);
}

uInt uInt::doMul(SegView lhs, SegView rhs)
{
    if(lhs.size() < rhs.size()) return doMul(rhs, lhs);
    if (exceedThreshold(lhs.size(), rhs.size()))
      return bigOrderedProduct(lhs, rhs);
    return smallOrderedProduct(lhs, rhs);
}
uInt uInt::doDiv(SegView divident, const uInt &divisor)
{
    if (divisor.size() == 0) 
      throw_division_by_zero_exception();
    if (divident < divisor) 
      return uInt();
    if (divisor.isSmallDivisor())
      return uInt::smallDiv(divident, uInt::downCast(divisor));
    return doDivMod(divident, divisor).quo;
}
uInt uInt::doMod(SegView divident, const uInt &divisor)
{
    if (divisor.size() == 0) 
      throw_division_by_zero_exception();
    if (divident < divisor) 
      return uInt(divident);
    if (divisor.isSmallDivisor())
      return uInt::smallMod(divident, uInt::downCast(divisor));
    return doDivMod(divident, divisor).rem;
}

uIntDivResult uInt::doDivMod(SegView divident, const uInt &divisor)
{
    if (divisor.size() == 0) 
      throw_division_by_zero_exception();
    if (divident < divisor) 
      return uIntDivResult{uInt(), uInt(divident)};
    if (divisor.isSmallDivisor())
      return uIntDivResult{
          uInt::smallDiv(divident, uInt::downCast(divisor)),
          uInt::smallMod(divident, uInt::downCast(divisor))};

    /*else*/ /*school-book algorithm*/
    uInt::size_type quotient_max_size = divident.size() - divisor.size() + 1;
    uIntDivResult res = {uInt{quotient_max_size, quotient_max_size},
                      uInt{divisor.size() + 1, divisor.size() - 1}};
    uInt cur_divident(divisor.size() + 1, 0);
    /// copy <divisor.digitcount - 1> divident's most significant digits to cur remainder
    std::copy_n(divident.cdata() + quotient_max_size, 
                divisor.size() - 1, res.rem.data());

    for (uInt::size_type i = quotient_max_size; i-- > 0;) {
      uInt::copyAndShift(cur_divident, res.rem);
      cur_divident[0] = divident[i];
      res.quo[i] = 
          uInt::smallDivMod(cur_divident, divisor, &res.rem);
    }
    res.quo.size = trimZero(res.quo.data(), res.quo.size());
    return res;
}



bool uInt::getbit(size_t pos) const noexcept
{
    auto dec = decompose(pos);
    if (dec.unit >= this->size()) return false;
    return (*this)[dec.unit] & (static_cast<base_type>(1u) << dec.digit);
}

uInt uInt::doAnd(const uInt &lhs, const uInt &rhs)
{
    uInt res(maxAndSize(lhs.size(), rhs.size()), 0);
    res.size = unsafeAnd(lhs, rhs, res.data());
    return res;
}
uInt uInt::doAnd(const uInt &lhs, uInt &&rhs)
{
    if (lhs.data() == rhs.data()) return lhs;
    rhs.size = unsafeAnd(lhs, rhs, rhs.data());
    return std::move(rhs);
}
uInt uInt::doAnd(uInt &&lhs, uInt &&rhs)
{
    if (lhs.capacity() < rhs.capacity())
      return doAnd(lhs, std::move(rhs));
    return doAnd(rhs, std::move(lhs));
}
uInt uInt::doOr(const uInt &lhs, const uInt &rhs)
{
    uInt res(maxOrSize(lhs.size(), rhs.size()), 0);
    res.size = unsafeOr(lhs, rhs, res.data());
    return res;
}
uInt uInt::doOr(const uInt &lhs, uInt &&rhs)
{
    if (lhs.data() == rhs.data() 
     || rhs.capacity() < maxOrSize(lhs.size(), rhs.size()))
      return lhs;
    rhs.size = unsafeOr(lhs, rhs, rhs.data());
    return std::move(rhs);
}
uInt uInt::doOr(uInt &&lhs, uInt &&rhs)
{
    if(lhs.capacity() < rhs.capacity())
      return doOr(lhs, std::move(rhs));
    return doOr(rhs, std::move(lhs));
}
uInt uInt::doXor(const uInt &lhs, const uInt &rhs)
{
    uInt res(maxXorSize(lhs.size(), rhs.size()), 0);
    res.size = unsafeXor(lhs, rhs, res.data());
    return res;
}
uInt uInt::doXor(const uInt &lhs, uInt &&rhs)
{
    if (lhs.data() == rhs.data()) return uInt();
    if (rhs.capacity() < maxXorSize(lhs.size(), rhs.size()))
      return doXor(lhs, rhs);
    rhs.size = unsafeXor(lhs, rhs, rhs.data());
    return std::move(rhs);
}
uInt uInt::doXor(uInt &&lhs, uInt &&rhs)
{
    if(lhs.capacity() < rhs.capacity())
      return doXor(lhs, std::move(rhs));
    return doXor(rhs, std::move(lhs));
}
uInt uInt::doShr(const uInt &lhs, int rhs)
{
    uInt res(maxShrSize(lhs.size(), rhs), 0);
    res.size = unsafeShr(lhs, rhs, res.data());
    return res;
}
uInt uInt::doShr(uInt &&lhs, int rhs)
{
    lhs.size = unsafeShr(lhs, rhs, lhs.data());
    return std::move(lhs);
}
uInt uInt::doShl(const uInt &lhs, int rhs)
{
    uInt res(maxShlSize(lhs.size(), rhs), 0);
    res.size = unsafeShl(lhs, rhs, res.data());
    return res;
}
uInt uInt::doShl(uInt &&lhs, int rhs)
{
    if(lhs.capacity() < maxShlSize(lhs.size(), rhs))
      return doShl(lhs, rhs);
    lhs.size = unsafeShl(lhs, rhs, lhs.data());
    return std::move(lhs);
}


uInt& uInt::parse(string_view strv) &
{
    const char *str = strv.data();
    size_type len = strv.size();
    while (len > 0 && *str == '0')
    { ++str; --len; }
    if (len == 0) 
    { this->clear(); return *this; }

    size_type estimate_size = 
      std::ceil((std::log2(ten) / k_BaseBinDigit + .00001) * len);
    estimate_size += 2;
    this->reset(estimate_size);
    this->clear();

    size_type start = (len - 1) % k_ioDecDigit + 1;
    this->size = unsafeAdd(*this, strToBase(str, start), this->data());
    while (start < len) {
      this->size = unsafeSmallMult(*this, k_ioUnit, this->data());
      this->size = unsafeAdd(*this, 
          strToBase(str + start, k_ioDecDigit), this->data());
      start += k_ioDecDigit;
    }
    return *this;
}


std::string uInt::toString() &&
{
    if (!*this) return "0";
    std::string res;
    while (*this) {
      wcalc_type first;
      this->size = unsafeSmallDivMod(
          *this, k_ioUnit, this->data(), &first);
      calc_type unit = first;
      if (*this) 
        for (int i = 0; i < k_ioDecDigit; ++i) {
          res.push_back('0' + unit % ten);
          unit /= ten;
        }
      else
        while (unit) {
          res.push_back('0' + unit % ten);
          unit /= ten;
        }
    }
    std::reverse(res.begin(), res.end());
    return res;
}


std::uintmax_t uInt::downCast(const uInt &x) noexcept
{
    std::uintmax_t res = 0;
    for (size_type i = x.size(); i-- > 0;)
      res = res * k_Base + x[i];
    return res;
}

base_type uInt::strToBase(const char *str, size_type len)
{
    calc_type res = 0;
    for (size_type i = 0; i < len; ++i)
      res = res * ten + (str[i] - '0');
    return res;
}

uIntDivResult divMod(const uInt &lhs, const uInt &rhs)
{
    return uInt::doDivMod(lhs, rhs);
}


void uInt::incrementSafeReserve()
{
    try{this->reserve(this->capacity() + 1);}
    catch(...) {
      for (size_type i = 0; i < this->size(); ++i)
        (*this)[i] = k_Base - 1;
      throw;
    }
}

void uInt::copyAndShift(uInt &dest, SegView src)
{
    std::copy_n(src.cdata(), src.size(), dest.data() + 1);
    dest.size = src.size() + 1;
}
void uInt::copy(uInt &dest, SegView src)
{
    std::copy_n(src.cdata(), src.size(), dest.data());
    dest.size = src.size();
}  

}