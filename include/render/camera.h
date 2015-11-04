#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "math/mat.h"
#include "math/vector.h"

#define R_CAMERA_DEFAULT_FOV			50.0f
#define R_CAMERA_DEFAULT_ZNEAR			0.1f
#define R_CAMERA_DEFAULT_ZFAR			1500.0f

class RCamera {
	friend class RRender;

	public:
		RCamera(float fov = R_CAMERA_DEFAULT_FOV, float znear = R_CAMERA_DEFAULT_ZNEAR, float zfar = R_CAMERA_DEFAULT_ZFAR);
		~RCamera();

		void set_viewable_area(float Fov, float zNear, float zFar);

		void toggle_third_person(int enabled, float radius = 1);

		void set_position(float x, float y, float z);
		void set_direction(float x, float y, float z);

		void get_position(vector3* vec);
		void get_direction(vector3* vec);

		void move(float x, float y, float z, float factor);
		void move_forward(float factor);
		void move_backward(float factor);
		void move_left(float factor);
		void move_right(float factor);

		void rotate_vert(float angle);
		void rotate_hor(float angle);
		void rotate_vert_abs(float angle);
		void rotate_hor_abs(float angle);

		void update();

		int is_point_visable(float x, float y, float z);
		int is_box_visable(float x1, float y1, float z1, float x2, float y2, float z2);

	private:
		void update_direction();
		void update_frustum();

		float fov;				/* field of view	*/
		float znear;			/* near clipping	*/
		float zfar;				/* far clipping		*/

		int third_person;
		float third_person_radius;

		vector3 pos;		/* XYZ position		*/
		vector3 dir;		/* vector direction	- updated when rotations change */
		vector3 up;		/* the up vector	*/

		float psi_rot;			/* up/down			*/
		float theta_rot;		/* left/right		*/

		float frustum[6][4];	/* frustum clipping planes */
};

#endif // CAMERA_H_INCLUDED
