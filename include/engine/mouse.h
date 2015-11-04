#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

class EMouse {
	public:
		EMouse() {
			x = 0;
			y = 0;

			left_pressed = 0;
			right_pressed = 0;
			middle_pressed = 0;

			wheel_up = 0;
			wheel_down = 0;
		}

		int x, y;

		int left_pressed;
		int right_pressed;
		int middle_pressed;

		int wheel_up;
		int wheel_down;
};

#endif // MOUSE_H_INCLUDED
