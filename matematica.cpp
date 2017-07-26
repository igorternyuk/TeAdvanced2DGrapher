#include "matematica.h"

#include <cmath>
#include <ctime>
#include <limits>
#include <stdexcept>

#include "matrix.h"
#include "parser.h"


iat::CubicSplineInterpolator::CubicSplineInterpolator()
{}

void iat::CubicSplineInterpolator::interpolate(std::vector<std::pair<double, double>> pointsTable)
{
   // Создаем векторы для хранения трех диагоналей трехдиагональной матрицы
   nodeNumber = pointsTable.size();
   //Инициализируем сплайны
   //Вычисляем остальные коэффициенты сплайна
   for(int i = 0; i < int(nodeNumber); ++i)
   {
       CubicSpline cbspl;
       cbspl.x = pointsTable[i].first;
       cbspl.a = pointsTable[i].second;
       splineTuple.push_back(cbspl);
   }
   // Создаем векторы для коэффициентов прогонки
   // Размер массива с коэффициентами прогонки на единицу меньше количества узлов
   std::vector<double> alpha(nodeNumber - 1), beta(nodeNumber - 1);
   alpha[0] = 0.0;
   beta[0] = 0.0;
   double A = 0, B = 0, C = 0, D = 0, hi, hi1;
   // Вычисляем коэффициенты прогонки (прямой ход)
   for(int i = 1; i < int(nodeNumber - 1); ++i)
   {
       hi = pointsTable[i].first - pointsTable[i - 1].first;
       hi1 = pointsTable[i + 1].first - pointsTable[i].first;
       A = hi;
       B = 2 * (hi + hi1);
       C = hi1;
       D = 6 * ((pointsTable[i + 1].second - pointsTable[i].second) / hi1 -
               (pointsTable[i].second - pointsTable[i - 1].second) / hi);
       alpha[i] = - C / (A * alpha[i - 1] + B);
       beta[i] = (D - A * beta[i - 1]) /
                 (A * alpha[i - 1] + B);
   }
   splineTuple[0].c = 0.0;
   splineTuple[nodeNumber - 1].c = (D - A* beta[nodeNumber - 2]) /
                                      (A * alpha[nodeNumber - 2] + B);
   //Обратный ход метода прогонки
   for(int i = nodeNumber - 2; i > 0; --i)
   {
       splineTuple[i].c = alpha[i] * splineTuple[i + 1].c + beta[i];
   }
   //Вычисляем остальные коэффициенты сплайна
   for(int i = nodeNumber - 1; i > 0; --i)
   {
       double hi = pointsTable[i].first - pointsTable[i - 1].first;
       splineTuple[i].b = (pointsTable[i].second - pointsTable[i - 1].second) / hi +
                           hi * (2 * splineTuple[i].c + splineTuple[i - 1].c) / 6;
       splineTuple[i].d = (splineTuple[i].c - splineTuple[i - 1].c) / hi;
   }
}

