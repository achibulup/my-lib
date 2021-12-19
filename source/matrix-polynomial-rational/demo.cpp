#include <iostream>
#include <map>
#include <functional>
#include <cctype>
#include "Matrix.h"
#include "polynomial.h"
#include "rational.h"
#include "solve_polynomial_rational.h"

using std::cin;
using std::cout;
using std::wcin;
using std::wcout;
using std::array;
int find_first_of(const std::string &str, const std::string &chars, int pos)
{
    return std::min(str.size(), str.find_first_of(chars, pos));
}
int find_first_not_of(const std::string &str, const std::string &chars, int pos)
{
    return std::min(str.size(), str.find_first_not_of(chars, pos));
}
std::vector<std::string> split(const std::string &str,
                               std::string delim = " \n\r")
{
    std::vector<std::string> tokens;
    int en = str.size();
    int it = find_first_not_of(str, delim, 0);
    while(it != en) {
      auto next = find_first_of(str, delim, it);
      tokens.push_back(str.substr(it, next - it));
      it = find_first_not_of(str,  delim, next);
    }
    return tokens;
}
std::vector<std::string> split(const std::string &str, char delim)
{
    return split(str, {delim});
}
template<typename Tp>
Matrix<Tp> plus(const Matrix<Tp> &lhs, const Matrix<Tp> &rhs)
{
    return lhs + rhs;
}
template<typename Tp>
Matrix<Tp> minus(const Matrix<Tp> &lhs, const Matrix<Tp> &rhs)
{
    return lhs - rhs;
}
template<typename Tp>
Matrix<Tp> mul(const Matrix<Tp> &lhs, const Matrix<Tp> &rhs)
{
    return lhs * rhs;
}
template<typename Tp>
void printVector(const std::vector<Tp> &vec)
{
    cout << '{';
    if (!vec.empty()) cout << vec[0];
    for(int i = 1; i < (int)vec.size(); ++i)
      cout << ", " << vec[i];
    cout << '}';
}
template<typename Tp>
class MatrixCalc
{
    using Elem = Rational<Polynomial<Tp>>;
  public:
    using Matrix_t = Matrix<Elem>;
    using Bind_t = BoundMatrices<Elem, 2>;

    using VariableMap = std::map<std::string, Matrix_t>;
    using BindMap = std::map<std::string, Bind_t>;

    using Functor = void(MatrixCalc::*const)();
    using MathOp = Matrix_t(*const)(const Matrix_t&, const Matrix_t&);

    using OpMap = std::map<std::string, Functor>;
    using MathMap = std::map<std::string, MathOp>;


