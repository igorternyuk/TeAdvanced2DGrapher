#ifndef MATEMATICA_H
#define MATEMATICA_H

#include <vector>

#include <complex.h>
#include "polynom.h"

namespace iat {
    class CubicSplineInterpolator
    {
    public:
        explicit CubicSplineInterpolator();
        void interpolate(std::vector<std::pair<double, double>> pointsTable);
        double calculate(double x);
        std::vector<double> allCoef();
    private:
        struct CubicSpline
        {
            double a, b, c, d, x;
        };
        std::vector<CubicSpline> splineTuple;
        unsigned int nodeNumber;
    };

    class LeastSquareMethod
    {
    public:
        explicit LeastSquareMethod(std::vector<std::pair<double, double>> xy);
        explicit LeastSquareMethod(std::vector<double> x, std::vector<double> y);
        void linearApproximation(double &a, double &b, double &error);
        void hyperbolicApproximation(double &a, double &b, double &error);
        void logarithmicApproximation(double &a, double &b, double &error);
        void exponentialApproximation(double &a, double &b, double &error);
        void quadraticApproximation(double &a, double &b, double &c, double &error);
        std::vector<double> polynomialLeastSquareMethod(double polynomDegree, double &error);
    private:
        int pointsNumber;
        std::vector<std::pair<double, double>> xy;
    };

    double factorial(double n);
    int signum(double value);

    //Integration

    double integralTablLeftHandRect(std::vector<std::pair<double, double>> integrandTable);
    double integralTablRightHandRect(std::vector<std::pair<double, double>> integrandTable);
    double integralTablTrapezoids(std::vector<std::pair<double, double>> integrandTable);
    double integralTablSimpson(std::vector<std::pair<double, double>> integrandTable);

    //Lagrange polynomial

    double integralTablPolLagr(std::vector<std::pair<double, double>> integrandTable,
                               double a, double b);
    double polynomLagrange(std::vector<std::pair<double, double>> pointsTable, double value);
    Polynom<double> polynomLagrange(std::vector<std::pair<double, double>> pointsTable);
    std::vector<double> polynomLagrangeVector(std::vector<std::pair<double, double>> pointsTable);
    std::string polynomLagrangeString(std::vector<std::pair<double, double>> pointsTable, int prec);

    //Derivatives

    double tablFuncLeftDer1(std::vector<std::pair<double, double>> pointsTable, double value);
    double tablFuncRightDer1(std::vector<std::pair<double, double>> pointsTable, double value);
    double tablFuncDer1(std::vector<std::pair<double, double>> pointsTable, double value);
    double tablFuncDer2(std::vector<std::pair<double, double>> pointsTable, double value);
    std::vector<std::complex<double>> solveSquareEquation(double a, double b, double c);
    std::vector<std::complex<double>> solveCubicEquation(double a, double b, double c, double d);

    /////With parser///////////////////////

    //Derivatives

    double func(std::string expr, char argName, double argValue);
    double funcLeftDer1(std::string expr, char argName, double argValue);
    double funcRightDer1(std::string expr, char argName, double argValue);
    double funcDer1(std::string expr, char argName, double argValue);
    double funcDer2(std::string expr, char argName, double argValue);

    //Multivariable functions

    double multiVariableFunc(std::string expr, std::vector<std::pair<char, double>> varList);
    double funcPartialDer1(std::string expr, std::vector<std::pair<char, double>> variables, char argName);

    //Integrals

    double integralLeftHandRect(std::string expr, char argName, double leftLim, double rightLim,
                                double eps, int &iterrNumber, std::string &iterProcess);
    double integralRightHandRect(std::string expr, char argName, double leftLim, double rightLim,
                                 double eps, int &iterrNumber, std::string &iterProcess);
    double integralMidpoint(std::string expr, char argName, double leftLim, double rightLim,
                            double eps, int &iterrNumber, std::string &iterProcess);
    double integralTrapezoids(std::string expr, char argName, double leftLim, double rightLim,
                              double eps, int &iterrNumber, std::string &iterProcess);
    double integralSimpson(std::string expr, char argName, double leftLim, double rightLim,
                           double eps, int &iterrNumber, std::string &iterProcess);

    //Equations

    double solveEquationChordsMethod(std::string expr, char argName, double a, double b, double eps, int &iterrNumber, std::string &iterProcess);
    double solveEquationTangentsMethod(std::string expr, char argName, double a, double b, double eps, int &iterrNumber, std::string &iterProcess);
    double solveEquationBisectionMethod(std::string expr, char argName, double a, double b, double eps, int &iterrNumber, std::string &iterProcess);
    double solveEquationCombinedMethod(std::string expr, char argName, double a, double b,
                                         double eps, int &iterrNumber, std::string &iterProcess);
    double solveEquationIterationMethod(std::string expr, char argName, double a, double b,
                                         double eps, double lambda, int &iterrNumber, std::string &iterProcess);
    std::vector<double> solveSystemOfNonLinearEquations(std::vector<std::string> equations, std::vector<std::pair<char, double>> variables,
                                         double eps, int & iterNumber, double &err, std::string &iterProcess);
}

#endif // MATEMATICA_H