double iat::CubicSplineInterpolator::calculate(double x)
{
    // Если сплайны еще не построены возвращаем NaN
    if(splineTuple.size() == 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    double dx;
    int j = nodeNumber - 1;
    // Если Х меньше абсциссы нулевого узла то пользуемся сплайном нулевого узла
    if(x <= splineTuple[0].x)
    {
        dx = x - splineTuple[1].x;
        j = 1;
    } else if(x >= splineTuple[nodeNumber - 1].x) // Если Х больше абсциссы последнего узла то пользуемся сплайном последнего узла
    {
        dx = x - splineTuple[nodeNumber - 1].x;
        j = nodeNumber - 1;
    } else
    {
        // Бинарный поиск нужного сегмента
        int i = 0;
        while(i + 1 < j)
        {
           int k = i + (j - i) / 2;
           if(x <= splineTuple[k].x)
              j = k;
           else
              i = k;
        }
        dx = x - splineTuple[j].x;
    }
    return splineTuple[j].a + splineTuple[j].b * dx + splineTuple[j].c * pow(dx, 2) / 2 +
           splineTuple[j].d * pow(dx, 3) / 6;
}

std::vector<double> iat::CubicSplineInterpolator::allCoef()
{
    std::vector<double> allCoefs;
    for(int i = 1; i < int(nodeNumber); ++i)
    {
        allCoefs.push_back(splineTuple[i].a);
        allCoefs.push_back(splineTuple[i].b);
        allCoefs.push_back(splineTuple[i].c);
        allCoefs.push_back(splineTuple[i].d);
    }
    return allCoefs;
}

iat::LeastSquareMethod::LeastSquareMethod(std::vector<std::pair<double, double>> xy):
                   pointsNumber(xy.size()), xy(xy)
{}

iat::LeastSquareMethod::LeastSquareMethod(std::vector<double> x, std::vector<double> y):
                   pointsNumber(x.size())
{
    for(int i = 0; i < pointsNumber; ++i)
    {
        std::pair<double, double> p(x[i], y[i]);
        xy.push_back(p);
    }
}

void iat::LeastSquareMethod::linearApproximation(double &a, double &b, double &error)
{
    Matrix<double> mtrA(2, 2, 0.0);
    Matrix<double> mtrB(2, 1, 0.0);
    for(int i = 0; i < pointsNumber; ++i)
    {
        mtrA[0][0] += pow(xy[i].first, 2);
        mtrA[0][1] += xy[i].first;
        mtrB[0][0] += xy[i].first * xy[i].second;
        mtrB[1][0] += xy[i].second;
    }
    mtrA[1][0] = mtrA.getValueAt(0, 1);
    mtrA[1][1] = (double)pointsNumber;
    if(mtrA.determinant() != 0)
    {
        Matrix<double> mtrRes = mtrA.inverse() * mtrB;
        a = mtrRes.getValueAt(0, 0);
        b = mtrRes.getValueAt(1, 0);
    }else
    {
        a = 0;
        b = 0;
    }
    for(int i = 0; i < pointsNumber; ++i)
    {
        error += pow(xy[i].second - a * xy[i].first - b, 2);
        error /= pointsNumber;
        error = sqrt(error);
    }
}

void iat::LeastSquareMethod::hyperbolicApproximation(double &a, double &b, double &error)
{
    Matrix<double> mtrA(2, 2, 0.0);
    Matrix<double> mtrB(2, 1, 0.0);
    for(int i = 0; i < pointsNumber; ++i)
    {
        mtrA[0][0] += pow(xy[i].first, -2);
        mtrA[0][1] += pow(xy[i].first, -1);
        mtrB[0][0] += xy[i].second / xy[i].first;
        mtrB[1][0] += xy[i].second;
    }
    mtrA[1][0] = mtrA.getValueAt(0, 1);
    mtrB[1][1] = (double)pointsNumber;
    if(mtrA.determinant() != 0)
    {
        Matrix<double> mtrRes = mtrA.inverse() * mtrB;
        a = mtrRes.getValueAt(0, 0);
        b = mtrRes.getValueAt(1, 0);
    }
    else
    {
        a = 0;
        b = 0;
    }
    for(int i = 0; i < pointsNumber; ++i)
    {
        error += pow(xy[i].second - a / xy[i].first - b, 2);
        error /= pointsNumber;
        error = sqrt(error);
    }
}

void iat::LeastSquareMethod::logarithmicApproximation(double &a, double &b, double &error)
{
    Matrix<double> mtrA(2, 2, 0.0);
    Matrix<double> mtrB(2, 1, 0.0);
    for(int i = 0; i < pointsNumber; ++i)
    {
        if(xy[i].first < 0) continue;
        mtrA[0][0] += pow(log(xy[i].first), 2);
        mtrA[0][1] += log(xy[i].first);
        mtrB[0][0] += xy[i].second * log(xy[i].first);
        mtrB[1][0] += xy[i].second;
    }
    mtrA[1][0] = mtrA.getValueAt(0, 1);
    mtrB[1][1] = (double)pointsNumber;
    if(mtrA.determinant() != 0)
    {
        Matrix<double> mtrRes = mtrA.inverse() * mtrB;
        a = mtrRes.getValueAt(0, 0);
        b = mtrRes.getValueAt(1, 0);
    }
    else
    {
        a = 0;
        b = 0;
    }
    for(int i = 0; i < pointsNumber; ++i)
    {
        if(xy[i].first < 0) continue;
        error += pow(xy[i].second - a * log(xy[i].first) - b, 2);
        error /= pointsNumber;
        error = sqrt(error);
    }
}

void iat::LeastSquareMethod::exponentialApproximation(double &a, double &b, double &error)
{
    Matrix<double> mtrA(2, 2, 0.0);
    Matrix<double> mtrB(2, 1, 0.0);
    for(int i = 0; i < pointsNumber; ++i)
    {
        if(xy[i].first < 0) continue;
        mtrA[0][0] += pow(xy[i].first, 2);
        mtrA[0][1] += xy[i].first;
        mtrB[0][0] += xy[i].first * log(xy[i].second);
        mtrB[1][0] += xy[i].second;
    }
    mtrA[1][0] = mtrA.getValueAt(0, 1);
    mtrB[1][1] = (double)pointsNumber;
    if(mtrA.determinant() != 0)
    {
        Matrix<double> mtrRes = mtrA.inverse() * mtrB;
        a = exp(mtrRes.getValueAt(0, 0));
        b = mtrRes.getValueAt(1, 0);
    }
    else
    {
        a = 0;
        b = 0;
    }
    for(int i = 0; i < pointsNumber; ++i)
    {
        error += pow(xy[i].second - a * exp(b * xy[i].first), 2);
        error /= pointsNumber;
        error = sqrt(error);
    }
}

void iat::LeastSquareMethod::quadraticApproximation(double &a, double &b, double &c, double &error)
{
    int pointsNumber = xy.size();
    Matrix<double> mtrA(3, 3, 0.0);
    Matrix<double> mtrB(3, 1, 0.0);
    for(int i = 0; i < pointsNumber; ++i)
    {
        mtrA[0][0] += pow(xy[i].first, 4);
        mtrA[0][1] += pow(xy[i].first, 3);
        mtrA[0][2] += pow(xy[i].first, 2);
        mtrA[1][2] += xy[i].first;
        mtrB[0][0] += pow(xy[i].first,2) * xy[i].second;
        mtrB[1][0] += xy[i].first * xy[i].second;
        mtrB[2][0] += xy[i].second;
    }
    mtrA[1][0] = mtrA.getValueAt(0, 1);
    mtrA[1][1] = mtrA.getValueAt(0, 2);
    mtrA[2][0] = mtrA.getValueAt(0, 2);
    mtrA[2][1] = mtrA.getValueAt(1, 2);
    mtrA[2][2] = (double)pointsNumber;
    if(mtrA.determinant() != 0)
    {
        Matrix<double> mtrRes = mtrA.inverse() * mtrB;
        a = mtrRes.getValueAt(0, 0);
        b = mtrRes.getValueAt(1, 0);
        c = mtrRes.getValueAt(2, 0);
    }else
    {
        a = 0;
        b = 0;
        c = 0;
    }
    for(int i = 0; i < pointsNumber; ++i)
    {
        error += pow(xy[i].second - a * pow(xy[i].first, 2) - b * xy[i].first - c, 2);
        error /= pointsNumber;
        error = sqrt(error);
    }
}

std::vector<double> iat::LeastSquareMethod::polynomialLeastSquareMethod(double polynomDegree, double &error)
{
    if(polynomDegree > pointsNumber - 1) polynomDegree = pointsNumber - 1;
    // Определяем размер матрицы СЛАУ для нахождения коэффициентов аппроксимации
    int N = polynomDegree + 1;
    Matrix<double> mtrA(N, N, 0.0);
    Matrix<double> mtrB(N, 1, 0.0);
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            for(int k = 0; k < pointsNumber; ++k)
                mtrA[i][j] += pow(xy[k].first, i + j);
        }
        for(int k = 0; k < pointsNumber; ++k)
            mtrB[i][0] += xy[k].second * pow(xy[k].first, i);
    }
    std::vector<double> res;
    if(mtrA.determinant() != 0)
    {
        Matrix<double> mtrRes = mtrA.inverse() * mtrB;
        for(int i = 0; i < N; ++i)            
            res.push_back(mtrRes.getValueAt(i, 0));
    }
    else
    {
        for(int i = 0; i < N; ++i)
            res.push_back(0.0);
        return res;
    }

    for(int i = 0; i < pointsNumber; ++i)
    {
        double polynomVal = 0.0;
        for(int k = 0; k < N; ++k)
        {
            polynomVal += res[k] * pow(xy[i].first, k); 
        }
        error += pow(xy[i].second - polynomVal, 2);
        error /= pointsNumber;
        error = sqrt(error);
    }
     return res;
}

