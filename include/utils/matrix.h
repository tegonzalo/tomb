/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* matrix.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 15/02/2012
*/

#ifndef __MATRIX_H
#define __MATRIX_H

#include <cmath>
#include <iostream>
#include <sstream>
#include <complex>
#include "libjson.h"

/*****************************/
/* Class matrix declarations */
/*****************************/

namespace Tomb
{

  template <class TYPE> class CVector;
  template <class TYPE> class RVector;

  template <class TYPE> class Matrix {

    protected:
      TYPE **_M;
      int _rows = 0, _cols = 0;
      bool _square = false;
      bool _diag = false;

    public:
      Matrix();
      Matrix(int);
      Matrix(int,int);
      Matrix(const std::string);
      Matrix(const Matrix&);
      Matrix(Matrix&&);
      ~Matrix();
      Matrix& operator=(const Matrix&);
      Matrix& operator=(Matrix&&);
      Matrix& operator=(const TYPE&);
      
      int rows() const;
      int cols() const;
      bool square() const;
      bool diag() const;
      TYPE *operator[](int) const;
      Matrix& operator+=(const Matrix&);
      const Matrix operator+(const Matrix&)const;
      Matrix& operator-=(const Matrix&);
      const Matrix operator-(const Matrix&) const;
      Matrix& operator*=(const Matrix&);
      const Matrix operator*(const Matrix&) const;
      const CVector<TYPE> operator*(const CVector<TYPE>&) const;
      const Matrix operator*(const RVector<TYPE>&) const;
      Matrix& operator*=(const TYPE&);
      const Matrix operator*(const TYPE&) const;
      Matrix& operator/=(const TYPE&);
      const Matrix operator/(const TYPE&) const;
      bool operator==(const Matrix&) const;
      bool operator==(const TYPE &) const;
      bool operator!=(const Matrix&) const;
      bool operator!=(const TYPE &) const;
      bool isLowerTriangular() const;
      bool isUpperTriangular() const;
      Matrix Adjoint(int,int) const;
      TYPE det() const;
      Matrix Transpose() const;
      void LUDecomposition(Matrix&, Matrix&) const;
      Matrix Inverse() const;
      Matrix PseudoInverse() const;
      Matrix TransposeConjugate() const;
      Matrix Conjugate() const;
      RVector<TYPE> Row(int) const;
      CVector<TYPE> Column(int) const;
      void SetRow(int, RVector<TYPE>);
      void SetColumn(int, CVector<TYPE>);
      void ChangeBasis(int, int);
      void Swap(int, int, int, int);
      void SwapRows(int, int);
      void SwapColumns(int, int);
      void MoveRow(int, int);
      void MoveRows(int, int, int);
      void MoveColumn(int, int);
      void MoveColumns(int, int, int);
      Matrix ExtractMatrix(int, int) const;
      Matrix ExtractMatrix(int, int, int, int) const;
      void DeleteRow(int);
      void DeleteColumn(int);
      void InsertRow(int, RVector<TYPE>);
      void InsertColumn(int, CVector<TYPE>);
      void Append(Matrix,std::string);
      void Append(Matrix);
      std::string Print() const;
      std::string Print(int) const;
      std::string Table() const;
      void FixZeros();
      static Matrix<TYPE> Identity(int);
      Matrix Abs() const;
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &);
  };

  template <class TYPE> const Matrix<TYPE> &operator*(const Matrix<TYPE> &, const TYPE &);
  template <class TYPE> const Matrix<TYPE> &operator*(const TYPE &, const  Matrix<TYPE> &);
  template <class TYPE> std::ostream &operator<<(std::ostream &, const Matrix<TYPE>);
}

/******************************/
/* Class CVector declarations */
/******************************/

namespace Tomb
{
  template <class TYPE> class CVector: public Matrix<TYPE> {
    public:
      CVector();
      CVector(int);
      CVector(const CVector&);
      CVector(CVector&&);
      CVector(const Matrix<TYPE>&);
      CVector(Matrix<TYPE>&&);
      ~CVector();
      CVector& operator=(const CVector&);
      CVector& operator=(CVector&&);
      
      TYPE& operator[](int) const;
      CVector& operator+=(const CVector&);
      const CVector operator+(const CVector&) const;
      CVector& operator-=(const CVector&);
      const CVector operator-(const CVector&) const;
      const Matrix<TYPE> operator*(const RVector<TYPE>&) const;
      const Matrix<TYPE> operator*(const Matrix<TYPE>&) const;
      CVector& operator*=(const TYPE&);
      const CVector operator*(const TYPE&) const;
      CVector& operator/=(const TYPE&);
      const CVector operator/(const TYPE&) const;
      bool operator==(const CVector&) const;
      bool operator==(const TYPE &n) const;
      bool operator!=(const CVector&) const;
      bool operator!=(const TYPE &n) const;
      RVector<TYPE> Transpose();
      void Append(CVector);
  };

  template <class TYPE> const CVector<TYPE> &operator*(const CVector<TYPE> &, const TYPE &);
  template <class TYPE> const CVector<TYPE> &operator*(const TYPE &, const  CVector<TYPE> &);
}

/******************************/
/* Class RVector declarations */
/******************************/

namespace Tomb
{
  template <class TYPE> class RVector: public Matrix<TYPE> {
    public:
      RVector();
      RVector(int);
      RVector(const RVector&);
      RVector(RVector&&);
      RVector(const Matrix<TYPE>&);
      RVector(Matrix<TYPE>&&);
      ~RVector();
      RVector& operator=(const RVector&);
      RVector& operator=(RVector&&);
      
      TYPE& operator[](int) const;
      RVector& operator+=(const RVector&);
      const RVector operator+(const RVector&) const;
      RVector& operator-=(const RVector&);
      const RVector operator-(const RVector&) const;
      const TYPE operator*(const CVector<TYPE>) const;
      const RVector operator*(const Matrix<TYPE>&) const;
      RVector& operator*=(const TYPE&);
      const RVector operator*(const TYPE&) const;
      RVector& operator/=(const TYPE&);
      const RVector operator/(const TYPE&) const;
      bool operator==(const RVector&) const;
      bool operator==(const TYPE &n) const;
      bool operator!=(const RVector&) const;
      bool operator!=(const TYPE &n) const;
      CVector<TYPE> Transpose();
      void Append(RVector);
      void Append(TYPE);
      std::string Table() const;
  };

  template <class TYPE> const RVector<TYPE> &operator*(const RVector<TYPE> &, const TYPE &);
  template <class TYPE> const RVector<TYPE> &operator*(const TYPE &, const  RVector<TYPE> &);
}

/****************************/
/* Class Matrix definitions */
/****************************/

namespace Tomb
{
  /* Constructor, empty */
  template <class TYPE> Matrix<TYPE>::Matrix() {
    //_M = new TYPE*;
    _M = nullptr;
    _rows = 0;
    _cols = 0;
    _square = false;
    _diag = false;
  }

  /* Constructor for square nxn matrices */
  template <class TYPE> Matrix<TYPE>::Matrix(int n){

    if(n < 0) {
      throw "Matrix::Matrix::Dimension of the matrix must be a positive integer";
    }

    _rows = n;
    _cols = n;

    try {
      _M = new TYPE*[_rows];
      for(int i=0; i<_rows; i++) {
        _M[i] = new TYPE[_cols];
        for(int j=0;j<_cols;j++) {
          _M[i][j] = 0;
        }
      }
    } catch (...) {
      throw;
    }
    _square = true;
    _diag = true;
  }