    void run()
    {
        while(true){
          try {
            std::string entry;
            cin >> entry;
            if (entry == "exit") break;
            auto it = ops.find(entry);
            if (it != ops.end())
              (this->*(it->second))();
            else {
              auto it = vars.find(entry);
              if (it != vars.end()) {
                std::string expression;
                std::getline(cin, expression);
                expression = entry + ' ' + expression;
                evalExpr(expression);
              }
              else {
                std::getline(cin, entry);
                throwInvalid("invalid command");
              }
            }
          }
          catch(const std::exception &e){
            cout << e.what() << '\n';
          }
        }
    }
    void evalExpr(std::string expr)
    {
        std::vector<std::string> tokens = split(expr);

        if (tokens.size() != 3 && tokens.size() != 5)
          throwInvalid("invalid expression");
        if (vars.find(tokens[0]) == vars.end())
          throwInvalid("invalid matrix name");

        if (tokens[1] == "->") {
          if (tokens.size() != 3) 
            throwInvalid("invalid expression");
          checkNameForMtr(tokens[2]);
          vars[tokens[2]] = vars[tokens[0]];
          return;
        }
        if (tokens.size() != 5 || tokens[3] != "->") 
          throwInvalid("invalid expression");
        if (vars.find(tokens[2]) == vars.end()){
          throwInvalid("invalid matrix name");
          return;
        }
        if (maths.find(tokens[1]) == maths.end())
          throwInvalid("invalid expression");
        if (tokens[4] == "print") 
          print(maths.at(tokens[1])(vars[tokens[0]], vars[tokens[2]]));
        else {
          checkNameForMtr(tokens[4]);
          vars[tokens[4]] = maths.at(tokens[1])(
                              vars[tokens[0]], vars[tokens[2]]);
        }
    }
    void list()
    {
        static_cast<const MatrixCalc&>(*this).list();
    }
    void list() const
    {
        cout << vars.size() << " matrices";
        if (vars.empty()) cout << "\n";
        else {
          cout << " :\n\n";
          for(auto &elem : vars) {
            cout << elem.first << "=\n";
            print(elem.second);
          }
          cout << "\n";
        }
    }
    void queryRank()
    {
        static_cast<const MatrixCalc&>(*this).queryRank();
    }
    void queryRank() const
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        auto it = vars.find(name);
        if (it != vars.end())
          cout << rank(it->second) << '\n';
        else throwInvalid("invalid matrix name");
    }
    void queryDet()
    {
        static_cast<const MatrixCalc&>(*this).queryDet();
    }
    void queryDet() const
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        auto it = vars.find(name);
        if (it != vars.end())
          cout << det(it->second) << '\n';
        else throwInvalid("invalid matrix name");
    }
    void queryCharPoly()
    {
        static_cast<const MatrixCalc&>(*this).queryCharPoly();
    }
    void queryCharPoly() const
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        auto it = vars.find(name);
        if (it != vars.end()) cout << getCharPoly(it->second) << '\n';
        else throwInvalid("invalid matrix name");
    }
    void queryEigenValue()
    {
        static_cast<const MatrixCalc&>(*this).queryEigenValue();
    }
    void queryEigenValue() const
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        auto it = vars.find(name);
        if (it != vars.end()) {
          printVector(solve(getCharPoly(it->second)));
          cout << '\n';
        }
        else throwInvalid("invalid matrix name");
    }

    void queryMatrix()
    {
        std::string mtr_name;
        cin >> mtr_name;
        checkValidName(mtr_name);
        checkNameForMtr(mtr_name);

        auto &mtr = vars[mtr_name];
        int r, c;
        cin >> r >> c;
        mtr.resize(r, c);
        for(int i = 1; i <= r; ++i)
        for(int j = 1; j <= c; ++j)
          cin >> mtr[{i, j}];
    }
    void queryBind()
    {
        std::string mtr_name1, mtr_name2, bind_name;
        cin >> bind_name;
        checkValidName(bind_name);
        checkNameForBind(bind_name);
        
        auto bit = binds.find(bind_name);
        cin >> mtr_name1 >> mtr_name2;
        checkValidName(mtr_name1);
        checkValidName(mtr_name2);

        auto it1 = varLookup(mtr_name1), it2 = varLookup(mtr_name2);
        if (it1 == vars.end() || it2 == vars.end())
          throwInvalid("invalid matrix name");
        if (bit != binds.end()) binds.erase(bit);
        binds.insert({bind_name, bind(it1->second, it2->second)});
    }
    void queryInvert()
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        auto it = vars.find(name);
        if (it != vars.end()){
          try {
            it->second = inverse(it->second);
          }
          catch(const std::exception &e) {
            std::cout << e.what() << '\n';
          }
        }
        else throwInvalid("invalid matrix name");
    }
    void queryTranspose()
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        auto it = vars.find(name);
        if (it != vars.end())
          it->second = transpose(it->second);
        else throwInvalid("invalid matrix name");
    }
    void queryEchelon()
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        {
            auto it = vars.find(name);
            if (it != vars.end()) {
              toRowEchelon(it->second);
              return;
            }
        }
        {
            auto it = binds.find(name);
            if (it != binds.end()) {
              toRowEchelon(it->second);
              return;
            }
        }
        throwInvalid("invalid name");
    }
    void queryRechelon()
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        {
            auto it = vars.find(name);
            if (it != vars.end()) {
              toReducedRowEchelon(it->second);
              return;
            }
        }
        {
            auto it = binds.find(name);
            if (it != binds.end()) {
              toReducedRowEchelon(it->second);
              return;
            }
        }
        throwInvalid("invalid name");
    }
    void queryPrint()
    {
        static_cast<const MatrixCalc>(*this).queryPrint();
    }
    void queryPrint() const
    {
        std::string name;
        cin >> name;
        checkValidName(name);
        {
            auto it = vars.find(name);
            if (it != vars.end()) {
              print(it->second);
              return;
            }
        }
        {
            auto it = binds.find(name);
            if (it != binds.end()) {
              print(it->second);
              return;
            }
        }
        throwInvalid("invalid name");
    }


    VariableMap vars;
    BindMap binds;
    static const OpMap ops;
    static const MathMap maths;

  private:
    static Polynomial<Tp> getCharPoly(const Matrix_t &mtr)
    {
        if (mtr.rows() != mtr.cols()) 
          throw std::invalid_argument("matrix is not square");
        Matrix_t lamt = Matrix_t::identity(mtr.rows());
        lamt = lamt * Elem(Polynomial<Tp>(1_x));
        return det(lamt - mtr).getNum();
    }

    static bool isValidName(const std::string &name)
    {
        return (std::isalpha(name[0]) || name[0] == '_');
    }
    static void checkValidName(const std::string &name)
    {
        if (!isValidName(name))
          throwInvalid(name+"invalid name");
    }
    void checkNameForMtr(const std::string &name) const
    {
        if (ops.count(name))
          throwInvalid(name+" coinsides with an Op name");
        if (binds.count(name))
          throwInvalid(name+" coinsides with a bind name");
    }
    void checkNameForBind(const std::string &name) const
    {
        if (ops.count(name))
          throwInvalid(" coinsides with an Op name");
        if (vars.count(name))
          throwInvalid(" coinsides with a var name");
    }
    typename VariableMap::iterator
    varLookup(const std::string &name)
    {
        checkNameForMtr(name);
        return vars.find(name);
    }
    typename BindMap::iterator
    bindLookup(const std::string &name)
    {
        checkNameForBind(name);
        return binds.find(name);
    }
    typename VariableMap::const_iterator
    varLookup(const std::string &name) const
    {
        checkNameForMtr(name);
        return vars.find(name);
    }
    typename BindMap::const_iterator
    bindLookup(const std::string &name) const
    {
        checkNameForBind(name);
        return binds.find(name);
    }

    static void throwInvalid(std::string message)
    {
        throw std::invalid_argument(message);
    }
};

