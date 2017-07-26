#include "matrix.h"
#include <sstream>

const std::map<iat::MatrixErrorCode, std::string> iat::ErrorMatrix::_matrixErrors =
        iat::ErrorMatrix::createMap();
const char* iat::ErrorMatrix::what() const throw()
{
    std::ostringstream oss;
    oss << std::runtime_error::what() << " (code:" << static_cast<int>(_reason) << "): "
    << _matrixErrors.at(_reason) << std::endl;
    return oss.str().c_str();
}

std::map<iat::MatrixErrorCode, std::string> iat::ErrorMatrix::createMap()
{
    std::map<MatrixErrorCode, std::string> map = {
        {
            MatrixErrorCode::DOT_PRODUCT_OF_BIDIMENSIONAL_MATRIXES,
            "Operands must be row-matrixes"
        },
        {
            MatrixErrorCode::CROSS_PRODUCT_OF_BIDIMENSIONAL_MATRIXES,
            "Operands must be row-matrixes and each of them must have only three elements"
        },
        {
            MatrixErrorCode::NEGATIVE_MATRIX_SIZE_PARAMETERS,
            "Could not create matrix with negative number of rows or columns"
        },
        {
            MatrixErrorCode::COULD_NOT_READ_MATRIX_INPUT_FILE,
            "File with matrix data not found"
        },
        {
            MatrixErrorCode::INDEXES_OUT_OF_BOUNDS,
            "Indexes out of bounds"
        },
        {
            MatrixErrorCode::ROW_INDEX_OUT_OF_BOUNDS,
            "Row index out of bounds"
        },
        {
            MatrixErrorCode::COLUMN_INDES_OUT_OF_BOUNDS,
            "Column index out of bounds"
        },
        {
            MatrixErrorCode::COMPLEX_NUMBER_COMPARISON,
            "Comparison operation for complex number is not defined"
        },
        {
            MatrixErrorCode::RIGHT_HAND_SIDE_OPERAND_ROW_COUNT_ERROR,
            "Row counts of the operands are not equal"
        },
        {
            MatrixErrorCode::SINGULAR_MATRIX_INVERSION,
            "Could not inverse matrix with zero determinant"
        },
        {
            MatrixErrorCode::RECTANGULAR_MATRIX_INVERSION,
            "Could not inverse rectangular matrix"
        },
        {
            MatrixErrorCode::RECTANGULAR_MATRIX_DETERMINANT,
            "Rectangular matrix has no determinant"
        },
        {
            MatrixErrorCode::NEGATIVE_EXPONENT,
            "Exponent must be equal or greater tham zero"
        },
        {
            MatrixErrorCode::NON_QUADRATIC_MATRIX_OF_EQUATION_SYSTEM,
            "Matrix of the system must be quadratic"
        },
        {
            MatrixErrorCode::SINGULAR_MATRIX_OF_EQUATION_SYSTEM,
            "Matrix rows is linearly dependent"
        },
        {
            MatrixErrorCode::NORM_OF_BIDIMENSIONAL_MATRIX,
            "Matrix must have only one row"
        },
        {
            MatrixErrorCode::NOT_EQUAL_OPERANDS_DIMENSIONS,
            "Dimensions of the operands do not coincide"
        },
        {
            MatrixErrorCode::MATRIX_MULTIPLICATION_ERROR,
            "Row count of the first matrix and column count of"
            " the second matrix are not equal"
        },
        {
            MatrixErrorCode::COULD_NOT_POWER_RECTANGULAR_MATRIX,
            "Could not power rectangular matrix"
        },
        {
            MatrixErrorCode::DIRECION_COSINES_OF_BIMENSIONAL_MATRIX,
            "Matrix must be a 3D-vector to calculate direction cosines"
        },
        { MatrixErrorCode::MATRIX_DIVISION_BY_ZERO, "Division by zero"},
        { MatrixErrorCode::UNKNOWN_ERROR, "Unknown error" }
    };
    return map;
}
