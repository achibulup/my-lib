#ifndef POLYNOMIAL_H_INCLUDED
#define POLYNOMIAL_H_INCLUDED


#include <vector>
#include <type_traits>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <complex>
#include "math_common.h"



template<char ...cs>
struct StringConstant
{
    using value_type = const char [sizeof...(cs) + 1];
    static constexpr value_type value = {cs..., 0};
    constexpr value_type& operator () () const noexcept
    {
        return value;
    }
    constexpr operator value_type& () const noexcept
    {
        return value;
    }
    template<int index>
    static constexpr char get () noexcept
    {
        static_assert(0 <= index && index <= size(), "access out of bound");
        return value[index];
    }
    static constexpr int size() noexcept
    {
        return sizeof...(cs);
    }
    static constexpr int length() noexcept
    {
        return sizeof...(cs);
    }
    constexpr char operator [] (int index) const noexcept
    {
        return value[index];
    }
    static constexpr char at(int index)
    {
        if (index < 0 || index > size()) 
          throw std::exception();
        return value[index];
    }
    static constexpr bool empty() noexcept
    {
        return size() == 0;
    }
    static constexpr const char* cbegin() noexcept
    {
        return value;
    }
    static constexpr const char* begin() noexcept
    {
        return cbegin();
    }
    static constexpr const char* cend() noexcept
    {
        return cbegin() + size();
    }
    static constexpr const char* end() noexcept
    {
        return cend();
    }
};
template<char ...cs>
constexpr char StringConstant<cs...>::value[sizeof...(cs) + 1];

template<typename Tp, typename var_name = StringConstant<'x'>>
class PolynomialLiteral
{
  public:
    using Coeficient = Tp;

    explicit PolynomialLiteral(Tp val) : coeficient{0, val}
    {
        trim_zero();
    }

    PolynomialLiteral(const PolynomialLiteral&) = default;
    PolynomialLiteral& operator = (const PolynomialLiteral&) & = default;

    int degree() const
    {
        return coeficient.size() - 1;
    }

    explicit operator bool () const
    {
        return degree() > 0 || coeficient.at(0);
    }

    void set(int deg, Tp value) &
    {
        if (deg < 0) coeficient.at(deg);
        if(deg > degree()) {
          if (!value) return;
          coeficient.resize(deg + 1);
        }
        coeficient.at(deg) = value;
        trim_zero();
    }

    Tp operator [] (int deg) const
    {
        return (deg <= degree() ? coeficient.at(deg) : 0);
    }

    friend PolynomialLiteral operator + (PolynomialLiteral val)
    {
        return val;
    }
    friend PolynomialLiteral operator - (PolynomialLiteral val)
    {
        for(auto &elem : val.coeficient)
          elem = -elem;
        return val;
    }

    friend PolynomialLiteral 
    operator + (PolynomialLiteral lhs, PolynomialLiteral rhs)
    {
        int max_deg = std::max(lhs.degree(), rhs.degree());
        PolynomialLiteral res;
        res.coeficient.resize(max_deg + 1);
        for(int i = 0; i <= max_deg; ++i)
          res.coeficient.at(i) = lhs[i] + rhs[i];
        res.trim_zero();
        return res;
    }
    friend PolynomialLiteral 
    operator - (PolynomialLiteral lhs, PolynomialLiteral rhs)
    {
        return lhs + -std::move(rhs);
    }
    friend PolynomialLiteral 
    operator * (PolynomialLiteral lhs, PolynomialLiteral rhs)
    {
        PolynomialLiteral res;
        if (!lhs || !rhs)  return res;
        int max_deg = lhs.degree() + rhs.degree();
        res.coeficient.resize(max_deg + 1);
        for(int i = 0; i <= lhs.degree(); ++i)
        for(int j = 0; j <= rhs.degree(); ++j)
          res.coeficient.at(i + j) += lhs[i] * rhs[j];
        return res;
    }

