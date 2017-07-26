#ifndef POLYNOM_H
#define POLYNOM_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <complex>
#include <ctime>
#include <stdexcept>

namespace iat {
    enum class PolynomRootSearchMethod
    {
        BISECTION,
        CHORDS,
        TANGENTS,
        COMBINED
    };

template<class T>
    class Polynom
    {
    public:
        Polynom();
        Polynom(int degree, T val);                          // Парметры конструктора - степень полинома и заполнитель для коэффициентов
        Polynom(const std::string&fileName);                 // Параметр конструктора - имя файла из которого будут считываться коэффициенты
        Polynom(const std::vector<T> &_coefficients);        // Параметр конструктора - вектор коэффициентов полинома
        Polynom(std::vector<T> &&_coefficients);
        Polynom(const std::initializer_list<T> &il);
        Polynom(const Polynom &other) = default;             //Default copy constructor
        Polynom(Polynom &&other) = default;                  //Default move constructor
        virtual ~Polynom() = default;
        inline bool isZero(){
            return _degree == 0 && _coefficients[0] == static_cast<T>(0);
        }
        void readPolynom(const std::string &fileName);       // Считать полином из файла
        bool setValueAt(int i, const T&value);               // Установить значение коэффициента элемента степени i
        T getCoefAt(int i) const;                            // Считать значение коэффициента элемента степени i
        int degree() const;                               // Получить степень полинома
        T calcVal(const T&) const;
        Polynom power(int) const;
        std::string convertToString(int prec) const;
        void showInConsole(int prec) const;
        void writeToFile(std::string fileName, int prec) const;
        bool rootInAnInterval(T leftLimit, T rightLimit, double eps, T &root,
                              PolynomRootSearchMethod method = PolynomRootSearchMethod::BISECTION);
        Polynom firstDerivative();
        Polynom derivative(int order);
        T derivativeAtPoint(int order, T arg);
        Polynom intergal(T initCondition);
        T definiteIntegral(T leftLimit, T rightLimit);
        //Operators
        Polynom &operator=(const Polynom &other) = default;
        Polynom &operator=(Polynom &&other) = default;
        T& operator[](int index);
        bool operator==(const Polynom&) const;
        bool operator!=(const Polynom& Polynom) const;
        Polynom operator+(const Polynom&) const;
        Polynom operator+(const T&);
        Polynom operator-(const Polynom&) const;
        Polynom operator-(const T&);
        Polynom operator*(const Polynom&) const;
        Polynom operator*(const T&);
        //Division operator returns a pair: first pair element - integral part, second - residual
        std::pair<Polynom<T>, Polynom<T>> operator/(const Polynom& denominator) const;
        Polynom operator/(const T&);
        Polynom &operator+=(const Polynom&);
        Polynom &operator+=(const T&);
        Polynom &operator-=(const Polynom&);
        Polynom &operator-=(const T&);
        Polynom &operator*=(const Polynom&);
        Polynom &operator*=(const T&);
        Polynom &operator/=(const T&);
        void validateDegree();

    private:
        int _degree;
        std::vector<T> _coefficients;

    };

    template<class T>
    void Polynom<T>::validateDegree()
    {
        bool isLeadingCoefficientZero = true;
        int i = _degree;
        while(isLeadingCoefficientZero)
        {
            if(!this->getCoefAt(i))
                _degree--;
            else
                isLeadingCoefficientZero = false;
            --i;
        }
    }

    template<class T>
    Polynom<T>::Polynom():Polynom(0, static_cast<T>(0))
    {}

    template<class T>
    Polynom<T>::Polynom(int degree, T value)
    {
        if(degree < 0)
            degree = 0;
        else
            this->_degree = degree;
        for(int i = 0; i <= degree; ++i)
            _coefficients.push_back(value);
    }

    template<class T>
    Polynom<T>::Polynom(const std::string& fileName)
    {
       std::ifstream fi(fileName);
       fi >> _degree;
       if(_degree < 0) _degree = 0;
       for(int i = 0; i <= _degree; ++i)
       {
           T value;
           fi >> value;
           _coefficients.push_back(value);
       }
       fi.close();
    }

