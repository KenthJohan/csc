#pragma once
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

/*

Column major:
double a[] =
{
	1.0, 2.0,//Column 0
	3.0, 4.0,//Column 1
};

(1.0 2.0) is column 0
(3.0 4.0) is column 1

true matrix form:
	1.0 3.0
	2.0 4.0

Matrix have rn*cn elements.
Vector have rn*cn elements where cn = 1.

Convention, decleare scalar starting with s:
	double sx;
Convention, decleare vector starting with v:
	double vx[2];
Convention, declare matrix starting with m:
	double mx[2*2]

*/



/**
 * @brief lin_v_nan_index Find the index of the first NaN element of vector \p v
 * @param v Input vector
 * @param n Numner of elements
 * @return (-1) or index of the first NaN element
 */
static int lin_v_nan_index (double const v[], unsigned n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		if (isnan (v [i]))
		{
			return (int)i;
		}
	}
	return -1;
}


static void lin_print_internal (double const ma[], unsigned rn, unsigned cn, unsigned rr, unsigned cc, char const * format, char const * rowstr)
{
	for (unsigned r = 0; r < rn; ++r)
	{
		for (unsigned c = 0; c < cn; ++c)
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
			printf (format, ma [cc*c + rr*r]);
#pragma GCC diagnostic pop
		}
		fputs (rowstr, stdout);
	}
}


static void lin_print (double const ma[], unsigned rn, unsigned cn, char const * format, char const * rowstr)
{
	lin_print_internal (ma, rn, cn, 1, rn, format, rowstr);
}


static void lin_print_t (double const ma[], unsigned rn, unsigned cn, char const * format, char const * rowstr)
{
	lin_print_internal (ma, cn, rn, cn, 1, format, rowstr);
}


/**
 * @brief lin_vs_macc Multiply and accumulate vector \p va by scalar \p sb.
 * @param vy Output vector
 * @param vx Input left side vector
 * @param sb Input right side scalar
 * @param n Input number of elements in vector \p vy and vector \p vx
 */
static void lin_vs_macc (double vy[], double const vx[], double sb, unsigned n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		vy [i] += vx [i] * sb;
	}
}


/**
 * @brief lin_vv_dot Dot product of two vectors
 * @param va Input vector
 * @param vb Input vector
 * @param n Input number of elements in vector \p va, \p vb
 * @return The sum of product
 */
static double lin_vv_dot (double const va[], double const vb[], unsigned n)
{
	double sum = 0;
	for (unsigned i = 0; i < n; ++i)
	{
		sum += va [i] * vb [i];
	}
	return sum;
}


/**
 * @brief lin_mv_mul Multiply matrix \p ma by vector \p vx.
 * @param vy Output vector
 * @param ma Input left side matrix
 * @param vx Input right side vector
 * @param rn Input number of rows in matrix \p ma and number of elements in vector \p vy
 * @param cn Input number of columns in matrix \p ma and number of elements in vector \p vx
 */
static void lin_mv_mul (double vy[], double const ma[], double const vx[], unsigned rn, unsigned cn)
{
	//TODO: this memset causes bad training?
	memset (vy, 0, rn*sizeof (double));
	for (unsigned i = 0; i < cn; ++i)
	{
		double const * vcolumn = ma + rn * i;
		lin_vs_macc (vy, vcolumn, vx [i], rn);
	}
}


/**
 * @brief lin_mv_mul_t Multiply transposed matrix \p ma by vector \p vx.
 * @param vy Output vector
 * @param ma Input left side matrix
 * @param vx Input right side vector
 * @param rn Number of rows in matrix \p ma and number of elements in vector \p vx
 * @param cn Number of columns in matrix \p ma and number of elements in vector \p vy
 */
static void lin_mv_mul_t (double vy[], double const ma[], double const vx[], unsigned rn, unsigned cn)
{
	memset (vy, 0, cn*sizeof (double));
	for (unsigned i = 0; i < cn; ++i)
	{
		vy [i] = lin_vv_dot (ma + rn * i, vx, rn);
	}
}


/**
 * @brief lin_vv_hadamard
 * @param vy Output vector
 * @param va Input vector
 * @param vb Input vector
 * @param n Input number of elements in vector \p vy, \p va, \p vb
 */
static void lin_vv_hadamard (double vy[], double const va[], double const vb[], unsigned n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		vy[i] = va[i] * vb[i];
	}
}


/**
 * @brief lin_vv_sub Subtract two vectors
 * @param vy Output vector
 * @param va Input vector
 * @param vb Input vector
 * @param n Input number of elements in vector \p vy, \p va, \p vb
 */
static void lin_vv_sub (double vy[], double const va[], double const vb[], unsigned n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		vy[i] = va[i] - vb[i];
	}
}


/**
 * @brief lin_v_fx
 * @param vy Output vector
 * @param vx Input vector
 * @param f(x) Input function that is invoked for each element
 * @param n Number of elements in vector \p vy and \p vx
 */
