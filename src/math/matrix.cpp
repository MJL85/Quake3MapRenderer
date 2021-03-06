#include <stdio.h>
#include <math.h>
#include <string.h>

#include "math/matrix.h"

matrix4::matrix4() {
}


matrix4::matrix4(float a[16]) {
	m[0][0] = a[0];	m[1][0] = a[4];		m[2][0] = a[8];		m[3][0] = a[12];
	m[0][1] = a[1];	m[1][1] = a[5];		m[2][1] = a[9];		m[3][1] = a[13];
	m[0][2] = a[2];	m[1][2] = a[6];		m[2][2] = a[10];	m[3][2] = a[14];
	m[0][3] = a[3];	m[1][3] = a[7];		m[2][3] = a[11];	m[3][3] = a[15];
}


matrix4::matrix4(float xx, float xy, float xz, float xd, float yx, float yy, float yz, float yd, float zx, float zy, float zz, float zd, float dx, float dy, float dz, float dd) {
	m[0][0] = xx;	m[1][0] = yx;		m[2][0] = zx;		m[3][0] = dx;
	m[0][1] = xy;	m[1][1] = yy;		m[2][1] = zy;		m[3][1] = dy;
	m[0][2] = xz;	m[1][2] = yz;		m[2][2] = zz;		m[3][2] = dz;
	m[0][3] = xd;	m[1][3] = yd;		m[2][3] = zd;		m[3][3] = dd;
}


const float* matrix4::operator[](int x) {
	return &m[x][0];
}


matrix4 matrix4::operator*(matrix4& b) const {
	return matrix4(

		(m[0][0] * b[0][0]) + (m[0][1] * b[1][0]) + (m[0][2] * b[2][0]) + (m[0][3] * b[3][0]),
		(m[0][0] * b[0][1]) + (m[0][1] * b[1][1]) + (m[0][2] * b[2][1]) + (m[0][3] * b[3][1]),
		(m[0][0] * b[0][2]) + (m[0][1] * b[1][2]) + (m[0][2] * b[2][2]) + (m[0][3] * b[3][2]),
		(m[0][0] * b[0][3]) + (m[0][1] * b[1][3]) + (m[0][2] * b[2][3]) + (m[0][3] * b[3][3]),

		(m[1][0] * b[0][0]) + (m[1][1] * b[1][0]) + (m[1][2] * b[2][0]) + (m[1][3] * b[3][0]),
		(m[1][0] * b[0][1]) + (m[1][1] * b[1][1]) + (m[1][2] * b[2][1]) + (m[1][3] * b[3][1]),
		(m[1][0] * b[0][2]) + (m[1][1] * b[1][2]) + (m[1][2] * b[2][2]) + (m[1][3] * b[3][2]),
		(m[1][0] * b[0][3]) + (m[1][1] * b[1][3]) + (m[1][2] * b[2][3]) + (m[1][3] * b[3][3]),

		(m[2][0] * b[0][0]) + (m[2][1] * b[1][0]) + (m[2][2] * b[2][0]) + (m[2][3] * b[3][0]),
		(m[2][0] * b[0][1]) + (m[2][1] * b[1][1]) + (m[2][2] * b[2][1]) + (m[2][3] * b[3][1]),
		(m[2][0] * b[0][2]) + (m[2][1] * b[1][2]) + (m[2][2] * b[2][2]) + (m[2][3] * b[3][2]),
		(m[2][0] * b[0][3]) + (m[2][1] * b[1][3]) + (m[2][2] * b[2][3]) + (m[2][3] * b[3][3]),

		(m[3][0] * b[0][0]) + (m[3][1] * b[1][0]) + (m[3][2] * b[2][0]) + (m[3][3] * b[3][0]),
		(m[3][0] * b[0][1]) + (m[3][1] * b[1][1]) + (m[3][2] * b[2][1]) + (m[3][3] * b[3][1]),
		(m[3][0] * b[0][2]) + (m[3][1] * b[1][2]) + (m[3][2] * b[2][2]) + (m[3][3] * b[3][2]),
		(m[3][0] * b[0][3]) + (m[3][1] * b[1][3]) + (m[3][2] * b[2][3]) + (m[3][3] * b[3][3])

	);
}


void matrix4::transpose() {
	float a[4][4];

	a[0][0] = m[0][0];	a[0][1] = m[1][0];	a[0][2] = m[2][0];	a[0][3] = m[3][0];
	a[1][0] = m[0][1];	a[1][1] = m[1][1];	a[1][2] = m[2][1];	a[1][3] = m[3][1];
	a[2][0] = m[0][2];	a[2][1] = m[1][2];	a[2][2] = m[2][2];	a[2][3] = m[3][2];
	a[3][0] = m[0][3];	a[3][1] = m[1][3];	a[3][2] = m[2][3];	a[3][3] = m[3][3];

	memcpy(m, a, sizeof(a));
}
