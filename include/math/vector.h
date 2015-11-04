#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

class vector3 {
	public:
		vector3();
		vector3(float x, float y, float z);

		void set(float x, float y, float z);

		void normalize();
		float dot(vector3* b);
		vector3 cross(vector3* b);

		vector3 operator+(vector3& b);
		vector3 operator-(vector3& b);
		vector3 operator*(vector3& b);
		vector3 operator/(vector3& b);

		void operator=(vector3& b);
		void operator+=(vector3& b);
		void operator-=(vector3& b);
		void operator*=(vector3& b);
		void operator/=(vector3& b);

		float x, y, z;
};

#endif // VECTOR_H_INCLUDED