double iat::factorial(double n)
{
    if (floor(n) == 0 || floor(n) == 1) return 1;
    else return n * factorial(n - 1);
}

int iat::signum(double value)
{
    return value >= 0 ? 1: -1;
}

double iat::polynomLagrange(std::vector<std::pair<double, double> > pointsTable, double value)
{
    double summa = 0, num, den, omega;                       // L = w1 * y1 + w2 * y2 + ... + wn * yn
    for(unsigned int i = 0; i < pointsTable.size(); ++i)
    {
        omega = 1;
        num = 1;
        den = 1;
        for(unsigned int j = 0; j < pointsTable.size(); ++j)
        {
            if (i != j)
            {
              num *= (value - pointsTable[j].first);
              den *= pointsTable[i].first - pointsTable[j].first;
            }
        }
        omega = num / den;
        summa += omega * pointsTable[i].second;
    }
    return summa;
}

iat::Polynom<double> iat::polynomLagrange(std::vector<std::pair<double, double> > pointsTable)
{
    Polynom<double> polynomSumma(0, 0);
    double omega = 1;
    double den = 1;
    for(unsigned int i = 0; i < pointsTable.size(); ++i)
    {
        omega = 1;
        den = 1;
        Polynom<double> polynomProduct(0, 1);
        for(unsigned int j = 0; j < pointsTable.size(); ++j)
        {
            if (i != j)
            {
              std::vector<double> polCoefs;
              //polCoefs.push_back(-pointsTable[j].first);
              //polCoefs.push_back(1);
              Polynom<double> pol{-pointsTable[j].first, 1};
              polynomProduct *= pol;
              den *= pointsTable[i].first - pointsTable[j].first;
            }
        }
        omega = pointsTable[i].second / den;
        polynomProduct *= omega;
        polynomSumma += polynomProduct;
    }
    return polynomSumma;
}

std::vector<double> iat::polynomLagrangeVector(std::vector<std::pair<double, double> > pointsTable)
{
    Polynom<double> result = polynomLagrange(pointsTable);
    std::vector<double> resVct;
    for(int i = 0; i <= result.degree(); ++i)
        resVct.push_back(result.getCoefAt(i));
    return resVct;
}

std::string iat::polynomLagrangeString(std::vector<std::pair<double, double> > pointsTable, int prec)
{
    Polynom<double> result = polynomLagrange(pointsTable);
    return result.convertToString(prec);
}

double iat::integralTablLeftHandRect(std::vector<std::pair<double, double>> integrandTable)
{
    if(!integrandTable.empty())
    {
        int n = integrandTable.size() - 1;
        double currentStep, Integral = 0;
        for(int i = 0; i < n; ++i)
        {
           currentStep = integrandTable[i + 1].first - integrandTable[i].first;
           Integral += integrandTable[i].second * currentStep;
        }
        return Integral;
    }
    else return 0.0;
}

double iat::integralTablRightHandRect(std::vector<std::pair<double, double>> integrandTable)
{
    if(!integrandTable.empty())
    {
        int n = integrandTable.size() - 1;
        double currentStep, Integral = 0;
        for(int i = 1; i <= n; ++i)
        {
           currentStep = integrandTable[i].first - integrandTable[i - 1].first;
           Integral += integrandTable[i].second * currentStep;
        }
        return Integral;
    }else return 0.0;
}

double iat::integralTablTrapezoids(std::vector<std::pair<double, double>> integrandTable)
{
    if(!integrandTable.empty())
    {
        int n = integrandTable.size() - 1;
        double currentStep, Integral = 0;
        for(int i = 1; i <= n; ++i)
        {
           currentStep = integrandTable[i].first - integrandTable[i - 1].first;
           Integral += (integrandTable[i].second + integrandTable[i - 1].second) * currentStep;
        }
        Integral /= 2;
        return Integral;
    }else return 0.0;
}

double iat::integralTablSimpson(std::vector<std::pair<double, double> > integrandTable)
{
    if(!integrandTable.empty())
    {
        double h1, h2; // steps
        double Integral = 0;
        int n = integrandTable.size() - 1;
        if(n % 2 != 0) throw std::runtime_error("The number of intervals must be even!");
        for(int i = 0; i <= n - 2; i += 2)
            {
                h1 = integrandTable[i + 1].first - integrandTable[i].first;
                h2 = integrandTable[i + 2].first - integrandTable[i + 1].first;
                Integral += ((h1 + h2) / (6 * h1 * h2)) *
                (h2 * (2 * h1 - h2) * integrandTable[i].second + pow(h1 + h2, 2) * integrandTable[i + 1].second +
                h1 * (2 * h2 - h1) * integrandTable[i + 2].second);
            }
        return Integral;
    }else return 0.0;

}

double iat::integralTablPolLagr(std::vector<std::pair<double, double> > integrandTable,
                           double a, double b)
{
    if(!integrandTable.empty())
    {
        int n = integrandTable.size() - 1;
        Matrix<double> mtrX(n, n, 0.0);
        Matrix<double> mtrI(n, 1, 0.0);
        //Заполняем матрицы
        for(int i = 0; i < mtrX.getRowCount(); ++i)
        {
            mtrI.setValueAt(i, 0, (pow(b, i + 1) - pow(a, i + 1)) / (i + 1));
            for(int j = 0; j < mtrX.getColumnCount(); ++j)
            {
                mtrX.setValueAt(i, j, pow(integrandTable[j].first, i));
            }
        }
        Matrix<double> mtrInv = mtrX.inverse();
        Matrix<double> mtrA = mtrInv * mtrI;
        double Integral = 0;
        for(int i = 0; i <= n; ++i)
        {
           Integral += mtrA.getValueAt(i,0) * integrandTable[i].second;
        }
        return Integral;
    }else return 0.0;
}

