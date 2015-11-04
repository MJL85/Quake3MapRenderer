#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

/**
 *	@file map.h
 *	@brief Abstract map
 */

/**
 *	@class EMap
 *	@brief Abstract map class.
 */
class EMap {
	public:
		EMap() {};
		virtual ~EMap() {};

		/**
		 *	@brief Load the given map.
		 *	@return 1 on success, 0 on failure
		 */
		virtual int load(char* file) = 0;

		/**
		 *	@brief Render the given map at the specified camera position.
		 */
		virtual void render(RCamera* camera) = 0;

		/**
		 *	@brief Return where the camera should be initially positioned.
		 */
		virtual int get_spawn_point(int index, float* angle, vector3* position) = 0;

	protected:

	private:
};

#endif // MAP_H_INCLUDED
