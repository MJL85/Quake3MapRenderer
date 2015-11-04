#include "definitions.h"
#include "math/mat.h"
#include "gl.h"
#include "render/camera.h"

#include "math/matrix.h"

/**
 *	By default the camera is in 1st person mode.
 */
RCamera::RCamera(float fov, float znear, float zfar) {
	this->fov = fov;
	this->znear = znear;
	this->zfar = zfar;

	third_person = 0;
	third_person_radius = 10.0f;

	pos.x = 0.0f;
	pos.y = 0.0f;
	pos.z = 0.0f;

	up.x = 0;
	up.y = 1;
	up.z = 0;

	psi_rot = 90.0f;
	theta_rot = 0.0f;
}


RCamera::~RCamera() {
}


/**
 *	@brief Set the viewable area
 *	@param Fov		The field of view ratio
 *	@param zNear	Where to clip on the near z-axis
 *	@param zFar		Where to clip on the far z-axis
 */
void RCamera::set_viewable_area(float Fov, float zNear, float zFar) {
	fov = Fov;
	znear = zNear;
	zfar = zFar;
}


/**
 *	@brief Toggle third person.
 *
 *	@param enabled	If 1 then third person is enabled
 *	@param radius	Distance between the focal point and the camera
 *
 *	TODO - When xEntity is implemented, 3rd person should center
 *			on a specified entity
 *			or
 *			camera should be able to fixate on a specific coordinate
 *			(currently only the origin)
 */
void RCamera::toggle_third_person(int enabled, float radius) {
	third_person = enabled;
	third_person_radius = radius;
}


/**
 *	@brief Set the absolute position of the camera
 *	@param x X coordinate
 *	@param y Y coordinate
 *	@param z Z coordinate
 */
void RCamera::set_position(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}


/**
 *	@brief Set the directional vector
 *	@param x X coordinate
 *	@param y Y coordinate
 *	@param z Z coordinate
 */
void RCamera::set_direction(float x, float y, float z) {
	dir.x = x;
	dir.y = y;
	dir.z = z;
}


/**
 *	@brief Vertically rotate the camera
 *	@param angle	The rotation angle in degrees
 */
void RCamera::rotate_vert(float angle) {
	psi_rot += angle;
	if (third_person)
		RANGE_BOUND(psi_rot, -180.0f, 179.9f);
		if (psi_rot < 0)
			psi_rot = 0.01f;
	else {
		psi_rot = FLOAT_MOD(psi_rot, 360.0);
		if (psi_rot < 0)
			psi_rot = 360.0f - psi_rot;
	}
}


/**
 *	@brief Horizontally rotate the camera
 *	@param angle	The rotation angle in degrees
 */
void RCamera::rotate_hor(float angle) {
	theta_rot = FLOAT_MOD(theta_rot + angle, 360.0);
	if (theta_rot < 0)
		theta_rot = 360.0f - theta_rot;
}


/**
 *	@brief Absolute vertical rotation
 *	@param angle	The rotation angle in degrees
 *
 *	Rotation is not from current direction but from origin.
 */
void RCamera::rotate_vert_abs(float angle) {
	psi_rot = angle;
	if (third_person)
		RANGE_BOUND(psi_rot, -90.0f, 90.0f);
	else {
		psi_rot = FLOAT_MOD(psi_rot, 360.0);
		if (psi_rot < 0)
			psi_rot = 360.0f - psi_rot;
	}
}


/**
 *	@brief Absolute horizontal rotation
 *	@param angle	The rotation angle in degrees
 *
 *	Rotation is not from current direction but from origin.
 */
void RCamera::rotate_hor_abs(float angle) {
	theta_rot = FLOAT_MOD(angle, 360.0);
	if (theta_rot < 0)
		theta_rot = 360.0f - theta_rot;
}


/**
 *	@brief Apply the current camera rotation.
 */
void RCamera::update_direction() {
	if (third_person)
		spherical_coords_deg(theta_rot, psi_rot, third_person_radius, &pos.x, &pos.y, &pos.z);
	else
		spherical_coords_deg(theta_rot, psi_rot, 1.0f, &dir.x, &dir.y, &dir.z);

	/* normalize the vectors */
	dir.normalize();
}


/**
 *	@brief Get current camera position
 *	@param vec	The position is set in the pointer to the vector
 */
void RCamera::get_position(vector3* vec) {
	if (!vec)
		return;
	vec->x = pos.x;
	vec->y = pos.y;
	vec->z = pos.z;
}


/**
 *	@brief Get current camera looking direction
 *	@param vec	The position is set in the pointer to the vector
 */
void RCamera::get_direction(vector3* vec) {
	if (!vec)
		return;
	vec->x = dir.x;
	vec->y = dir.y;
	vec->z = dir.z;
}


/**
 *	@brief Set the camera position for the ModelView matrix.
 */
void RCamera::update() {
	update_direction();

	if (third_person) {
		gluLookAt(pos.x, pos.y, pos.z,
					0, 0, 0,
					up.x, up.y, up.z
		);
	} else {
		gluLookAt(pos.x, pos.y, pos.z,
					pos.x + dir.x,
					pos.y + dir.y,
					pos.z + dir.z,
					up.x, up.y, up.z
		);
	}

	update_frustum();
}


/**
 *	@brief Move the camera along the x,y,z vector by the given factor.
 *	@param x 		X direction
 *	@param y 		Y direction
 *	@param z 		Z direction
 *	@param factor	How many units to move along the vector (scales x/y/z)
 */
void RCamera::move(float x, float y, float z, float factor) {
	pos.x += x * factor;
	pos.y += y * factor;
	pos.z += z * factor;
}


