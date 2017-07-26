#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <ctime>
#include <cmath>
#include <complex>
#include <algorithm>
#include <functional>
#include <utility>
#include <stdexcept>

namespace iat {

    enum class MatrixErrorCode
    {
        DOT_PRODUCT_OF_BIDIMENSIONAL_MATRIXES = 100,   //100 "Operands must be row-matrixes"
        CROSS_PRODUCT_OF_BIDIMENSIONAL_MATRIXES = 101, //101 "Operands must be row-matrixes and each of them must have only three elements"
        NEGATIVE_MATRIX_SIZE_PARAMETERS = 102,         //102 "Could not create matrix with negative number of rows or columns"
        COULD_NOT_READ_MATRIX_INPUT_FILE = 103,        //103 "File with matrix data not found"
        INDEXES_OUT_OF_BOUNDS = 104,                   //104 "Indexes out of bounds"
        ROW_INDEX_OUT_OF_BOUNDS = 105,                 //105 "Row index out of bounds"
        COLUMN_INDES_OUT_OF_BOUNDS = 106,              //106 "Column index out of bounds"
        COMPLEX_NUMBER_COMPARISON = 107,               //107 "Comparison operation for complex number is not defined"
        RIGHT_HAND_SIDE_OPERAND_ROW_COUNT_ERROR = 108, //108 "Row counts of the operands are not equal
        SINGULAR_MATRIX_INVERSION = 109,               //109 "Could not inverse matrix with zero determinant"
        RECTANGULAR_MATRIX_INVERSION = 110,            //110 "Could not inverse rectangular matrix"
        RECTANGULAR_MATRIX_DETERMINANT = 111,          //111"Rectangular matrix has no determinant"
        NEGATIVE_EXPONENT = 112,                       //112std::invalid_argument("exponent");
        COULD_NOT_POWER_RECTANGULAR_MATRIX = 120,      //Could not power rectangular matrix"
        NON_QUADRATIC_MATRIX_OF_EQUATION_SYSTEM = 113, //113"Matrix of the system must be quadratic"
        SINGULAR_MATRIX_OF_EQUATION_SYSTEM = 114,      //114"Matrix rows is linearly dependent"
        NORM_OF_BIDIMENSIONAL_MATRIX = 115,            //115"Matrix must have only one row"
        NOT_EQUAL_OPERANDS_DIMENSIONS = 116,           //116"Dimensions of the operands do not coincide"
        MATRIX_MULTIPLICATION_ERROR = 117,             //117"Row count of the first matrix and column count of the second matrix are not equal"
        MATRIX_DIVISION_BY_ZERO = 118,                  //118"Division by zero"
        DIRECION_COSINES_OF_BIMENSIONAL_MATRIX = 121,  //121"Matrix must be a 3D-vector to calculate direction cosines"
        UNKNOWN_ERROR = 119
    };

    class ErrorMatrix : public std::runtime_error
    {
        public:
            explicit ErrorMatrix():
            std::runtime_error("ErrorMatrix"),
            _reason(MatrixErrorCode::UNKNOWN_ERROR) {}
            explicit ErrorMatrix(MatrixErrorCode code):
            std::runtime_error("ErrorMatrix"),
            _reason(code) {}
            const char* what() const throw() override;
        private:
            MatrixErrorCode _reason;
            static const std::map<MatrixErrorCode, std::string> _matrixErrors;
            static std::map<MatrixErrorCode, std::string> createMap();
    };

    template <typename T>
    class Matrix
    {
    public:
        Matrix();
        Matrix(const int& size, bool isEntityMatrix);// Identity or zero matrix
        Matrix(const int& size, T value);
        Matrix(const int& _rowCount, const int& _colCount, T value);   // Rectangular matrix
        Matrix(const std::vector<std::vector<T>> &vals);
        Matrix(const std::vector<std::vector<T>> &&vals);
        Matrix(const std::string& fileName);                              // Read matrix from a text file
        Matrix(const Matrix& other);                                 // Copy constructor for lvalue of other Matrix
        Matrix(Matrix &&other);                                 // Move constructor for rvalue of other Matrix
        ~Matrix();
        inline bool isQuadratic() const { return _rowCount == _colCount; }
        inline bool isRectangular() const { return _rowCount != _colCount; }
        inline bool isRowMarix() const { return _rowCount == 1; }
        inline bool isColumnMatrix() const { return _colCount == 1; }
        bool isOrthogonal();
        bool isZero();
        bool isIdentity();
        void swap(Matrix<T> &other);
        void fillByRandomNumbers(const int& leftLimit, const int& rightLimit);
        bool setValueAt(int row, int col, const T &value);
        const T& getValueAt(int row, int col) const;
        Matrix getRow(const int& row) const;    // Get row by index
        Matrix getColumn(const int& col) const; // Get column by index
        T maxElement() const;
        T minElement() const;
        int getRowCount() const; // Get number of rows
        int getColumnCount() const; // Get number of columns
        bool swapRows(const int&, const int&);
        bool swapColumns(const int&, const int&);
        bool swapElements(const int&, const int&, const int&, const int&);
        Matrix appendMatrixFromRightSide(const Matrix&) const;
        bool deleteRow(const int&);
        bool deleteColumn(const int&);
        Matrix transpose() const;
        Matrix inverse() const;
        Matrix getMinorAt(const int&, const int&) const;
        int rank() const;
        T determinant() const;
        Matrix power(int) const;
        std::vector<std::tuple<int, int, T>> find_if(std::function<bool (int, int)> foo);
        Matrix solveEquationSystem(const Matrix &rightSideVector);

