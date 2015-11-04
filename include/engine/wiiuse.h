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
 *
 *	@brief API header file.
 *
 *	If this file is included from inside the wiiuse source
 *	and not from a third party program, then wiimote_internal.h
 *	is also included which extends this file.
 */

#ifndef WIIUSE_H_INCLUDED
#define WIIUSE_H_INCLUDED

/* ignore this, this is used internally for wiiuse */
#ifdef __WIIUSE__
	#ifndef WIIUSE_INTERNAL_H_INCLUDED
		#error wiiuse.h included directly.  Must include wiiuse_internal.h instead.
	#endif
#endif

/* include the bluetooth stuff */
#include <bluetooth/bluetooth.h>

/* wiiuse version and API version */
#define WIIUSE_VERSION					0.1
#define WIIUSE_API_VERSION				0.1

/* led bit masks */
#define WIIMOTE_LED_NONE				0x00
#define WIIMOTE_LED_1					0x10
#define WIIMOTE_LED_2					0x20
#define WIIMOTE_LED_3					0x40
#define WIIMOTE_LED_4					0x80

/* button codes */
#define WIIMOTE_BUTTON_TWO				0x0001
#define WIIMOTE_BUTTON_ONE				0x0002
#define WIIMOTE_BUTTON_B				0x0004
#define WIIMOTE_BUTTON_A				0x0008
#define WIIMOTE_BUTTON_MINUS			0x0010
#define WIIMOTE_BUTTON_ZACCEL_BIT6		0x0020
#define WIIMOTE_BUTTON_ZACCEL_BIT7		0x0040
#define WIIMOTE_BUTTON_HOME				0x0080
#define WIIMOTE_BUTTON_LEFT				0x0100
#define WIIMOTE_BUTTON_RIGHT			0x0200
#define WIIMOTE_BUTTON_DOWN				0x0400
#define WIIMOTE_BUTTON_UP				0x0800
#define WIIMOTE_BUTTON_PLUS				0x1000
#define WIIMOTE_BUTTON_ZACCEL_BIT4		0x2000
#define WIIMOTE_BUTTON_ZACCEL_BIT5		0x4000
#define WIIMOTE_BUTTON_UNKNOWN			0x8000

/**
 *	@brief Check if a button is pressed.
 *	@param pressed	The buttons currently pressed.
 *	@param button	The button you are interested in.
 *	@return 1 if the button is pressed, 0 if not.
 */
#define IS_PRESSED(pressed, button)		((pressed & button) == button)

/**
 *	@brief Check if a button is being held.
 *	@param pressed	The buttons currently pressed.
 *	@param button	The button you are interested in.
 *	@return 1 if the button is held, 0 if not.
 */
#define IS_HELD(wm, button)				((wm->buttons_held & button) == button)

/**
 *	@brief Check if a button is released on this event.					\n\n
 *			This does not mean the button is not pressed, it means		\n
 *			this button was just now released.
 *	@param pressed	The buttons currently pressed.
 *	@param button	The button you are interested in.
 *	@return 1 if the button is released, 0 if not.
 *
 */
#define IS_RELEASED(wm, button)			((wm->buttons_released & button) == button)

/*
 *	Largest known payload is 21 bytes.
 *	Add 2 for the prefix and round up to a power of 2.
 */
#define MAX_PAYLOAD			32

typedef unsigned char byte;
typedef char sbyte;

struct wiimote_t;
struct vec3b_t;
struct orient_t;
struct gforce_t;

/**
 *	@brief Event callback.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param btns		What buttons are currently pressed.  They are OR'ed together.
 *	@param accel	Acceleration of the device along each axis.
 *					This is the raw data reported by the wiimote.
 *	@param orient	Orientation (roll, pitch, yaw) of the device.
 *	@param gforce	Pull of gravity on each axis of the device (measured in gravity units).
 *
 *	@see wiimote_init()
 *
 *	A registered function of this type is called automatically by the wiiuse
 *	library when an event occurs on the specified wiimote.
 */
typedef void (*wiimote_event_cb)(struct wiimote_t* wm, int btns, struct vec3b_t accel, struct orient_t orient, struct gforce_t gforce);