    template<class T>
    Polynom<T>::Polynom(const std::vector<T> &coefficients)
    {
        _degree = coefficients.size() - 1;
        this->_coefficients = coefficients;
    }

    template<class T>
    Polynom<T>::Polynom(const std::initializer_list<T> &il)
    {
        _degree = il.size() - 1;
        for(auto el: il)
            _coefficients.push_back(el);
    }

    template<class T>
    Polynom<T>::Polynom(std::vector<T> &&coefficients)
    {
        _degree = coefficients.size() - 1;
        this->_coefficients = std::move(coefficients);
    }

    template<class T>
    void Polynom<T>::readPolynom(const std::string &fileName)
    {
        std::ifstream fi(fileName);
        fi >> _degree;
        for(int i = 0; i <= _degree; ++i)
        {
            T value;
            fi >> value;
            _coefficients.push_back(value);
        }
    }

    template<class T>
    bool Polynom<T>::setValueAt(int i, const T &value)
    {
        if(i >= 0 && i < _coefficients.size())
        {
          _coefficients[i] = value;
          return true;
        }
        return false;
    }

    template<class T>
    T Polynom<T>::getCoefAt(int i) const
    {
        if(i >= 0 && i < int(_coefficients.size()))
            return _coefficients[i];
        else if(i >= int(_coefficients.size()))
            return static_cast<T>(0.0);
        else
            return std::numeric_limits<T>::quiet_NaN();
    }

    template<class T>
    int Polynom<T>::degree() const
    {
        return _degree;
    }

    template<class T>
    T Polynom<T>::calcVal(const T &value) const
    {
        T result = static_cast<T>(0);
        for(int i = 0; i <= _degree; ++i)
            result += getCoefAt(i) * pow(value, i);
        return result;
    }

    template<class T>
    Polynom<T> Polynom<T>::power(int exponent) const
    {
        Polynom<T> product(0, static_cast<T>(1));
        Polynom<T> polynom = *this;
        for(int i = 1; i <= exponent; ++i)
            product *= polynom;
        return product;
    }

    template<class T>
    void Polynom<T>::showInConsole(int prec) const
    {
        std::cout << convertToString(prec) << std::endl;
    }

    template<class T>
    std::string Polynom<T>::convertToString(int prec) const
    {
        std::stringstream stream;
        stream << std::setprecision(prec) << getCoefAt(_degree) << " * X**" << _degree;
        for(int i = _degree - 1; i > 0; --i)
        {
            if(getCoefAt(i) > 0)
                stream << " + " << getCoefAt(i) << " * X**" << i;
            else if(getCoefAt(i) < 0)
                stream << " - " << fabs(getCoefAt(i)) << " * X**" << i;
            else
                continue;
        }
        if(getCoefAt(0) > 0)
            stream << " + " << std::setprecision(prec) << getCoefAt(0);
        else if(getCoefAt(0) < 0)
            stream << " - " << std::setprecision(prec) << fabs(getCoefAt(0));
        return stream.str();
    }

    template<class T>
    void Polynom<T>::writeToFile(std::string fileName, int prec) const
    {
        std::ofstream of(fileName);
        of << convertToString(prec);
        of.close();
    }

    template<class T>
    bool Polynom<T>::rootInAnInterval(T a, T b, double eps,
                                      T &root, PolynomRootSearchMethod method)
    {
        static int ITERR_LIM = 5000;
        if(calcVal(a) * calcVal(b) > 0)
            return false;
        if(method != PolynomRootSearchMethod::BISECTION &&
           derivativeAtPoint(2, a) < 0)
            std::swap(a, b);
        T x_next = a;
        int counter = 0;
        while(true)
        {
            if(method == PolynomRootSearchMethod::BISECTION ||
               method == PolynomRootSearchMethod::COMBINED)
            {
                if(method == PolynomRootSearchMethod::BISECTION)
                {
                    T c = (a + b) / 2;
                    if(calcVal(c) * calcVal(a) > 0)
                        a = c;
                    if(calcVal(c) * calcVal(b) > 0)
                        b = c;
                }
                else
                {
                    a -= calcVal(a) / derivativeAtPoint(1, a);
                    b -= calcVal(b) / (calcVal(b) - calcVal(a)) * (b - a);
                }
                if(fabs(a - b) < eps)
                {
                    root = 0.5 * (a + b);
                    return true;
                }
            }
            else if(method == PolynomRootSearchMethod::CHORDS ||
                    method == PolynomRootSearchMethod::TANGENTS)
            {
                a = x_next;
                x_next = method == PolynomRootSearchMethod::CHORDS ?
                         a - calcVal(a) / (calcVal(b) - calcVal(a)) * (b - a):
                         a - calcVal(a) / derivativeAtPoint(1, a);
                if(fabs(a - x_next) < eps)
                {
                    root = x_next;
                    return true;
                }
            }
            std::cout << "Iterration number: " << counter << std::endl;
            if(++counter > ITERR_LIM) break;
        }
        return false;
    }

