/*
 *	wiiuse
 *
 *	Written By:
 *		Michael Laforest	< para >
 *		Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *	Copyright 2005
 *
 *	This file is part of wiiuse.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *	$Header$
 *
 */

/**
 *	@file
 *	@brief API source file
 *
 *	The file must be linked to any third party
 *	program that is utilizing wiiuse as an
 *	external library.
 */

#include <stdio.h>
#include "engine/wiiuse.h"

#ifdef __WIN32__
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

typedef int (*entry_func_t)(struct wiiuse_api_t**);

struct wiiuse_api_t* wiiuse_api = NULL;
void* wiiuse_mod = NULL;

/* check if versions are equal - floats can be slightly off */
#define CHECK_VERSIONS_EQUAL(a, b)		((a == b) ? 1 : ((a > b) ? ((a - b) > 0.01f) : ((b - a) > 0.0001f)))

/**
 *	@brief Load the wiiuse library and initialize the function pointers.
 *
 *	@param wiiuse_file	The relative or absolute path to the wiiuse library file.
 *
 *	@return The version of the wiiuse library loaded.
 *
 *	@see wiiuse_shutdown()
 *
 *	If the version of wiiuse being used has a different API
 *	version as expected, this function will fail and return 0.
 */
float wiiuse_startup(char* wiiuse_file) {
	entry_func_t entry_func = NULL;

	if (!wiiuse_file)
		return 0;

	if (wiiuse_api)
		/* already loaded */
		return 0;

	/* load the module */
	wiiuse_mod = dlopen(wiiuse_file, RTLD_NOW);

	if (!wiiuse_mod)
		/* can not load module */
		return 0;

	/* get the entry point */
	entry_func = (entry_func_t)dlsym(wiiuse_mod, "wiiuse_main");

	if (!entry_func) {
		wiiuse_shutdown();
		return 0;
	}

	/* call the entry function */
	entry_func(&wiiuse_api);

	/* make sure the API versions are the same */
	if (CHECK_VERSIONS_EQUAL(wiiuse_api->api_version, WIIUSE_API_VERSION)) {
		wiiuse_shutdown();
		return 0;
	}

	/* set all the function pointers */
	wiimote_init			= wiiuse_api->_wiimote_init;
	wiimote_disconnected	= wiiuse_api->_wiimote_disconnected;
	wiimote_rumble			= wiiuse_api->_wiimote_rumble;
	wiimote_toggle_rumble	= wiiuse_api->_wiimote_toggle_rumble;
	wiimote_set_leds		= wiiuse_api->_wiimote_set_leds;
	wiimote_motion_sensing	= wiiuse_api->_wiimote_motion_sensing;
	wiimote_read_data		= wiiuse_api->_wiimote_read_data;
	wiimote_status			= wiiuse_api->_wiimote_status;
	wiimote_get_by_id		= wiiuse_api->_wiimote_get_by_id;
	wiimote_find			= wiiuse_api->_wiimote_find;
	wiimote_connect			= wiiuse_api->_wiimote_connect;
	wiimote_disconnect		= wiiuse_api->_wiimote_disconnect;
	wiimote_poll			= wiiuse_api->_wiimote_poll;

	return wiiuse_api->version;
}


/**
 *	@brief Unload the library.
 *
 *	@see wiiuse_startup()
 */
void wiiuse_shutdown() {
	if (!wiiuse_mod)
		return;

	/* unload the module */
	dlclose(wiiuse_mod);

	wiiuse_api = NULL;

	wiimote_init			= NULL;
	wiimote_disconnected	= NULL;
	wiimote_rumble			= NULL;
	wiimote_toggle_rumble	= NULL;
	wiimote_set_leds		= NULL;
	wiimote_motion_sensing	= NULL;
	wiimote_read_data		= NULL;
	wiimote_status			= NULL;
	wiimote_find			= NULL;
	wiimote_connect			= NULL;
	wiimote_disconnect		= NULL;
	wiimote_poll			= NULL;

}