        friend T dotProduct(const Matrix &first, const Matrix &second)
        {
            if(first.isRowMarix() && second.isRowMarix() &&
               first._colCount == second._colCount)
            {
                T result = static_cast<T>(0);
                for(int i = 0; i < first._colCount; ++i)
                    result += first[0][i] * second[0][i];
                return result;
            }
            else if(first.isColumnMatrix() && second.isColumnMatrix() &&
                    first._rowCount == second._rowCount)
            {
                return dotProduct(first.transpose(), second.transpose());
            }
            else
                throw ErrorMatrix(MatrixErrorCode::DOT_PRODUCT_OF_BIDIMENSIONAL_MATRIXES);
        }

        friend Matrix crossProduct(const Matrix &first, const Matrix &second)
        {
            if(first.isRowMarix() && second.isRowMarix() &&
               first._colCount == 3 && second._colCount == 3)
            {
                Matrix result(1, 3, 0);
                result[0][0] = first[0][1] * second[0][2] - first[0][2] * second[0][1];
                result[0][1] = first[0][2] * second[0][0] - first[0][0] * second[0][2];
                result[0][2] = first[0][0] * second[0][1] - first[0][1] * second[0][0];
                return result;
            }
            else if(first._rowCount == 3 && second._rowCount == 3 &&
                    first.isColumnMatrix() && second.isColumnMatrix())
            {
                return crossProduct(first.transpose(), second.transpose()).transpose();
            }
            else
                throw ErrorMatrix(MatrixErrorCode::CROSS_PRODUCT_OF_BIDIMENSIONAL_MATRIXES);
        }

        friend T scalarTripleProduct(const Matrix &first, const Matrix &second, const Matrix &third)
        {
            if(first.isRowMarix() && second.isRowMarix() &&
               third.isRowMarix() && first._colCount == 3 &&
               second._colCount == 3 && third._colCount == 3)
            {
                Matrix<T> tmp(3, 3, static_cast<T>(0));
                for(int i = 0; i < 3; ++i)
                {
                    tmp[0][i] = first[0][i];
                    tmp[1][i] = second[0][i];
                    tmp[2][i] = third[0][i];
                }
                return tmp.determinant();
            }
            if(first._rowCount == 3 && second._rowCount == 3 &&
               third._rowCount == 3 && first.isColumnMatrix() &&
               second.isColumnMatrix() && third.isColumnMatrix())
            {
                scalarTripleProduct(first.transpose(), second.transpose(), third.transpose());
            }
            else
                throw ErrorMatrix(MatrixErrorCode::CROSS_PRODUCT_OF_BIDIMENSIONAL_MATRIXES);
        }

        friend Matrix vectorTripleProduct(const Matrix &first, const Matrix &second, const Matrix &third)
        {
            return crossProduct(first, crossProduct(second, third));
        }

        friend T angleBetweenVectors(const Matrix &first, const Matrix &second)
        {
            return 180 * acos(dotProduct(first, second) / first.norm() / second.norm()) / M_PI;
        }

        T norm() const;
        Matrix directionCosines() const;
        //std::vector<std::tuple<int, int, T>> find_if(bool(*foo)(int, int));
        Matrix &operator=(const Matrix& other);
        Matrix &operator=(Matrix &&other); // Move assignment operator
        T* & operator[](const int&); // for lvalue
        const T* operator[](const int&) const; //for rvalue
        Matrix operator+(const Matrix&) const;
        Matrix operator+(const T&) const;
        Matrix operator-(const Matrix&) const;
        Matrix operator-(const T&) const;
        Matrix operator*(const Matrix&) const;
        Matrix operator*(const T&) const;
        Matrix operator/(const T&) const;
        Matrix &operator+=(const Matrix&);
        Matrix &operator+=(const T&);
        Matrix &operator-=(const Matrix&);
        Matrix &operator-=(const T&);
        Matrix &operator*=(const Matrix&);
        Matrix &operator*=(const T&);
        Matrix &operator/=(const T&);
        bool operator==(const Matrix&) const;
        bool operator!=(const Matrix& matrix) const;
    private:
        int _rowCount, _colCount;
        T ** _pValues;
        mutable bool _isDeterminantValid = false;
        mutable T _det = static_cast<T>(0);
        mutable bool _isRankValid = false;
        mutable T _rank = static_cast<T>(1);
        mutable bool _isInverseMatrixValid = false;
        bool isRowIndexValid(int row) const { return row >= 0 && row < _rowCount;}
        bool isColumnIndexValid(int col) const { return col >= 0 && col < _colCount; }
    };

    //Default constructor creates matrix of one zero element
    template <typename T>
    Matrix<T>::Matrix():Matrix(1, false)
    {}

    //This overloading of Matrix constructor creates identity or zero matrix
    template <typename T>
    Matrix<T>::Matrix(const int& size, bool isEntityMatrix):
    Matrix(size, size, 0)
    {
        if(isEntityMatrix)
        {
            for(int i = 0; i < size; ++i)
              _pValues[i][i] = 1;
        }
    }

    //This overloading of Matrix constructor creates square matrix
    template <typename T>
    Matrix<T>::Matrix(const int& size, T value):
        Matrix(size, size, value)
    {}

