#include <stdio.h>
#include <math.h>

#include "math/vector.h"

vector3::vector3() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
}


vector3::vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}


void vector3::set(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}


void vector3::normalize() {
	float s = sqrt(x*x + y*y + z*z);
	x /= s;
	y /= s;
	z /= s;
}


float vector3::dot(vector3* b) {
	return ((x * b->x) + (y * b->y) + (z * b->z));
}


vector3 vector3::cross(vector3* b) {
	float cx, cy, cz;

	cx = ((y * b->z) - (b->y * z));
	cy = ((-x * b->z) + (z * b->x));
	cz = ((x * b->y) - (y * b->x));

	return vector3(cx, cy, cz);
}


vector3 vector3::operator+(vector3& b) {
	return vector3(x+b.x, y+b.y, z+b.z);
}


vector3 vector3::operator-(vector3& b) {
	return vector3(x-b.x, y-b.y, z-b.z);
}


vector3 vector3::operator*(vector3& b) {
	return vector3(x*b.x, y*b.y, z*b.z);
}


vector3 vector3::operator/(vector3& b) {
	return vector3(x/b.x, y/b.y, z/b.z);
}


void vector3::operator=(vector3& b) {
	x = b.x;
	y = b.y;
	z = b.z;
}


void vector3::operator+=(vector3& b) {
	x += b.x;
	y += b.y;
	z += b.z;
}


void vector3::operator-=(vector3& b) {
	x -= b.x;
	y -= b.y;
	z -= b.z;
}


void vector3::operator*=(vector3& b) {
	x *= b.x;
	y *= b.y;
	z *= b.z;
}


void vector3::operator/=(vector3& b) {
	x /= b.x;
	y /= b.y;
	z /= b.z;
}