static void lin_v_fx (double vy[], double const vx[], double (*f)(double x), unsigned n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		vy [i] = f (vx [i]);
	}
}



/**
 * @brief lin_v_f Set function \p f return value for every element is \p vy
 * @param vy Output vector
 * @param f() Input function that is invoked for each element
 * @param n Number of elements in vector \p vy and \p vx
 */
static void lin_v_f (double vy[], double (*f)(void), unsigned n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		vy [i] = f ();
	}
}



static double sigmoid (double x)
{
	double r = 1.0 / (1.0 + exp (-x));
	return r;
}


static double sigmoid_pd (double x)
{
	x = x * (1.0 - x);
	return x;
}


static double lin_rnd ()
{
	double r = rand() / (double)RAND_MAX;
	r = r - 0.5;
	return r;
}


void lin_test_mv_mul ()
{
#define RN 3
#define CN 2
	double ma[RN*CN] =
	{
	1.0, 2.0, 3.0,//Column 0
	4.0, 5.0, 6.0,//Column 1
	};
	double vx[CN] = {1.0, 10.0};
	double vy[RN] = {0};
	lin_mv_mul (vy, ma, vx, RN, CN);
	lin_print (ma, RN, CN, "% 3.1f", "\n"); printf ("\n");
	lin_print (vx, CN, 1, "% 3.1f", "\n"); printf ("\n");
	lin_print (vy, RN, 1, "% 3.1f", "\n"); printf ("\n");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
	assert (vy[0] == (vx[0]*ma[0]+vx[1]*ma[3]));
	assert (vy[1] == (vx[0]*ma[1]+vx[1]*ma[4]));
	assert (vy[2] == (vx[0]*ma[2]+vx[1]*ma[5]));
#pragma GCC diagnostic pop
	assert (vy[0] == 41.0);
	assert (vy[1] == 52.0);
	assert (vy[2] == 63.0);
#undef RN
#undef CN
}


void lin_test_mv_mul_t ()
{
#define RN 3
#define CN 2
	double ma[RN*CN] =
	{
	1.0, 2.0, 3.0,//Column 0
	4.0, 5.0, 6.0,//Column 1
	};
	double vx1[RN] = {1.0, 10.0, 100.0};
	double vy1[CN] = {0};
	lin_mv_mul_t (vy1, ma, vx1, RN, CN);
	lin_print_t (ma, RN, CN, "% 3.1f", "\n"); printf ("\n");
	lin_print_t (vx1, RN, 1, "% 3.1f", "\n"); printf ("\n");
	lin_print_t (vy1, CN, 1, "% 3.1f", "\n"); printf ("\n");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
	assert (vy1[0] == (vx1[0]*ma[0] + vx1[1]*ma[1] + vx1[2]*ma[2]));
	assert (vy1[1] == (vx1[0]*ma[3] + vx1[1]*ma[4] + vx1[2]*ma[5]));
#pragma GCC diagnostic pop
	assert (vy1[0] == 321.0);
	assert (vy1[1] == 654.0);
#undef RN
#undef CN
}
























#define VN_HADAMARD_PRODUCT(n, r, a, b)\
{\
	for (size_t _i = 0; _i < (n); _i++)\
		(r)[_i] = (a)[_i] * (b)[_i];\
}


//y := y + M v
#define MN_MAC(w, h, y, m, v)\
{\
	for (size_t _r = 0; _r < h; _r = _r + 1)\
	for (size_t _c = 0; _c < w; _c = _c + 1)\
	{\
		(y) [_r] += (m) [MAJX (w, _r, _c)] * (v) [_c];\
	}\
}


//y := y + M^T v
#define MN_MAC_TRANSPOSE(w, h, y, mt, v)\
{\
	for (size_t _r = 0; _r < h; _r = _r + 1)\
	for (size_t _c = 0; _c < w; _c = _c + 1)\
	{\
		(y) [_r] += (mt) [MAJX (w, _c, _r)] * (v) [_c];\
	}\
}


#define MM_MUL(w, m, a, b)\
{\
	for (size_t _c = 0; _c < w; _c = _c + 1)\
	for (size_t _r = 0; _r < w; _r = _r + 1)\
	{\
		(m) [MAJX (w, _r, _c)] = 0.0f;\
		for(size_t _k = 0; _k < w; _k = _k + 1)\
			(m) [MAJX (w, _r, _c)] += (a) [MAJX (w, _r, _k)] * (b) [MAJX (w, _k, _c)];\
	}\
}


// Clear vector
// (v := {x | x = 0})
#define VN_CLR(n, v) \
{\
	for (size_t _i = 0; _i < (n); _i++)\
		(v)[_i] = 0;\
}
















//Matrix can be stored in column major or row major.
//Column major means that a continuous array is a column vector.
//Row major means that a continuous array is a row vector.
#define MAJR(w, r, c) (((w)*(r)) + (c))
#define MAJC(w, r, c) (((w)*(c)) + (r))
#define MAJX MAJC





#define V4_SUM(v, s)\
{\
	(s) = 0;\
	for (size_t _i = 0; _i < 4; _i++)\
		(s) += (v) [_i]\
}


