/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org/

  Copyright (c) 2000-2009 Torus Knot Software Ltd

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  -----------------------------------------------------------------------------
*/
#pragma once

#include <cassert>
#include <algorithm>
#include <iostream>
#include <GL/glew.h>

typedef float Real;

/** Class encapsulating a standard 4x4 homogeneous matrix.
    @remarks
    OGRE uses column vectors when applying matrix multiplications,
    This means a vector is represented as a single column, 4-row
    matrix. This has the effect that the transformations implemented
    by the matrices happens right-to-left e.g. if vector V is to be
    transformed by M1 then M2 then M3, the calculation would be
    M3 * M2 * M1 * V. The order that matrices are concatenated is
    vital since matrix multiplication is not commutative, i.e. you
    can get a different result if you concatenate in the wrong order.
    @par
    The use of column vectors and right-to-left ordering is the
    standard in most mathematical texts, and is the same as used in
    OpenGL. It is, however, the opposite of Direct3D, which has
    inexplicably chosen to differ from the accepted standard and uses
    row vectors and left-to-right matrix multiplication.
    @par
    OGRE deals with the differences between D3D and OpenGL etc.
    internally when operating through different render systems. OGRE
    users only need to conform to standard maths conventions, i.e.
    right-to-left matrix multiplication, (OGRE transposes matrices it
    passes to D3D to compensate).
    @par
    The generic form M * V which shows the layout of the matrix 
    entries is shown below:
    <pre>
    [ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
    | m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
    | m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
    [ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
    </pre>
*/
class Matrix4
{
protected:
  /// The matrix entries, indexed by [row][col].
  union {
    Real m[4][4];
    Real _m[16]; // This shit is stored transposed compared to OpenGL...
  };

public:
  /** Default constructor.
      @note
      It does <b>NOT</b> initialize the matrix for efficiency.
  */
  inline Matrix4()
  {
  }

  inline Matrix4(Real m00, Real m01, Real m02, Real m03,
		 Real m10, Real m11, Real m12, Real m13,
		 Real m20, Real m21, Real m22, Real m23,
		 Real m30, Real m31, Real m32, Real m33 )
  {
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
    m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
  }

  /** Exchange the contents of this matrix with another. 
   */
  inline void swap(Matrix4& other)
  {
    std::swap(m[0][0], other.m[0][0]);
    std::swap(m[0][1], other.m[0][1]);
    std::swap(m[0][2], other.m[0][2]);
    std::swap(m[0][3], other.m[0][3]);
    std::swap(m[1][0], other.m[1][0]);
    std::swap(m[1][1], other.m[1][1]);
    std::swap(m[1][2], other.m[1][2]);
    std::swap(m[1][3], other.m[1][3]);
    std::swap(m[2][0], other.m[2][0]);
    std::swap(m[2][1], other.m[2][1]);
    std::swap(m[2][2], other.m[2][2]);
    std::swap(m[2][3], other.m[2][3]);
    std::swap(m[3][0], other.m[3][0]);
    std::swap(m[3][1], other.m[3][1]);
    std::swap(m[3][2], other.m[3][2]);
    std::swap(m[3][3], other.m[3][3]);
  }

  inline Real* operator [] ( size_t iRow )
  {
    assert( iRow < 4 );
    return m[iRow];
  }

  inline const Real *operator [] ( size_t iRow ) const
  {
    assert( iRow < 4 );
    return m[iRow];
  }

  inline Matrix4 concatenate(const Matrix4 &m2) const
  {
    Matrix4 r;
    r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
    r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
    r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
    r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

    r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
    r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
    r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
    r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

    r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
    r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
    r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
    r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

    r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
    r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
    r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
    r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

    return r;
  }

  /** Matrix concatenation using '*'.
   */
  inline Matrix4 operator * ( const Matrix4 &m2 ) const
  {
    return concatenate( m2 );
  }

  /*
  inline Vector4 operator * (const Vector4& v) const
  {
    return Vector4(
		   m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
		   m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
		   m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
		   m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
		   );
  }
  */

