#ifndef SOLVE_POLYNOMIAL_RATIONAL_H_INCLUDED
#define SOLVE_POLYNOMIAL_RATIONAL_H_INCLUDED

#include <cmath>
#include "polynomial.h"
#include "rational.h"

template<typename Tp, 
         typename = typename std::enable_if<
           std::is_integral<Tp>::value>::type>
bool isSquare(Tp x)
{
    if (x < 0) return false;
    Tp sqrt = llroundl(sqrtl(x));
    return (sqrt * sqrt == x);
}
template<typename Tp>
bool isSquare(Rational<Tp> x)
{
    return isSquare(x.getNum()) && isSquare(x.getDenom());
}

template<typename Tp>
std::vector<Rational<Tp>> solve(Polynomial<Rational<Tp>> equation)
{
    if (equation.degree() == 0) 
      throw std::invalid_argument("zero degree equation not supported!");

    if (equation.degree() == 1) return {-equation[0] / equation[1]};

    else if (equation.degree() == 2) {
      Rational<Tp> a = equation[2], b = equation[1], c = equation[0];
      Rational<Tp> delta = b * b - 4 * a * c;
      if (isSquare(delta)) {
        if (!delta) return {-b / (2 * a)};
        Rational<Tp> sdelta = frac(Tp(llroundl(sqrtl(delta.getNum()))),
                                   Tp(llroundl(sqrtl(delta.getDenom()))));
        std::vector<Rational<Tp>> res{(-b + sdelta) / (2 * a),
                                      (-b - sdelta) / (2 * a)};
        using std::swap;
        if (res[0] > res[1]) swap(res[0], res[1]);
        return res;
      }
      return {};
    }

    /*else*/
    std::vector<Rational<Tp>> res;
    int trailing_zero = 0;
    while(!equation[trailing_zero]) ++trailing_zero;
    if (trailing_zero) {
      res.push_back(0);
      equation /= 1_x^(trailing_zero);
    }

    if (equation.degree() == 0) {}

    else if (equation.degree() <= 2) {
      std::vector<Rational<Tp>> res2 = solve(std::move(equation));
      res.insert(res.end(), res2.begin(), res2.end());
    }
    
    else {
      Tp denom = equation[0].getDenom();
      for(int i = 1; i <= equation.degree(); ++i)
        denom = lcm(denom, equation[i].getDenom());
      for(int i = 0; i <= equation.degree(); ++i)
        equation.set(i, equation[i] * denom);
      Tp lead = abs(equation[equation.degree()].getNum());
      Tp free = abs(equation[0].getNum());
      std::vector<Tp> nums, denoms;
      for(Tp num = lit1; num <= free; ++num) 
        if (free % num == 0) nums.push_back(num);
      for(Tp denom = lit1; denom <= lead; ++denom) 
        if (lead % denom == 0) denoms.push_back(denom);
      for(Tp num : nums)
      for(Tp denom : denoms)
        if (num % denom == 0) {
          Rational<Tp> root = frac(num, denom);
          if (equation(root) == 0) res.push_back(root);
          if (equation(-root) == 0) res.push_back(-root);
        }
    }
    std::sort(res.begin(), res.end());
    res.resize(std::unique(res.begin(), res.end()) - res.begin());
    return res;
}

#endif