#define V4_DOT(s, u, v)\
{\
	(s) = 0;\
	for (size_t _i = 0; _i < 4; _i++)\
		(s) += (u)[_i] * (v)[_i];\
}


#define V4_MUL_SCALAR(r, v, s)\
{\
	for (size_t _i = 0; _i < 4; _i++)\
		(r)[_i] = (v)[_i] * (s);\
}


// Clear vector
// (v := {x | x = 0})
#define V4_CLR(v) \
{\
	for (size_t _i = 0; _i < 4; _i++)\
		(v)[_i] = 0;\
}


//Translation vector
#define M4_TX 12
#define M4_T0 12
#define M4_TY 13
#define M4_T1 13
#define M4_TZ 14
#define M4_T2 14
#define M4_TW 15
#define M4_T3 15

//Column vectors
#define M4_V0 0
#define M4_VX 0
#define M4_V1 4
#define M4_VY 4
#define M4_V2 8
#define M4_VZ 8
#define M4_V3 12
#define M4_VT 12


#define M4_IDENTITY(m)\
{\
	for (size_t _i = 0; _i < 4; _i++)\
	for (size_t _j = 0; _j < 4; _j++)\
		(m)[_i*4 + _j] = (_i == _j ? 1.0 : 0.0);\
}


#define M4_PRINT(m, fmt)\
{\
	for (size_t _r = 0; _r < 4; _r++)\
	{\
		for (size_t _c = 0; _c < 4; _c++)\
			printf (fmt, (m) [MAJX(4, _r, _c)]);\
		printf ("\n");\
	}\
}


//M = A*B
#define M4_MUL_CPY(m, a, b)\
{\
	for (size_t _c = 0; _c < 4; _c = _c + 1)\
	for (size_t _r = 0; _r < 4; _r = _r + 1)\
	{\
		(m) [MAJX(4, _r, _c)] = 0.0f;\
		for(size_t _k = 0; _k < 4; _k = _k + 1)\
			(m) [MAJX(4, _r, _c)] += (a) [MAJX(4, _r, _k)] * (b) [MAJX(4, _k, _c)];\
	}\
}


#define M4_MAC(m, a, b)\
{\
	for (size_t _c = 0; _c < 4; _c = _c + 1)\
	for (size_t _r = 0; _r < 4; _r = _r + 1)\
	for (size_t _k = 0; _k < 4; _k = _k + 1)\
		(m) [MAJX(4, _r, _c)] += (a) [MAJX(4, _r, _k)] * (b) [MAJX(4, _k, _c)];\
}


//y := y + M^T v
#define M4_MAC_TRANSPOSE(y, mt, v)\
{\
	for (size_t _r = 0; _r < 4; _r = _r + 1)\
	for (size_t _c = 0; _c < 4; _c = _c + 1)\
	{\
		(y) [_r] += (mt) [MAJX (4, _c, _r)] * (v) [_c];\
	}\
}


#define M4_ROTX(m, a)\
(m)[5] = cos (a);\
(m)[6] = sin (a);\
(m)[9] = -sin (a);\
(m)[10] = cos (a);\

#define M4_ROTY(m, a)\
(m)[0] = cos (a);\
(m)[2] = -sin (a);\
(m)[8] = sin (a);\
(m)[10] = cos (a);\

#define M4_ROTZ(m, a)\
(m)[0] = cos (a);\
(m)[1] = sin (a);\
(m)[4] = -sin (a);\
(m)[5] = cos (a);\


#define V4_SET(v,x,y,z,w)\
{\
(v)[0] = x;\
(v)[1] = y;\
(v)[2] = z;\
(v)[3] = w;\
}\


#define M4_FRUSTUM_NZ(m, l, r, b, t, n, f)\
(m)[0] = (2*(n))/((r)-(l));\
(m)[5] = (2*(n))/((t)-(b));\
(m)[8] = ((r)+(l))/((r)-(l));\
(m)[9] = ((t)+(b))/((t)-(b));\
(m)[10] = (-(f)-(n))/((f)-(n));\
(m)[11] = -1;\
(m)[14] = (-2*(f)*(n))/((f)-(n));\


static void m4f_perspective (float m [4*4], float fov, float aspect, float near, float far)
{
	float tangent = tan ((M_PI/180.0f) * (fov / 2.0f));
	float height = near * tangent;
	float width = height * aspect;
	M4_FRUSTUM_NZ (m, -width, width, -height, height, near, far);
}


static void m4f_mul (float m [4*4], float a [4*4], float b [4*4])
{
	float t [4*4];
	M4_MUL_CPY (t, a, b);
	memcpy (m, t, sizeof (t));
}


void v4f_normalize (float r [4], float v [4])
{
	float sum;
	V4_DOT (sum, v, v);
	if (sum == 0) return;
	sum = 1.0f / sqrtf (sum);
	//printf ("s %f\n", sum);
	V4_MUL_SCALAR (r, v, sum);
}