std::vector<std::complex<double>> iat::solveCubicEquation(double a, double b, double c, double d)
{
    double p = c / a - pow(b, 2) / (3 * pow(a, 2));
    double q = (2 * pow(b, 3) - 9 * a * b * c + 27 * a * a * d) / pow(3 * a, 3);
    std::complex<double> alpha, beta;
    std::complex<double> Q = pow(p / 3, 3) + pow(q / 2, 2);
    if(Q.real() >= 0)
    {
        alpha = signum(-0.5 * q + sqrt(Q.real())) * pow(fabs(-0.5 * q + sqrt(Q.real())), 1 / 3.0);
        beta = signum(-0.5 * q - sqrt(Q.real())) * pow(fabs(-0.5 * q - sqrt(Q.real())), 1 / 3.0);
    }else
    {
        std::complex<double> Zalpha = -0.5 * q + sqrt(Q);
        std::complex<double> Zbeta = -0.5 * q - sqrt(Q);
        alpha = pow(Zalpha, 1 / 3.0);
        beta = pow(Zbeta , 1 / 3.0);
    }
    std::complex<double> y1 = alpha + beta;
    std::complex<double> unit(-1,0);
    std::complex<double> j = sqrt(unit);
    std::complex<double> x1 = y1 - b / (3 * a);
    std::complex<double> x2 = -0.5 * (alpha + beta) + 0.5 * (alpha - beta) * sqrt(3.0) * j - b / (3 * a);
    std::complex<double> x3 = -0.5 * (alpha + beta) - 0.5 * (alpha - beta) * sqrt(3.0) * j - b / (3 * a);
    std::vector<std::complex<double>> roots;
    roots.push_back(x1);
    roots.push_back(x2);
    roots.push_back(x3);
    return roots;
}

std::vector<std::complex<double>> iat::solveSquareEquation(double a, double b, double c)
{
    std::complex<double> discr(pow(b, 2) - 4 * a * c, 0.0);
    std::complex<double> x1 = (-b + sqrt(discr)) / (2 * a);
    std::complex<double> x2 = (-b - sqrt(discr)) / (2 * a);
    std::vector<std::complex<double>> roots;
    roots.push_back(x1);
    roots.push_back(x2);
    return roots;
}

double iat::tablFuncLeftDer1(std::vector<std::pair<double, double> > pointsTable, double value)
{
    const double deltaArgValue = 0.001;
    double func = polynomLagrange(pointsTable, value);
    double funcMinusDelta = polynomLagrange(pointsTable, value - deltaArgValue);
    double funcMinusTwoDeltas = polynomLagrange(pointsTable, value - 2 * deltaArgValue);
    return (3 * func - 4 * funcMinusDelta + funcMinusTwoDeltas) / (2 * deltaArgValue);
}

double iat::tablFuncRightDer1(std::vector<std::pair<double, double> > pointsTable, double value)
{
   const double deltaArgValue = 0.001;
   double func = polynomLagrange(pointsTable, value);
   double funcPlusDelta = polynomLagrange(pointsTable, value + deltaArgValue);
   double funcPlusTwoDeltas = polynomLagrange(pointsTable, value + 2 * deltaArgValue);
   return (-3 * func + 4 * funcPlusDelta - funcPlusTwoDeltas) / (2 * deltaArgValue);
}

double iat::tablFuncDer1(std::vector<std::pair<double, double> > pointsTable, double value)
{
    const double deltaArgValue = 0.001;
    double funcMinusDelta = polynomLagrange(pointsTable, value - deltaArgValue);
    double funcPlusDelta = polynomLagrange(pointsTable, value + deltaArgValue);
    return (funcPlusDelta - funcMinusDelta) / (2 * deltaArgValue);
}

double iat::tablFuncDer2(std::vector<std::pair<double, double> > pointsTable, double value)
{
    const double deltaArgValue = 0.001;
    double funcPlusDelta = polynomLagrange(pointsTable, value + deltaArgValue);
    double func= polynomLagrange(pointsTable, value);
    double funcMinusDelta = polynomLagrange(pointsTable, value - deltaArgValue);
    return (funcPlusDelta - 2 * func + funcMinusDelta) / pow(deltaArgValue, 2);
}

double iat::func(std::string expr, char argName, double argValue)
{
    std::pair<char, double> var(argName, argValue);
    std::vector<std::pair<char, double>> vctVars;
    vctVars.push_back(var);
    Parser prs(expr, vctVars);
    return prs.calculateExpression();
}

double iat::funcLeftDer1(std::string expr, char argName, double argValue)
{
    const double deltaArgValue = 0.001;
    double funcX= func(expr, argName, argValue);
    double funcMinusTwoDeltas = func(expr, argName, argValue - 2 * deltaArgValue);
    double funcMinusDelta = func(expr, argName, argValue - deltaArgValue);
    return (3 * funcX - 4 * funcMinusDelta + funcMinusTwoDeltas) / (2 * deltaArgValue);
}

double iat::funcRightDer1(std::string expr, char argName, double argValue)
{
    const double deltaArgValue = 0.001;
    double funcPlusDelta = func(expr, argName, argValue + deltaArgValue);
    double funcX= func(expr, argName, argValue);
    double funcPlusTwoDeltas = func(expr, argName, argValue + 2 * deltaArgValue);
    return (-3 * funcX + 4 * funcPlusDelta - funcPlusTwoDeltas) / (2 * deltaArgValue);
}

double iat::funcDer1(std::string expr, char argName, double argValue)
{
    const double deltaArgValue = 0.001;
    double funcPlusDelta = func(expr, argName, argValue + deltaArgValue);
    double funcMinusDelta = func(expr, argName, argValue - deltaArgValue);
    double res = (funcPlusDelta - funcMinusDelta) / (2 * deltaArgValue);
    return res;
}

double iat::funcDer2(std::string expr, char argName, double argValue)
{
    const double deltaArgValue = 0.001;
    double funcPlusDelta = func(expr, argName, argValue + deltaArgValue);
    double funcX= func(expr, argName, argValue);
    double funcMinusDelta = func(expr, argName, argValue - deltaArgValue);
    return (funcPlusDelta - 2 * funcX + funcMinusDelta) / pow(deltaArgValue, 2);
}

double iat::multiVariableFunc(std::string expr, std::vector<std::pair<char, double>> varList)
{
    Parser prs(expr, varList);
    return prs.calculateExpression();
}