    template<class T>
    Polynom<T> Polynom<T>::firstDerivative()
    {
        if(_degree > 0)
        {
            Polynom<T> der(_degree - 1, static_cast<T>(0));
            for(int i = 1; i <= _degree; ++i)
                der[i - 1] = i * this->getCoefAt(i);
            return der;
        }
        else
            return Polynom<T>();
    }

    template<class T>
    Polynom<T> Polynom<T>::derivative(int order)
    {
        Polynom<T> res(*this);
        if(order > _degree) order = _degree;
        for(int i = 0; i < order; ++i)
            res = res.firstDerivative();
        return res;
    }

    template<class T>
    T Polynom<T>::derivativeAtPoint(int order, T arg)
    {
        Polynom<T> der((*this).derivative(order));
        return der.calcVal(arg);
    }

    template<class T>
    Polynom<T> Polynom<T>::intergal(T initCondition)
    {
        Polynom<T> res(_degree + 1, static_cast<T>(0));
        res[0] = initCondition;
        for(int i = 1; i <= res.degree(); ++i)
            res[i] = this->getCoefAt(i - 1) / i;
        return res;
    }

    template<class T>
    T Polynom<T>::definiteIntegral(T leftLimit, T rightLimit)
    {
        Polynom<T> integral((*this).intergal(0));
        return integral.calcVal(rightLimit) -
               integral.calcVal(leftLimit);
    }

    template<class T>
    bool Polynom<T>::operator==(const Polynom &pol) const
    {
        if(_degree != pol.degree()) return false;
        bool isEqual = true;
        for(int i = 0; i < _degree; ++i)
        {
            if(getCoefAt(i) != pol.getCoefAt(i))
            {
                isEqual = false;
                break;
            }
        }
        return isEqual;
    }

    template<class T>
    bool Polynom<T>::operator!=(const Polynom &pol) const
    {
        if(_degree != pol.degree()) return true;
        bool isNotEqual = false;
        for(int i = 0; i < _degree; ++i)
        {
            if(getCoefAt(i) != pol.getCoefAt(i))
            {
                isNotEqual = true;
                break;
            }
        }
        return isNotEqual;
    }

    //lvalue
    template<class T>
    T& Polynom<T>::operator[](int index)
    {
        if(index < 0 || index > _degree)
            throw std::invalid_argument("index out of range");
        return _coefficients.at(index);
    }

    template<class T>
    Polynom<T> Polynom<T>::operator+(const Polynom& pol) const
    {
        int resDegree = std::max(_degree, pol._degree);
        Polynom<T> summa(resDegree, static_cast<T>(0));
        for(int i = 0; i <= resDegree; ++i)
            summa._coefficients[i] = getCoefAt(i) + pol.getCoefAt(i);
        return summa;
    }

    template<class T>
    Polynom<T> Polynom<T>::operator+(const T& value)
    {
        Polynom<T> summa(_degree, static_cast<T>(0));
        for(int i = 0; i <= _degree; ++i)
            summa._coefficients[i] = getCoefAt(i);
        summa._coefficients[0] += value;
        return summa;
    }