template<typename Tp>
const typename MatrixCalc<Tp>::OpMap MatrixCalc<Tp>::ops = {
  {"matrix", &MatrixCalc<Tp>::queryMatrix},
  {"rank", &MatrixCalc<Tp>::queryRank},
  {"det", &MatrixCalc<Tp>::queryDet},
  {"poly", &MatrixCalc<Tp>::queryCharPoly},
  {"evalue", &MatrixCalc<Tp>::queryEigenValue},
  {"bind", &MatrixCalc<Tp>::queryBind},
  {"invert", &MatrixCalc<Tp>::queryInvert},
  {"transpose", &MatrixCalc<Tp>::queryTranspose},
  {"echelon", &MatrixCalc<Tp>::queryEchelon},
  {"rechelon", &MatrixCalc<Tp>::queryRechelon},
  {"print", &MatrixCalc<Tp>::queryPrint},
  {"list", &MatrixCalc<Tp>::list}
};

template<typename Tp>
const typename MatrixCalc<Tp>::MathMap MatrixCalc<Tp>::maths = {
  {"+", static_cast<Matrix_t(*)(const Matrix_t&, const Matrix_t&)>(plus)},
  {"-", static_cast<Matrix_t(*)(const Matrix_t&, const Matrix_t&)>(minus)},
  {"*", static_cast<Matrix_t(*)(const Matrix_t&, const Matrix_t&)>(mul)}
};

#define autoStart(name, ...) int _##name##_ = [](){\
  __VA_ARGS__\
  return 0;\
}();

int main()
{
    Polynomial<Rational<int>> poly1 = (1_x + 1) * (1_x + 2) * (1_x + 3);
    Polynomial<Rational<int>> poly2;
    cin >> poly2;// input (x^3 + 14x^2 + 7x + 8);
    cout << frac(poly1, poly2); // (x + 3)/(x + 4)
    MatrixCalc<Rational<Polynomial<Rational<int>>>>().run();
    // input :
    // matrix m1 3 3
    // 1 2 3
    // 4 5 6
    // 7 8 10
    // m1 -> m2
    // poly m1
    // invert m1
    // print m1
    // m2 * m1 -> m3
    // m1 + m2 -> print
    // matrix m4 3 3
    // (x + 1 1 1
    // 1 (x + 1) 1
    // 1 1 (x + 1)
    // det m4
    // matrix X 3 1
    // 4
    // -2
    // 1
    // bind b m2 X
    // print b
    // rechelon b
    // print b
    // exit
    return 0;
}