/**
 *	@brief Callback that handles a read event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param data		Pointer to the filled data block.
 *	@param len		Length in bytes of the data block.
 *
 *	@see wiimote_init()
 *
 *	A registered function of this type is called automatically by the wiiuse
 *	library when the wiimote has returned the full data requested by a previous
 *	call to wiimote_read_data().
 */
typedef void (*wiimote_read_cb)(struct wiimote_t* wm, byte* data, unsigned short len);

/**
 *	@brief Callback that handles a controller status event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *	@param attachment		Is there an attachment? (1 for yes, 0 for no)
 *	@param speaker			Is the speaker enabled? (1 for yes, 0 for no)
 *	@param ir				Is the IR support enabled? (1 for yes, 0 for no)
 *	@param led				What LEDs are lit.
 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *	@see wiimote_init()
 *
 *	A registered function of this type is called automatically by the wiiuse
 *	library when either the controller status changed or the controller
 *	status was requested explicitly by wiimote_status().
 */
typedef void (*wiimote_ctrl_status_cb)(struct wiimote_t* wm, int attachment, int speaker, int ir, int led[4], float battery_level);

/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	@see wiimote_init()
 *
 *	A registered function of this type is called automatically by the wiiuse
 *	library when a disconnection occurs.  This can happen if the POWER button
 *	is pressed or if the connection is interrupted.
 */
typedef void (*wiimote_dis_cb)(struct wiimote_t* wm);


/**
 *	@struct read_req_t
 *	@brief Data read request structure.
 */
struct read_req_t {
	wiimote_read_cb cb;			/**< read data callback						*/
	byte* buf;					/**< buffer where read data is written		*/
	unsigned short offset;		/**< the offset that the read started at	*/
	unsigned short size;		/**< the length of the data read			*/
	unsigned short wait;		/**< num bytes still needed to finish read	*/

	struct read_req_t* next;	/**< next read request in the queue			*/
};


/**
 *	@struct vec3b_t
 *	@brief Unsigned x,y,z byte vector.
 */
typedef struct vec3b_t {
	byte x, y, z;
} vec3b_t;


/**
 *	@struct vec3f_t
 *	@brief Signed x,y,z float struct.
 */
typedef struct vec3f_t {
	float x, y, z;
} vec3f_t;


/**
 *	@struct orient_t
 *	@brief Orientation struct.
 */
typedef struct orient_t {
	float roll;
	float pitch;
	float yaw;
} orient_t;


/**
 *	@struct gforce_t
 *	@brief Gravity force struct.
 */
typedef struct gforce_t {
	float x, y, z;
} gforce_t;


/**
 *	@struct wiimote_t
 *	@brief Wiimote structure.
 */
typedef struct wiimote_t {
	int unid;						/**< user specified id						*/
	bdaddr_t bdaddr;				/**< bt address								*/
	char bdaddr_str[16];			/**< readable bt address					*/
	int state;						/**< various state flags					*/
	int out_sock;					/**< output socket)							*/
	int in_sock;					/**< input socket 							*/
	int leds;						/**< currently lit leds						*/

	wiimote_event_cb event_cb;		/**< event callback							*/
	wiimote_dis_cb dis_cb;			/**< disconnect callback					*/
	wiimote_ctrl_status_cb stat_cb;	/**< controller status callback				*/

	byte handshake_state;			/**< the state of the connection handshake	*/

	struct read_req_t* read_req;	/**< list of data read requests				*/

	struct vec3b_t cal_zero;		/**< wiimote zero calibration				*/
	struct vec3b_t cal_g;			/**< wiimote gravity calibration			*/

	byte event[MAX_PAYLOAD];		/**< event buffer							*/

	unsigned int buttons_held;		/**< what buttons are being held down		*/
	unsigned int buttons_released;	/**< what buttons were released this event	*/
} wiimote;




/*****************************************
 *
 *	Include API specific stuff
 *
 *****************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* wiiuse.c */
