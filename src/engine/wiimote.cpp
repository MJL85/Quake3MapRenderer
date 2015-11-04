/**
 *	@file wiimote.cpp
 *	@brief Wiimote wrapper for wiiuse.
 */

#include <stdio.h>
#include <unistd.h>

#include "definitions.h"
#include "engine/engine.h"
#include "engine/wiiuse.h"
#include "engine/wiimote.h"

static void handle_event(struct wiimote_t* wm, int btns, vec3b_t accel, orient_t orient, gforce_t gforce);
static void handle_read(struct wiimote_t* wm, byte* data, unsigned short len);
static void handle_ctrl_status(struct wiimote_t* wm, int attachment, int speaker, int ir, int led[4], float battery_level);
static void handle_disconnect(wiimote* wm);

#ifdef __cplusplus
extern "C" {
#endif

_wiimote_init_fptr				wiimote_init;
_wiimote_disconnected_fptr		wiimote_disconnected;
_wiimote_rumble_fptr			wiimote_rumble;
_wiimote_toggle_rumble_fptr		wiimote_toggle_rumble;
_wiimote_set_leds_fptr			wiimote_set_leds;
_wiimote_motion_sensing_fptr	wiimote_motion_sensing;
_wiimote_read_data_fptr			wiimote_read_data;
_wiimote_status_fptr			wiimote_status;
_wiimote_get_by_id_fptr			wiimote_get_by_id;

_wiimote_find_fptr				wiimote_find;
_wiimote_connect_fptr			wiimote_connect;
_wiimote_disconnect_fptr		wiimote_disconnect;

_wiimote_poll_fptr				wiimote_poll;

#ifdef __cplusplus
}
#endif


EWiimote::EWiimote() {
	roll = 0.0f;
	pitch = 0.0f;
	connected = 0;
}


EWiimote::~EWiimote() {
	connected = 0;
	wiimote_disconnect(wm[0]);
	wiiuse_shutdown();
}


/**
 *	@brief Initialize the wiimote object.
 *	@return 1 on success, 0 on failure
 */
int EWiimote::init() {
	INFO("Initializing wiimote...");

	float ver = wiiuse_startup(WIIUSE_LIB_PATH);
	if (!ver) {
		ERROR("WIIMOTE: Failed to load wiiuse library, \"" WIIUSE_LIB_PATH "\".");
		return 0;
	}

	int ids[] = { WIIMOTE_ID_1 };

	wm = wiimote_init(1, ids, handle_event, handle_ctrl_status, handle_disconnect);

	int found = wiimote_find(wm, 1, 5);
	if (!found) {
		ERROR("WIIMOTE: Did not find any wiimotes to connect to.");
		wiiuse_shutdown();
		return 0;
	}

	connected = wiimote_connect(wm, 1);
	if (connected)
		INFO("WIIMOTE: Connected to %i wiimotes (of %i found).", connected, found);
	else {
		INFO("WIIMOTE: Failed to connect to any wiimote.");
		wiiuse_shutdown();
		return 0;
	}

	wiimote_set_leds(wm[0], WIIMOTE_LED_1 | WIIMOTE_LED_4);
	wiimote_rumble(wm[0], 1);
	usleep(200000);
	wiimote_rumble(wm[0], 0);

	connected = 1;
	return 1;
}


/**
 *	@brief Poll for any events on the wiimotes.
 */
void EWiimote::poll() {
	wiimote_poll(wm, 1);
}


/**
 *	@brief Return the roll in degrees.
 */
float EWiimote::get_roll() {
	return roll;
}


/**
 *	@brief Return the pitch in degrees.
 */
float EWiimote::get_pitch() {
	return pitch;
}


/**
 *	@brief Set the roll in degrees.
 */
void EWiimote::set_roll(float r) {
	roll = r;
}


/**
 *	@brief Set the pitch in degrees.
 */
void EWiimote::set_pitch(float p) {
	pitch = p;
}



/*
 *	@brief Return 1 if the wiimote is connectd, 0 if not.
 */
int EWiimote::is_connected() {
	return connected;
}



/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param btns		What buttons are currently pressed.  They are OR'ed together.
 *	@param accel	Acceleration of the device along each axis.
 *					This is the raw data reported by the wiimote.
 *	@param orient	Orientation (roll, pitch, yaw) of the device.
 *	@param gforce	Pull of gravity on each axis of the device (measured in gravity units).
 *
 *	This function is called automatically by the wiiuse library when an
 *	event occurs on the specified wiimote.
 */