    template<typename Tp2, typename = typename std::enable_if<
                                        std::is_arithmetic<Tp2>::value>::type>
    friend PolynomialLiteral operator + (PolynomialLiteral lhs, Tp2 rhs)
    {
        return std::move(lhs) + (PolynomialLiteral(rhs)^0);
    }
    template<typename Tp2, typename = typename std::enable_if<
                                        std::is_arithmetic<Tp2>::value>::type>
    friend PolynomialLiteral operator + (Tp2 lhs, PolynomialLiteral rhs)
    {
        return std::move(rhs) + lhs;
    }
    template<typename Tp2, typename = typename std::enable_if<
                                        std::is_arithmetic<Tp2>::value>::type>
    friend PolynomialLiteral operator - (PolynomialLiteral lhs, Tp2 rhs)
    {
        return std::move(lhs) + -rhs;
    }
    template<typename Tp2, typename = typename std::enable_if<
                                        std::is_arithmetic<Tp2>::value>::type>
    friend PolynomialLiteral operator - (Tp2 lhs, PolynomialLiteral rhs)
    {
        return lhs + -std::move(rhs);
    }
    template<typename Tp2, typename = typename std::enable_if<
                                        std::is_arithmetic<Tp2>::value>::type>
    friend PolynomialLiteral operator * (Tp2 lhs, PolynomialLiteral rhs)
    {
        return (PolynomialLiteral(lhs)^0) * std::move(rhs);
    }
    template<typename Tp2, typename = typename std::enable_if<
                                        std::is_arithmetic<Tp2>::value>::type>
    friend PolynomialLiteral operator * (PolynomialLiteral lhs, Tp2 rhs)
    {
        return rhs * std::move(lhs);
    }
    template<typename Tp2, typename = typename std::enable_if<
                                        std::is_arithmetic<Tp2>::value>::type>
    friend PolynomialLiteral operator / (PolynomialLiteral lhs, Tp2 rhs)
    {
        PolynomialLiteral res;
        for(int i = lhs.degree(); i >= 0; --i)
          res.set(i, lhs[i] / rhs);
        res.trim_zero();
        return res;
    }

    PolynomialLiteral operator ^ (int deg) const
    {
        PolynomialLiteral res;
        if (*this) 
          res.set(deg, (*this)[1]);
        return res;
    }

    template<typename Input>
    decltype(Tp{} * Input{}) operator () (Input input) const
    {
        decltype(Tp{} * Input{}) res = (*this)[0];
        if (degree() >= 1) res += (*this)[1] * input;
        decltype(Tp{} * Input{}) exp = input;
        for(int i = 2; i <= degree(); ++i) {
          exp *= input;
          res += (*this)[i] * exp;
        }
        return res;
    }

  private:
    PolynomialLiteral() : coeficient(1) {}

    void trim_zero() 
    {
        while(coeficient.size() > 1 && !coeficient.back())
          coeficient.pop_back();
    }

    std::vector<Tp> coeficient;
};

template<typename var_name>
PolynomialLiteral<long double, var_name>
toFloat(PolynomialLiteral<long long, var_name> val)
{
    PolynomialLiteral<long double> res;
    for(int i = val.degree(); i >= 0; --i)
      res.set(i, val[i]);
    return res;
}
template<typename var_name>
PolynomialLiteral<long double, var_name>
operator + (PolynomialLiteral<long long, var_name> lhs, 
            PolynomialLiteral<long double, var_name> rhs)
{
    return toFloat(std::move(lhs)) + std::move(rhs);
}
template<typename var_name>
PolynomialLiteral<long double, var_name>
operator + (PolynomialLiteral<long double, var_name> &&lhs, 
            PolynomialLiteral<long long, var_name> &&rhs)
{
    return std::move(rhs) + std::move(lhs);
}
template<typename var_name>
PolynomialLiteral<long double, var_name>
operator - (PolynomialLiteral<long long, var_name> &&lhs, 
            PolynomialLiteral<long double, var_name> &&rhs)
{
    return std::move(lhs) + -std::move(rhs);
}
template<typename var_name>
PolynomialLiteral<long double, var_name>
operator - (PolynomialLiteral<long double, var_name> &&lhs, 
            PolynomialLiteral<long long, var_name> &&rhs)
{
    return std::move(lhs) + -std::move(rhs);
}

