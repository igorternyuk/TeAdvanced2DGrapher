#include <QString>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctype.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include "parser.h"

iat::Parser::Parser(const std::string &inputString, std::vector<std::pair<char,double>> vars):
       Parser(inputString, vars, "radian")
{}

iat::Parser::Parser(const std::string &inputString, std::vector<std::pair<char,double>> vars, std::string angleUnit)
{
    this->m_angleUnit = angleUnit;
    m_inputWithVariables = inputString;
    int leftParentethisNumber = 0;
    int rightParentethisNumber = 0;
    for(auto &s : inputString)
    {
        if (s == '(') leftParentethisNumber++;
        if (s == ')') rightParentethisNumber++;
    }
    if(leftParentethisNumber != rightParentethisNumber)
        throw std::runtime_error("Unbalanced parentethises") ;
    //Добавляем в вектор с парами = имя переменной - значение 3 константы
    //число ПИ, число Непера и константу золотого сечения
    m_vctVariables = vars;
    std::pair<char, double> Pi('P', M_PI);
    m_vctVariables.push_back(Pi);
    std::pair<char, double> E('E', M_E);
    m_vctVariables.push_back(E);
    std::pair<char, double> G('G', (1 + sqrt(5)) / 2);
    m_vctVariables.push_back(G);

    // Во внешнем циклке перебираем все элементы вектора переменных
 //   for(std::pair<char, double> p : vctVariables) // В этом амперсанде может быть подвох!
    for(auto p : m_vctVariables) // В этом амперсанде может быть подвох!
    {
        // Во внутреннем циклке сравниваем каждый символ входной строки с текущим элементом в векторе переменных

        for(unsigned int i = 0; i < m_inputWithVariables.length(); ++i)
           {
            if(m_inputWithVariables.at(i) == p.first)
            {
                //Скопировать часть строки с начала до х
                std::string buf1 = m_inputWithVariables.substr(0,i);
                //Скопировать часть строки с х + 1 до конца
                std::string buf2 = m_inputWithVariables.substr(i + 1, m_inputWithVariables.length() - i);
                //Конвертировать значение переменной в строку обернув в скобки
                std::ostringstream sstream;
                //sstream << "(" << variable.second << ")";
                sstream << p.second;
                std::string res = sstream.str();
                // добавляем к первой части значение икса
                buf1 += res;
                //Конкатинируем строки
                m_inputWithVariables = buf1 + buf2;
            }
          }
    }
    m_input = m_inputWithVariables.c_str();
}

const iat::Parser::MyTokens iat::Parser::m_tokens;

std::string iat::Parser::parseToken()
{
    //std::cout << "input= " << input << std::endl;
    while (isspace(*m_input)) ++m_input;
    // Проверка является токен числом
    if (isdigit(*m_input))
    {
        std::string number;
        while (isdigit(*m_input) || *m_input == '.') number.push_back(*m_input++);
        return number;
    }
    // Проверка является токен оператором или функцией
    for (const auto& t : m_tokens.tokens)
    {
        if (strncmp(m_input, t.c_str(), t.size()) == 0)
        {
            m_input += t.size();
            return t;
        }
    }
    // Если совпадений нет возвращаем пустую строку
    return "";
}

iat::Parser::Expression iat::Parser::parseUnaryExpression() {
    auto token = parseToken();
    if (token.empty()) throw std::runtime_error("Invalid input expression");
    if (token == "(") {
        auto result = parse();
        if (parseToken() != ")") throw std::runtime_error("Expected ')'");
        return result;
    }
    if (isdigit(token[0]))
        return Expression(token);
    return Expression(token, parseUnaryExpression());
}

int getPriority(const std::string& binaryOperation) {
    if (binaryOperation == "+" || binaryOperation == "-") return 1;
    if (binaryOperation == "*" || binaryOperation == "/" || binaryOperation == "mod" ||
        binaryOperation == "e") return 2;
    if (binaryOperation == "&" || binaryOperation == "|" || binaryOperation == "^" ) return 3;
    if (binaryOperation == "<=" || binaryOperation == ">=" || binaryOperation == "<" ||
        binaryOperation == ">" || binaryOperation == "==" || binaryOperation == "!=") return 4;
    if (binaryOperation == "**") return 5;
    return 0;
}

