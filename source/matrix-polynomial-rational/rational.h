#include <numeric>
#include "math_common.h"

template<typename Tp = int>
class Rational
{
  public:
    constexpr Rational() noexcept(noexcept(Tp(lit1))) 
    : m_num(0), m_denom(lit1) {}
    constexpr Rational(Tp num) noexcept(noexcept(Tp(lit1))) 
    : m_num(num), m_denom(lit1) {}
    constexpr Rational(Tp num, Tp denom) noexcept(noexcept(Tp(lit1))) 
    : m_num(num), m_denom(denom) 
    {
        this->reduce();
    }

    constexpr Rational(Literal0) noexcept(noexcept(Tp(lit1))) 
    : Rational() {}
    constexpr Rational(Literal1) noexcept(noexcept(Tp(lit1))) 
    : m_num(lit1), m_denom(lit1) {}

    constexpr Tp getNum() const noexcept(noexcept(Tp(this->m_num)))
    {
        return this->m_num;
    }

    constexpr Tp getDenom() const noexcept(noexcept(Tp(this->m_denom)))
    {
        return this->m_denom;
    }

    explicit constexpr operator bool() const noexcept(noexcept(!!(this->m_num)))
    {
        return !!(this->m_num);
    }

    constexpr Rational operator + () const noexcept(noexcept(Tp(this->m_num)))
    {
        return *this;
    }
    constexpr Rational operator - () const noexcept(noexcept(Tp(this->m_num)))
    {
        Rational res = *this;
        res.m_num = -res.m_num;
        return res;
    }

    constexpr Rational& operator += (Rational other) & 
    {
        this->m_num = this->m_num * other.m_denom + other.m_num * this->m_denom;
        this->m_denom *= other.m_denom;
        this->reduce();
        return *this;
    }
    constexpr Rational& operator -= (Rational other) & 
    {
        return *this += -other;
    }
    constexpr Rational& operator *= (Rational other) & 
    {
        this->m_num *= other.m_num;
        this->m_denom *= other.m_denom;
        this->reduce();
        return *this;
    }
    constexpr Rational& operator /= (Rational other) &
    {
        if (!other) 
        throw std::exception();
        this->m_num *= other.m_denom;
        this->m_denom *= other.m_num;
        this->reduce();
        return *this;
    }


    friend constexpr bool operator == (Rational lhs, Rational rhs) noexcept
    {
        return lhs.getNum() * rhs.getDenom() == rhs.getNum() * lhs.getDenom();
    }
    friend constexpr bool operator != (Rational lhs, Rational rhs) noexcept
    {
        return !(lhs == rhs);
    }
    friend constexpr bool operator < (Rational lhs, Rational rhs) noexcept
    {
        return lhs.getNum() * rhs.getDenom() < rhs.getNum() * lhs.getDenom();
    }
    friend constexpr bool operator > (Rational lhs, Rational rhs) noexcept
    {
        return rhs < lhs;
    }
    friend constexpr bool operator <= (Rational lhs, Rational rhs) noexcept
    {
        return !(rhs < lhs);
    }
    friend constexpr bool operator >= (Rational lhs, Rational rhs) noexcept
    {
        return !(lhs < rhs);
    }

    
    friend constexpr Rational operator + (Rational lhs, Rational rhs)
    {
        lhs += rhs;
        return lhs;
    }
    friend constexpr Rational operator - (Rational lhs, Rational rhs)
    {
        lhs -= rhs;
        return lhs;
    }
    friend constexpr Rational operator * (Rational lhs, Rational rhs)
    {
        lhs *= rhs;
        return lhs;
    }
    friend constexpr Rational operator / (Rational lhs, Rational rhs)
    {
        lhs /= rhs;
        return lhs;
    }

  private:
    Tp m_num, m_denom;

    void reduce()
    {
        if (this->m_denom < 0) {
          this->m_num = -this->m_num;
          this->m_denom = -this->m_denom;
        }
        Tp g = abs(gcd(abs(this->m_num), this->m_denom));
        this->m_num /= g;
        this->m_denom /= g;
    }
};
template<typename Tp>
Rational<Tp> frac(Tp a, Tp b)
{
    return Rational<Tp>(a, b);
}
template<typename Tp>
constexpr bool operator == (Rational<Tp> lhs, Literal0) noexcept
{
    return !lhs.getNum();
}
template<typename Tp>
constexpr bool operator < (Rational<Tp> lhs, Literal0) noexcept
{
    return lhs.getNum() < 0;
}
template<typename Tp>
constexpr bool operator == (Literal0, Rational<Tp> rhs) noexcept
{
    return rhs == 0;
}
template<typename Tp>
constexpr bool operator < (Literal0, Rational<Tp> rhs) noexcept
{
    return 0 < rhs.getNum();
}
template<typename Tp>
constexpr bool operator != (Rational<Tp> lhs, Literal0) noexcept
{
    return !(lhs == 0);
}
template<typename Tp>
constexpr bool operator > (Rational<Tp> lhs, Literal0) noexcept
{
    return 0 < lhs;
}
template<typename Tp>
constexpr bool operator <= (Rational<Tp> lhs, Literal0) noexcept
{
    return !(0 < lhs);
}
template<typename Tp>
constexpr bool operator >= (Rational<Tp> lhs, Literal0) noexcept
{
    return !(lhs < 0);
}
template<typename Tp>
constexpr bool operator != (Literal0, Rational<Tp> rhs) noexcept
{
    return !(0 == rhs);
}
template<typename Tp>
constexpr bool operator > (Literal0, Rational<Tp> rhs) noexcept
{
    return rhs < 0;
}
template<typename Tp>
constexpr bool operator <= (Literal0, Rational<Tp> rhs) noexcept
{
    return !(rhs < 0);
}
template<typename Tp>
constexpr bool operator >= (Literal0, Rational<Tp> rhs) noexcept
{
    return !(0 < rhs);
}

template<typename istr, typename Tp,
         typename=decltype(std::declval<istr&>()>>std::declval<char*>())>
istr&& operator >> (istr &&is, Rational<Tp> &x)
{
    Tp num;
    is >> num;
    x = num;
    return std::forward<istr>(is);
}
template<typename ostr, typename Tp, 
         typename=decltype(std::declval<ostr&>()<<std::declval<const char*>())>
ostr&& operator << (ostr &&os, Rational<Tp> x)
{
    if(x.getDenom() == lit1) os << x.getNum();
    else os << parenthesized(x.getNum()) <<'/'<< parenthesized(x.getDenom());
    return std::forward<ostr>(os);
}