double iat::funcPartialDer1(std::string expr, std::vector<std::pair<char, double>> variables,char argName)
{
    double delta = 0.001;

    for (auto &p : variables)
    {
        if(p.first == argName)
        {
            p.second +=delta;
            break;
        }
    }
    Parser prs_plus(expr, variables);
    double funcPlusDeltaValue =  prs_plus.calculateExpression();
    for (auto &p : variables)
    {
        if(p.first == argName)
        {
            p.second -= 2 * delta;
            break;
        }
    }
    iat::Parser prs_minus(expr, variables);
    double funcMinusDeltaValue =  prs_minus.calculateExpression();
    return (funcPlusDeltaValue - funcMinusDeltaValue) / 2 / delta;
}

double iat::integralLeftHandRect(std::string expr, char argName, double leftLim, double rightLim,
                                     double eps, int &iterrNumber, std::string &iterProcess)
{
    double a = leftLim, b = rightLim;
    int n = 6;
    double h = (b - a) / n;
    double Inext = 0, Iprev = 0;
    iterrNumber = 0;
    std::stringstream stream;
    stream << "Выполняется численное интегрирование методом левых прямоугольников\n";
    const int MAX_ITERRATIONS_NUMBER = 5000;
    bool flag = false;
    do
    {
        stream << "------------------------------------------------------\n" ;
        stream << "Выполняется итеррация номер : " << ++iterrNumber << "\n";
        Iprev = Inext;
        Inext = 0;
        n *= 2;
        h = (b - a) / n;

        for(int i = 0; i < n; ++i)
        {
            Inext += func(expr, argName, a + i * h);
        }
        Inext *= h;
        stream << "Количество интервалов разбиения на данной итеррации равно:\n n = " << n << "\n";
        stream << "Ширина i-го интервала на данной итеррации равна:\n hi = " << h << "\n";
        stream << "Значение интеграла на данной  итеррации равно:\n Ii = " << Inext << "\n";
        stream << "Погрешность данной  итеррации равна:\n ΔIi = Ii - Ii-1 = " << Inext << "\n";
        if(fabs(Inext - Iprev) < fabs(eps))
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ ДОСТИГНУТА!\n";
            stream << "ЗНАЧЕНИЕ ИНТЕГРАЛА РАВНО: I = " << Inext << "\n";
            flag = true;
        }
        else if (iterrNumber > MAX_ITERRATIONS_NUMBER )
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ НЕ ДОСТИГНУТА!\n";
            break;
        }
    }while(!flag);
    iterProcess = stream.str();
    return Inext;
}

double iat::integralRightHandRect(std::string expr, char argName, double leftLim, double rightLim,
                                      double eps, int &iterrNumber, std::string &iterProcess)
{
    double a = leftLim, b = rightLim;
    int n = 6;
    double h = (b - a) / n;
    double Inext = 0, Iprev = 0;
    iterrNumber = 0;
    const int MAX_ITERRATIONS_NUMBER = 5000;
    bool flag = false;
    std::stringstream stream;
    stream << "Выполняется численное интегрирование методом правых прямоугольников\n";
    do
    {
        stream << "Выполняется итеррация номер : \n" << ++iterrNumber << "\n";
        Iprev = Inext;
        Inext = 0;
        n *= 2;
        h = (b - a) / n;
        for(int i = 1; i <= n; ++i)
        {
            Inext += func(expr, argName, a + i * h);
        }
        Inext *= h;
        stream << "Количество интервалов разбиения на данной итеррации равно:\n n = " << n << "\n";
        stream << "Ширина i-го интервала на данной итеррации равна:\n hi = " << h << "\n";
        stream << "Значение интеграла на данной  итеррации равно:\n Ii = " << Inext << "\n";
        stream << "Погрешность данной  итеррации равна:\n ΔIi = Ii - Ii-1 = " << Inext << "\n";
        if(fabs(Inext - Iprev) < fabs(eps))
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ ДОСТИГНУТА!\n";
            stream << "ЗНАЧЕНИЕ ИНТЕГРАЛА РАВНО: I = " << Inext << "\n";
            flag = true;
        }
        else if (iterrNumber > MAX_ITERRATIONS_NUMBER )
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ НЕ ДОСТИГНУТА!\n";
            break;
        }
    }while(!flag);
    iterProcess = stream.str();
    return Inext;
}

double iat::integralMidpoint(std::string expr, char argName, double leftLim, double rightLim,
                                 double eps, int &iterrNumber, std::string &iterProcess)
{
    double a = leftLim, b = rightLim;
    int n = 6;
    double h = (b - a) / n;
    double Inext = 0, Iprev = 0;
    iterrNumber = 0;
    const int MAX_ITERRATIONS_NUMBER = 5000;
    bool flag = false;
    std::stringstream stream;
    stream << "Выполняется численное интегрирование методом средних прямоугольников\n";
    do
    {
        stream << "Выполняется итеррация номер : " << ++iterrNumber << "\n";
        Iprev = Inext;
        Inext = 0;
        n *= 2;
        h = (b - a) / n;
        for(int i = 1; i <= n; ++i)
        {
            Inext += func(expr, argName, a + i * h - 0.5 * h);
        }
        Inext *= h;
        stream << "Количество интервалов разбиения на данной итеррации равно:\n n = " << n << "\n";
        stream << "Ширина i-го интервала на данной итеррации равна:\n hi = " << h << "\n";
        stream << "Значение интеграла на данной  итеррации равно:\n Ii = " << Inext << "\n";
        stream << "Погрешность данной  итеррации равна:\n ΔIi = Ii - Ii-1 = " << Inext << "\n";
        if(fabs(Inext - Iprev) < fabs(eps))
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ ДОСТИГНУТА!\n";
            stream << "ЗНАЧЕНИЕ ИНТЕГРАЛА РАВНО: I = " << Inext << "\n";
            flag = true;
        }
        else if (iterrNumber > MAX_ITERRATIONS_NUMBER )
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ НЕ ДОСТИГНУТА!\n";
            break;
        }
    }while(!flag);
    iterProcess = stream.str();
    return Inext;
}