iat::Parser::Expression iat::Parser::parseBinaryExpression(int minPriority) {
    auto leftExpression = parseUnaryExpression();
    for (;;) {
        auto op = parseToken();
        auto priority = getPriority(op);
        if (priority <= minPriority) {
            m_input -= op.size();
            return leftExpression;
        }
        auto rightExpression = parseBinaryExpression(priority);
        leftExpression = Expression(op, leftExpression, rightExpression);
    }
}

iat::Parser::Expression iat::Parser::parse() {
    return parseBinaryExpression(0);
}

double iat::Parser::evaluateExpression(const Expression &e)
{
    switch (e.args.size()) {
    case 2: {
        auto a = evaluateExpression(e.args[0]);
        auto b = evaluateExpression(e.args[1]);
        if (e.token == "+") return a + b;
        if (e.token == "-") return a - b;
        if (e.token == "e") return a * pow(10, b);
        if (e.token == "*") return a * b;
        if (e.token == "/")
        {
            if(b != 0)
            return a / b;
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "**")
        {
            return pow(a, b);
        }
        if (e.token == "mod")
        {
            if(b != 0)
                 return (int)a % (int)b;
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "<=") return a <= b ? 1 : 0;
        if (e.token == ">=") return a >= b ? 1 : 0;
        if (e.token == "<") return a < b ? 1 : 0;
        if (e.token == ">") return a > b ? 1 : 0;
        if (e.token == "==") return a == b ? 1 : 0;
        if (e.token == "!=") return a != b ? 1 : 0;
        if (e.token == "&") return (a != 0 && b != 0) ? 1 : 0;
        if (e.token == "|") return (a != 0 || b != 0) ? 1 : 0;
        if (e.token == "^") return ((a != 0 && b == 0) || (a == 0 && b != 0)) ? 1 : 0;
        throw std::runtime_error("Unknown binary operator");
    }
    case 1: {
        auto a = evaluateExpression(e.args[0]);
        if (e.token == "e") return pow(10, a);
        if (e.token == "+") return +a;
        if (e.token == "-") return -a;
        if (e.token == "!") return (a != 0) ? 0 : 1;
        if (e.token == "factorial")
        {
            double wholePart = abs(floor(a));
            return factorial(wholePart);
        }
        if (e.token == "inv") return (a != 0) ? pow(a, -1) : 0.0;
        if (e.token == "sign") return (a >= 0) ? 1 : -1;
        if (e.token == "abs") return fabs(a);
                if (e.token == "cbrt") return pow(a,pow(3,-1));
        if (e.token == "sqr") return pow(a,2);
        if (e.token == "cube") return pow(a,3);
        if (e.token == "gradtorad") return M_PI * a / 180;
        if (e.token == "radtograd") return 180 * a / M_PI;
        if (e.token == "_exp") return exp(a);
        if (e.token == "ln")
        {
            if(a >= 0)
                 return log(a);
            else throw  std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "log8")
        {
            if(a >= 0)
                 return log10(a)/log10(8);
            else throw  std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "log10")
        {
            if(a >= 0)
                 return log10(a);
            else throw  std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "log16")
        {
            if(a >= 0)
                 return log10(a)/log10(16);
            else throw  std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "log2")
        {
            if(a >= 0)
                 return log2(a);
            else throw  std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "arcsh") return asinh(a);
        if (e.token == "arcch")
        {
            if(a >= 1)
             return acosh(a);
            else throw std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "arcth")
        {
            if(fabs(a) < 1)
                 return atanh(a);
            else throw std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "arccth")
        {
            if(a == 0)
                throw std::runtime_error("Division by zero!");
            if(fabs(a) > 1)
                return atanh(pow(a,-1));
            else throw std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "arcsech")
        {
            if(a != 0)
                 return asinh(pow(a,-1));
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arccsech")
        {
            if(a == 0)
               throw std::runtime_error("Division by zero!");
            if(a <= 1)
             return acosh(pow(a,-1));
            else throw std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "sqrt")
        {
            if(a >= 0)
                 return sqrt(a);
            else throw std::runtime_error("Negative radicand!");
        }
        if (e.token == "sin")
        {
            if(m_angleUnit == "radian") return sin(a);
            if(m_angleUnit == "gradus") return sin(M_PI * a / 180);
            if(m_angleUnit == "grad") return sin(M_PI * a / 200);
            return sin(a);
        }
        if (e.token == "cos")
        {
            if(m_angleUnit == "radian") return cos(a);
            if(m_angleUnit == "gradus") return cos(M_PI * a / 180);
            if(m_angleUnit == "grad") return cos(M_PI * a / 200);
            return cos(a);
        }
        if (e.token == "tg")
        {
            if(m_angleUnit == "radian") return tan(a);
            if(m_angleUnit == "gradus") return tan(M_PI * a / 180);
            if(m_angleUnit == "grad") return tan(M_PI * a / 200);
            return tan(a);
        }
        if (e.token == "ctg")
        {
            if(tan(a) != 0)
            {
                if(m_angleUnit == "radian") return pow(tan(a), -1);
                if(m_angleUnit == "gradus") return pow(tan(M_PI * a / 180),-1);
                if(m_angleUnit == "grad") return pow(tan(M_PI * a / 200), -1);
                return pow(tan(a), -1);
            }
            else throw std::runtime_error("Division by zero!");
        }

        if (e.token == "csecans")
        {
            if(cos(a) != 0)
            {
                if(m_angleUnit == "radian") return pow(cos(a), -1);
                if(m_angleUnit == "gradus") return pow(cos(M_PI * a / 180),-1);
                if(m_angleUnit == "grad") return pow(cos(M_PI * a / 200), -1);
                return pow(cos(a), -1);
            }
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "secans")
        {
            if(sin(a) != 0)
            {
                if(m_angleUnit == "radian") return pow(sin(a), -1);
                if(m_angleUnit == "gradus") return pow(sin(M_PI * a / 180),-1);
                if(m_angleUnit == "grad") return pow(sin(M_PI * a / 200), -1);
            }
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arcsin")
        {
            if(fabs(a) <= 1)
                   return asin(a);
            else throw std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "arccos")
        {
            if(fabs(a) <= 1)
                    return acos(a);
            else throw std::runtime_error("argNameument is out of the function domen");
        }
        if (e.token == "arctg") return atan(a);
        if (e.token == "arcctg")
        {
            if(a != 0)
                    return atan(1 / a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arcsecans")
        {
            if(a != 0)
                    return asin(1 / a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "arccsecans")
        {
            if(a != 0)
                     return acos(1 / a);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "sh") return sinh(a);
        if (e.token == "ch") return cosh(a);
        if (e.token == "th") return tanh(a);
        if (e.token == "cth")
        {
            if(tanh(a) != 0)
                      return pow(tanh(a),-1);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "sech")
        {
            if(sinh(a) != 0)
                       return pow(sinh(a), -1);
            else throw std::runtime_error("Division by zero!");
        }
        if (e.token == "csech") return 1 / cosh(a);
        {
            if(cosh(a) != 0)
                return pow(cosh(a), -1);
            else throw std::runtime_error("Division by zero!");
        }
        throw std::runtime_error("Unknown unary operator");
    }
    case 0:
    {
        QString resString = QString::fromStdString(e.token.c_str()); //For Qt
        //return std::atof(e.token.c_str());
        return resString.toDouble();
    }
    }
    throw std::runtime_error("Unknown expression type");
}

double iat::Parser::calculateExpression()
{
    auto result = evaluateExpression(parse());
    return result;
}

unsigned long iat::Parser::factorial(unsigned int n)
{
    return (n == 0 || n == 1)? 1 : n * factorial(n - 1);
}