/**
 *	@brief Moves the camera forward
 *	@param factor	How much to move by
 */
void RCamera::move_forward(float factor) {
	move(dir.x, dir.y, dir.z, factor);
}


/**
 *	@brief Moves the camera backward
 *	@param factor	How much to move by
 */
void RCamera::move_backward(float factor) {
	move(dir.x, dir.y, dir.z, -factor);
}


/**
 *	@brief Move left - strafe.
 *	@param factor	How much to move by
 */
void RCamera::move_left(float factor) {
	vector3 v = dir.cross(&up);
	v.normalize();
	move(v.x, v.y, v.z, -factor);
}


/**
 *	@brief Move right - strafe.
 *	@param factor	How much to move by
 */
void RCamera::move_right(float factor) {
	vector3 v = dir.cross(&up);
	v.normalize();
	move(v.x, v.y, v.z, factor);
}


/**
 *	@brief Update the frustum clipping planes.
 */
void RCamera::update_frustum() {
	float pm[16];
	float mvm[16];

	/* get the projection matrix */
	glGetFloatv(GL_PROJECTION_MATRIX, pm);

	/* get the model view matrix */
	glGetFloatv(GL_MODELVIEW_MATRIX, mvm);

	matrix4 m_pm(pm);
	matrix4 m_mvm(mvm);

	matrix4 clip = m_mvm * m_pm;

	clip.transpose();

	frustum[0][0] = clip[3][0] - clip[0][0];
	frustum[0][1] = clip[3][1] - clip[0][1];
	frustum[0][2] = clip[3][2] - clip[0][2];
	frustum[0][3] = clip[3][3] - clip[0][3];
	normalize_plane(&frustum[0][0], &frustum[0][1], &frustum[0][2], &frustum[0][3]);

	frustum[1][0] = clip[3][0] + clip[0][0];
	frustum[1][1] = clip[3][1] + clip[0][1];
	frustum[1][2] = clip[3][2] + clip[0][2];
	frustum[1][3] = clip[3][3] + clip[0][3];
	normalize_plane(&frustum[1][0], &frustum[1][1], &frustum[1][2], &frustum[1][3]);

	frustum[2][0] = clip[3][0] + clip[1][0];
	frustum[2][1] = clip[3][1] + clip[1][1];
	frustum[2][2] = clip[3][2] + clip[1][2];
	frustum[2][3] = clip[3][3] + clip[1][3];
	normalize_plane(&frustum[2][0], &frustum[2][1], &frustum[2][2], &frustum[2][3]);

	frustum[3][0] = clip[3][0] - clip[1][0];
	frustum[3][1] = clip[3][1] - clip[1][1];
	frustum[3][2] = clip[3][2] - clip[1][2];
	frustum[3][3] = clip[3][3] - clip[1][3];
	normalize_plane(&frustum[3][0], &frustum[3][1], &frustum[3][2], &frustum[3][3]);

	frustum[4][0] = clip[3][0] - clip[2][0];
	frustum[4][1] = clip[3][1] - clip[2][1];
	frustum[4][2] = clip[3][2] - clip[2][2];
	frustum[4][3] = clip[3][3] - clip[2][3];
	normalize_plane(&frustum[4][0], &frustum[4][1], &frustum[4][2], &frustum[4][3]);

	frustum[5][0] = clip[3][0] + clip[2][0];
	frustum[5][1] = clip[3][1] + clip[2][1];
	frustum[5][2] = clip[3][2] + clip[2][2];
	frustum[5][3] = clip[3][3] + clip[2][3];
	normalize_plane(&frustum[5][0], &frustum[5][1], &frustum[5][2], &frustum[5][3]);
}


/**
 *	@brief Check if the specified point is within the viewing frustum
 *	@return Returns 1 on success, 0 on failure
 */
int RCamera::is_point_visable(float x, float y, float z) {
	int i = 0;

	for(; i < 6; ++i) {
		if ((frustum[i][0] * x) + (frustum[i][1] * y) + (frustum[i][2] * z) + frustum[i][3] <= 0)
			return 0;
	}

	return 1;
}


/**
 *	@brief Check if the specified box is within the viewing frustum
 *	@return Returns 1 on success, 0 on failure
 */
int RCamera::is_box_visable(float x1, float y1, float z1, float x2, float y2, float z2) {
	int i = 0;

	for(; i < 6; i++ ) {
		if ((((frustum[i][0] * x1) + (frustum[i][1] * y1) + (frustum[i][2] * z1) + frustum[i][3]) > 0) ||
			(((frustum[i][0] * x2) + (frustum[i][1] * y1) + (frustum[i][2] * z1) + frustum[i][3]) < 0) ||
			(((frustum[i][0] * x1) + (frustum[i][1] * y2) + (frustum[i][2] * z1) + frustum[i][3]) > 0) ||
			(((frustum[i][0] * x2) + (frustum[i][1] * y2) + (frustum[i][2] * z1) + frustum[i][3]) > 0) ||
			(((frustum[i][0] * x1) + (frustum[i][1] * y1) + (frustum[i][2] * z2) + frustum[i][3]) > 0) ||
			(((frustum[i][0] * x2) + (frustum[i][1] * y1) + (frustum[i][2] * z2) + frustum[i][3]) > 0) ||
			(((frustum[i][0] * x1) + (frustum[i][1] * y2) + (frustum[i][2] * z2) + frustum[i][3]) > 0) ||
			(((frustum[i][0] * x2) + (frustum[i][1] * y2) + (frustum[i][2] * z2) + frustum[i][3]) > 0))
				continue;

		return 0;
	}

	return 1;
}