double iat::integralTrapezoids(std::string expr, char argName, double leftLim, double rightLim,
                                   double eps, int &iterrNumber, std::string &iterProcess)
{
    double a = leftLim, b = rightLim;
    int n = 6;
    double h = (b - a) / n;
    double Iprev = 0, Inext = 0;
    iterrNumber = 0;
    const int MAX_ITERRATIONS_NUMBER = 5000;
    bool flag = false;
    std::stringstream stream;
    stream << "Выполняется численное интегрирование методом трапеций \n";
    do
    {
        stream << "------------------------------------------------------\n" ;
        stream << "Выполняется итеррация номер : " << ++iterrNumber << "\n";
        Iprev = Inext;
        Inext = 0;
        n *= 2;
        h = (b - a) / n;
        for(int i = 0; i < n; ++i)
        {
            Inext += 0.5 * (func(expr, argName, a + i * h) +
                     func(expr, argName, a + (i + 1) * h));
        }
        Inext *= h;
        stream << "Количество интервалов разбиения на данной итеррации равно:\n n = " << n << "\n";
        stream << "Ширина i-го интервала на данной итеррации равна:\n hi = " << h << "\n";
        stream << "Значение интеграла на данной  итеррации равно:\n Ii = " << Inext << "\n";
        stream << "Погрешность данной  итеррации равна:\n ΔIi = Ii - Ii-1 = " << Inext << "\n";
        if(fabs(Iprev - Inext) < fabs(eps))
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ ДОСТИГНУТА!\n";
            stream << "ЗНАЧЕНИЕ ИНТЕГРАЛА РАВНО: I = " << Inext << "\n";
            flag = true;
        }
        else if (iterrNumber > MAX_ITERRATIONS_NUMBER )
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ НЕ ДОСТИГНУТА!\n";
            break;
        }
    }while(!flag);
    iterProcess = stream.str();
    return Inext;
}

double iat::integralSimpson(std::string expr, char argName, double leftLim, double rightLim,
                                double eps, int &iterrNumber, std::string &iterProcess)
{
    double a = leftLim, b = rightLim;
    int n = 6;
    double h = (b - a) / n;
    double Inext = 0, Iprev = 0;
    iterrNumber = 0;
    const int MAX_ITERRATIONS_NUMBER = 5000;
    bool flag = false;
    std::stringstream stream;
    stream << "Выполняется численное интегрирование по методу Томаса Симпсона\n";
    do
    {
        stream << "------------------------------------------------------\n" ;
        stream << "Выполняется итеррация номер : " << ++iterrNumber << "\n";
        Iprev = Inext;
        Inext = 0;
        n *= 2;
        h = (b - a) / n;
        for(int k = 1; k < n; k += 2)
        {
            Inext += func(expr, argName, a + (k - 1) * h) +
                     4 * func(expr, argName, a + k * h) +
                     func(expr, argName, a + (k + 1) * h);
        }
        Inext *= h / 3;
        stream << "Количество интервалов разбиения на данной итеррации равно:\n n = " << n << "\n";
        stream << "Ширина i-го интервала на данной итеррации равна:\n hi = " << h << "\n";
        stream << "Значение интеграла на данной  итеррации равно:\n Ii = " << Inext << "\n";
        stream << "Погрешность данной  итеррации равна:\n ΔIi = Ii - Ii-1 = " << Inext << "\n";
        if(fabs(Inext - Iprev) < fabs(eps))
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ ДОСТИГНУТА!\n";
            stream << "ЗНАЧЕНИЕ ИНТЕГРАЛА РАВНО: I = " << Inext << "\n";
            flag = true;
        }
        else if (iterrNumber > MAX_ITERRATIONS_NUMBER )
        {
            stream << "------------------------------------------------------\n" ;
            stream << "НЕОБХОДИМАЯ ТОЧНОСТЬ НЕ ДОСТИГНУТА!\n";
            break;
        }
    }while(!flag);
    iterProcess = stream.str();
    return Inext;
}

double iat::solveEquationChordsMethod(std::string expr, char argName, double a, double b,
                                  double eps, int &iterrNumber, std::string &iterProcess)
{
    const int MAX_ITERRATIONS_NUMBER = 5000;
    std::ostringstream stream;
    if(func(expr, argName, b) * func(expr, argName, a) > 0)
    {
        stream << "На границах указанного интервала изоляции корня значения функции имеют одниковые знаки!\n";
        stream << "Укажите такие значения границ интервала изоляции, чтобы левая часть уравнения f(X)=0 меняла знак\n";
        stream << "при изменении аргумента от одной границе к другой\n";
        iterProcess = stream.str();
        iterrNumber = 0;
        return 0.0;
    }
    if(funcDer2(expr, 'X', a) < 0)
    {
        double buf = a;
        a = b;
        b = buf;
    }
    double Xnext = a;
    bool flag = false;
    do
    {
        a = Xnext;
        Xnext = a - func(expr, argName, a) * (b - a) / (func(expr, argName, b) - func(expr, argName, a));
        iterrNumber++;
        stream << "------------------------------------------------------\n" ;
        stream << "Итеррация номер: " << iterrNumber << "\n";
        stream << "Значение корня на текущей итеррации:\n Xi = " << Xnext << "\n";
        stream << "Значение погрешности на текущей итеррации:\n ΔXi+1 - Xi = " << fabs(Xnext - a) << "\n";
        if(fabs(Xnext - a) < eps)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НАЙДЕНО : Х = " << Xnext << "\n";
            flag = true;
        }else if(iterrNumber > MAX_ITERRATIONS_NUMBER)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НЕ НАЙДЕНО!ПОРОБУЙТЕ СУЗИТЬ ИЛИ ИЗМЕНИТЬ ИНТЕРВАЛ ИЗОЛЯЦИИ КОРНЯ.\n";
            break;
        }

    }while(!flag);
    iterProcess = stream.str();
    return Xnext;
}

