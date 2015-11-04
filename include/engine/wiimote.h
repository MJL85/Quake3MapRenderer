#ifndef WIIMOTE_H_INCLUDED
#define WIIMOTE_H_INCLUDED

#include "engine/wiiuse.h"

/**
 *	@file wiimote.h
 *	@brief Wiimote wrapper for wiiuse.
 */

/* location of wiiuse library module */
#define WIIUSE_LIB_PATH					"/home/para/Projects/wii/wiiuse/src/wii.so"
#define WIIMOTE_ID_1					1

#define ROLL_SCALAR			3.0f
#define PITCH_SCALAR		3.0f

/**
 *	@class EWiimote
 *	@brief Wiimote object. Only one is needed for all wiimotes.
 */
class EWiimote {
	public:
		EWiimote();
		~EWiimote();

		int init();
		void poll();

		float get_roll();
		float get_pitch();

		void set_roll(float r);
		void set_pitch(float p);

		int is_connected();

	private:
		wiimote** wm;

		int connected;

		float roll;
		float pitch;
};

#endif /* WIIMOTE_H_INCLUDED */
