#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

/** Global Engine Instance */
extern class EEngine g_engine;

#include "engine/wiimote.h"
#include "render/render.h"
#include "engine/texture_manager.h"
#include "engine/map.h"
#include "engine/mouse.h"

/**
 *	@file engine.h
 *	@brief Game engine.
 */

#define MOUSE_SENSITIVITY_SCALER		5.0f

/**
 *	@class EEngine
 *	@brief The game engine
 */
class EEngine {
	public:
		~EEngine();

		int init();
		void shutdown();

		int exec();
		void check_sdl_events();

		ETextureManager* get_texture_manager() const;
		EWiimote wiimote;

	private:
		void handle_key_press(SDL_Event* e);
		void handle_mouse_event(SDL_Event* e);

		RRender* renderer;
		RCamera* camera;						/* default camera */
		ETextureManager* texture_manager;
		EMouse mouse;

		int initialized;

		EMap* map;
};


#endif // ENGINE_H_INCLUDED