    //This overloading of Matrix constructor creates rectangular matrix
    template <typename T>
    Matrix<T>::Matrix(const int& rowCount, const int& colCount, T value):
        _rowCount(rowCount), _colCount(colCount)
    {
       if(_rowCount > 0 && _colCount > 0)
       {
           _pValues = new T*[_rowCount];
           for(int i = 0; i < _rowCount; ++i)
                 _pValues[i] = new T[_colCount];
           for(int i = 0; i < _rowCount; ++i)
               for(int j = 0; j < colCount; ++j)
                 _pValues[i][j] = value;
       }
       else
           throw ErrorMatrix(MatrixErrorCode::NEGATIVE_MATRIX_SIZE_PARAMETERS);
    }

    //This overloading of Matrix constructor creates matrix from bidimensional vector(lvalue)
    template <typename T>
    Matrix<T>::Matrix(const std::vector<std::vector<T>> &vals):
        Matrix(vals.size() > 0 ? vals.size(): 1,
               vals.size() > 0 ? vals[0].size(): 1,
               0)
    {
        for(int i = 0; i < int(vals.size()); ++i)
        {
            for(int j = 0; j < int(vals[i].size()); ++j)
            {
                _pValues[i][j] = vals[i][j];
            }
        }
    }

    //This overloading of Matrix constructor creates matrix from bidimensional vector(rvalue)
    template <typename T>
    Matrix<T>::Matrix(const std::vector<std::vector<T>> &&vals):
        Matrix(vals)
    {}
    //This overloading of Matrix constructor reads matrix dimensions
    //and elements from a text file
    template <typename T>
    Matrix<T>::Matrix(const std::string &fileName)
    {
      std::ifstream file;
      file.open(fileName.c_str());
      if(!file.is_open())
      {
          throw ErrorMatrix(MatrixErrorCode::COULD_NOT_READ_MATRIX_INPUT_FILE);
      }
      file >> _rowCount >> _colCount;
      if(_rowCount > 0 && _colCount > 0)
      {
          _pValues = new T*[_rowCount];
          for(int i = 0; i < _rowCount; ++i)
                _pValues[i] = new T[_colCount];
          for(int i = 0; i < _rowCount; ++i)
              for(int j = 0; j < _colCount; ++j)
              {
                  T tmp;
                  file >> tmp;
                  _pValues[i][j] = tmp;
              }
          file.close();
      }
      else
      {
          file.close();
          throw ErrorMatrix(MatrixErrorCode::NEGATIVE_MATRIX_SIZE_PARAMETERS);
      }
    }

    //Copy constructor
    template <typename T>
    Matrix<T>::Matrix(const Matrix &other):
    _rowCount(other.getRowCount()), _colCount(other.getColumnCount()),
    _isDeterminantValid(other._isDeterminantValid), _det(other._det),
    _isRankValid(other._isRankValid),_rank(other._rank),
    _isInverseMatrixValid(other._isInverseMatrixValid)
    {
        _pValues = new T*[_rowCount];
        for(int j = 0; j < _rowCount; ++j)
            _pValues[j] = new T[_colCount];
        for(int i = 0; i < _rowCount; ++i)
            for(int j = 0; j < _colCount; ++j)
                _pValues[i][j] = other[i][j];
    }

    //Move constructor
    template <typename T>
    Matrix<T>::Matrix(Matrix &&other):
    _rowCount(other._rowCount), _colCount(other._colCount),
    _pValues(std::move(other)._pValues),_isDeterminantValid(other._isDeterminantValid),
    _det(other._det), _isRankValid(other._isRankValid),
    _rank(other._rank), _isInverseMatrixValid(other._isInverseMatrixValid)
    //std::move(other)._pValues is more safe than std::move(other._pValues) as far as
    //casts other to const volatile rvalue before accessing to its members
    {
        other._pValues = nullptr;
        other._rowCount = 0;
        other._colCount = 0;
        other._isRankValid = false;
        other._isDeterminantValid = false;
        other._isInverseMatrixValid = false;
    }

    template <typename T>
    Matrix<T>::~Matrix()
    {
        for(int i = 0; i < _rowCount; ++i)
            delete[] _pValues[i];
        delete _pValues;
    }

    template <typename T>
    bool Matrix<T>::isOrthogonal()
    {
        if(isQuadratic() && this->rank() == this->getColumnCount())
        {
            return this->inverse() == this->transpose();
        }
        return false;
    }

    template <typename T>
    bool Matrix<T>::isZero()
    {
        for(int i = 0; i < _rowCount; ++i)
        {
            for(int j = 0; j < _colCount; ++j)
            {
                if(_pValues[i][j] != static_cast<T>(0))
                    return false;
            }
        }
        return true;
    }

    template <typename T>
    bool Matrix<T>::isIdentity()
    {
        for(int i = 0; i < _rowCount; ++i)
        {
            for(int j = 0; j < _colCount; ++j)
            {
                if(i == j)
                {
                    if(_pValues[i][j] != static_cast<T>(1))
                         return false;
                }
                else
                {
                    if(_pValues[i][j] != static_cast<T>(0))
                         return false;
                }
            }
        }
        return true;
    }

    template <typename T>
    void Matrix<T>::swap(Matrix<T> &other)
    {
        std::swap(*this, other);
    }

