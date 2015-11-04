/**
 *	@file engine.cpp
 *	@brief Game engine.
 */

#include <stdio.h>
#include <unistd.h>

#include "definitions.h"
#include "engine/wiimote.h"
#include "engine/wiiuse.h"
#include "engine/Q3map.h"
#include "engine/engine.h"

/** Global Engine Instance */
EEngine g_engine;


EEngine::~EEngine() {
	shutdown();
}


/**
 *	@brief Initialize the game engine.
 *	@return 1 on success, 0 on failure
 */
int EEngine::init() {
	INFO("Initializing game engine...");
	initialized = 1;

	/* set the initial camera position */
	camera = new RCamera(50.0f, 0.1f, 5000.0f);
	camera->set_position(0, 0, -50);

	/* initialize the rendering engine */
	renderer = new RRender();
	if (!renderer->init(camera)) {
		delete renderer;
		delete camera;
		shutdown();
		return 0;
	}

	/* initialize the texture manager */
	texture_manager = new ETextureManager();
	texture_manager->init();

	/**** temp stuff ****/
		map = new EQ3Map();
		if (!map->load("data/q3dm1.bsp")) {
		//if (!map->load("data/q3dm17.bsp")) {
			shutdown();
			return 0;
		}
		renderer->set_map(map);

		vector3 pos;
		float angle;
		map->get_spawn_point(1, &angle, &pos);
		camera->set_position(pos.x, pos.y, pos.z);
		camera->rotate_hor(angle);
	/**** temp stuff ****/

	/* tell SDL to make key presses repeating */
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY/10, SDL_DEFAULT_REPEAT_INTERVAL);

	/* connect to a wiimote */
	if (!wiimote.init())
		shutdown();

	return 1;
}


/**
 *	@brief Shutdown the engine.
 */
void EEngine::shutdown() {
	if (!initialized)
		return;

	INFO("Shutting down game engine...");

	if (map) {
		delete map;
		map = NULL;
	}

	if (camera) {
		delete camera;
		camera = NULL;
	}

	if (renderer) {
		delete renderer;
		renderer = NULL;
	}

	if (texture_manager) {
		delete texture_manager;
		texture_manager = NULL;
	}

	initialized = 0;

	exit(0);
}


/**
 *	@brief Start the main engine loop.
 *	@return 1 on success, 0 on failure
 *
 *	The engine must first be initialized before the main loop may begin.
 */
int EEngine::exec() {
	/* make sure the engine was initialized */
	if (!initialized)
		return 0;

	INFO("Entering main engine loop...");

	while (1) {
		/* check if there are any pending events from SDL */
		check_sdl_events();

		/* check for anything from the wiimote */
		wiimote.poll();

		/* render the frame */
		renderer->render();

		/*
		 *	Just sleep enough to get the schedulers attention
		 *	that we don't want to hog resources
		 */
		//usleep(1);
	}
}


/**
 *	@brief Check for events from SDL
 */
void EEngine::check_sdl_events() {
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_VIDEORESIZE:
			{
				/* resize the window */
				renderer->resize_window(event.resize.w, event.resize.h);
				break;
			}
			case SDL_KEYDOWN:
			{
				/* key press */
				handle_key_press(&event);
				break;
			}
			case SDL_KEYUP:
			{
				/* key release */
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEMOTION:
			{
				/* mouse event */
				handle_mouse_event(&event);
				break;
			}
			case SDL_QUIT:
			{
				/* shutdown */
				shutdown();
				break;
			}
			default:
			{
			}
		}
	}

}


/**
 *	@brief Texture manager accesser.
 */
ETextureManager* EEngine::get_texture_manager() const {
	return texture_manager;
}


/**
 *	@brief Handle a key press event.
 *	@param e	The SDL event
 */
inline void EEngine::handle_key_press(SDL_Event* e) {
	if (e->key.keysym.sym == SDLK_w)
		camera->move_forward(10);
	else if (e->key.keysym.sym == SDLK_s)
		camera->move_backward(10);
	else if (e->key.keysym.sym == SDLK_a)
		camera->move_left(10);
	else if (e->key.keysym.sym == SDLK_d)
		camera->move_right(10);
	else if (e->key.keysym.sym == SDLK_ESCAPE)
		shutdown();
}



/**
 *	@brief Handle a mouse event.
 *	@param e	The SDL event
 */
void EEngine::handle_mouse_event(SDL_Event* e) {
	switch (e->type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			int pressed = (e->button.type == SDL_MOUSEBUTTONDOWN);

			switch (e->button.button) {
				case SDL_BUTTON_LEFT:
				{
					mouse.left_pressed = pressed;

#if 0
					if (pressed) {
						/* grab the mouse */
						SDL_WM_GrabInput(SDL_GRAB_ON);
					} else {
						/* release the mouse */
						SDL_WM_GrabInput(SDL_GRAB_OFF);
					}
#endif

					break;
				}
				case SDL_BUTTON_MIDDLE:
				{
					mouse.middle_pressed = pressed;
					break;
				}
				case SDL_BUTTON_RIGHT:
				{
					mouse.right_pressed = pressed;
					break;
				}
				case SDL_BUTTON_WHEELUP:
				{
					mouse.wheel_up = pressed;
					break;
				}
				case SDL_BUTTON_WHEELDOWN:
				{
					mouse.wheel_down = pressed;
					break;
				}
				default:
				{
					WARNING("Unknown mouse button pressed");
				}
			}

			/* update the xy position */
			mouse.x = e->button.x;
			mouse.y = e->button.y;

			break;
		}

		case SDL_MOUSEMOTION:
		{
			/* if left is pressed, rotate the camera */
			if (mouse.left_pressed) {
				//camera->rotate_hor((float)-(e->motion.xrel) / MOUSE_SENSITIVITY_SCALER);
				//camera->rotate_vert((float)(e->motion.yrel) / MOUSE_SENSITIVITY_SCALER);

				camera->rotate_hor((float)-(e->motion.x - mouse.x) / MOUSE_SENSITIVITY_SCALER);
				camera->rotate_vert((float)(e->motion.y - mouse.y) / MOUSE_SENSITIVITY_SCALER);

			}

			/* update the xy position */
			mouse.x = e->button.x;
			mouse.y = e->button.y;

			break;
		}

	}
}