template<typename var_name>
PolynomialLiteral<long double, var_name>
operator * (PolynomialLiteral<long long, var_name> lhs, 
            PolynomialLiteral<long double, var_name> rhs)
{
    return toFloat(std::move(lhs)) * std::move(rhs);
}
template<typename var_name>
PolynomialLiteral<long double, var_name>
operator * (PolynomialLiteral<long double, var_name> &&lhs, 
            PolynomialLiteral<long long, var_name> &&rhs)
{
    return std::move(rhs) * std::move(lhs);
}

template<typename var_name, typename Tp, 
         typename = typename std::enable_if<
                              std::is_floating_point<Tp>::value>::type>
PolynomialLiteral<long double, var_name> 
operator * (Tp lhs, PolynomialLiteral<long long, var_name> rhs)
{
    return (PolynomialLiteral<long double, var_name>(lhs)^0) * std::move(rhs);
}
template<typename var_name, typename Tp, 
         typename = typename std::enable_if<
                              std::is_floating_point<Tp>::value>::type>
PolynomialLiteral<long double, var_name>
operator * (PolynomialLiteral<long long, var_name> lhs, Tp rhs)
{
    return rhs * std::move(lhs);
}
template<typename var_name, typename Tp, 
         typename = typename std::enable_if<
                              std::is_floating_point<Tp>::value>::type>
PolynomialLiteral<long double, var_name>
operator / (PolynomialLiteral<long long, var_name> lhs, Tp rhs)
{
    PolynomialLiteral<long double, var_name> res;
    for(int i = lhs.degree(); i >= 0; --i)
      res.set(i, lhs[i] / rhs);
    return res;
}

PolynomialLiteral<long long, StringConstant<'x'>> 
operator "" _x(unsigned long long coef)
{
    return PolynomialLiteral<long long, StringConstant<'x'>>(coef);
}
PolynomialLiteral<long double, StringConstant<'x'>> 
operator "" _x(long double coef)
{
    return PolynomialLiteral<long double, StringConstant<'x'>>(coef);
}
PolynomialLiteral<long long, StringConstant<'y'>> 
operator "" _y(unsigned long long coef)
{
    return PolynomialLiteral<long long, StringConstant<'y'>>(coef);
}
PolynomialLiteral<long double, StringConstant<'y'>> 
operator "" _y(long double coef)
{
    return PolynomialLiteral<long double, StringConstant<'y'>>(coef);
}
PolynomialLiteral<long long, StringConstant<'z'>> 
operator "" _z(unsigned long long coef)
{
    return PolynomialLiteral<long long, StringConstant<'z'>>(coef);
}
PolynomialLiteral<long double, StringConstant<'z'>> 
operator "" _z(long double coef)
{
    return PolynomialLiteral<long double, StringConstant<'z'>>(coef);
}












template<typename Tp, typename var_name = StringConstant<'x'>>
class Polynomial
{
  public:
    using Coeficient = Tp;

    Polynomial() noexcept = default;
    Polynomial(Tp val) : Polynomial{val} {}
    Polynomial(std::initializer_list<Tp> coefs) : coeficient{coefs}
    {
        trim_zero();
    }
    template<typename Tp2, typename = typename std::enable_if<
                            std::is_arithmetic<Tp2>::value>::type>
    Polynomial(Tp2 val) : Polynomial(static_cast<Tp>(val)) {}
    Polynomial(Literal0) noexcept : Polynomial() {}
    Polynomial(Literal1) : Polynomial(Tp(lit1)) {}

    Polynomial& operator = (Literal0) 
    {
        coeficient.clear();
        return *this;
    }