  /** Matrix addition.
   */
  inline Matrix4 operator + ( const Matrix4 &m2 ) const
  {
    Matrix4 r;

    r.m[0][0] = m[0][0] + m2.m[0][0];
    r.m[0][1] = m[0][1] + m2.m[0][1];
    r.m[0][2] = m[0][2] + m2.m[0][2];
    r.m[0][3] = m[0][3] + m2.m[0][3];

    r.m[1][0] = m[1][0] + m2.m[1][0];
    r.m[1][1] = m[1][1] + m2.m[1][1];
    r.m[1][2] = m[1][2] + m2.m[1][2];
    r.m[1][3] = m[1][3] + m2.m[1][3];

    r.m[2][0] = m[2][0] + m2.m[2][0];
    r.m[2][1] = m[2][1] + m2.m[2][1];
    r.m[2][2] = m[2][2] + m2.m[2][2];
    r.m[2][3] = m[2][3] + m2.m[2][3];

    r.m[3][0] = m[3][0] + m2.m[3][0];
    r.m[3][1] = m[3][1] + m2.m[3][1];
    r.m[3][2] = m[3][2] + m2.m[3][2];
    r.m[3][3] = m[3][3] + m2.m[3][3];

    return r;
  }

  /** Matrix subtraction.
   */
  inline Matrix4 operator - ( const Matrix4 &m2 ) const
  {
    Matrix4 r;
    r.m[0][0] = m[0][0] - m2.m[0][0];
    r.m[0][1] = m[0][1] - m2.m[0][1];
    r.m[0][2] = m[0][2] - m2.m[0][2];
    r.m[0][3] = m[0][3] - m2.m[0][3];

    r.m[1][0] = m[1][0] - m2.m[1][0];
    r.m[1][1] = m[1][1] - m2.m[1][1];
    r.m[1][2] = m[1][2] - m2.m[1][2];
    r.m[1][3] = m[1][3] - m2.m[1][3];

    r.m[2][0] = m[2][0] - m2.m[2][0];
    r.m[2][1] = m[2][1] - m2.m[2][1];
    r.m[2][2] = m[2][2] - m2.m[2][2];
    r.m[2][3] = m[2][3] - m2.m[2][3];

    r.m[3][0] = m[3][0] - m2.m[3][0];
    r.m[3][1] = m[3][1] - m2.m[3][1];
    r.m[3][2] = m[3][2] - m2.m[3][2];
    r.m[3][3] = m[3][3] - m2.m[3][3];

    return r;
  }

  /** Tests 2 matrices for equality.
   */
  inline bool operator == ( const Matrix4& m2 ) const
  {
    if( 
       m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
       m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
       m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
       m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
      return false;
    return true;
  }

  /** Tests 2 matrices for inequality.
   */
  inline bool operator != ( const Matrix4& m2 ) const
  {
    if( 
       m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
       m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
       m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
       m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
      return true;
    return false;
  }

  inline Matrix4 transpose(void) const
  {
    return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
		   m[0][1], m[1][1], m[2][1], m[3][1],
		   m[0][2], m[1][2], m[2][2], m[3][2],
		   m[0][3], m[1][3], m[2][3], m[3][3]);
  }

  static const Matrix4 ZERO;
  static const Matrix4 IDENTITY;

  inline Matrix4 operator*(Real scalar) const
  {
    return Matrix4(
		   scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
		   scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
		   scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
		   scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
  }

  /** Function for writing to a stream.
   */
  inline friend std::ostream& operator <<
  ( std::ostream& o, const Matrix4& mat )
  {
    for (size_t i = 0; i < 4; ++i)
      {
	for(size_t j = 0; j < 4; ++j)
	  {
	    o << mat[i][j] << ", ";
	  }
	o << '\n';
      }
    o << "\n\n";
    return o;
  }

  void loadToGL() const {
    glLoadTransposeMatrixf(_m);
  }
  void multToGL() const {
    glMultTransposeMatrixf(_m);
  }
  void loadFromGL() {
    glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, _m);
  }
		
  Matrix4 adjoint() const;
  Real determinant() const;
  Matrix4 inverse() const;
};