    template <typename T>
    void Matrix<T>::fillByRandomNumbers(const int& leftLimit, const int& rightLimit)
    {
       srand(time(NULL));
       for(int i = 0; i < _rowCount; ++i)
        for(int j = 0; j < _colCount; ++j)
          _pValues[i][j] = static_cast<T>(leftLimit + rand() % (rightLimit - leftLimit));
       _isRankValid = false;
       _isDeterminantValid = false;
       _isInverseMatrixValid = false;
    }

    template <typename T>
    bool Matrix<T>::setValueAt(int row, int col, const T &value)
    {
        bool result = false;
        if(isRowIndexValid(row) && isColumnIndexValid(col))
        {
            _pValues[row][col] = value;
            _isRankValid = false;
            _isDeterminantValid = false;
            result = true;
        }
        return result;
    }

    template <typename T>
    const T &Matrix<T>::getValueAt(int row, int col) const
    {
        if(isRowIndexValid(row) && isColumnIndexValid(col))
        {
            return _pValues[row][col];
        }
        else
            throw ErrorMatrix(MatrixErrorCode::INDEXES_OUT_OF_BOUNDS);
    }

    template <typename T>
    Matrix<T> Matrix<T>::getRow(const int& row) const // Получить строку по индексу
    {
       if(isRowIndexValid(row))
       {
          Matrix columnMatrix(1, _colCount, static_cast<T>(0));
          for(int j = 0; j < _colCount; ++j)
              columnMatrix[0][j] = _pValues[row][j];
          return columnMatrix;
       }
       else
          throw ErrorMatrix(MatrixErrorCode::ROW_INDEX_OUT_OF_BOUNDS);
    }

    template <typename T>
    Matrix<T> Matrix<T>::getColumn(const int& col) const // Получить строку по индексу
    {
        if(isColumnIndexValid(col))
        {
           Matrix rowMatrix(_rowCount, 1, static_cast<T>(0));
           for(int i = 0; i < _rowCount; ++i)
               rowMatrix[i][0] = _pValues[i][col];
           return rowMatrix;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::COLUMN_INDES_OUT_OF_BOUNDS);
    }

    template <typename T>
    int Matrix<T>::getRowCount() const
    {
       return _rowCount;
    }

    template <typename T>
    int Matrix<T>::getColumnCount() const
    {
       return _colCount;
    }

    template <typename T>
    bool Matrix<T>::swapRows(const int& row1, const int& row2)
    {
         bool result = false;
         if(isRowIndexValid(row1) && isRowIndexValid(row2))
         {
             if(row1 == row2)
             {
                 result = true;
             }
             else
             {
                 Matrix<T> tmp = getRow(row1);
                 for(int j = 0; j < _colCount; ++j)
                 {
                     _pValues[row1][j] = _pValues[row2][j];
                     _pValues[row2][j] = tmp[0][j];
                 }
                 _isRankValid = false;
                 _isDeterminantValid = false;
                 _isInverseMatrixValid = false;
                 result = true;
             }
         }
         return result;
    }

    template <typename T>
    bool Matrix<T>::swapColumns(const int& col1, const int& col2)
    {
        bool result = false;
        if(isColumnIndexValid(col1) && isColumnIndexValid(col2))
        {
            if(col1 == col2)
            {
                result = true;
            }
            else
            {
                Matrix<T> tmp = getColumn(col1);
                for(int i = 0; i < _rowCount; ++i)
                {
                    _pValues[i][col1] = _pValues[i][col2];
                    _pValues[i][col2] = tmp[i][0];
                }
                   _isRankValid = false;
                   _isDeterminantValid = false;
                   _isInverseMatrixValid = false;
                   result = true;
            }
        }
        return result;
    }

    template <typename T>
    bool Matrix<T>::swapElements(const int &row1, const int &col1,
                              const int &row2, const int &col2)
    {
        bool result = false;
        if(isRowIndexValid(row1) && isColumnIndexValid(col1) &&
           isRowIndexValid(row2) && isColumnIndexValid(col2))
        {
           if(row1 == row2 && col1 == col2)
               result = true;
           else
           {
               T tmp = _pValues[row1][col1];
               _pValues[row1][col1] = _pValues[row2][col2];
               _pValues[row2][col2] = tmp;
               _isRankValid = false;
               _isDeterminantValid = false;
               _isInverseMatrixValid = false;
               result = true;
           }
        }
        return result;
    }

    template <typename T>
    T Matrix<T>::maxElement() const
    {
        T maxValue = _pValues[0][0];
        for(int i = 0; i < _rowCount; ++i)
          for(int j = 0; j < _colCount; ++j)
           {
              if(_pValues[i][j] > maxValue)
                  maxValue = _pValues[i][j];
           }
        return maxValue;
    }

    template <typename T>
    T Matrix<T>::minElement() const
    {
        T minValue = _pValues[0][0];
        for(int i = 0; i < _rowCount; ++i)
          for(int j = 0; j < _colCount; ++j)
           {
              if(_pValues[i][j] < minValue)
                  minValue = _pValues[i][j];
           }
        return minValue;
    }