    template<typename Lit>
    Polynomial(const PolynomialLiteral<Lit, var_name> &lit) : Polynomial()
    {
        coeficient.resize(lit.degree() + 1);
        for(int i = lit.degree(); i >= 0; --i)
          coeficient.at(i) = lit[i];
        trim_zero();
    }


    int degree() const
    {
        return coeficient.size() + coeficient.empty() - 1;
    }

    explicit operator bool () const
    {
        return !coeficient.empty();
    }

    void set(int deg, Tp value) &
    {
        if (deg < 0) coeficient.at(deg);
        if(!*this || deg > degree()) {
          if (!value) return;
          coeficient.resize(deg + 1);
        }
        coeficient.at(deg) = value;
        trim_zero();
    }

    Tp operator [] (int deg) const
    {
        return (deg <= degree() && (*this || deg) ? coeficient.at(deg) : 0);
    }

    friend Polynomial operator + (Polynomial val)
    {
        return val;
    }
    friend Polynomial operator - (Polynomial val)
    {
        for(auto &elem : val.coeficient)
          elem = -elem;
        return val;
    }

    friend bool operator == (const Polynomial &lhs, const Polynomial &rhs)
    {
        if (lhs.degree() != rhs.degree()) return false;
        for(int i = lhs.degree(); i >= 0; --i)
          if (lhs[i] != rhs[i]) return false;
        return true;
    }
    friend bool operator != (const Polynomial &lhs, const Polynomial &rhs)
    {
        return !(lhs == rhs);
    }
    friend bool operator < (const Polynomial &lhs, const Polynomial &rhs)
    {
        int max_deg = std::max(lhs.degree(), rhs.degree());
        for(int i = max_deg; i >= 0; --i)
          if (lhs[i] != rhs[i]) return lhs[i] < rhs[i];
        return false;
    }
    friend bool operator > (const Polynomial &lhs, const Polynomial &rhs)
    {
        return rhs < lhs;
    }
    friend bool operator <= (const Polynomial &lhs, const Polynomial &rhs)
    {
        return !(rhs < lhs);
    }
    friend bool operator >= (const Polynomial &lhs, const Polynomial &rhs)
    {
        return !(lhs < rhs);
    }

    Polynomial& operator += (const Polynomial &rhs) &
    {
        int max_deg = std::max(degree(), rhs.degree());
        set(max_deg + 1, lit1);
        for(int i = max_deg; i >= 0; --i)
          set(i, (*this)[i] + rhs[i]);
        set(max_deg + 1, 0);
        return *this;
    }
    Polynomial& operator -= (const Polynomial &rhs) &
    {
        int max_deg = std::max(degree(), rhs.degree());
        set(max_deg + 1, lit1);
        for(int i = max_deg; i >= 0; --i)
          set(i, (*this)[i] - rhs[i]);
        set(max_deg + 1, 0);
        return *this;
    }
    Polynomial& operator *= (const Polynomial &rhs) &
    {
        if (!*this || !rhs) return *this = {};
        Polynomial lhs = *this;
        *this = {};
        for(int i = lhs.degree(); i >= 0; --i)
        for(int j = rhs.degree(); j >= 0; --j)
          set(i + j, (*this)[i + j] + lhs[i] * rhs[j]);
        return *this;
    }
    Polynomial& operator /= (const Polynomial &rhs) &
    {
        if (!rhs) throw std::invalid_argument("divide by zero!");
        if (degree() < rhs.degree()) return *this = {};
        if (rhs.degree() == 0) {
          for(auto &coef : coeficient)
            coef /= rhs[0];
          return *this;
        }
        Polynomial lhs = std::move(*this), remainder;
        divide(lhs, rhs, *this, remainder);
        return *this;
    }
    Polynomial& operator %= (const Polynomial &rhs) &
    {
        if (!rhs) throw std::invalid_argument("divide by zero!");
        if (degree() < rhs.degree()) return *this;
        if (rhs.degree() == 0) return *this = {};
        Polynomial lhs = std::move(*this), quotient;
        divide(lhs, rhs, quotient, *this);
        return *this;
    }

