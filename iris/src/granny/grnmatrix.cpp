/*****
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****/

#include <math.h>
#include "granny/grnmatrix.h"

static float identity[16] = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1
};

GrnMatrix::GrnMatrix (float *f)
{
  for (int i = 0; i < 16; i++)
    matrix[i] = f[i];
}

GrnMatrix::GrnMatrix ()
{
  for (int i = 0; i < 16; i++)
    matrix[i] = identity[i];
}

GrnMatrix::~GrnMatrix ()
{
}

float &GrnMatrix::operator[] (int i)
{
  return matrix[i];
}


void GrnMatrix::invert ()
{
#define MAT(m,r,c) (m)[(c)*4+(r)]
#define m11 MAT(matrix,0,0)
#define m12 MAT(matrix,0,1)
#define m13 MAT(matrix,0,2)
#define m14 MAT(matrix,0,3)
#define m21 MAT(matrix,1,0)
#define m22 MAT(matrix,1,1)
#define m23 MAT(matrix,1,2)
#define m24 MAT(matrix,1,3)
#define m31 MAT(matrix,2,0)
#define m32 MAT(matrix,2,1)
#define m33 MAT(matrix,2,2)
#define m34 MAT(matrix,2,3)
#define m41 MAT(matrix,3,0)
#define m42 MAT(matrix,3,1)
#define m43 MAT(matrix,3,2)
#define m44 MAT(matrix,3,3)

  float det;
  float d12, d13, d23, d24, d34, d41;
  float tmp[16];

  d12 = (m31 * m42 - m41 * m32);
  d13 = (m31 * m43 - m41 * m33);
  d23 = (m32 * m43 - m42 * m33);
  d24 = (m32 * m44 - m42 * m34);
  d34 = (m33 * m44 - m43 * m34);
  d41 = (m34 * m41 - m44 * m31);
  tmp[0] = (m22 * d34 - m23 * d24 + m24 * d23);
  tmp[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
  tmp[2] = (m21 * d24 + m22 * d41 + m24 * d12);
  tmp[3] = -(m21 * d23 - m22 * d13 + m23 * d12);

  det = m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2] + m14 * tmp[3];

  if (det == 0.0)
      {
        for (int i = 0; i < 16; i++)
          matrix[i] = identity[i];
      }
  else
      {
        float invDet = 1.0 / det;
        tmp[0] *= invDet;
        tmp[1] *= invDet;
        tmp[2] *= invDet;
        tmp[3] *= invDet;

        tmp[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * invDet;
        tmp[5] = (m11 * d34 + m13 * d41 + m14 * d13) * invDet;
        tmp[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * invDet;
        tmp[7] = (m11 * d23 - m12 * d13 + m13 * d12) * invDet;

        d12 = m11 * m22 - m21 * m12;
        d13 = m11 * m23 - m21 * m13;
        d23 = m12 * m23 - m22 * m13;
        d24 = m12 * m24 - m22 * m14;
        d34 = m13 * m24 - m23 * m14;
        d41 = m14 * m21 - m24 * m11;

        tmp[8] = (m42 * d34 - m43 * d24 + m44 * d23) * invDet;
        tmp[9] = -(m41 * d34 + m43 * d41 + m44 * d13) * invDet;
        tmp[10] = (m41 * d24 + m42 * d41 + m44 * d12) * invDet;
        tmp[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * invDet;
        tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * invDet;
        tmp[13] = (m31 * d34 + m33 * d41 + m34 * d13) * invDet;
        tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * invDet;
        tmp[15] = (m31 * d23 - m32 * d13 + m33 * d12) * invDet;

        for (int i = 0; i < 16; i++)
          matrix[i] = tmp[i];
      }
#undef m11
#undef m12
#undef m13
#undef m14
#undef m21
#undef m22
#undef m23
#undef m24
#undef m31
#undef m32
#undef m33
#undef m34
#undef m41
#undef m42
#undef m43
#undef m44
#undef MAT
}

Point GrnMatrix::operator* (const Point & v)
{
  Point result;
  result.points[0] = (matrix[0] * v.points[0]) +
    (matrix[4] * v.points[1]) + (matrix[8] * v.points[2]) + matrix[12];
  result.points[1] = (matrix[1] * v.points[0]) +
    (matrix[5] * v.points[1]) + (matrix[9] * v.points[2]) + matrix[13];
  result.points[2] = (matrix[2] * v.points[0]) +
    (matrix[6] * v.points[1]) + (matrix[10] * v.points[2]) + matrix[14];
  return result;
}

void GrnMatrix::operator*= (const GrnMatrix & m)
{
  float tmp[16];
#define A(row,col) matrix[(col<<2)+row]
#define B(row,col) m.matrix[(col<<2)+row]
#define T(row,col) tmp[(col<<2)+row]
  int i;
  for (i = 0; i < 4; i++)
      {
        T (i, 0) =
          A (i, 0) * B (0, 0) + A (i, 1) * B (1, 0) + A (i, 2) * B (2,
                                                                    0) + A (i,
                                                                            3)
          * B (3, 0);
        T (i, 1) =
          A (i, 0) * B (0, 1) + A (i, 1) * B (1, 1) + A (i, 2) * B (2,
                                                                    1) + A (i,
                                                                            3)
          * B (3, 1);
        T (i, 2) =
          A (i, 0) * B (0, 2) + A (i, 1) * B (1, 2) + A (i, 2) * B (2,
                                                                    2) + A (i,
                                                                            3)
          * B (3, 2);
        T (i, 3) =
          A (i, 0) * B (0, 3) + A (i, 1) * B (1, 3) + A (i, 2) * B (2,
                                                                    3) + A (i,
                                                                            3)
          * B (3, 3);
      }
  for (i = 0; i < 16; i++)
    matrix[i] = tmp[i];
#undef A
#undef B
#undef T
}

void GrnMatrix::operator= (const GrnMatrix & m)
{
  for (int i = 0; i < 16; i++)
    matrix[i] = m.matrix[i];
}