    template<class T>
    Polynom<T> Polynom<T>::operator-(const Polynom& pol) const
    {
        int resDegree = std::max(_degree, pol._degree);
        Polynom<T> difference(resDegree, static_cast<T>(0));
        for(int i = 0; i <= resDegree; ++i)
            difference._coefficients[i] = getCoefAt(i) - pol.getCoefAt(i);
        return difference;
    }

    template<class T>
    Polynom<T> Polynom<T>::operator-(const T& value)
    {
        Polynom<T> difference(_degree, static_cast<T>(0));
        for(int i = 0; i <= _degree; ++i)
            difference._coefficients[i] = getCoefAt(i);
        difference._coefficients[0] -= value;
        return difference;
    }

    template<class T>
    Polynom<T> Polynom<T>::operator*(const Polynom& pol) const
    {
        int productDegree = _degree + pol._degree;
        Polynom<T> product(productDegree, static_cast<T>(0));
        for(int i = 0; i <= _degree; ++i)
            for(int j = 0; j <= pol._degree; ++j)
                product._coefficients[i + j] += this->getCoefAt(i) * pol.getCoefAt(j);
       return product;
    }

    template<class T>
    Polynom<T> Polynom<T>::operator*(const T& value)
    {
        Polynom<T> product(_degree, static_cast<T>(0));
        for(int i = 0; i <= _degree; ++i)
            product._coefficients[i] = getCoefAt(i) * value;
        return product;
    }

    template<class T>
    std::pair<Polynom<T>, Polynom<T>> Polynom<T>::operator/(const Polynom& denominator) const
    {
        Polynom<T> numerator(*this);
        Polynom<T> integralPart(numerator.degree() - denominator.degree(),
                                static_cast<T>(0));
        Polynom<T> residual(denominator.degree() - 1, static_cast<T>(0));
        if(numerator.degree() < denominator.degree())
            residual = numerator;
        else
        {
            Polynom<T> tmp(numerator);
            bool isEndOfOperation = false;
            int i = integralPart.degree();
            while(!isEndOfOperation)
            {
                integralPart[i] = (tmp[tmp.degree()] / denominator.getCoefAt(denominator.degree()));
                Polynom<T> facient(i, static_cast<T>(0));
                facient[facient.degree()] = integralPart[i];
                tmp -= denominator * facient;
                tmp.validateDegree();
                if(tmp.degree() < denominator.degree())
                    isEndOfOperation = true;
                --i;
            }
            residual = tmp;
        }
        integralPart.validateDegree();
        residual.validateDegree();
        return std::make_pair(integralPart, residual);
    }

    template<class T>
    Polynom<T> Polynom<T>::operator/(const T &value)
    {
        Polynom<T> product(_degree, static_cast<T>(0));
        for(int i = 0; i <= _degree; ++i)
            if(value != static_cast<T>(0))
                product._coefficients[i] = getCoefAt(i) / value;
        return product;
    }

    template<class T>
    Polynom<T>& Polynom<T>::operator+=(const Polynom &other)
    {
        if(other.degree() > _degree)
        {
            _degree = other.degree();
            _coefficients.resize(_degree + 1);
        }
        for(int i = 0; i <= _degree; ++i)
            _coefficients[i] += other.getCoefAt(i);
        return *this;
    }

    template<class T>
    Polynom<T>& Polynom<T>::operator+=(const T& value)
    {
          _coefficients[0] += value;
          return *this;
    }

    template<class T>
    Polynom<T>& Polynom<T>::operator-=(const Polynom& other)
    {

        if(other.degree() > _degree) _coefficients.resize(other.degree());
        for(int i = 0; i <= _degree; ++i)
            _coefficients[i] -= other.getCoefAt(i);
        return *this;
    }

    template<class T>
    Polynom<T>& Polynom<T>::operator-=(const T& value)
    {
        _coefficients[0] -= value;
        return *this;
    }

    template<class T>
    Polynom<T>& Polynom<T>::operator*=(const Polynom& other)
    {
        //Определяем степень результирующего полинома
        int productDegree = _degree + other.degree();
        Polynom<T> product(productDegree, static_cast<T>(0));
        for(int i = 0; i <= _degree; ++i)
            for(int j = 0; j <= other.degree(); ++j)
                product._coefficients[i + j] += getCoefAt(i) * other.getCoefAt(j);
        _degree = productDegree;
        _coefficients.resize(productDegree + 1);
        for(int i = 0; i <= productDegree; ++i)
            _coefficients[i] = product.getCoefAt(i);
       return *this;
    }

