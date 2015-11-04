#include <math.h>
#include <assert.h>
#include "definitions.h"
#include "math/mat.h"


/*
 *	Convert theta and psi rotations
 *	to spherical coordinates and store
 *	int x, y, z.
 *
 *	x, y, z are in degrees
 */
void spherical_coords_deg(float theta, float psi, float radius, float* x, float* y, float* z) {
	theta = RADIAN(theta);
	psi = RADIAN(psi);

	*x = radius * sin(theta) * sin(psi);
	*y = radius * cos(psi);
	*z = radius * cos(theta) * sin(psi);
}


/*
 *	Convert theta and psi rotations
 *	to spherical coordinates and store
 *	int x, y, z.
 *
 *	x, y, z are in radians
 */
void spherical_coords_rad(float theta, float psi, float radius, float* x, float* y, float* z) {
	*x = radius * sin(theta) * sin(psi);
	*y = radius * cos(psi);
	*z = radius * cos(theta) * sin(psi);
}


/*
 *	Swap coordinate system for x,y,z.
 *	left->right hand
 *	right->left hand
 */
void swizzle_coords_f(float* x, float* y, float* z) {
	if (!x || !y || !z)
		return;
	SWAP(float, *y, *z);

}


/*
 *	Swap coordinate system for x,y,z.
 *	left->right hand
 *	right->left hand
 */
void swizzle_coords_i(int* x, int* y, int* z) {
	if (!x || !y || !z)
		return;
	SWAP(int, *y, *z);
}


/*
 *	Normalize a plane defined by the four points.
 */
void normalize_plane(float* a, float* b, float* c, float* d) {
	assert(a && b && c && d);

	float mag = sqrt((*a * *a) + (*b * *b) + (*c * *c));

	*a /= mag;
	*b /= mag;
	*c /= mag;
	*d /= mag;
}

