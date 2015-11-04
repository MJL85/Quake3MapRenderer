#include <stdio.h>

#include <sys/time.h>

#include "definitions.h"
#include "engine/engine.h"
#include "render/render.h"
#include "gl.h"

void test_draw();


RRender::~RRender() {
	INFO("Shutting down renderer...");

	/* shut down SDL */
	INFO("Shutting down SDL...");
	SDL_Quit();
}


/**
 *	@brief Initialize the renderer.
 *	@param cam	Camera to render from, can be NULL if no camera exists yet
 *	@return 1 on success, 0 on failure
 */
int RRender::init(RCamera* cam) {
	INFO("Initializing renderer...");

	camera = cam;
	width = 640;
	height = 480;

	fps_usec = 0;
	fps_frames = 0;
	fps = 0.0f;
	max_fps = DEFAULT_MAX_FPS;

	map = NULL;

	/* initialize SDL */
	INFO("Initializing SDL...");
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ERROR("Failed to initialize SDL: %s", SDL_GetError());
		return 0;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	/* set window size */
	//SDL_Surface* screen =
	SDL_SetVideoMode(width, height, 16, SDL_RESIZABLE | SDL_OPENGL);

	/* set OpenGL stuff */
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* set the size of the window */
	resize_window(width, height);

	return 1;
}


/**
 *	@brief Set the camera to render from
 *	@param cam	Pointer to a RCamera object, can be NULL
 */
void RRender::set_camera(RCamera* cam) {
	camera = cam;
}


/**
 *	@brief Set the map to render
 *	@param map	Pointer to a EMap object, can be NULL
 */
void RRender::set_map(EMap* m) {
	map = m;
}


/**
 *	@brief Set the maximum frames per second to render.
 *	@param max	Number of frames per second.
 */
void RRender::set_max_fps(unsigned int max) {
	max_fps = max;
}


/**
 *	@brief Resize the rendering window
 *	@param new_width	The new width of the window
 *	@param new_height	The new height of the window
 */
void RRender::resize_window(int new_width, int new_height) {
	width = new_width;
	height = new_height;

	/* SDL invalidates the OpenGL context on a resize, must remake it */
	SDL_SetVideoMode(width, height, 16, SDL_RESIZABLE | SDL_OPENGL);

	/* setup viewport */
	glViewport(0, 0, width, height);

	/* setup the projection matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->fov, ((float)width / (float)height), camera->znear, camera->zfar);

	/* switch back and initialize the model view matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/**
 *	@brief Render the updated scene.
 *
 *	If there is no camera set, nothing will be rendered.
 */
void RRender::render() {
	/* if there is no camera do not render anything */
	if (!camera)
		return;

	/* if there is no map do not render anything - TODO, it probably will eventually */
	if (!map)
		return;

	if (!fps_can_render())
		return;

	/* clear everything */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* update the camera */
	camera->update();

  	/* draw */
  	glPushMatrix();
		/* rotate based on the rotation degrees of the wiimote */
		if (g_engine.wiimote.is_connected()) {
			glRotatef(g_engine.wiimote.get_roll(), 0, 0, 1);
			glRotatef(g_engine.wiimote.get_pitch(), 1, 0, 0);
		}

		map->render(camera);
	glPopMatrix();

	/* swap buffers */
	SDL_GL_SwapBuffers();

	/* calculate frame rate */
	++fps_frames;
	calculate_framerate();
}


/**
 *	@brief Update the framerate based on the current time and the number of rendered frames.
 */
inline float RRender::calculate_framerate() {
	struct timeval now_tv;
	gettimeofday(&now_tv, NULL);

	unsigned long now_usec = ((now_tv.tv_sec * 1000000) + now_tv.tv_usec);

	if (now_usec >= fps_usec) {
		unsigned long over_usec = (now_usec - fps_usec);
		float elapsed_sec = (1.0f + (over_usec / 1000000.0f));

		fps_usec = (now_usec + 1000000);
		fps = (fps_frames / elapsed_sec);

		fps_frames = 0;

		INFO("Rendering at %f fps.", fps);
	}

	return fps;
}


/**
 *	@brief Limit the FPS so that the CPU is not consumed by the rendering.
 *	@return 1 if enough time has elapsed to allow another frame to be drawn, 0 if not.
 */
inline int RRender::fps_can_render() {
	struct timeval now_tv;
	gettimeofday(&now_tv, NULL);

	unsigned long now_usec = ((now_tv.tv_sec * 1000000) + now_tv.tv_usec);
	unsigned long elapsed_usec = (now_usec - (fps_usec - 1000000));

	/*
	 *	1 second is segmented into max_fps partitions.
	 *	Make sure that enough time has elapsed to be at
	 *	or past the fps_frames'th segment to allow
	 *	rendering of this frame.
	 */
	return (elapsed_usec >= (fps_frames * (1000000 / max_fps)));
}