    friend Polynomial operator + (Polynomial lhs, const Polynomial &rhs)
    {
        lhs += rhs;
        return lhs;
    }
    friend Polynomial operator - (Polynomial lhs, const Polynomial &rhs)
    {
        lhs -= rhs;
        return lhs;
    }
    friend Polynomial operator * (Polynomial lhs, const Polynomial &rhs)
    {
        lhs *= rhs;
        return lhs;
    }
    friend Polynomial operator / (Polynomial lhs, const Polynomial &rhs)
    {
        lhs /= rhs;
        return lhs;
    }
    friend Polynomial operator % (Polynomial lhs, const Polynomial &rhs)
    {
        lhs %= rhs;
        return lhs;
    }

    friend Polynomial operator + (const Polynomial &lhs, Tp rhs)
    {
        return lhs + static_cast<Polynomial>(rhs);
    }
    friend Polynomial operator - (const Polynomial &lhs, Tp rhs)
    {
        return lhs - static_cast<Polynomial>(rhs);
    }
    friend Polynomial operator * (const Polynomial &lhs, Tp rhs)
    {
        return lhs * static_cast<Polynomial>(rhs);
    }
    friend Polynomial operator / (const Polynomial &lhs, Tp rhs)
    {
        return lhs / static_cast<Polynomial>(rhs);
    }
    friend Polynomial operator % (const Polynomial &lhs, Tp rhs)
    {
        return lhs % static_cast<Polynomial>(rhs);
    }

    friend Polynomial operator + (Tp lhs, const Polynomial &rhs)
    {
        return static_cast<Polynomial>(lhs) + rhs;
    }
    friend Polynomial operator - (Tp lhs, const Polynomial &rhs)
    {
        return static_cast<Polynomial>(lhs) - rhs;
    }
    friend Polynomial operator * (Tp lhs, const Polynomial &rhs)
    {
        return static_cast<Polynomial>(lhs) * rhs;
    }
    friend Polynomial operator / (Tp lhs, const Polynomial &rhs)
    {
        return static_cast<Polynomial>(lhs) / rhs;
    }
    friend Polynomial operator % (Tp lhs, const Polynomial &rhs)
    {
        return static_cast<Polynomial>(lhs) % rhs;
    }

    Tp operator () (Tp input) const
    {
        Tp res = (*this)[0];
        if (degree() >= 1) res += (*this)[1] * input;
        Tp exp = input;
        for(int i = 2; i <= degree(); ++i) {
          exp *= input;
          res += (*this)[i] * exp;
        }
        return res;
    }


