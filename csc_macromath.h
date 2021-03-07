/*
SPDX-License-Identifier: GPL-2.0
SPDX-FileCopyrightText: 2021 Johan Söderlind Åström <johan.soderlind.astrom@gmail.com>
*/
#pragma once

//Matrix can be stored in column major or row major.
//Column major means that a continuous array is a column vector.
//Row major means that a continuous array is a row vector.
#define MAJR(w, r, c) (((w)*(r)) + (c))
#define MAJC(w, r, c) (((w)*(c)) + (r))
#define MAJX MAJC
#define DIMENSION(x) x



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