typedef struct wiimote_t** (*_wiimote_init_fptr)(int wiimotes, int* unids, wiimote_event_cb event_cb, wiimote_ctrl_status_cb stat_cb, wiimote_dis_cb dis_cb);
typedef void (*_wiimote_disconnected_fptr)(struct wiimote_t* wm);
typedef void (*_wiimote_rumble_fptr)(struct wiimote_t* wm, int status);
typedef void (*_wiimote_toggle_rumble_fptr)(struct wiimote_t* wm);
typedef void (*_wiimote_set_leds_fptr)(struct wiimote_t* wm, int leds);
typedef void (*_wiimote_motion_sensing_fptr)(struct wiimote_t* wm, int status);
typedef int (*_wiimote_read_data_fptr)(struct wiimote_t* wm, wiimote_read_cb read_cb, byte* buffer, unsigned int offset, unsigned short len);
typedef void (*_wiimote_status_fptr)(struct wiimote_t* wm);
typedef struct wiimote_t* (*_wiimote_get_by_id_fptr)(struct wiimote_t** wm, int wiimotes, int unid);

/* connect.c */
typedef int (*_wiimote_find_fptr)(struct wiimote_t** wm, int max_wiimotes, int timeout);
typedef int (*_wiimote_connect_fptr)(struct wiimote_t** wm, int wiimotes);
typedef void (*_wiimote_disconnect_fptr)(struct wiimote_t* wm);

/* events.c */
typedef void (*_wiimote_poll_fptr)(struct wiimote_t** wm, int wiimotes);

#ifdef __cplusplus
}
#endif

/**
 *	@struct wiiuse_api_t
 *	@brief API structure that is filled by the library
 *			when the entry point is invoked.
 *
 *	Unless you are modifying wiiuse, you probably
 *	will not be interested in any members
 *	that begin with an underscore (_).
 */
struct wiiuse_api_t {
	float version;					/**< wiiuse version		*/
	float api_version;				/**< wiiuse API version	*/

	_wiimote_init_fptr				_wiimote_init;
	_wiimote_disconnected_fptr		_wiimote_disconnected;
	_wiimote_rumble_fptr			_wiimote_rumble;
	_wiimote_toggle_rumble_fptr		_wiimote_toggle_rumble;
	_wiimote_set_leds_fptr			_wiimote_set_leds;
	_wiimote_motion_sensing_fptr	_wiimote_motion_sensing;
	_wiimote_read_data_fptr			_wiimote_read_data;
	_wiimote_status_fptr			_wiimote_status;
	_wiimote_get_by_id_fptr			_wiimote_get_by_id;

	_wiimote_find_fptr				_wiimote_find;
	_wiimote_connect_fptr			_wiimote_connect;
	_wiimote_disconnect_fptr		_wiimote_disconnect;

	_wiimote_poll_fptr				_wiimote_poll;
};

#ifndef __WIIUSE__

/*
 *	Operating system dependent macros.
 */
#ifdef __WIN32__
	#define dlopen(file, x)         (void*)LoadLibrary(file)
	#define dlsym(dll, func)        (void*)GetProcAddress((HMODULE)(dll), (func))
	#define dlclose(dll)            FreeLibrary((HMODULE)(dll))

	char* _dlerror();
	#define dlerror()				_dlerror()
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* api/wiiuse.c */
float wiiuse_startup(char* wiiuse_file);
void wiiuse_shutdown();

extern _wiimote_init_fptr				wiimote_init;
extern _wiimote_disconnected_fptr		wiimote_disconnected;
extern _wiimote_rumble_fptr				wiimote_rumble;
extern _wiimote_toggle_rumble_fptr		wiimote_toggle_rumble;
extern _wiimote_set_leds_fptr			wiimote_set_leds;
extern _wiimote_motion_sensing_fptr		wiimote_motion_sensing;
extern _wiimote_read_data_fptr			wiimote_read_data;
extern _wiimote_status_fptr				wiimote_status;
extern _wiimote_get_by_id_fptr			wiimote_get_by_id;

extern _wiimote_find_fptr				wiimote_find;
extern _wiimote_connect_fptr			wiimote_connect;
extern _wiimote_disconnect_fptr			wiimote_disconnect;

extern _wiimote_poll_fptr				wiimote_poll;

#ifdef __cplusplus
}
#endif

#endif /* __WIIUSE__ */




#endif /* WIIUSE_H_INCLUDED */