    template<typename istr>
    friend istr&& operator >> (istr &&is, Polynomial &poly)
    {
#define return_early return std::forward<istr>(is)
#define failure_return {poly = {}; is.setstate(is.failbit); return_early;}
        static constexpr int name_len = var_name::size();
        poly = {};
        if (!is) failure_return;
        ignoreVoid(is);
        if (is.peek() == is.eof()) failure_return;
        char entry = is.peek();
        if (entry == '(') {
          is.get();
          int peek = ignoreVoid(is).peek();
          if (peek == is.eof() || static_cast<char>(peek) == ')')
            failure_return;
        }
        //multi-term input has to be enclosed by parentheses
        //single-term input should have no space between the coef and the exp notation

        Tp current_coef;

        /*first term*/{
          ignoreVoid(is);
          bool neg = (is.peek() != is.eof() && static_cast<char>(is.peek()) == '-');
          if (neg) is.get();
          bool check_var = tryGetVar(is, var_name{}, name_len);
          if (check_var) current_coef = lit1;
          else {
            if(neg) is.unget();
            if (!(is >> current_coef)) failure_return;
          }
          if (check_var && neg) current_coef = -current_coef;

          int deg = 0;
          if (!check_var) {
            if (entry == '(') ignoreVoid(is);
            if (entry == '(' || !nextIsVoid(is)) {
              if (!check_var) check_var = tryGetVar(is, var_name{}, name_len);
              if (entry != '(' && !check_var) failure_return;
            }
          }
          if (check_var) {    
            deg = 1;

            if (entry == '(') ignoreVoid(is);
            if (entry == '(' || !nextIsVoid(is)) {
            int peek = is.peek();
            bool is_exp = peek != is.eof() && static_cast<char>(peek) == '^';
            if (entry != '(' && !is_exp) failure_return;
            if (is_exp) {
              is.get();

              if (entry == '(') ignoreVoid(is);
              else if (!nextIsVoid(is)) failure_return;
              if (!(is >> deg)) failure_return;
            }
            }
          }

          poly.set(deg, current_coef + poly[deg]);
        }/*first term*/

        if (entry == '(') while(true) {
          ignoreVoid(is);
          int peek = is.peek();
          if (peek == is.eof()) failure_return;
          char sign = peek;
          if (sign == ')') {
            is.get();
            break;
          }

          if (sign != '+' && sign != '-') failure_return;
          is.get();

          ignoreVoid(is);
          bool check_var = tryGetVar(is, var_name{}, name_len);
          if (check_var) current_coef = lit1;
          else if (!(is >> current_coef)) failure_return;
          if (sign == '-') current_coef = -current_coef;

          int deg = 0;
          ignoreVoid(is);
          if (check_var || tryGetVar(is, var_name{}, name_len)) {
            deg = 1;

            ignoreVoid(is);
            peek = is.peek();
            if (peek != is.eof() && static_cast<char>(peek) == '^') {
                is.get();
                
                ignoreVoid(is);
                if (!(is >> deg)) failure_return;
            }
          }
                
          poly.set(deg, current_coef + poly[deg]);
        }
        return std::forward<istr>(is);
#undef return_early
#undef failure_return
    }

    template<typename ostr>
    friend decltype((std::declval<ostr&>() << std::declval<std::string>()),
                    std::declval<ostr>())
    operator << (ostr &&os, Polynomial poly)
    {
        if (!poly) return (os << "0"), os;
        {
          Tp coef = poly[poly.degree()];
          if (coef < 0) {
            os << "-";
            coef = -coef;
          }
          os << termStr(var_name{}, coef, poly.degree());
        }
        for(int i = poly.degree() - 1; i >= 0; --i) 
          if (poly[i]) {
            os << " ";
            Tp coef = poly[i];
            if (poly[i] < 0) {
              os << "- ";
              coef = -coef;
            } 
            else os << "+ ";
            os << termStr(var_name{}, coef, i);
          }
        return std::forward<ostr>(os);
    }

  private:
    static void divide(const Polynomial &lhs, const Polynomial &rhs,
                       Polynomial &quotient, Polynomial &remainder)
    {
        if (!rhs) throw std::invalid_argument("divide by zero!");
        if (lhs.degree() < rhs.degree()) {
          quotient = 0;
          remainder = lhs;
          return;
        }
        if (rhs.degree() == 0) {
          quotient = lhs;
          for(auto &coef : quotient.coeficient)
            coef /= rhs[0];
          remainder = 0;
          return;
        }
        int quo_deg = lhs.degree() - rhs.degree();
        quotient = {};
        remainder = {};
        for(int i = lhs.degree(); i > quo_deg; --i)
          remainder.set(i - quo_deg - 1, lhs[i]);
        for(int i = quo_deg; i >= 0; --i) {
          for(int j = remainder.degree(); j >= 0; --j)
            remainder.set(j + 1, remainder[j]);
          remainder.set(0, lhs[i]);
          Tp current_quo_coef = remainder[rhs.degree()] / rhs[rhs.degree()];
          quotient.set(i, current_quo_coef);
          remainder -= current_quo_coef * rhs;
        }
    }