static void handle_event(struct wiimote_t* wm, int btns, vec3b_t accel, orient_t orient, gforce_t gforce) {
	printf("\n\n--- EVENT [wiimote id %i] ---\n", wm->unid);
	if (btns) {
		/* if a button is pressed, report it */
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_A))		printf("A pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_B))		printf("B pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_UP))	printf("UP pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_DOWN))	printf("DOWN pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_LEFT))	printf("LEFT pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_RIGHT))	printf("RIGHT pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_MINUS))	printf("MINUS pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_PLUS))	printf("PLUS pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_ONE))	printf("ONE pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_TWO))	printf("TWO pressed\n");
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_HOME))	printf("HOME pressed\n");

		/*
		 *	Pressing minus will tell the wiimote we are no longer interested in movement.
		 *	This is useful because it saves battery power.
		 */
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_MINUS))
			wiimote_motion_sensing(wm, 0);

		/*
		 *	Pressing plus will tell the wiimote we are interested in movement.
		 */
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_PLUS))
			wiimote_motion_sensing(wm, 1);

		/*
		 *	Pressing B will toggle the rumble
		 *
		 *	if B is pressed but is not held, toggle the rumble
		 */
		if (IS_PRESSED(btns, WIIMOTE_BUTTON_B) && !IS_HELD(wm, WIIMOTE_BUTTON_B))
			wiimote_toggle_rumble(wm);
	}

	printf("Zero:\tX=%i\tY=%i\tZ=%i\n", wm->cal_zero.x, wm->cal_zero.y, wm->cal_zero.z);
	printf("  1g:\tX=%i\tY=%i\tZ=%i\n", wm->cal_g.x, wm->cal_g.y, wm->cal_g.z);
	printf(" Raw:\tX=%i\tY=%i\tZ=%i\n", accel.x, accel.y, accel.z);

	printf("gforce x: %f\n", gforce.x);
	printf("gforce y: %f\n", gforce.y);
	printf("gforce z: %f\n", gforce.z);

	printf("roll  = %f\n", orient.roll);
	printf("pitch = %f\n", orient.pitch);
	printf("yaw   = %f\n", orient.yaw);



	float roll = g_engine.wiimote.get_roll();
	float pitch = g_engine.wiimote.get_pitch();

	roll = (roll + orient.roll) / 2;
	pitch = (pitch + orient.pitch) / 2;

	roll /= ROLL_SCALAR;
	pitch /= PITCH_SCALAR;

	g_engine.wiimote.set_roll(roll);
	g_engine.wiimote.set_pitch(pitch);
}


/**
 *	@brief Callback that handles a read event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param data		Pointer to the filled data block.
 *	@param len		Length in bytes of the data block.
 *
 *	This function is called automatically by the wiiuse library when
 *	the wiimote has returned the full data requested by a previous
 *	call to wiimote_read_data().
 *
 *	You can read data on the wiimote, such as Mii data, if
 *	you know the offset address and the length.
 *
 *	The \a data pointer was specified on the call to wiimote_read_data().
 *	At the time of this function being called, it is not safe to deallocate
 *	this buffer.
 */
static void handle_read(struct wiimote_t* wm, byte* data, unsigned short len) {
	printf("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
	printf("finished read of size %i\n", len);
	int i = 0;
	for (; i < len; ++i) {
		if (!(i%16))
			printf("\n");
		printf("%x ", data[i]);
	}
	printf("\n\n");
}


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
 *	This occurs when either the controller status changed
 *	or the controller status was requested explicitly by
 *	wiimote_status().
 *
 *	One reason the status can change is if the nunchuk was
 *	inserted or removed from the expansion port.
 */
static void handle_ctrl_status(struct wiimote_t* wm, int attachment, int speaker, int ir, int led[4], float battery_level) {
	printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	printf("attachment:      %i\n", attachment);
	printf("speaker:         %i\n", speaker);
	printf("ir:              %i\n", ir);
	printf("led 1:           %i\n", led[0]);
	printf("led 2:           %i\n", led[1]);
	printf("led 3:           %i\n", led[2]);
	printf("led 4:           %i\n", led[3]);
	printf("battery:         %f %%\n", battery_level);
}


/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
static void handle_disconnect(wiimote* wm) {
	printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}
