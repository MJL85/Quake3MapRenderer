#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL/SDL.h>

#include "definitions.h"
#include "render/camera.h"
#include "engine/map.h"

/**
 *	@file render.h
 *	@brief Renderer
 */

#define DEFAULT_MAX_FPS			100

/**
 *	@class RRender
 *	@brief Rendering class.
 */
class RRender {
	friend class EEngine;

	public:
		~RRender();

		int init(RCamera* cam);
		void set_camera(RCamera* cam);
		void set_map(EMap* m);
		void set_max_fps(unsigned int max);

		void resize_window(int new_width, int new_height);

		void render();

	private:
		inline float calculate_framerate();
		inline int fps_can_render();

		RCamera* camera;
		EMap* map;

		int width;
		int height;

		unsigned int max_fps;			/* maximum frame rate to render at		*/
		unsigned long fps_usec;			/* when the next second occurs			*/
		unsigned int fps_frames;		/* current frames for this second		*/
		float fps;						/* number of frames from last second	*/
};

#endif // RENDERER_H_INCLUDED