    template<typename Tp>
    static std::string termStr(const char *var, Tp val, int deg)
    {
        std::ostringstream res;
        if (val == 1 && deg > 0) {}
        else {
          if (deg > 0) res << parenthesized(val);
          else res << val;
        }
        if (deg > 0) res << var;
        if (deg > 1) res << "^" << deg;
        return res.str();
    }

    void trim_zero() 
    {
        while(!coeficient.empty() && !coeficient.back())
          coeficient.pop_back();
    }

    std::vector<Tp> coeficient;
};


template<typename Tp1, typename Tp2, typename var_name>
Polynomial<Tp1, var_name> 
operator + (Tp1 lhs, PolynomialLiteral<Tp2, var_name> rhs)
{
    return lhs + Polynomial<Tp1, var_name>(std::move(rhs));
}
template<typename Tp1, typename Tp2, typename var_name>
Polynomial<Tp2, var_name> 
operator + (PolynomialLiteral<Tp1, var_name> lhs, Tp2 rhs)
{
    return Polynomial<Tp2, var_name>(std::move(lhs)) + rhs;
}
template<typename Tp1, typename Tp2, typename var_name>
Polynomial<Tp1, var_name> 
operator - (Tp1 lhs, PolynomialLiteral<Tp2, var_name> rhs)
{
    return lhs - Polynomial<Tp1, var_name>(std::move(rhs));
}
template<typename Tp1, typename Tp2, typename var_name>
Polynomial<Tp2, var_name> 
operator - (PolynomialLiteral<Tp1, var_name> lhs, Tp2 rhs)
{
    return Polynomial<Tp2, var_name>(std::move(lhs)) - rhs;
}
template<typename Tp1, typename Tp2, typename var_name>
Polynomial<Tp1, var_name> 
operator * (Tp1 lhs, PolynomialLiteral<Tp2, var_name> rhs)
{
    return lhs * Polynomial<Tp1, var_name>(std::move(rhs));
}
template<typename Tp1, typename Tp2, typename var_name>
Polynomial<Tp2, var_name> 
operator * (PolynomialLiteral<Tp1, var_name> lhs, Tp2 rhs)
{
    return Polynomial<Tp2, var_name>(std::move(lhs)) * rhs;
}
template<typename Tp1, typename Tp2, typename var_name>
Polynomial<Tp2, var_name> 
operator / (PolynomialLiteral<Tp1, var_name> lhs, Tp2 rhs)
{
    return Polynomial<Tp2, var_name>(std::move(lhs)) / rhs;
}



template<typename Tp, 
         typename = typename std::enable_if<
           std::is_floating_point<Tp>::value>::type>