  /* Constructor for nxm matrices */
  template <class TYPE> Matrix<TYPE>::Matrix(int n, int m){


    if(n < 0 or m < 0) {
      throw "Matrix::Matrix::Dimension of the matrix must be a positive integer";
    }
    _rows = n;
    _cols = m;

    try {
      _M = new TYPE*[_rows];
      for(int i=0; i<_rows; i++) {
        _M[i] = new TYPE[_cols];
        for(int j=0;j<_cols;j++) {
          _M[i][j] = 0;
        }
      }
    } catch (...) {
      throw;
    }
    if(n==m) {
      _square = true;
    } else {
      _square = false;
    }
    _diag = true;

  }

  /* Constructor from a json string */
  template <class TYPE> Matrix<TYPE>::Matrix(const std::string id) {
    try {
      _rows = 0;
      _cols = 0;
      
      JSONNode json = libjson::parse(id);
      ParseJSON(json);
      
    } catch (...) {
      throw;
    }
  }

  /* Copy constructor */
  template <class TYPE> Matrix<TYPE>::Matrix(const Matrix &M) {

    _rows = M.rows();
    _cols = M.cols();

    try {
      _M = new TYPE*[_rows];
      for(int i=0; i<_rows; i++) {
        _M[i] = new TYPE[_cols];
        for(int j=0;j<_cols;j++) {
          _M[i][j] = M[i][j];
        }
      }
    } catch (...) {
      throw;
    }
    if(M.square()) {
      _square = true;
    } else {
      _square = false;
    }
    _diag = diag();

  }

  /* Move constructor */
  template <class TYPE> Matrix<TYPE>::Matrix(Matrix &&M) :
    _rows(std::move(M._rows)),
    _cols(std::move(M._cols)),
    _M(std::move(M._M)),
    _square(std::move(M._square)),
    _diag(std::move(M._diag))
  {

    // Copy constructor part
    /*_rows = M.rows();
    _cols = M.cols();

    try {
      _M = new TYPE*[_rows];
      for(int i=0; i<_rows; i++) {
        _M[i] = new TYPE[_cols];
        for(int j=0;j<_cols;j++) {
          _M[i][j] = M[i][j];
        }
      }
    } catch (...) {
      throw;
    }
    if(M.square()) {
      _square = true;
    } else {
      _square = false;
    }
    _diag = diag();*/
    
    // Set M to default
    M._M = nullptr;
    M._rows = 0;
    M._cols = 0;
    M._square = false;
    M._diag = false;

  }

  /* Destructor */
  template <class TYPE> Matrix<TYPE>::~Matrix() {
    
    if(_M != nullptr )
    {
      for(int i=0; i<_rows; i++) 
        delete [] _M[i];
      delete [] _M;
    }
    _rows = 0;
    _cols = 0;
    _square = false;
    _diag = false;
  }

  /* Get rows */
  template <class TYPE> int Matrix<TYPE>::rows() const{
    return _rows;
  }

  /* Get cols */
  template <class TYPE> int Matrix<TYPE>::cols() const{
    return _cols;
  }

  /* Evaluates if the matrix is square */
  template <class TYPE> bool Matrix<TYPE>::square() const{
    return _square;
  }

  /* Evaluates if the matrix is diagonal */
  template <class TYPE> bool Matrix<TYPE>::diag() const {
    for(int i=0; i<_rows; i++) {
      for(int j=0; j<_cols; j++) {
        if(i!=j) {
          if(_M[i][j] != 0) {
            return false;
          }
        }
      }
    }
    return true;
  }

  /* Overloaded [] operator */
  template <class TYPE> TYPE *Matrix<TYPE>::operator[] (int i) const {
    return _M[i];
  }

  /* Overloaded = operator with matrices */
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator=(const Matrix &N){

    if(this != &N) {
      
      if(this->rows()) {
        this->~Matrix();
      }
      _rows = N.rows();
      _cols = N.cols();
      
      try {
        if(N.rows() and N.cols()) {
          _M = new TYPE*[_rows];
          for(int i=0; i<_rows; i++) {
            _M[i] = new TYPE[_cols];
            for(int j=0;j<_cols;j++) {
              _M[i][j] = N[i][j];
            }
          }
        
        }
        
        
        _square = N.square();
        _diag = N.diag();
        
        return *this;
        
      } catch (...) {
        throw;
      }
      
    } else {
      return *this;
    }
  }

  /* Move assignment operator */
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator=(Matrix &&N){

    if(this != &N) {
      
      // Assigment operator part
      if(this->rows()) {
        this->~Matrix();
      }
      /*_rows = N.rows();
      _cols = N.cols();
      
      try {
        if(N.rows() and N.cols()) {
          _M = new TYPE*[_rows];
          for(int i=0; i<_rows; i++) {
            _M[i] = new TYPE[_cols];
            for(int j=0;j<_cols;j++) {
              _M[i][j] = N[i][j];
            }
          }
        
        }
        
        
        _square = N.square();
        _diag = N.diag();*/
      _rows = std::move(N._rows);
      _cols = std::move(N._cols);
      _M = std::move(N._M);
      _square = std::move(N._square);
      _diag = std::move(N._diag);
        
        // Set N to default
        N._M = nullptr;
        N._rows = 0;
        N._cols = 0;
        N._square = false;
        N._diag = false;
    
        return *this;
        
      /*} catch (...) {
        throw;
      }*/
      
    } else {
      return *this;
    }
  }

