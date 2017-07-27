#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <ctype.h>
#include <cstring>
#include <stdexcept>

namespace iat {
    enum class ParserErrorCode
    {
        INVALID_INPUT_EXPRESSION = 100,    //100"Invalid input expression"
        CLOSED_PARENTHESIS_EXPECTED = 101, //101"Expected ')'"
        DIVISION_BY_ZERO = 102,            //102"Division by zero!"
        UNKNOWN_BINARY_OPERATOR = 103,     //103"Unknown binary operator"
        ARGUMENT_OUT_OF_RANGE = 104,       //104"Argument out of range"
        UNKNOWN_UNARY_OPERATOR = 105,      //105"Unknown unary operator"
        UNKNOW_EXPRESSION_TYPE = 106,      //106"Unknown expression type"
        UNBALANCED_PARENTHESIS = 107,      //107"Unbalanced parentethises"
        UNKNOWN_ERROR = 200                //200"Unknown error"
    };

    class ErrorParser: public std::runtime_error
    {
    public:
        explicit ErrorParser(): std::runtime_error("ErrorParser"),
            _reason(ParserErrorCode::UNKNOWN_ERROR) {}
        explicit ErrorParser(ParserErrorCode code):
            std::runtime_error("ErrorParser"), _reason(code) {}
        const char*  what() const throw() override;
    private:
        ParserErrorCode _reason;
        static const std::map<ParserErrorCode, std::string> _parserErrors;
        static std::map<ParserErrorCode, std::string> createMap();
    };

    class Parser {

    public:
        explicit Parser(const std::string &inputString, std::vector<std::pair<char,double>> vars);
        explicit Parser(const std::string &inputString, std::vector<std::pair<char,double>> vars, std::string m_angleUnit);
        double calculateExpression();
    private:

        const char* m_input;
        std::string m_angleUnit;
        std::string m_inputWithVariables;
        std::vector<std::pair<char, double>> m_vctVariables;
        static const struct MyTokens
        {
            const std::vector<std::string> tokens =
            {
              "+", "-","e", "**", "*", "/","(", ")",  "<=", ">=",
              "<", ">", "==", "!=", "&", "|","^","!", "mod", "abs",
              "factorial","sign", "inv", "cbrt","sqrt", "sqr", "cube",
              "gradtorad", "radtograd", "_exp","ln", "log2", "log8",
              "log10","log16", "sin","cos","tg","ctg","secans","csecans",
              "arcsin","arccos","arctg","arcctg","arcsecans", "arccsecans",
              "sh","ch", "th", "cth","sech","csech", "arcsh","arcch",
              "arcth","arccth","arcsech","arccsech"
            };

        } m_tokens;
        struct Expression {
            Expression(std::string token) : token(token) {}
            Expression(std::string token, Expression a) : token(token), args{ a } {}
            Expression(std::string token, Expression a, Expression b) : token(token), args{ a, b } {}
            std::string token;
            std::vector<Expression> args;
        };
        std::string parseToken();
        Expression parseUnaryExpression();
        Expression parseBinaryExpression(int minPriority);
        Expression parse();
        double evaluateExpression(const Expression &e);
        unsigned long int factorial(unsigned int n);
    };
}


#endif // PARSER_H