std::vector<Tp> solve(Polynomial<Tp> equation)
{
    using std::sqrt;
    using std::cbrt;
    using std::abs;

    if (!equation) 
      throw std::invalid_argument("infinite solution");
    std::vector<Tp> res;

    int trailing_zero = 0;
    while(!equation[trailing_zero]) ++trailing_zero;
    if (trailing_zero) {
      res.push_back(0);
      equation /= 1_x^(trailing_zero);
    }

    if(equation.degree() == 0) {}

    else if (equation.degree() == 1) res.push_back(-equation[0] / equation[1]);

    else if (equation.degree() == 2) {
      Tp a = equation[2], b = equation[1], c = equation[0];
      Tp delta = b * b - 4 * a * c;
      const Tp inv2a = 1 / (2 * a);
      if (delta == 0) res.push_back(-b * inv2a);
      else if (delta > 0) {
        Tp sdelta = sqrt(delta);
        res.push_back((-b - sdelta) * inv2a);
        res.push_back((-b + sdelta) * inv2a);
      }
    }

    else if (equation.degree() == 3) {
      Tp a = equation[3], b = equation[2], c = equation[1], d = equation[0];
      Tp delta0 = b * b - 3 * a * c;
      Tp delta1 = 2 * b * b * b - 9 * a * b * c + 27 * a * a * d;
      const Tp inv3a = 1 / (3 * a);
      if (!delta0 && !delta1) res.push_back(-b * inv3a);
      else {
        Tp sub = delta1 * delta1 - 4 * delta0 * delta0 * delta0;
        if (sub >= 0) {
          Tp C = cbrt((delta1 + sqrt(sub)) * 0.5);
          Tp root1 = -(b + C + delta0 / C) * inv3a;
          res.push_back(root1);
          if (approxEqual<Tp>(sub, 0))
            res.push_back(-(b - C) * inv3a);
        }
        else {
          std::complex<Tp> C(delta1 * .5, sqrt(-sub) * .5);
          const std::complex<Tp> unity{-.5, .5 * sqrt(static_cast<Tp>(3))};
          C = pow(C, 0.33333333333333l);
          res.push_back(-(b + C.real() * 2) * inv3a);
          C *= unity;
          res.push_back(-(b + C.real() * 2) * inv3a);
          C *= unity;
          res.push_back(-(b + C.real() * 2) * inv3a);
        }
      }
    }

    else throw std::invalid_argument("equation with degree 3 or more is not supported :<");

    std::sort(res.begin(), res.end());
    res.resize(std::unique(res.begin(), res.end()) - res.begin());
    return res;
}
template<typename Tp, 
         typename = typename std::enable_if<
           std::is_integral<Tp>::value>::type>
std::vector<double> solve(const Polynomial<Tp> &equation)
{
    Polynomial<double> conv;
    for(int i = equation.degree(); i >= 0; --i)
      conv.set(i, equation[i]);
    return solve(conv);
}
std::vector<long double> solve(const Polynomial<long long> &equation)
{
    Polynomial<long double> conv;
    for(int i = equation.degree(); i >= 0; --i)
      conv.set(i, equation[i]);
    return solve(conv);
}
template<typename Tp>
std::vector<long double> solve(PolynomialLiteral<Tp> equation)
{
    return solve(static_cast<Polynomial<long double>>(equation));
}






template<typename istr>
bool tryGetVar(istr &is, const char *var_name, int len)
{
    int cur = 0;
    while (cur < len) {
      int peek = is.peek();
      if (peek == is.eof() || static_cast<char>(peek) != var_name[cur]) {
        while (cur--) is.unget();
        return false;
      }
      is.get();
      ++cur;
    }
    return true;
}

template<typename ostr, typename Tp, typename var_name, typename = decltype(
             std::declval<ostr&>().operator << (std::declval<const char*>()))>
ostr&& operator << (ostr &&os, PolynomialLiteral<Tp, var_name> poly)
{
    return std::forward<ostr>(os) << static_cast<Polynomial<Tp, var_name>>(poly);
}
// template<typename ostr, typename Tp, const char *var_name, typename = decltype(
//              std::declval<ostr&>().operator << (std::declval<const char*>()))>
// ostr& operator << (ostr &&os, Polynomial<Tp, var_name> poly)
// {
//     if (!poly) return (os << "0"), os;
//     {
//       if (poly[poly.degree()] < 0) 
//         os << "-";
//       if (poly.degree() > 0)
//         os <<"(";
//       if (poly[poly.degree()] < 0)
//         os <<-poly[poly.degree()];
//       else os <<poly[poly.degree()];
//       if (poly.degree() > 0)
//         os <<")"<<var_name;
//       if (poly.degree() > 1)
//         os <<"^"<<poly.degree();
//     }
//     for(int i = poly.degree() - 1; i >= 0; --i) 
//       if (poly[i]) {
//         os << " ";
//         if (poly[i] < 0) 
//           os << "- ("<<-poly[i]<<")";
//         else os<< "+ ("<<poly[i]<<")";
//         if (i > 0) os <<var_name;
//         if (i > 1) os <<"^"<<i;
//       }
//     return os;
// }
#endif //POLYNOMIAL_H_INCLUDED