double iat::solveEquationTangentsMethod(std::string expr, char argName, double a, double b,
                                    double eps, int &iterrNumber, std::string &iterProcess)
{
        const int MAX_ITERRATIONS_NUMBER = 5000;
        std::ostringstream stream;
        if(func(expr, argName, b) * func(expr, argName, a) > 0)
        {
            stream << "На границах указанного интервала изоляции корня значения функции имеют одниковые знаки!\n";
            stream << "Укажите такие значения границ интервала изоляции, чтобы левая часть уравнения f(X)=0 меняла знак\n";
            stream << "при изменении аргумента от одной границе к другой\n";
            iterProcess = stream.str();
            iterrNumber = 0;
            return 0.0;
        }
        if(funcDer2(expr, 'X', b) < 0)
        {
            double buf = a;
            a = b;
            b = buf;
        }
        double Xnext = b;
        bool flag = false;
        do
        {
            b = Xnext;
            double derivative = funcDer1(expr, argName, b);
            if(derivative == 0)
            {
                stream << "------------------------------------------------------\n" ;
                stream << "Текущее значение приближения совпало с точкой экстремума функции f(X) = 0.\n";
                stream << "Попробуйте изменить границы интервала изоляции корня.\n";
                break;
            }
            Xnext -= func(expr, argName, b) / derivative;
            iterrNumber++;
            stream << "------------------------------------------------------\n" ;
            stream << "Итеррация номер: " << iterrNumber << "\n";
            stream << "Значение корня на текущей итеррации:\n Xi = " << Xnext << "\n";
            stream << "Значение погрешности на текущей итеррации:\n ΔXi+1 - Xi = " << fabs(Xnext - b)  << "\n";
            if(fabs(Xnext - b) < eps)
            {
                stream << "------------------------------------------------------\n" ;
                stream << "РЕШЕНИЕ НАЙДЕНО : Х = " << Xnext << "\n";
                flag = true;
            }else if(iterrNumber > MAX_ITERRATIONS_NUMBER)
            {
                stream << "------------------------------------------------------\n" ;
                stream << "РЕШЕНИЕ НЕ НАЙДЕНО!ПОРОБУЙТЕ СУЗИТЬ ИЛИ ИЗМЕНИТЬ ИНТЕРВАЛ ИЗОЛЯЦИИ КОРНЯ.\n";
                break;
            }
        }while(!flag);
        iterProcess = stream.str();
        return Xnext;
}

double iat::solveEquationBisectionMethod(std::string expr, char argName, double a, double b,
                                     double eps, int &iterrNumber, std::string &iterProcess)
{
    const int MAX_ITERRATIONS_NUMBER = 5000;
    std::ostringstream stream;
    if(func(expr, argName, b) * func(expr, argName, a) > 0)
    {
        stream << "На границах указанного интервала изоляции корня значения функции имеют одниковые знаки!\n";
        stream << "Укажите такие значения границ интервала изоляции, чтобы левая часть уравнения f(X)=0 меняла знак\n";
        stream << "при изменении аргумента от одной границе к другой\n";
        iterProcess = stream.str();
        iterrNumber = 0;
        return 0.0;
    }
    double c;
    bool flag = false;
    while(!flag)
    {
        c = 0.5 * (a + b);
        if(func(expr, argName, c) * func(expr, argName, a) > 0)
            a = c;
        else
            b = c;
        iterrNumber++;
        stream << "------------------------------------------------------\n" ;
        stream << "Итеррация номер: " << iterrNumber << "\n";
        stream << "Значение корня на текущей итеррации:\n Xi = " << c << "\n";
        stream << "Значение погрешности на текущей итеррации:\n ΔXi+1 - Xi = " << fabs(a - b)  << "\n";
        if(fabs(a - b) < eps)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НАЙДЕНО : Х = " << 0.5 * (a + b) << "\n";
            flag = true;
        }else if(iterrNumber > MAX_ITERRATIONS_NUMBER)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НЕ НАЙДЕНО!ПОРОБУЙТЕ СУЗИТЬ ИЛИ ИЗМЕНИТЬ ИНТЕРВАЛ ИЗОЛЯЦИИ КОРНЯ.\n";
            break;
        }
    }
    iterProcess = stream.str();
    return 0.5 * (a + b);
}

double iat::solveEquationCombinedMethod(std::string expr, char argName, double a, double b,
                                     double eps, int &iterrNumber, std::string &iterProcess)
{
    const int MAX_ITERRATIONS_NUMBER = 5000;
    std::ostringstream stream;
    if(func(expr, argName, b) * func(expr, argName, a) > 0)
    {
        stream << "На границах указанного интервала изоляции корня значения функции имеют одниковые знаки!\n";
        stream << "Укажите такие значения границ интервала изоляции, чтобы левая часть уравнения f(X)=0 меняла знак\n";
        stream << "при изменении аргумента от одной границе к другой\n";
        iterProcess = stream.str();
        iterrNumber = 0;
        return 0.0;
    }
    bool flag = false;
    if(funcDer2(expr, argName, b) < 0)
    {
        double buf = a;
        a = b;
        b = buf;
    }
    stream << "Выполняется поиск решения комбинированным методом хорд и касательных.\n";
    while(!flag)
    {
        a -= func(expr, argName, a) * (b - a) / (func(expr, argName, b) - func(expr, argName, a));
        double derivative = funcDer1(expr, argName, b);
        if(derivative == 0)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "Текущее значение приближения совпало с точкой экстремума функции f(X) = 0.\n";
            stream << "Попробуйте изменить границы интервала изоляции корня.\n";
            break;
        }
        b -= func(expr, argName, b) / derivative;
        iterrNumber++;
        stream << "------------------------------------------------------\n" ;
        stream << "Итеррация номер: " << iterrNumber << "\n";
        stream << "Значение корня на текущей итеррации:\n Xi = " << 0.5 * (a + b) << "\n";
        stream << "Значение погрешности на текущей итеррации:\n ΔXi+1 - Xi = " << fabs(a - b) << "\n";
        if(fabs(a - b) < eps)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НАЙДЕНО : Х = " << 0.5 * (a + b) << "\n";
            flag = true;
        }else if(iterrNumber > MAX_ITERRATIONS_NUMBER)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НЕ НАЙДЕНО!ПОРОБУЙТЕ СУЗИТЬ ИЛИ ИЗМЕНИТЬ ИНТЕРВАЛ ИЗОЛЯЦИИ КОРНЯ.\n";
            break;
        }
    }
    iterProcess = stream.str();
    return 0.5 * (a + b);
}

