#ifndef MAT_H_INCLUDED
#define MAT_H_INCLUDED

#define PI						3.1415926535898
#define RADIAN(deg)				(deg * (PI / 180.0))

#define FLOAT_MOD(i, m)          ((i) - ((int)((i) / (m)) * (m)))

/*
 *	Keep the the value i between lower and upper.
 */
#define RANGE_BOUND(i, lower, upper)	do {						\
											if (i > upper)			\
												i = upper;			\
											else if (i < lower)		\
												i = lower;			\
										} while (0)

/*
 *	Swap two variables of the given type.
 *
 *	ex:
 *		float x = 10, y = 20;
 *		SWAP(float, x, y);
 */
#define SWAP(type, x, y)		do {				\
									type t = x;		\
									x = y;			\
									y = t;			\
								} while (0)

#ifdef __cplusplus
extern "C"
{
#endif

void spherical_coords_deg(float theta, float psi, float radius, float* x, float* y, float* z);
void spherical_coords_rad(float theta, float psi, float radius, float* x, float* y, float* z);

void swizzle_coords_f(float* x, float* y, float* z);
void swizzle_coords_i(int* x, int* y, int* z);

void normalize_plane(float* a, float* b, float* c, float* d);

#ifdef __cplusplus
}
#endif

#endif // MAT_H_INCLUDED