    template <typename T>
    Matrix<T> Matrix<T>::appendMatrixFromRightSide(const Matrix& matrix) const
    {
        if(_rowCount == matrix._rowCount)
        {
            Matrix mtr(_rowCount, _colCount + matrix._colCount, static_cast<T>(0));
            for(int i = 0; i < mtr.getRowCount(); ++i)
                for(int j = 0; j < mtr.getColumnCount(); ++j)
                {
                    if(j < _colCount)
                        mtr[i][j] = _pValues[i][j];
                    else if(j - i == _rowCount)
                        mtr[i][j] = 1.0;
                    else
                        mtr[i][j] = 0.0;
                }
            return mtr;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::RIGHT_HAND_SIDE_OPERAND_ROW_COUNT_ERROR);
    }

    template <typename T>
    bool Matrix<T>::deleteRow(const int& row)
    {
        bool result = false;
        if(isRowIndexValid(row))
        {
           for(int i = row; i < _rowCount - 1; ++i)
               for(int j = 0; j < _colCount; ++j)
                   _pValues[i][j] = _pValues[i + 1][j];
           delete[] _pValues[_rowCount - 1];
           --_rowCount;
           _isRankValid = false;
           _isDeterminantValid = false;
           _isInverseMatrixValid = false;
           result = true;
        }
        return result;
    }

