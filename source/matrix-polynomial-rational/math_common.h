#ifndef MATH_COMMON_H_INCLUDED
#define MATH_COMMON_H_INCLUDED
#include <sstream>
#include <numeric>
bool isAlpha(unsigned char c)
{
    return std::isalpha(c);
}
template<typename istr>
bool nextIsVoid(istr &is)
{
    int peek = is.peek();
    if (peek == is.eof()) return true;
    char c = peek;
    return c == ' ' || c == '\n' || c == '\r';
}
template<typename istr>
istr& ignoreVoid(istr &is)
{
    if (!is) return is;
    while(true) {
      int peek = is.peek();
      if (peek == is.eof()) break;
      char c = peek;
      if (c != ' ' && c != '\n' && c != '\r')
        break;
      is.get();
    }
    return is;
}

using Literal0 = decltype(nullptr);
using Literal1 = std::integral_constant<int, 1>;
constexpr Literal1 lit1{};


template<typename num>
num gcd(num a, num b)
{
    while(b) std::swap(a %= b, b);
    return a;
}
template<typename num>
num lcm(num a, num b)
{
    return a * b / gcd(a, b);
}
template<typename num>
num pow(num base, std::intmax_t exp)
{
    if (exp == 0) return 1;
    while(exp % 2 == 0) {
      exp /= 2;
      base *= base;
    }
    num res = base;
    while(exp /= 2){
      base *= base;
      if (exp % 2 == 1) 
        res *= base;
    }
    return res;
}
template<typename Tp>
Tp abs(Tp val)
{
    return val < 0 ? -val : val;
}


template<typename Tp>
std::string parenthesized(Tp val)
{
    std::ostringstream osstr;
    osstr << val;
    std::string res = osstr.str();
    if (res.find_first_of(" /") != res.npos) 
      res = '(' + res + ')';
    return res;
}



#endif // MATH_COMMON_H_INCLUDED