    template<class T>
    Polynom<T>& Polynom<T>::operator*=(const T& value)
    {
        for(int i = 0; i <= _degree; ++i)
            _coefficients[i] *= value;
        return *this;
    }

    template<class T>
    Polynom<T>& Polynom<T>::operator/=(const T& value)
    {
        if(value != static_cast<T>(0))
        for(int i = 0; i <= _degree; ++i)
            _coefficients[i] /= value;
        return *this;
    }

    //Перегрузка операторов икрементации и декрементации
    template <class T>
    Polynom<T>& operator++(Polynom<T> &pol)
    {
        pol.setValueAt(0, pol.getCoefAt(0) + static_cast<T>(1));
        return pol;
    }

    template <class T>
    Polynom<T> operator++(Polynom<T> &pol, int)
    {
        Polynom<T> old = pol;
        pol.setValueAt(0, pol.getCoefAt(0) + static_cast<T>(1));
        return old;
    }

    template <class T>
    Polynom<T>& operator--(Polynom<T> &pol)
    {
        pol.setValueAt(0, pol.getCoefAt(0) - static_cast<T>(1));
        return pol;
    }

    template <class T>
    Polynom<T> operator--(Polynom<T> &pol, int)
    {
        Polynom<T> old = pol;
        pol.setValueAt(0, pol.getCoefAt(0) - static_cast<T>(1));
        return old;
    }
    //Перегрузка операторов потокового ввода-вывода добавить для rvalue
    ////////////////////////////////Console///////////////////////////
    template<class T>
    std::istream& operator>>(std::istream &stream, Polynom<T> &pol)
    {
        for(int i = 0; i <= pol.degree(); ++i)
            {
                T tmp;
                stream >> tmp;
                pol.setValueAt(i, tmp);
            }
        return stream;
    }

    //For lvalue
    template <typename T>
    std::ostream& operator<<(std::ostream &stream, Polynom<T> &pol)
    {
        std::ostringstream oss;
        oss << pol;
        stream << oss.str();
        return stream;
    }

    //For rvalue
    template <typename T>
    std::ostream& operator<<(std::ostream &stream, Polynom<T> &&pol)
    {
        stream << pol;
        return stream;
    }
    //////////////////////////////Files//////////////////////////
    template <typename T>
    std::ifstream& operator>>(std::ifstream &stream, Polynom<T> &pol)
    {
        for(int i = 0; i <= pol.degree(); ++i)
            {
                T tmp;
                stream >> tmp;
                pol.setValueAt(i, tmp);
            }
        return stream;
    }

    template <typename T>
    std::ofstream& operator<<(std::ofstream &stream, Polynom<T> &pol)
    {
        std::ostringstream oss;
        oss << pol;
        stream << oss.str();
        return stream;
    }
    //For rvalue
    template <typename T>
    std::ofstream& operator<<(std::ofstream &stream, Polynom<T> &&pol)
    {
        stream << pol;
        return stream;
    }
    //////////////////////////Stringstreams///////////////////////////
    template <typename T>
    std::istringstream& operator>>(std::istringstream &stream, Polynom<T> &pol)
    {
        for(int i = 0; i <= pol.degree(); ++i)
            {
                T tmp;
                stream >> tmp;
                pol.setValueAt(i, tmp);
            }
        return stream;
    }

    template <typename T>
    std::ostringstream& operator<<(std::ostringstream &stream, Polynom<T> &pol)
    {
        for(int i = 0; i <= pol.degree(); ++i)
           stream << std::setprecision(15) << pol.getCoefAt(i) << " ";
        stream << std::endl;
        return stream;
    }
    //For rvalue
    template <typename T>
    std::ostringstream& operator<<(std::ostringstream &stream, Polynom<T> &&pol)
    {
        stream << pol;
        return stream;
    }

}

#endif // POLYNOM_H