double iat::solveEquationIterationMethod(std::string expr, char argName, double a, double b, double eps,
                                    double lambda, int &iterrNumber, std::string &iterProcess)
{
    const int MAX_ITERRATIONS_NUMBER = 5000;
    std::ostringstream stream;
    if(func(expr, argName, b) * func(expr, argName, a) > 0)
    {
        stream << "На границах указанного интервала изоляции корня значения функции имеют одниковые знаки!\n";
        stream << "Укажите такие значения границ интервала изоляции, чтобы левая часть уравнения f(X)=0 меняла знак\n";
        stream << "при изменении аргумента от одной границе к другой\n";
        iterProcess = stream.str();
        iterrNumber = 0;
        return 0.0;
    }
    double Xnext = 0.5 * (a + b), Xprev;
    bool flag = false;
    stream << "Выполняется поиск решения методом простых итерраций.\n";
    stream << "Текущее значение коэффициента λ равно: " << lambda << "\n";
    /*stream << "Для сходимости итеррационного процесса коэффициент λ должен находиться в пределах: \n";
    double lambdaLim1 = 2 / funcDer1(expr, argName, a);
    double lambdaLim2 = 2 / funcDer1(expr, argName, b);
    //stream << "0.0 < λ < " << min(lambdaLim1, lambdaLim2) << "\n";
    stream << "0.0 < λ1 < " << lambdaLim1 << "\n";
    stream << "0.0 < λ2 < " << lambdaLim2 << "\n";*/
    do
    {
        Xprev = Xnext;
        Xnext = Xprev - lambda * func(expr, argName, Xprev);
        iterrNumber++;
        stream << "------------------------------------------------------\n" ;
        stream << "Итеррация номер: " << iterrNumber << "\n";
        stream << "Значение корня на текущей итеррации:\n Xi = " << Xnext << "\n";
        stream << "Значение погрешности на текущей итеррации:\n ΔXi+1 - Xi = " << fabs(Xnext - Xprev) << "\n";
        if(fabs(Xnext - Xprev) < eps)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НАЙДЕНО : Х = " << 0.5 * (Xnext + Xprev) << "\n";
            flag = true;
        }else if(iterrNumber > MAX_ITERRATIONS_NUMBER)
        {
            stream << "------------------------------------------------------\n" ;
            stream << "РЕШЕНИЕ НЕ НАЙДЕНО!ПОРОБУЙТЕ СУЗИТЬ ИЛИ ИЗМЕНИТЬ ИНТЕРВАЛ ИЗОЛЯЦИИ КОРНЯ.\n";
            break;
        }
    }while(!flag);
    iterProcess = stream.str();
    return 0.5 * (Xnext + Xprev);
}

std::vector<double> iat::solveSystemOfNonLinearEquations(std::vector<std::string> equations, std::vector<std::pair<char, double>> variables,
                                     double eps, int &iterNumber, double &err, std:: string &iterProcess)
{
    //Эта функция решает систему нелинейных уравнений методом Ньютона
    //Максимальное число итерраций
    const int MAX_ITERATION_NUMBER = 5000;
    std::stringstream stream;
    //ofstream fo("result_SystemOfNonLinearEquations.txt");
    int eqNumber = equations.size();
    int numberOfIterrations = 0;
    std::vector<double> result;
    //result.reserve(eqNumber + 1);
    for(int i = 0; i < eqNumber; ++i)
        result.push_back(0.0);
    // Создаем вектор значений функции F1, F2 .. Fn
    std::vector<std::pair<char, double>> varList = variables;
    do
    {
        stream << "------------------------------------------------------\n" ;
        stream << "Выполняется итеррация номер : " << ++numberOfIterrations << "\n";
        // Создаем матрицу значений функции с обратным знаком
        if(numberOfIterrations > MAX_ITERATION_NUMBER)
        {
            stream << "РЕШЕНИЕ НЕ НАЙДЕНО! ПОПРОБУЙТЕ ЗАДАТЬ ДРУГОЕ НАЧАЛЬНОЕ ПРИБЛИЖЕНИЕ.\n";
            break;
        }
        Matrix<double> mtrF(eqNumber, 1, 0.0);
        // Заполняем ее
        for(int i = 0; i < eqNumber; ++i)
        {
            mtrF.setValueAt(i, 0, -multiVariableFunc(equations[i], varList));
        }
        //Создаем матрицу Якоби
        Matrix<double> mtrJacobi(eqNumber, eqNumber, 0.0);
        //Заполняем матрицу Якоби
        stream << "Элементы матрицы Якоби на текущей итеррации:\n";
        for(int i = 0; i < mtrJacobi.getRowCount(); ++i)
        {
            for(int j = 0; j < mtrJacobi.getColumnCount(); ++j)
            {
                double tmp = funcPartialDer1(equations[i], varList, varList[j].first);
                mtrJacobi.setValueAt(i, j, tmp);

                stream << "J[" << i << "][" << j << "] = " << mtrJacobi.getValueAt(i, j) << " ";
            }
          stream << "\n";
        }
        // Проверяем неравенство ее детерминанта нулю и находим приращения
        if(mtrJacobi.determinant() == 0)
            throw std::runtime_error("Zero Jacobian!");
        Matrix<double> mtrInvJacobi = mtrJacobi.inverse();
        //Создаем матрицу-столбец невязок
        Matrix<double> mtrResiduals = mtrInvJacobi * mtrF;
        stream << "Вектор-столбец невязок на текущей итеррации:\n";
        for(int i = 0; i < mtrResiduals.getRowCount(); ++i)
            stream << "Δ" << variables[i].first << "i = " <<  mtrResiduals.getValueAt(i, 0) << "\n";
        err = 0;
        // Рассчитываем среднеквадратическую невязку
        for(int i = 0; i < mtrResiduals.getRowCount(); ++i)
             err += pow(mtrResiduals.getValueAt(i, 0), 2);
        err /= eqNumber;
        err = sqrt(err);
        stream << "Среднеквадратическая невязка на текущей итеррации:" << err << "\n";
        // Рассчитываем новые значения переменных
        stream << "Значения переменных на текущей итеррации: \n";
        for(int i = 0; i < eqNumber; ++i)
        {
            varList[i].second += mtrResiduals.getValueAt(i, 0);
            result[i] = varList[i].second;
            stream <<  variables[i].first << "i = " << result[i] << "\n";
        }
        // Проверяем среднеквадратическую невязку
    }while (err > eps);
    iterNumber = numberOfIterrations;
    if(numberOfIterrations < MAX_ITERATION_NUMBER)
    {
        stream << "РЕШЕНИЕ НАЙДЕНО : \n";
        for(int i = 0; i < eqNumber; ++i)
           stream << variables[i].first << " = " << result[i] << "\n";
        stream << "Среднеквадратическая невязка : " << err << "\n";
    }
    stream << "------------------------------------------------------\n" ;
    iterProcess = stream.str();
    return result;
}
