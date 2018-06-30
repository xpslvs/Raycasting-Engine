/* matrix.hh */
#ifndef MATRIX_HH
#define MATRIX_HH
#include <cstring>
#include <initializer_list>
#include "vector.hh"

/* Uses row-column notation
 */
template <int R, int C, class T>
class Matrix
{
public:
    Matrix(void);
    Matrix(const T *);
    Matrix(const std::initializer_list<Vector<R, T>> &);
    template <class U>
    operator Matrix<R, C, U>(void) const;
    inline const T& operator ()(int, int) const;
    inline       T& operator ()(int, int);

    Matrix<C, R, T> transpose(void) const;

    inline Matrix operator +(void) const;
    inline Matrix operator -(void) const;
           Matrix operator +(const Matrix&) const;
           Matrix operator -(const Matrix&) const;
    template <int N>
           Matrix<C, N, T> operator *(const Matrix<N, R, T>&) const;
           Matrix operator *(const T&) const;
           Matrix operator /(const T&) const;
    inline bool   operator ==(const Matrix&) const;
    inline bool   operator !=(const Matrix&) const;
    inline Matrix operator +=(const Matrix&);
    inline Matrix operator -=(const Matrix&);
    inline Matrix operator *=(const T&);
    inline Matrix operator /=(const T&);
private:
    T _element[R * C];
};

template <int N, class T>
class Matrix<N, N, T>
{
    T      determinant(void) const;
    Matrix adjugate(void) const;
    Matrix inverse(void) const;

    inline Matrix       operator *=(const Matrix&) const;
    inline Vector<N, T> operator *(const Vector<N, T>&) const;
};


template <int R, int C, class T>
Matrix<R, C>::Matrix(void)
{
    std::memset(this->_element, T(), R * C * sizeof(T));
}

template <int R, int C, class T>
Matrix<R, C>::Matrix(const T *a)
{
    std::memcpy(this->_element, a, R * C * sizeof(T));
}

template <int R, int C, class T>
Matrix<R, C>::Matrix(const std::initializer_list<Vector<R, T>> &aggregate)
{
    auto iter = aggregate.begin();
    for(int j = 0; j < C; ++j)
    {
        for(int i = 0; i < R; ++i)
        {
            (*this)(i, j) = iter != aggregate.end() ? *iter[i] : T();
        }
        if(iter != aggregate.end())
            ++iter;
    }
}

template <int R, int C, class T>
template <class U>
Matrix<R, C, T>::operator Matrix<R, C, U>(void) const
{
    Matrix<R, C, U> a{};
    for(int i = 0; i < R; ++i)
    {
        for(int j = 0; j < C; ++j)
        {
            a(i, j) = static_cast<U>((*this)(i, j));
        }
    }
    return a;
}

inline const T& Matrix<R, C, T>::operator ()(int row, int col) const
{
    return this->_element[row * R + col];
}

inline T&  Matrix<R, C, T>::operator ()(int row, int col)
{
    return this->_element[row * R + col];
}

template <int R, int C, class T>
Matrix<C, R, T> Matrix<R, C, T>::transpose(void) const
{
    Matrix<C, R, T> a{};
    for(int i = 0; i < R; ++i)
    {
        for(int j = 0; j < C; ++j)
        {
            a(j, i) = (*this)(i, j);
        }
    }
    return a;
}

template <int R, int C, class T>
inline Matrix<R, C, T> Matrix<R, C, T>::operator +(void) const
{
    return (*this);
}

template <int R, int C, class T>
inline Matrix<R, C, T> Matrix<R, C, T>::operator -(void) const
{
    return Matrix<R, C, T>() - (*this);
}
       Matrix operator +(const Matrix&) const;
       Matrix operator -(const Matrix&) const;
template <int N>
       Matrix<C, N> operator *(const Matrix<N, R>&) const;
       Matrix operator *(const T&) const;
       Matrix operator /(const T&) const;
inline bool   operator ==(const Matrix&) const;
inline bool   operator !=(const Matrix&) const;
inline Matrix operator +=(const Matrix&);
inline Matrix operator -=(const Matrix&);
inline Matrix operator *=(const T&);
inline Matrix operator /=(const T&);
#endif /* MATRIX_HH */