  /* Overloaded = operator with scalars */
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator=(const TYPE& n) {

    try {
      if(this->square() or !n or n == 1) {
      
        int rows = _rows;
        int cols = _cols;
        
        this->~Matrix();
      
        if(rows and cols) {
          
          _rows = rows;
          _cols = cols;
          
        } else {
          _rows = 1;
          _cols = 1;
        }
        
        _M = new TYPE*[_rows];
        for(int i=0; i<_rows; i++) {
          _M[i] = new TYPE[_cols];
          for(int j=0; j<_cols; j++) {
            _M[i][j] = 0;
            if(!n) _M[i][j] = 0;
          }
          _M[i][i] = n;
        }
      
        _diag = true;
        _square = true;
      } else {
        throw "Matrix::operator=::Matrix must be square for this operation";
      }
      
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded += operator */
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator+=(const Matrix &N) {

    if(_rows!=N.rows() or _cols!=N.cols()) {
      throw "Matrix::operator+=::Matrices must have the same dimension";
    } else {
      for(int i=0; i<_rows; i++) {	
        for(int j=0; j<_cols; j++) {
          _M[i][j] += N[i][j];
        }
      }
      this->FixZeros();
    }

    _diag = diag();

    return *this;
  }

  /* Overloaded + operator */
  template <class TYPE> const Matrix<TYPE> Matrix<TYPE>::operator+(const Matrix &N) const{

    Matrix<TYPE> M(_rows, _cols);

    try {
      M = *this;
      M += N;
    
      return M;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded -= operator */
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator-=(const Matrix &N) {

    if(_rows!=N.rows() or _cols!=N.cols()) {
      throw "Matrix::operator-=::Matrices must have the same dimension";
    } else {
      for(int i=0; i<_rows; i++) {
        for(int j=0; j<_cols; j++) {
          _M[i][j] -= N[i][j];
        }
      }

      this->FixZeros();
    }

    _diag = diag();

    return *this;
  }

  /* Overloaded - operator */
  template <class TYPE> const Matrix<TYPE> Matrix<TYPE>::operator-(const Matrix &N) const {

    Matrix<TYPE> M(_rows, _cols);

    try {
      M = *this;
      M-=N;

      return M;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded *= operator */
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator*=(const Matrix &N) {

    Matrix M(_rows, N.cols());

    if(_cols!=N.rows()) {
      throw "Matrix::operator*=::Matrices have the wrong dimensions";
    } else {
      for(int i=0; i<_rows; i++) {
        for(int j=0; j<N.cols(); j++) {
          M[i][j] = 0;
          for(int k=0; k<_cols; k++) {
            M[i][j] += _M[i][k]*N[k][j];
          }
        }
      }
      for(int i=0; i<_rows; i++) {
        for(int j=0; j<M.cols(); j++) {                
          _M[i][j] = M[i][j];
        }
      }

      this->FixZeros();
    }

    if(this->rows() == this->cols()) {
      _square = true;
    } else {
      _square = false;
    }
    _diag = diag();

    return *this;
  }

  /* Overloaded * operator with matrices */
  template <class TYPE> const Matrix<TYPE> Matrix<TYPE>::operator*(const Matrix &N) const {

    Matrix<TYPE> M(_rows, N.cols());

    try {
      if(M.rows()==this->rows() and M.cols() == this->cols()) {
        M = *this;
        M*=N;
      } else {
        for(int i=0; i<M.rows(); i++) {
          for(int j=0; j<M.cols(); j++) {
            M[i][j] = 0;
            for(int k=0; k<this->cols(); k++) {
              M[i][j] += _M[i][k]*N[k][j];
            }
          }
        }
      }

      M.FixZeros();
      return M;
      
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with column vectors */
  template <class TYPE> const CVector<TYPE> Matrix<TYPE>::operator*(const CVector<TYPE> &V) const {

    try {
      if(this->cols() != V.rows()) {
        throw "Matrix::operator*::Matrices have the wrong dimensions";
      } else {
        CVector<TYPE> W(this->rows());
        for(int i=0; i<this->rows(); i++) {
          W[i] = this->Row(i)*V;
        }
        W.FixZeros();
        return W;
      }
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with row vectors */
  template <class TYPE> const Matrix<TYPE> Matrix<TYPE>::operator*(const RVector<TYPE> &V) const {

    try {
      if(this->cols() != 1) {
        throw "Matrix::operator*::Matrices have the wrong dimensions";
      } else {
        CVector<TYPE> W(this->Column(0));
        return W*V;
      }
    } catch (...) {
      throw;
    }
  }

  /* Overloaded *= operator with scalars*/
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator*=(const TYPE &n) {

    for(int i=0; i<_rows; i++) {
      for(int j=0; j<_cols; j++) {
        _M[i][j] = n*_M[i][j];
      }
    }

    this->FixZeros();
    _diag = diag();

    return *this;
  }

  /* Overloaded * operator with scalars*/
  template <class TYPE> const Matrix<TYPE> Matrix<TYPE>::operator*(const TYPE &n) const {

    Matrix<TYPE> M(_rows, _cols);

    M = *this;
    M*=n;
  
    return M;
  }

  /* Overloaded /= operator with scalars*/
  template <class TYPE> Matrix<TYPE> &Matrix<TYPE>::operator/=(const TYPE &n) {

    if(n == 0) {
      throw "Matrix::operator/=::Division by zero attempted";
    }
    
    for(int i=0; i<_rows; i++) {
      for(int j=0; j<_cols; j++) {
        _M[i][j] = _M[i][j]/n;
      }
    }

    this->FixZeros();

    _diag = diag();
    
    return *this;
  }

  /* Overloaded / operator with scalars*/
  template <class TYPE> const Matrix<TYPE> Matrix<TYPE>::operator/(const TYPE &n) const {

    try {
      Matrix<TYPE> M(_rows, _cols);

      M = *this;
      M/=n;

      return M;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded == operator with matrices */
  template <class TYPE> bool Matrix<TYPE>::operator==(const Matrix &N) const {

    if(_rows != N.rows() or _cols!= N.cols()){
      return false;
    } else {
      for(int i=0; i<_rows; i++) {
        for(int j=0; j<_cols; j++) {
          if(_M[i][j] != N[i][j]) {
            if(fabs(_M[i][j] - N[i][j]) > _M[i][j] * pow(10.,-2)) {
              return false;
            }
          }
        }
      }
    }
    return true;
  }

  /* Overloaded == operator with scalars */
  template <class TYPE> bool Matrix<TYPE>::operator==(const TYPE &n) const {
    if(n != 0) {
      if(this->square()) {
        for(int i=0; i<this->rows(); i++) {
          if(this->_M[i][i] != n) {
            return false;
          }
        }
        return true;
      } else {
        return false;
      }
    } else {
      for(int i=0; i<this->rows(); i++) {
        for(int j=0; j<this->cols(); j++) {
          if(fabs(this->_M[i][j]) > pow(10.,-10.)) {
            return false;
          }
        }
      }
      return true;
    }
  }

  /* Overloaded != operator with matrices */
  template <class TYPE> bool Matrix<TYPE>::operator!=(const Matrix &N) const {
    if(*this == N) {
      return false;
    } else {
      return true;
    }
  }

  /* Overloaded != operator with scalars */
  template <class TYPE> bool Matrix<TYPE>::operator!=(const TYPE &n) const {
    if(*this == n) {
      return false;
    } else {
      return true;
    }
  }

  /* Check whether the matrix is lower triangular */
  template <class TYPE> bool Matrix<TYPE>::isLowerTriangular() const {
    
    try {
      if(!_square) return false;
      for(int i=0; i<rows(); i++) {
        for(int j=i+1; j<rows(); j++) {
          if((*this)[i][j] != 0) return false;
        }
      }
      
      return true;
      
    } catch (...) {
      throw;
    }
  }

  /* Check whether the matrix is upper triangular */
  template <class TYPE> bool Matrix<TYPE>::isUpperTriangular() const {
    
    try {
      
      if(!_square) return false;
      for(int i=0; i<rows(); i++) {
        for(int j=0; j<i; j++) {
          if((*this)[i][j] != 0) 	return false;
        }
      }
      
      return true;
      
    } catch (...) {
      throw;
    }
  }

  /* Obtain the adjoint matrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::Adjoint(int row, int col) const {

    if(!_rows or !_cols) {
      throw "Matrix::Adjoint::Matrix is empty, cannot do adjoint";
    }
    
    if(row < 0 or row > _rows-1 or col < 0 or col > _cols-1) {
      throw "Matrix::Adjoint::Element doesn\'t exist";
    }
    
    Matrix M(_rows-1, _cols-1);

    int k=0;
    for(int i=0; i<_rows; i++) {
      for(int j=0; j<_cols; j++) {
        if(i!=row and j!=col) {
          M[k/(_cols-1)][k%(_cols-1)] = _M[i][j];
          k++;
        }
      }
    }
    
    return M;
  }

  /* Calculate the determinant */
  template <class TYPE> TYPE Matrix<TYPE>::det() const {

    try {
          
      TYPE determinant=0;
      int sign=1;

      if(!_square) throw "Matrix::det::Matrix must be square to calculate the determinant";
      
      if(diag() or isLowerTriangular() or isUpperTriangular()) {
        determinant = 1;
        for(int i=0; i<_rows; i++) {
          determinant*=_M[i][i];
        }
        return determinant;
      }
      
      // Determinant calculation usign LU decomposition		
      Matrix<TYPE> L(rows());
      Matrix<TYPE> U(rows());
      LUDecomposition(L,U);
      determinant = L.det()*U.det();
      
      // Analytic determinant calculation (slower)
      /*for(int i=0; i<_cols; i++) {
        Matrix N(_rows-1, _cols-1);
        N = Adjoint(0,i);
        determinant += sign*_M[0][i]*(N.det());
        sign*=-1;
      }*/
      
      return determinant;
      
    } catch (...) {
      throw;
    }
  }

  /* Obtain the Transpose matrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::Transpose() const {

    try {
      Matrix M(_cols, _rows);

      for(int i=0; i<_cols; i++) {
        for(int j=0; j<_rows; j++) {
          M[i][j] = _M[j][i];
        }
      }
      return M;
    } catch (...) {
      throw;
    }
  }

  /* Obtain the left right decomposition using Crout's algorith */
  template <class TYPE> void Matrix<TYPE>::LUDecomposition(Matrix<TYPE> &L, Matrix<TYPE> &U) const {
    
    try {
      if(!_square) throw "Matrix::LUDecomposition::LUDecomposition only works for square matrices";
      if(diag()) {
        U = 1;
        L = *this;
        return ;
      }
      if(isLowerTriangular()) {
        L = *this;
        U = 1;
        return ;
      }
      if(isUpperTriangular()) {
        U = *this;
        L = 1;
        return ;
      }
      
      double sum = 0;

      for (int i = 0; i < rows(); i++) {
        U[i][i] = 1;
      }

      for (int j = 0; j < rows(); j++) {
        for (int i = j; i < rows(); i++) {
          sum = 0;
          for (int k = 0; k < j; k++) {
            sum = sum + L[i][k] * U[k][j];	
          }
          L[i][j] = (*this)[i][j] - sum;
        }

        for (int i = j; i < rows(); i++) {
          sum = 0;
          for(int k = 0; k < j; k++) {
            sum = sum + L[j][k] * U[k][i];
          }
          if (L[j][j] == 0) {
            throw "Matrix::LUDecomposition::det(L) close to 0!, Can't divide by 0";
          }
          U[j][i] = ((*this)[j][i] - sum) / L[j][j];
        }
      }
      
      
    } catch (...) {
      throw;
    }
  }

  /* Obtain the inverse matrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::Inverse() const {

    try {
      
      Matrix<TYPE> M(_rows,_cols);
      
      
      Matrix<TYPE> L(_rows);
      Matrix<TYPE> U(_rows);
      LUDecomposition(L,U);
      
      //TYPE d = det();
      
      TYPE d = L.det()*U.det();

      if(!square() or !d) throw "Matrix::Inverse::Matrix must be square and with determinant non zero to be invertible";
      
      // Forward-backward algorithm to solve LUx = b
      for(int j=0; j<_cols; j++) {
        RVector<TYPE> y(_rows);
        for(int i=0; i<_rows; i++) {
        y[i] = (i == j) ? 1 : 0;
        for (int k=0; k<i; k++) {
          y[i] -= L[i][k] * y[k];
        }
        y[i] /= L[i][i];
        }
        
        for (int i=_rows-1; i>=0; i--) {
        M[i][j] = y[i];
        for (int k=i+1; k<_rows; k++) {
          M[i][j] -= U[i][k] * M[k][j];
          }
        M[i][j] /= U[i][i];
        }
      }
      
      // Analytic algorithm for inverses (slower)
      /*for(int i=0; i<_rows; i++) {
        for(int j=0; j<_cols; j++) {
          M[i][j] = (1-2*((i+j)%2))*Adjoint(i,j).det();
        }
      }
      
      M = M.Transpose()/d;
      */
      
      return M;
      
    
    } catch (...) {
      throw;
    }
  }

  /* Obtain the pseudoinverse matrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::PseudoInverse() const {

    try {
      if(this->square()) {
        return Inverse();
      } else {
        
        Matrix<TYPE> SquareMatrix((*this)*(Transpose()));
        Matrix<TYPE> ReturnMatrix(cols(),rows());
        if(SquareMatrix.det() != 0) {
          ReturnMatrix = (Transpose())*(SquareMatrix.Inverse());
        } else {
          throw "Matrix::PseudoInverse::Matrix doesn\'t have pseudoinverse";
        }
        return ReturnMatrix;
      }
    } catch (...) {
      throw;
    }
  }

  /* Obtain the transpose conjugate matrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::TransposeConjugate() const {

    try {
      Matrix<TYPE> M = this->Transpose();
    
      if(std::is_same<TYPE, std::complex<double> >::value or std::is_same<TYPE, std::complex<float> >::value or std::is_same<TYPE, std::complex<int> >::value) {
        M = M.Conjugate();
      } else if(std::is_same<TYPE, Matrix>::value or std::is_base_of<Matrix,TYPE>::value) {
        for(int i=0; i<rows(); i++) {
          for(int j=0; j<cols(); j++) {
            M[i][j] = M[i][j].TransposeConjugate();
          }
        }
      }
      return M;
      
    } catch (...) {
      throw;
    }
  }

  /* Obtain the conjugate matrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::Conjugate() const {

    try {
      Matrix<TYPE> M(*this);
    
      if(std::is_same<TYPE, std::complex<double> >::value or std::is_same<TYPE, std::complex<float> >::value or std::is_same<TYPE, std::complex<int> >::value) {
        for(int i=0; i<rows(); i++) {
          for(int j=0; j<cols(); j++) {
            M[i][j] = M[i][j].conj();
          }
        }
      } else if(std::is_same<TYPE, Matrix>::value or std::is_base_of<Matrix,TYPE>::value) {
        for(int i=0; i<rows(); i++) {
          for(int j=0; j<cols(); j++) {
            M[i][j] = M[i][j].TransposeConjugate();
          }
        }
      }
    
      return M;
    } catch (...) {
      throw;
    }
  }

  /* Extract the ith row */
  template <class TYPE> RVector<TYPE> Matrix<TYPE>::Row(int i) const
  {   
    try
    {
      int I = i;
      if(i < 0) I = rows()+i; 
      if(I > rows()-1)
        throw "Matrix::Row::Element doesn\'t exist";

      RVector<TYPE> V(this->cols());
      for(int j=0; j<this->cols(); j++)
        V[j] = (*this)[I][j];

      return V;

    }
    catch (...) { throw; }
  }

  /* Extract the ith column */
  template <class TYPE> CVector<TYPE> Matrix<TYPE>::Column(int i) const
  {   
    try
    {
      int I = i;
      if(i < 0) I = cols()+i;
      if(I > cols()-1)
        throw "Matrix::Column::Element doesn\'t exist";

      CVector<TYPE> V(this->rows());
      for(int j=0; j<this->rows(); j++)
        V[j] = (*this)[j][I];

      return V;

    }
    catch (...) { throw; }
  }

  /* Set a row */
  template <class TYPE> void Matrix<TYPE>::SetRow(int i, RVector<TYPE> V)
  {
    int I = i;
    if(i < 0) I = rows()+i; 
    if(I > rows()-1) 
      throw "Matrix::SetRow::Element doesn\'t exist";

    if(this->cols() != V.cols())
      throw "Matrix::SetRow::Dimensions don\'t match";

    for(int j=0; j<this->cols(); j++)
      _M[I][j] = V[j];

  }

  /* Set a column */
  template <class TYPE> void Matrix<TYPE>::SetColumn(int i, CVector<TYPE> V)
  {
    int I = i;
    if(i < 0) I = cols()+i;   
    if(I > cols()-1)
      throw "Matrix::SetColumn::Element doesn\'t exist";

    if(this->rows() != V.rows())
      throw "Matrix::SetColumn::Dimensions don\'t match";

    for(int j=0; j<this->rows(); j++)
      _M[j][I] = V[j];

  }

  /* Change the basis of eigenvectors */
  template <class TYPE> void Matrix<TYPE>::ChangeBasis(int i, int j) {
    
    try {
      if(i < 0 or i > rows()-1 or i > cols()-1 or j <0 or j > rows()-1 or j > cols()-1) {
        throw "Matrix::ChangeBasis::Element doesn\'t exist";
      }
      this->SwapRows(i,j);
      this->SwapColumns(i,j);
    } catch (...) {
      throw;
    }
  }

  /* Swap values */
  template <class TYPE> void Matrix<TYPE>::Swap(int i1, int j1, int i2, int j2) {
    
    if(i1 < 0 or i1 > rows()-1 or j1 < 0 or j1 > cols()-1 or i2 < 0 or i2 > rows()-1 or j2 < 0 or j2 > cols()-1) {
      throw "Matrix::Swap::Element doesn\'t exist";
    }
    
    TYPE aux = (*this)[i1][j1];
    (*this)[i1][j1] = (*this)[i2][j2];
    (*this)[i2][j2] = aux;
  }

  /* Swap rows */
  template <class TYPE> void Matrix<TYPE>::SwapRows(int i, int j) {
    
    try {
      RVector<TYPE> Row = this->Row(i);
      this->SetRow(i, this->Row(j));
      this->SetRow(j, Row);
    } catch (...) {
      throw;
    }
  }

  /* Swap columns */
  template <class TYPE> void Matrix<TYPE>::SwapColumns(int i, int j) {
    
    try {
      CVector<TYPE> Column = this->Column(i);
      this->SetColumn(i, this->Column(j));
      this->SetColumn(j, Column);
    } catch (...) {
      throw;
    }
  }

  /* Move row */
  template <class TYPE> void Matrix<TYPE>::MoveRow(int from, int to) {
    
    try {
      if(from < to) {
        for(int i=from; i<to; i++) {
          this->SwapRows(i,i+1);
        }
      } else {
        for(int i=from; i>to; i--) {
          this->SwapRows(i,i-1);
        }
      }
    } catch (...) {
      throw;
    }
  }

  /* Move column */
  template <class TYPE> void Matrix<TYPE>::MoveColumn(int from, int to) {
    try {
      if(from < to) {
        for(int i=from; i<to; i++) {
          this->SwapColumns(i,i+1);
        }
      } else {
        for(int i=from; i>to; i--) {
          this->SwapColumns(i,i-1);
        }
      }
    } catch (...) {
      throw;
    }
  }

  /* Move rows */
  template <class TYPE> void Matrix<TYPE>::MoveRows(int from1, int from2, int to) {

    try {
      int fr1,fr2;
      
      if(from1 < from2) {
        fr1 = from1;
        fr2 = from2;
      } else {
        fr1 = from2;
        fr2 = from1;
      }
      
      if(fr2 < to) {
        for(int i=fr1; i<=fr2; i++)
          this->MoveRow(fr1,to);
      } else if(fr1 >= to) {
        for(int i=0; i<=fr2-fr1; i++)
          this->MoveRow(fr1+i,to+i);
      } else if(fr1 < to and fr2 >= to) {
        for(int i=0; i<to-fr1; i++)
          this->MoveRow(fr2+i+1,fr1+i);
      } else {
        throw "Matrix::MoveRows::Cannot move rows";
      }
    } catch (...) {
      throw;
    }
  }

  /* Move columns */
  template <class TYPE> void Matrix<TYPE>::MoveColumns(int from1, int from2, int to) {
    
    try {
      int fr1,fr2;
      
      if(from1 < from2) {
        fr1 = from1;
        fr2 = from2;
      } else {
        fr1 = from2;
        fr2 = from1;
      }
      
      if(fr2 < to) {
        for(int i=fr1; i<=fr2; i++) {
          this->MoveColumn(fr1, to);
        }
      } else if (fr1 > to) {
        for(int i=0; i<=fr2-fr1; i++) {
          this->MoveColumn(fr1+i, to+i);
        }
      } else {
        throw "Matrix::MoveColumns::Cannot move columns";
      }
    } catch (...) {
      throw;
    }
  }

  /* Extracts a block submatrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::ExtractMatrix(int i, int j) const {

    try {
      if(i < 0 or i > rows()-1 or j < 0 or j > cols()-1) {
        throw "Matrix::ExtractMatrix::Element doesn\'t exist";
      }
      
      Matrix<double> M(j-i+1);
      for(int k=0; k<j-i+1; k++) {
        for(int l=0; l<j-i+1; l++) {
          M[k][l] = (*this)[i+k][i+l];
        }
      }
      return M;
    } catch (...) {
      throw;
    }
  }

  /* Extracts a non-square submatrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::ExtractMatrix(int i1, int i2, int j1, int j2) const {

    try {
      if( !(i2-i1+1) or !(j2-j1+1)) {
        return Matrix<double>();
      }
      if(i1 < 0 or i1 > rows()-1 or i2 < 0 or i2 > rows()-1 or j1 < 0 or j1 > cols()-1 or j2 < 0 or j2 > cols()-1) {
        throw "Matrix::ExtractMatrix::Element doesn\'t exist";
      } 
      Matrix<double> M(i2-i1+1, j2-j1+1);
      for(int k=0; k<M.rows(); k++) {
        for(int l=0; l<M.cols(); l++) {
          M[k][l] = (*this)[i1+k][j1+l];
        }
      }
      return M;
    } catch (...) {
      throw;
    }
  }

  /* Delete a row */
  template <class TYPE> void Matrix<TYPE>::DeleteRow(int i) {
    
    try {
      if(i < 0 or i > rows()-1) {
        throw "Matrix::DeleteRow::Element doesn\'t exist";
      }
      this->MoveRow(i,this->rows()-1);
      delete [] _M[this->rows()-1];
      _rows--;
    } catch (...) {
      throw;
    }
  }

  /* Delete a column */
  template <class TYPE> void Matrix<TYPE>::DeleteColumn(int i) {
    
    try {
      if(i < 0 or i > cols()-1) {
        throw "Matrix::DeleteColumn::Element doesn\'t exist";
      }
      this->MoveColumn(i, this->cols()-1);
      _cols--;
    } catch (...) {
      throw;
    }
  }

  /* Insert a row */
  template <class TYPE> void Matrix<TYPE>::InsertRow(int i, RVector<TYPE> R) {
    
    try {
      this->Append(R, "BOTTOM");
      this->MoveRow(this->rows()-1, i);
    } catch (...) {
      throw;
    }
  }

  /* Insert a column */
  template <class TYPE> void Matrix<TYPE>::InsertColumn(int i, CVector<TYPE> C) {
    
    try {
      this->Append(C, "RIGHT");
      this->MoveColumn(this->cols()-1, i);
    } catch (...) {
      throw;
    }
  }

  /* Append a matrix to the bottom or right side of the current matrix */
  template <class TYPE> void Matrix<TYPE>::Append(Matrix<TYPE> N, std::string _where) {

    try {
      std::string where = _where;
      
      std::string::iterator init = where.begin();
      std::string::iterator end = where.end();

      while (init != end) {
        *init = std::toupper((unsigned char)*init);
        ++init;
      }

      if(this->cols() == N.cols() and where=="BOTTOM") {
        
        Matrix<TYPE> M(this->rows()+N.rows(), this->cols());

        for(int i=0; i<this->rows(); i++) {
          M.SetRow(i, this->Row(i));
        }
        for(int i=0; i<N.rows(); i++) {
          M.SetRow(this->rows()+i, N.Row(i));
        }
        
        *this = M;

      } else if(this->cols() == N.cols() and where=="TOP") {

        Matrix<TYPE> M(this->rows()+N.rows(), this->cols());

        for(int i=0; i<N.rows(); i++) {
          M.SetRow(i, N.Row(i));
        }
        for(int i=0; i<this->rows(); i++) {
          M.SetRow(N.rows()+i, this->Row(i));
        }

        *this = M;

    
      } else if(this->rows() == N.rows() and where=="RIGHT") {

        Matrix<TYPE> M(this->rows(), this->cols()+N.cols());

        for(int i=0; i<this->cols(); i++) {
          M.SetColumn(i, this->Column(i));
        }
        for(int i=0; i<N.cols(); i++) {
          M.SetColumn(this->cols()+i, N.Column(i));
        }
        
        *this = M;

      } else if(this->rows() == N.rows() and where=="LEFT") {
    
        Matrix<TYPE> M(this->rows(), this->cols()+N.cols());

        for(int i=0; i<N.cols(); i++) {
          M.SetColumn(i, N.Column(i));
        }

        for(int i=0; i<this->cols(); i++) {
          M.SetColumn(N.cols()+i, this->Column(i));
        }

        *this = M;

      } else if(where == "BOTTOM" or where == "TOP" or where == "LEFT" or where == "RIGHT") {
        throw "Matrix::Append::Dimensions don\'t match";
      } else {
        throw "Matrix::Append::Unknown parameter";
      }

    } catch (...) {
      throw;
    }
  }

  /* Append a matrix to the bottom of the current matrix, default */
  template <class TYPE> void Matrix<TYPE>::Append(Matrix<TYPE> N) {
    
    try {
      this->Append(N,"BOTTOM");
    } catch (...) {
      throw;
    }
  }

  /* Print the matrix*/
  template <class TYPE> std::string Matrix<TYPE>::Print() const {

    std::stringstream String;
    for(int i=0; i<_rows; i++) {
      for(int j=0; j<_cols; j++) {
        String << _M[i][j];
        if(j<_cols-1) {
          String << "  ";
        }
      }
      if(i<_rows-1) {
        String << std::endl;
      }
    }
    return String.str();
    
  }

  /* Print the matrix from row l*/
  template <class TYPE> std::string Matrix<TYPE>::Print(int l) const {

    std::stringstream String;
    for(int i=l; i<_rows; i++) {
      for(int j=0; j<_cols; j++) {
        String << _M[i][j];
        if(j<_cols-1) {
          String << "  ";
        }
      }
      if(i<_rows-1) {
        String << std::endl;
      }
    }
    return String.str();
  }

  /* Print the matrix as a table */
  template <class TYPE> std::string Matrix<TYPE>::Table() const {
    
    std::stringstream String;
    String << "{";
    for(int i=0; i<_rows; i++) {
      String << "{";
      for(int j=0; j<_cols; j++) {
        if(j < _cols-1) {
          String << _M[i][j] << ", ";
        } else {
          String << _M[i][j];
        }
      }
      if(i < _rows-1) {
        String << "}, ";
      } else {
        String << "}";
      }
    }
    String << "}";
    
    return String.str();
  }

  /* Fix the zeros of the matrix */
  template <class TYPE> void Matrix<TYPE>::FixZeros() {
    for(int i=0; i<this->rows(); i++) {
      for(int j=0; j<this->cols(); j++) {
        for(int k=0; k<this->rows(); k++) {
          for(int l=0; l<this->cols(); l++) {
            if((*this)[i][j] and (*this)[k][l]) {
              if(fabs((double)(*this)[k][l]/(*this)[i][j]) < 1e-10) {
                (*this)[k][l] = 0;
              } else if(fabs((double)(*this)[i][j]/(*this)[k][l]) < 1e-10) {
                (*this)[i][j] = 0;
              }
            }
          }
        }
      }
    }
  }

  /* Creates an indentity matrix of dimension n */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::Identity(int n) {
    
    try {
      Matrix<TYPE> M(n);
      for(int i=0; i<n; i++) {
        M[i][i] = 1;
      }
      return M;
    } catch (...) {
      throw;
    }
  }
  
  /* Returns the absolute value of the elements of the matrix */
  template <class TYPE> Matrix<TYPE> Matrix<TYPE>::Abs() const
  {
    Matrix<TYPE> N(*this);
    for(int i=0; i<N.rows(); i++)
      for(int j=0; j<N.cols(); j++)
        N[i][j] = fabs(N[i][j]);
    return N;
  }

  /* Print to json format */
  template <class TYPE> JSONNode Matrix<TYPE>::json(std::string name) const {
    
    try {
      JSONNode matrix(JSON_ARRAY);
      if(name != "") {
        matrix.set_name(name);
      }
      for(int i=0; i<_rows; i++) {
        JSONNode row(JSON_ARRAY);
        for(int j=0; j<_cols; j++) {
          row.push_back(JSONNode("", (*this)[i][j]));
        }
        matrix.push_back(row);
      }
    
      return matrix;
    
    } catch (...) {
      throw;
    }
  }

  /* Parses a json object into the attributes of the class */
  template <class TYPE> void Matrix<TYPE>::ParseJSON(const JSONNode & n) {
    
    try {
      if(this->rows()) {
        this->~Matrix();
      }
      
      _rows = n.size();
      _diag = true;
      
      _M = new TYPE*[_rows];
      JSONNode::const_iterator it = n.begin();
      for(int i=0; i < _rows; i++) {
        _cols = it->size();
        
        if(_cols) {
          _M[i] = new TYPE[_cols];
          JSONNode::const_iterator jt = it->begin();
          for(int j=0; j<_cols; j++) {
            if(std::is_same<TYPE, int>::value) {
              _M[i][j] = jt->as_int();
            } else if(std::is_same<TYPE, float>::value or std::is_same<TYPE, double>::value) {
              _M[i][j] = jt->as_float();
            }
            if(i != j and _M[i][j] != 0) {
              _diag = false;
            }
            ++jt;
          }
        } else {
          _cols = 1;
          _M[i] = new TYPE[1];
          if(std::is_same<TYPE, int>::value) {
            _M[i][0] = it->as_int();
          } else if(std::is_same<TYPE, float>::value or std::is_same<TYPE, double>::value) {
            _M[i][0] = it->as_float();
          }
          _diag = false;
        }
        ++it;
      }
    
      if(_rows == _cols) {
        _square = true;
      } else {
        _square = false;
      }
    } catch (...) {
      throw;
    }
  }
    
  /* Overloaded * operator with scalars on the right*/
  template <class TYPE> const Matrix<TYPE> &operator*(const Matrix<TYPE> &M, const TYPE &n) {
    try {
      return M*n;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with scalars on the left*/
  template <class TYPE> const Matrix<TYPE> &operator*(const TYPE &n, const  Matrix<TYPE> &M) {
    try {
      return M*n;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded << operator with matrices */
  template <class TYPE> std::ostream &operator<<(std::ostream &stream, const Matrix<TYPE> M) {
    stream << M.Print();
    return stream;
  }
}  

/*****************************/
/* Class CVector definitions */
/*****************************/

namespace Tomb
{
  /* Constructor, empty */
  template <class TYPE> CVector<TYPE>::CVector() : Matrix<TYPE>(){
  }

  /* Constructor */
  template <class TYPE> CVector<TYPE>::CVector(int rows) : Matrix<TYPE>(rows, 1){
  }

  /* Copy constructor 1 */
  template <class TYPE> CVector<TYPE>::CVector(const CVector &V) : Matrix<TYPE>(V) {
    
  }

  /* Move constructor 1 */
  template <class TYPE> CVector<TYPE>::CVector(CVector &&V) : Matrix<TYPE>(std::move(V)) {
    
  }

  /* Copy constructor 2 */
  template <class TYPE> CVector<TYPE>::CVector(const Matrix<TYPE> &M) : Matrix<TYPE>(M) {
    
  }

  /* Move constructor 2 */
  template <class TYPE> CVector<TYPE>::CVector(Matrix<TYPE> &&M) : Matrix<TYPE>(std::move(M)) {
    
  }

  /* Destructor */
  template <class TYPE> CVector<TYPE>::~CVector() {
    
  }

  /* Overloaded [] operator */
  template <class TYPE> TYPE &CVector<TYPE>::operator[](int i) const {
    if(i<0)
      return this->_M[this->rows()+i][0];
    return this->_M[i][0];
  }

  /* Overloaded = operator */
  template <class TYPE> CVector<TYPE> &CVector<TYPE>::operator=(const CVector &V) {
    try {
      Matrix<TYPE>::operator=(V);
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Move = operator */
  template <class TYPE> CVector<TYPE> &CVector<TYPE>::operator=(CVector &&V) {
    try {
      Matrix<TYPE>::operator=(std::move(V));
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded += operator */
  template <class TYPE> CVector<TYPE> &CVector<TYPE>:: operator+=(const CVector &V) {
    
    if(this->rows() != V.rows()) {
      throw "CVector::operator+=::Dimensions don\'t match";
    } else {
      for(int i=0; i<this->rows(); i++) {
        (*this)[i] += V[i];
      }

      this->FixZeros();
    }
    return *this;
  }

  /* Overloaded + operator */
  template <class TYPE> const CVector<TYPE> CVector<TYPE>::operator+(const CVector &V) const {

    try {
      CVector<TYPE> W(this->rows());
      W = *this;
      W += V;
      return W;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded -= operator */
  template <class TYPE> CVector<TYPE> &CVector<TYPE>::operator-=(const CVector &V) {
    if(this->rows() != V.rows()) {
      throw "CVector::operator-=::Dimensions don\'t match";
    } else {
      for(int i=0; i<this->rows(); i++) {
        (*this)[i] -= V[i];
      }
      this->FixZeros();
    }
    return *this;
  }

  /* Overloaded - operator */
  template <class TYPE> const CVector<TYPE> CVector<TYPE>::operator-(const CVector &V) const {
    
    try {
      CVector<TYPE> W(this->rows());
      W = *this;
      W -= V;
      return W;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with row vectors*/
  template <class TYPE> const Matrix<TYPE> CVector<TYPE>::operator*(const RVector<TYPE> &V) const {

    try {
      Matrix<double> M(this->rows(), V.cols());
      for(int i=0; i<this->rows(); i++) {
        for(int j=0; j<V.cols(); j++) {
          M[i][j] = (*this)[i]*V[j];
        }
      }
      M.FixZeros();
      return M;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with matrices */
  template <class TYPE> const Matrix<TYPE> CVector<TYPE>::operator*(const Matrix<TYPE> &M) const {
    
    try {
      if(M.rows() != 1) {
        throw "CVector::operator*::Dimensions don\'t match";
      } else {
        RVector<TYPE> W(M.Row(0));
        return (*this)*W;
      }
    } catch (...) {
      throw;
    }
  }

  /* Overloaded *= operator with scalars */
  template <class TYPE> CVector<TYPE> &CVector<TYPE>::operator*=(const TYPE &n) {
    for(int i=0; i<this->rows(); i++) {
      (*this)[i] *= n;
    }
    this->FixZeros();
    return *this;
  }

  /* Overloaded * operator with scalars */
  template <class TYPE> const CVector<TYPE> CVector<TYPE>::operator*(const TYPE &n) const {
    return CVector(*this)*=n;
  }

  /* Overloaded /= operator with scalars */
  template <class TYPE> CVector<TYPE> &CVector<TYPE>::operator/=(const TYPE &n) {
    if(n != 0) {
      for(int i=0; i<this->rows(); i++) {
        (*this)[i] /= n;
      }
      this->FixZeros();
    } else {
      throw "CVector::operator/=::Division by zero attempted";
    }
    return *this;
  }

  /* Overloaded / operator with scalar */
  template <class TYPE> const CVector<TYPE> CVector<TYPE>::operator/(const TYPE &n) const {
    try {
      return CVector(*this) /= n;
    } catch (...) {
      throw;
    }
  }

  /* Obtain the Transpose of a column vector, that is a row vector */
  template <class TYPE> RVector<TYPE> CVector<TYPE>::Transpose() {
    try {
      RVector<TYPE> V(this->rows());
      for(int i=0; i<this->rows(); i++) {
        V[i] = (*this)[i];
      }
      return V;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded == operator with vectors */
  template <class TYPE> bool CVector<TYPE>::operator==(const CVector &V) const {
    if(this->rows() != V.rows()){
      return false;
    } else {
      for(int i=0; i<this->rows(); i++) {
        if((*this)[i] != V[i]) {
          if(fabs((*this)[i] - V[i]) > (*this)[i] * pow(10.,-5)) {
            return false;
          }
        }
      }
    }
    return true;
  }

  /* Overloaded == operator with scalars */
  template <class TYPE> bool CVector<TYPE>::operator==(const TYPE &n) const {
  if(n != 0) {
  if(this->square()) {
    for(int i=0; i<this->rows(); i++) {
    if(this->_M[i][i] != n) {
    return false;
    }
  }
  return true;
  } else {
  return false;
  }
  } else {
  for(int i=0; i<this->rows(); i++) {
  for(int j=0; j<this->cols(); j++) {
    if(fabs(this->_M[i][j]) > pow(10.,-10.)) {
    return false;
    }
  }
  }
  return true;
  }
  }

  /* Overloaded != operator */
  template <class TYPE> bool CVector<TYPE>::operator!=(const CVector &V) const {
    if(*this == V) {
      return false;
    } else {
      return true;
    }
  }

  /* Overloaded != operator with scalars */
  template <class TYPE> bool CVector<TYPE>::operator!=(const TYPE &n) const {
    if(*this == n) {
      return false;
    } else {
      return true;
    }
  }

  /* Append a column vector to the bottom */
  template <class TYPE> void CVector<TYPE>::Append(CVector<TYPE> V){

    try {
      Matrix<TYPE>::Append(V,"BOTTOM");
      
    } catch (...) {
      throw;
    }
  } 

  /* Overloaded * operator with scalars on the right*/
  template <class TYPE> const CVector<TYPE> &operator*(const CVector<TYPE> &C, const TYPE &n) {
    try {
      return C*n;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with scalars on the left */
  template <class TYPE> const CVector<TYPE> &operator*(const TYPE &n, const  Matrix<TYPE> &C) {
    try {
      return C*n;
    } catch (...) {
      throw;
    }
  }
}

/*****************************/
/* Class RVector definitions */
/*****************************/

namespace Tomb
{
  /* Constructor, empty */
  template <class TYPE> RVector<TYPE>::RVector(): Matrix<TYPE>(){
  }

  /* Constructor */
  template <class TYPE> RVector<TYPE>::RVector(int cols) : Matrix<TYPE>(1, cols){
  }

  /* Copy constructor 1 */
  template <class TYPE> RVector<TYPE>::RVector(const RVector &V) : Matrix<TYPE>(V) {
    
  }

  /* Move constructor 1 */
  template <class TYPE> RVector<TYPE>::RVector(RVector &&V) : Matrix<TYPE>(std::move(V)) {
    
  }

  /* Copy constructor 2 */
  template <class TYPE> RVector<TYPE>::RVector(const Matrix<TYPE> &M) : Matrix<TYPE>(M) {
    
  }

  /* Move constructor 2 */
  template <class TYPE> RVector<TYPE>::RVector(Matrix<TYPE> &&M) : Matrix<TYPE>(std::move(M)) {
    
  }

  /* Destructor */
  template <class TYPE> RVector<TYPE>::~RVector() {
    
  }

  /* Overloaded [] operator */
  template <class TYPE> TYPE &RVector<TYPE>::operator[](int i) const
  {
    if(i<0)
      return this->_M[0][this->cols()+i];
    return this->_M[0][i];
  }

  /* Overloaded = operator */
  template <class TYPE> RVector<TYPE> &RVector<TYPE>::operator=(const RVector &V) {
    try {
      Matrix<TYPE>::operator=(V);
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Move = operator */
  template <class TYPE> RVector<TYPE> &RVector<TYPE>::operator=(RVector &&V) {
    try {
      Matrix<TYPE>::operator=(std::move(V));
      return *this;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded += operator */
  template <class TYPE> RVector<TYPE> &RVector<TYPE>:: operator+=(const RVector &V) {
    
    if(this->cols() != V.cols()) {
      throw "RVector::operator+=::Dimensions don\'t match";
    } else {
      for(int i=0; i<this->cols(); i++) {
        (*this)[i] += V[i];
      }	
      
      this->FixZeros();
    }
    return *this;
  }

  /* Overloaded + operator */
  template <class TYPE> const RVector<TYPE> RVector<TYPE>::operator+(const RVector &V) const {
    
    try {
      RVector<TYPE> W(this->cols());
      W = *this;
      W += V;
      return W;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded -= operator */
  template <class TYPE> RVector<TYPE> &RVector<TYPE>::operator-=(const RVector &V) {
    if(this->cols() != V.cols()) {
      throw "RVector::operator-=::Dimensions don\'t match";
    } else {
      for(int i=0; i<this->cols(); i++) {
        (*this)[i] -= V[i];
      }
      this->FixZeros();
    }
    return *this;
  }

  /* Overloaded - operator */
  template <class TYPE> const RVector<TYPE> RVector<TYPE>::operator-(const RVector &V) const {
    try {
      RVector<TYPE> W(this->cols());
      W = *this;
      W -= V;
      return W;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with column vectors */
  template <class TYPE> const TYPE RVector<TYPE>::operator*(const CVector<TYPE> V) const {
    try {
      TYPE Result = 0;
      if(this->cols() != V.rows()) {
        throw "RVector::operator*::Dimensions don\'t match";
      } else {
        for(int i=0; i<this->cols(); i++) {
          Result += (*this)[i]*V[i];
        }
        for(int i=0; i<this->cols(); i++) {
          if(Result and (*this)[i]) {
            if(fabs( (double) Result/(*this)[i]) < 1e-10) {
              Result = 0;
            }
          }
        }
      }
      return Result;
    } catch (...) { 
      throw;
    }
  }

  /* Overloaded * operator with matrices */
  template <class TYPE> const RVector<TYPE> RVector<TYPE>::operator*(const Matrix<TYPE> &M) const {

    try {
      RVector<TYPE> W(M.cols());
      if(this->cols() != M.rows()) {
        throw "RVector::operator*::Dimensions don\'t match";
      } else {
        for(int i=0; i<M.cols(); i++) {
          W[i] = (*this)*M.Column(i);
        }
        W.FixZeros();
        return W;
      }
      return W;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded *= operator with scalars */
  template <class TYPE> RVector<TYPE> &RVector<TYPE>::operator*=(const TYPE &n) {
    for(int i=0; i<this->cols(); i++) {
      (*this)[i] *= n;
    }
    this->FixZeros();
    return *this;
  }

  /* Overloaded * operator with scalars */
  template <class TYPE> const RVector<TYPE> RVector<TYPE>::operator*(const TYPE &n) const {
    try {
      return RVector(*this)*=n;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded /= operator with scalars */
  template <class TYPE> RVector<TYPE> &RVector<TYPE>::operator/=(const TYPE &n) {
    if(n != 0) {
      for(int i=0; i<this->cols(); i++) {
        (*this)[i] /= n;
      }
      this->FixZeros();
    } else {
      throw "RVector::operator/=::Division by zero attempted";
    }
    return *this;
  }

  /* Overloaded / operator with scalars */
  template <class TYPE> const RVector<TYPE> RVector<TYPE>::operator/(const TYPE &n) const {
    try {
      return RVector(*this) /= n;
    } catch (...) {
      throw;
    }
  }

  /* Obtain the Transpose of a row vector, that is a column vector */
  template <class TYPE> CVector<TYPE> RVector<TYPE>::Transpose() {
    try {
      CVector<TYPE> V(this->cols());
      for(int i=0; i<this->cols(); i++) {
        V[i] = (*this)[i];
      }
      return V;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded == operator with vectors */
  template <class TYPE> bool RVector<TYPE>::operator==(const RVector &V) const {
    if(this->cols() != V.cols()){
      return false;
    } else {
      for(int i=0; i<this->cols(); i++) {
        if((*this)[i] != V[i]) {
          if(fabs(V[i]) > 1e-5) {
            if(fabs(((*this)[i] - V[i])/V[i]) > 1e-4) {
              return false;
            }
          } else if(fabs((*this)[i]) > 1e-5) {
            if(fabs(((*this)[i] - V[i])/(*this)[i]) > 1e-4) {
              return false;
            }
          }
        }
      }
    }
    return true;
  }

  /* Overloaded == operator with scalars */
  template <class TYPE> bool RVector<TYPE>::operator==(const TYPE &n) const {

    if(n != 0) {
      if(this->square()) {
        for(int i=0; i<this->rows(); i++) {
          if(this->_M[i][i] != n) {
            return false;
          }
        }
        return true;
      } else {
        return false;
      }
    } else {
      for(int i=0; i<this->rows(); i++) {
        for(int j=0; j<this->cols(); j++) {
          if(fabs(this->_M[i][j]) > 1e-10) {
            return false;
          }
        }
      }
      return true;
    }
  }

  /* Overloaded != operator */
  template <class TYPE> bool RVector<TYPE>::operator!=(const RVector &V) const {
    if(*this == V)
      return false;
    return true;
  }

  /* Overloaded != operator with scalars */
  template <class TYPE> bool RVector<TYPE>::operator!=(const TYPE &n) const {
    if(*this == n)
      return false;
    return true;
  }

  /* Appends a row vector to the side */
  template <class TYPE> void RVector<TYPE>::Append(RVector<TYPE> V) {

    try {
      Matrix<TYPE>::Append(V, "RIGHT");
    } catch (...) {
      throw;
    }
  }

  /* Appends an element to the side */
  template <class TYPE> void RVector<TYPE>::Append(TYPE n) {
    
    try {
      RVector<TYPE> R(1);
      R[0] = n;
      Append(R);
    } catch (...) {
      throw;
    }
  }

  /* Print the row vector for table representation*/
  template <class TYPE> std::string RVector<TYPE>::Table() const {
    
    std::stringstream String;
    String << "{";
    for(int j=0; j<this->_cols; j++) {
      if(j < this->_cols-1) {
        String << this->_M[0][j] << ", ";
      } else {
        String << this->_M[0][j];
      }
    }

    String << "}";
    
    return String.str();
  }

  /* Overloaded * operator with scalars on the right */
  template <class TYPE> const RVector<TYPE> &operator*(const RVector<TYPE> &R, const TYPE &n) {
    try {
      return R*n;
    } catch (...) {
      throw;
    }
  }

  /* Overloaded * operator with scalars on the left*/
  template <class TYPE> const RVector<TYPE> &operator*(const TYPE &n, const  RVector<TYPE> &R) {
    try {
      return R*n;
    } catch (...) {
      throw;
    }
  }
}

#endif /* __MATRIX_H */
