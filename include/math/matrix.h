#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include "math/mat.h"

class matrix4 {
	public:
		matrix4();
		matrix4(float a[16]);
		matrix4(float xx, float xy, float xz, float xd, float yx, float yy, float yz, float yd, float zx, float zy, float zz, float zd, float dx, float dy, float dz, float dd);

		const float* operator[](int x);

		matrix4 operator*(matrix4& b) const;

		void transpose();

	private:
		float m[4][4];
};



#endif // MATRIX_H_INCLUDED