    template <typename T>
    bool Matrix<T>::deleteColumn(const int& col)
    {
        bool result = false;
        if(isColumnIndexValid(col))
        {
            for(int j = col; j < _colCount - 1; ++j)
                for(int i = 0; i < _rowCount; ++i)
                    _pValues[i][j] = _pValues[i][j + 1];
            --_colCount;
            _isRankValid = false;
            _isDeterminantValid = false;
            _isInverseMatrixValid = false;
            result = true;
        }
        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::transpose() const
    {
        Matrix matrix(_colCount, _rowCount, static_cast<T>(0));
        for(int i = 0; i < matrix.getRowCount(); ++i)
            for(int j = 0; j < matrix.getColumnCount(); ++j)
                matrix[i][j] = _pValues[j][i];
        return matrix;
    }

    template <typename T>
    Matrix<T> Matrix<T>::inverse() const
    {
        static Matrix inverseMatrix(1, 1, static_cast<T>(0));
        if(_isInverseMatrixValid)
        {
            return inverseMatrix;
        }
        else
        {
            if(isQuadratic())
            {
               inverseMatrix = Matrix(_rowCount, _rowCount, static_cast<T>(0));
               T det = this->determinant();
               if (det == static_cast<T>(0))
               {
                   throw ErrorMatrix(MatrixErrorCode::SINGULAR_MATRIX_INVERSION);
               }
               for(int i = 0; i < inverseMatrix.getRowCount(); ++i)
                   for(int j = 0; j < inverseMatrix.getColumnCount(); ++j)
                       inverseMatrix[i][j] = pow(-1, i + j) * this->getMinorAt(i, j).determinant();
               inverseMatrix = inverseMatrix.transpose();
               inverseMatrix /= det;
               _isInverseMatrixValid = true;
               return inverseMatrix;
            }
            else
                throw ErrorMatrix(MatrixErrorCode::RECTANGULAR_MATRIX_INVERSION);
        }
    }

    template <typename T>
    Matrix<T> Matrix<T>::getMinorAt(const int & row, const int &col) const
    {
        Matrix mtr = *this;
        mtr.deleteRow(row);
        mtr.deleteColumn(col);
        return mtr;
    }

    template <typename T>
    int Matrix<T>::rank() const
    {
        if(_isRankValid)
        {
            return _rank;
        }
        else
        {
            Matrix tmpMatrix = *this;
            int rank = 0;
            int limit = std::min(tmpMatrix.getRowCount(), tmpMatrix.getColumnCount());
            for(int i = 0; i < limit; i++)
            {
              T kk = 0;
              if(tmpMatrix[i][i] != static_cast<T>(0))   // если первый эл-т на главной диагонали не равен нулю делим строку на него
                 kk = tmpMatrix[i][i];
              else
              {
                  /* Если первый эл-т на главной диагонали равен нулю то перебирам в цикле
                   * все элементы текущего столбца и если такой находится то складываем его строку с текущей
                   * а если такого элемента нет значит все элементи вниз по столбцу нулевые и можно переходить
                   * к следующей строке
                   */
                  bool flag = false;
                  for(int ii = i; ii < limit; ++ii)
                      if(tmpMatrix[ii][i] != static_cast<T>(0))
                      {
                         kk = tmpMatrix[ii][i];
                         for(int col = i; col < tmpMatrix.getColumnCount(); col++)
                               tmpMatrix[i][col] += tmpMatrix[ii][col];
                         flag = true;
                         break;
                      }
                  if(!flag)
                    continue;
              }
              for(int j = i; j < tmpMatrix.getColumnCount(); j++)  // Если достигли последней строки прекращаем цикл
                   tmpMatrix[i][j] /= kk;
               //Складываем текущую строку умноженную на первые эл-ты следующих строк взятые с минусом
               // со всеми строками ниже текущей и получаем в i-м столбце нули ниже главной диагонали
              for(int row = i + 1; row < limit; row++)
              {
                T coef = -tmpMatrix[row][i];
                for(int col = i; col < tmpMatrix.getColumnCount(); col++)
                      tmpMatrix[row][col] += coef * tmpMatrix[i][col];
              }
            }
            // Подсчитываем количество нелулевых строк полученной ступенчатой матрицы
            // Это и будет рангом матрицы
            for(int i = 0; i < limit; i++)
                for(int j = 0; j < tmpMatrix.getColumnCount(); j++)
                 {
                    if(tmpMatrix[i][j] != static_cast<T>(0))
                    {
                        rank++;
                        break;
                    }
                  }
            _isRankValid = true;
            _rank = rank;
            return rank;
        }
    }

    template <typename T>
    T Matrix<T>::determinant() const
    {
       if(_isDeterminantValid)
       {
           //cout << "Вернули кеш" << endl;
           return _det;
       }
       else
       {
           T newDet;
           if(isQuadratic() && _pValues != nullptr)
           {
               if(_rowCount == 1)
               {
                   newDet = _pValues[0][0];
               }
               else  if(_rowCount == 2)
               {
                   newDet = _pValues[0][0] * _pValues[1][1] -
                           _pValues[0][1] * _pValues[1][0];
               }
               else
               {
                  Matrix tmpMatrix = *this;
                  T det = 0;
                  for(int i = 0; i < _rowCount; ++i)
                      det += pow(-1, i) * tmpMatrix[0][i] * tmpMatrix.getMinorAt(0, i).determinant();
                  newDet = det;
               }
           }
           else
           {
                throw ErrorMatrix(MatrixErrorCode::RECTANGULAR_MATRIX_DETERMINANT);
           }
           _isDeterminantValid = true;
           _det = newDet;
           return newDet;
       }
    }

    template <typename T>
    Matrix<T> Matrix<T>::power(int exponent) const
    {
        if(exponent < 0)
        {
            throw ErrorMatrix(MatrixErrorCode::NEGATIVE_EXPONENT);
        }
        if (isQuadratic())
        {
            Matrix matrix = *this;
            Matrix power(matrix.getRowCount(), true);
            if(exponent == 0) return power; //Типа возведение в нулевую степень возвращает единичную матрицу
            for(int i = 1; i <= exponent; ++i)
                power *= matrix;
            return power;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::COULD_NOT_POWER_RECTANGULAR_MATRIX);
    }

    template <typename T>
    std::vector<std::tuple<int, int, T>> Matrix<T>::find_if(std::function<bool (int, int)> foo)
    {
        std::vector<std::tuple<int, int, T>> res;
        for(int i = 0; i < _rowCount; ++i)
            for(int j = 0; j < _colCount; ++j)
            {
                if(foo(i, j))
                {
                    res.push_back(std::make_tuple(i,j,_pValues[i][j]));
                }
            }
        return res;
    }

    template <typename T>
    Matrix<T> Matrix<T>::solveEquationSystem(const Matrix &rightSideVector)
    {
        if(!this->isQuadratic())
            throw ErrorMatrix(MatrixErrorCode::NON_QUADRATIC_MATRIX_OF_EQUATION_SYSTEM);
        if(this->rank() < _rowCount)
            throw ErrorMatrix(MatrixErrorCode::SINGULAR_MATRIX_OF_EQUATION_SYSTEM);
        return this->inverse() * rightSideVector;
    }

    template <typename T>
    T Matrix<T>::norm() const
    {
        if(_rowCount == 1)
        {
            T result = static_cast<T>(0);
            for(int i = 0; i < _colCount; ++i)
            {
                result += pow(_pValues[0][i],2);
            }
            return sqrt(result);
        }
        else if(_colCount == 1)
        {
            return this->transpose().norm();
        }
        else
            throw ErrorMatrix(MatrixErrorCode::NORM_OF_BIDIMENSIONAL_MATRIX);
    }

    template <typename T>
    Matrix<T> Matrix<T>::directionCosines() const
    {
        if(_rowCount == 1)
        {
            Matrix result(1, _colCount, 0);
            T modulus = this->norm();
            for(int i = 0; i < _colCount; ++i)
                result[0][i] = _pValues[0][i] / modulus;
            return result;
        }
        else if(_colCount == 1)
        {
            return this->transpose().directionCosines();
        }
        else
            throw ErrorMatrix(MatrixErrorCode::DIRECION_COSINES_OF_BIMENSIONAL_MATRIX);
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator=(const Matrix& other)
    {
        if (this == &other) return *this;
        //стр. 67 Скот Мейерс - 55 способов улучщить стуктуру и код ваших программ
        // хотя он не рекомендует ставить сторожевое условие вначале
        T** pOrigValues = _pValues;
        _pValues = new T*[other.getRowCount()];
        for(int j = 0; j < other.getColumnCount(); ++j)
            _pValues[j] = new T[other.getColumnCount()];

        for(int i = 0; i < other.getRowCount(); ++i)
            for(int j = 0; j < other.getColumnCount(); ++j)
            {
                _pValues[i][j] = other[i][j];
            }

        for(int i = 0; i < _rowCount; ++i)
            delete[] pOrigValues[i];
        delete pOrigValues;

        _rowCount = other.getRowCount();
        _colCount = other.getColumnCount();
        _isRankValid = other._isRankValid;
        _isDeterminantValid = other._isDeterminantValid;
        _isInverseMatrixValid = other._isInverseMatrixValid;

        return *this;
    }

    //Move assignment operator
    template <typename T>
    Matrix<T>& Matrix<T>::operator=(Matrix &&other)
    {
        if (this == &other) return *this;
        /*------------------------------*/
        // release the current object’s resources
        for(int i = 0; i < _rowCount; ++i)
            delete[] _pValues[i];
        delete _pValues;
        /*-----------------------------*/
        // pilfer other’s resource
        _rowCount = other._rowCount;
        _colCount = other._colCount;
        _pValues = std::move(other)._pValues;
        _isRankValid = other._isRankValid;
        _isDeterminantValid = other._isDeterminantValid;
        _isInverseMatrixValid = other._isInverseMatrixValid;

        // reset other
        other._rowCount = 0;
        other._colCount = 0;
        other._pValues = nullptr;
        other._isRankValid = false;
        other._rank = 1;
        other._isDeterminantValid = false;
        other._det = 0;
        other._isInverseMatrixValid = false;

        return *this;
    }

    template <typename T>
    Matrix<T> Matrix<T>::operator+(const Matrix& matrix) const
    {
        if(_rowCount == matrix.getRowCount() && _colCount == matrix.getColumnCount())
        {
            Matrix result(_rowCount, _colCount, static_cast<T>(0));
            for(int i = 0; i < _rowCount; ++i)
              for(int j = 0; j < _colCount; ++j)
                  result[i][j] = _pValues[i][j] + matrix[i][j];
            return result;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::NOT_EQUAL_OPERANDS_DIMENSIONS);
    }

    template <typename T>
    Matrix<T> Matrix<T>::operator+(const T& increment) const
    {
        Matrix result(_rowCount, _colCount, static_cast<T>(0));
        for(int i = 0; i < _rowCount; ++i)
          for(int j = 0; j < _colCount; ++j)
             result[i][j] = _pValues[i][j] + increment;
        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::operator-(const Matrix& matrix) const
    {
        if(_rowCount == matrix.getRowCount() && _colCount == matrix.getColumnCount())
        {
            Matrix result(_rowCount, _colCount, static_cast<T>(0));
            for(int i = 0; i < _rowCount; ++i)
              for(int j = 0; j < _colCount; ++j)
                 result[i][j] = _pValues[i][j] - matrix[i][j];
            return result;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::NOT_EQUAL_OPERANDS_DIMENSIONS);
    }

    template <typename T>
    Matrix<T> Matrix<T>::operator-(const T& decrement) const
    {
        Matrix result(_rowCount, _colCount, static_cast<T>(0));
        for(int i = 0; i < _rowCount; ++i)
          for(int j = 0; j < _colCount; ++j)
             result[i][j] = _pValues[i][j] - decrement;
        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>:: operator*(const Matrix& matrix) const
    {
        if(this->_colCount == matrix._rowCount)
        {
            Matrix<T> result(_rowCount, matrix.getColumnCount(), static_cast<T>(0));
            T sum = static_cast<T>(0);
            for(int i = 0; i < _rowCount; ++i)             // Перебор по строкам первой матрицы
                for(int j = 0; j < matrix.getColumnCount(); ++j)  // Перебор по столбцам второй матрицы
                {
                    sum = static_cast<T>(0);
                    for(int k = 0; k < _colCount; ++k)
                        sum += _pValues[i][k] * matrix[k][j]; // Накопление произведений елементов i-й строки 1-й матрицы и j-го столбца 2-й матрицы
                    result[i][j] = sum;
                }
           return result;
        }
        else
                        throw ErrorMatrix(MatrixErrorCode::MATRIX_MULTIPLICATION_ERROR);
    }

    template <typename T>
    Matrix<T> Matrix<T>:: operator*(const T& scale) const
    {
        Matrix result(_rowCount, _colCount, static_cast<T>(0));
        for(int i = 0; i < _rowCount; ++i)
          for(int j = 0; j < _colCount; ++j)
             result[i][j] = _pValues[i][j] * scale;
        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>:: operator/(const T& scale) const
    {
        if(scale != static_cast<T>(0))
        {
            Matrix<T> result(_rowCount, _colCount, static_cast<T>(0));
            for(int i = 0; i < _rowCount; ++i)
              for(int j = 0; j < _colCount; ++j)
                 result[i][j] = _pValues[i][j] / scale;
            return result;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::MATRIX_DIVISION_BY_ZERO);
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator+=(const Matrix& matrix)
    {
        if(_rowCount == matrix.getRowCount() &&
           _colCount == matrix.getColumnCount())
        {
            *this = (*this) + matrix;
            _isRankValid = false;
            _isDeterminantValid = false;
            _isInverseMatrixValid = false;
            return *this;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::NOT_EQUAL_OPERANDS_DIMENSIONS);
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator+=(const T& increment)
    {
         *this = (*this) + increment;
        _isRankValid = false;
        _isDeterminantValid = false;
        _isInverseMatrixValid = false;
        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>:: operator-=(const Matrix& matrix)
    {
        if(_rowCount == matrix.getRowCount() &&
           _colCount == matrix.getColumnCount())
        {
            *this = (*this) - matrix;
            _isRankValid = false;
            _isDeterminantValid = false;
            _isInverseMatrixValid = false;
            return *this;
        }
        else
           throw ErrorMatrix(MatrixErrorCode::NOT_EQUAL_OPERANDS_DIMENSIONS);
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator-=(const T& decrement)
    {
         *this = (*this) - decrement;
        _isRankValid = false;
        _isDeterminantValid = false;
        _isInverseMatrixValid = false;
        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator*=(const Matrix& matrix)
    {
        if(_colCount == matrix.getRowCount())
        {
            *this = matrix * (*this);
            _isRankValid = false;
            _isDeterminantValid = false;
            _isInverseMatrixValid = false;
            return *this;
        }
        else
           throw ErrorMatrix(MatrixErrorCode::MATRIX_MULTIPLICATION_ERROR);
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator*=(const T& scale)
    {
        *this = (*this) * scale;
        _isRankValid = false;
        _isDeterminantValid = false;
        _isInverseMatrixValid = false;
        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator/=(const T& scale)
    {
        if(scale != static_cast<T>(0))
        {
            *this = (*this) / scale;
            _isRankValid = false;
            _isDeterminantValid = false;
            _isInverseMatrixValid = false;
            return *this;
        }
        else
            throw ErrorMatrix(MatrixErrorCode::MATRIX_DIVISION_BY_ZERO);
    }

    template <typename T>
    Matrix<T>& operator++(Matrix<T> &mtr)
    {
        return mtr += static_cast<T>(1);
    }

    template <typename T>
    Matrix<T> operator++(Matrix<T> &mtr, int)
    {
        Matrix<T> old = mtr;
        mtr += static_cast<T>(1);
        return old;
    }

    template <typename T>
    Matrix<T>& operator--(Matrix<T> &mtr)
    {
        return mtr -= static_cast<T>(1);
    }

    template <typename T>
    Matrix<T> operator--(Matrix<T> &mtr, int)
    {
        Matrix<T> old = mtr;
        mtr -= static_cast<T>(1);
        return old;
    }

    template <typename T>
    bool Matrix<T>::operator==(const Matrix<T>& matrix) const
    {
        if(_rowCount == matrix.getRowCount() && _colCount == matrix.getColumnCount())
        {
           for(int i = 0; i < _rowCount; ++i)
               for(int j = 0; j < _colCount; ++j)
               {
                   if(_pValues[i][j] != matrix[i][j])
                       return false;
               }
           return true;
        } else
           return false;
    }

    template <typename T>
    bool Matrix<T>::operator!=(const Matrix<T>& matrix) const
    {
        if(_rowCount == matrix.getRowCount() && _colCount == matrix.getColumnCount())
        {
           for(int i = 0; i < _rowCount; ++i)
               for(int j = 0; j < _colCount; ++j)
               {
                   if(_pValues[i][j] != matrix[i][j])
                       return true;
               }
           return false;
        }
        else
           return false;
    }

    //This overloading is for lvalue
    template <typename T>
    T* &Matrix<T>::operator[](const int& row)
    {
        if(isRowIndexValid(row))
        {
            _isRankValid = false;
            _isDeterminantValid = false;
            _isInverseMatrixValid = false;
            return _pValues[row];
        }
        else
            throw ErrorMatrix(MatrixErrorCode::ROW_INDEX_OUT_OF_BOUNDS);
    }

    //This overloading is for rvalue
    template <typename T>
    const T* Matrix<T>::operator[](const int& row) const
    {
        if(isRowIndexValid(row))
            return _pValues[row];
        else
            throw ErrorMatrix(MatrixErrorCode::ROW_INDEX_OUT_OF_BOUNDS);
    }

    template <typename T>
    std::istream& operator>>(std::istream &stream, Matrix<T> &matrix)
    {
        for(int i = 0; i < matrix.getRowCount(); ++i)
            for(int j = 0; j < matrix.getColumnCount(); ++j)
            {
                T tmp;
                stream >> tmp;
                matrix[i][j] = tmp;
            }
        return stream;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream &stream, Matrix<T> &matrix)
    {
        std::ostringstream oss;
        oss << matrix;
        stream << oss.str();
        return stream;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream &stream, Matrix<T> &&matrix)
    {
        stream << matrix;
        return stream;
    }

    template <typename T>
    std::ifstream& operator>>(std::ifstream &stream, Matrix<T> &matrix)
    {
        for(int i = 0; i < matrix.getRowCount(); ++i)
            for(int j = 0; j < matrix.getColumnCount(); ++j)
            {
                T tmp;
                stream >> tmp;
                matrix[i][j] = tmp;
            }
        return stream;
    }

    template <typename T>
    std::ofstream& operator<< (std::ofstream &stream, Matrix<T> &matrix)
    {
        std::ostringstream oss;
        oss << matrix;
        stream << oss.str();
        return stream;
    }

    template <typename T>
    std::ofstream& operator<<(std::ofstream &stream, Matrix<T> &&matrix)
    {
        stream << matrix;
        return stream;
    }

    template <typename T>
    std::istringstream& operator>>(std::istringstream &isstream, Matrix<T> &matrix)
    {
        for(int i = 0; i < matrix.getRowCount(); ++i)
            for(int j = 0; j < matrix.getColumnCount(); ++j)
            {
                T tmp;
                isstream >> tmp;
                matrix[i][j] = tmp;
            }
        return isstream;
    }

    //for lvalue
    template <typename T>
    std::ostringstream& operator<<(std::ostringstream &sstream, Matrix<T> &matrix)
    {
        for(int i = 0; i < matrix.getRowCount(); ++i)
        {
            for(int j = 0; j < matrix.getColumnCount(); ++j)
                sstream << matrix[i][j] << " ";
            sstream << std::endl;
        }
        return sstream;
    }

    //for rvalue
    template <typename T>
    std::ostringstream& operator<<(std::ostringstream &sstream, Matrix<T> &&matrix)
    {
        sstream << matrix;
        return sstream;
    }
}

#endif // MATRIX_H
