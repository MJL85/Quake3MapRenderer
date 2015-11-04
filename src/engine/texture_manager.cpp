#include <stdio.h>

#include "definitions.h"
#include "gl.h"
#include "str.h"
#include <SDL/SDL_image.h>
#include "engine/texture_manager.h"

ETextureManager::~ETextureManager() {
	INFO("Shutting down texture manager...");

	/* delete all the textures */
	texture_t* nptr = NULL;
	while (textures) {
		nptr = textures->next;

		if (textures->file)
			free(textures->file);

		/* tell OpenGL to delete the texture */
		glDeleteTextures(1, &textures->gl_id);

		delete textures;

		textures = nptr;
	}
}


/**
 *	@brief Initialize the texture manager
 */
void ETextureManager::init() {
	INFO("Initializing texture manager...");

	textures = NULL;
	num_loaded = 0;
}


/**
 *	@brief Load a texture into OpenGL
 *	@param file		Name of the image file to load.
 *	@return Returns the GL texture id if successful, or 0 if failed
 */
unsigned int ETextureManager::load(char* file) {
	texture_t* textptr;
	GLuint text_id;

	/* check if the texture has already been loaded */
	textptr = cached(file);
	if (textptr)
		/* already cached, no need to load it again */
		return textptr->gl_id;

	/* load the image */
	SDL_Surface* surface = IMG_Load(file);
	if (!surface) {
		ERROR("TextureManager: Error loading image: %s", IMG_GetError());
		return 0;
	}

	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &text_id);
	glBindTexture(GL_TEXTURE_2D, text_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, surface->w, surface->h, GL_BGR_EXT, GL_UNSIGNED_BYTE, surface->pixels);

	SDL_FreeSurface(surface);

	/* create a link node for this texture */
	textptr = new texture_t;
	textptr->file = strdup(file);
	textptr->gl_id = text_id;
	textptr->next = NULL;

	/* add this texture to the list */
	if (!textures) {
		/* this texture is the first */
		textures = textptr;

		INFO("TextureManager: Loaded texture \"%s\" (gl %i).", file, text_id);
		++num_loaded;
		return text_id;
	} else {
		/* add to the beginning since that's faster than the end */
		textptr->next = textures;
		textures = textptr;

		INFO("TextureManager: Loaded texture \"%s\" (gl %i).", file, text_id);
		++num_loaded;
		return text_id;
	}

	WARNING("TextureManager: Failed to load texture \"%s\".", file);
	return 0;
}


/**
 *	@brief Check if a texture has already been loaded
 *	@param file		Name of the image file to look for
 *	@return Pointer to the texture if it exists, NULL if not.
 *
 *	If a texture has already been loaded then it does not need
 *	to be loaded again. This function will return a pointer to
 *	a texture object based on the file name if it has already
 *	been cached.
 */
texture_t* ETextureManager::cached(char* file) {
	texture_t* tptr = textures;

	for (; tptr; tptr = tptr->next) {
		if (!cstrcmp(tptr->file, file))
			/* found it */
			return tptr;
	}

	return NULL;
}


/**
 *	@brief Load a texture into OpenGL
 *	@param file			Name of the image file to load.
 *	@param extensions	Array of character pointers of file extensions, including the period.
 *	@return Returns the GL texture id if successful, or 0 if failed
 *
 *	This function will take a filename and try various user defined
 *	extensions to try to find the file that matches.
 *
 *	For example:
 *
 *		char* ext[] = { ".jpg", ".tga", ".gif" };
 *		try_load("foo", ext)
 *
 *	Will first try to load "foo.jpg", if it fails it will try
 *	"foo.tga" and "foo.gif" until either one is found or none
 *	match an existing file.
 */
unsigned int ETextureManager::try_load(char* file, char* extensions[]) {
	char filepath[512];
	FILE* fptr;
	int i = 0;
	int filename_len;

	strncpy(filepath, file, 511);
	filename_len = strlen(file);

	/* try each extension */
	for (; extensions[i]; ++i) {
		filepath[filename_len] = 0;
		strcat(filepath, extensions[i]);

		if ((fptr = fopen(filepath, "r")))
			fclose(fptr);

		if (fptr) {
			/* this file exists - append the extension to the file name */
			strcat(file, extensions[i]);

			/* load the image */
			return load(file);
		}
	}

	return 0;
}


/**
 *	@brief [Static] Change the gamma of an image
 *	@param data		Pointer to the image data
 *	@param width	Width of the image
 *	@param height	Height of the image
 *	@param bbp		Bytes per pixel
 *	@param factor	How much to modify the gamma by
 *
 *	Code taken from gametutorials.com, credited to Aftershock engine.
 */
void ETextureManager::modify_gamma(byte* data, int width, int height, int bbp, float factor) {
	int size = width * height * bbp;
	int i = 0;

	float scale, temp;
	float r, g, b;

	for (; i < size; i += 3) {
		scale = 1.0f;
		temp = 0.0f;

		r = (float)data[i];
		g = (float)data[i+1];
		b = (float)data[i+2];

		r = r * factor / 255.0f;
		g = g * factor / 255.0f;
		b = b * factor / 255.0f;

		if(r > 1.0f && (temp = (1.0f/r)) < scale) scale = temp;
		if(g > 1.0f && (temp = (1.0f/g)) < scale) scale = temp;
		if(b > 1.0f && (temp = (1.0f/b)) < scale) scale = temp;

		scale *= 255.0f;
		r *= scale;
		g *= scale;
		b *= scale;

		data[i] = (byte)r;
		data[i+1] = (byte)g;
		data[i+2] = (byte)b;
	}
}


/**
 *	@brief Get the number of loaded textures
 */
int ETextureManager::get_num_loaded() const {
	return num_loaded;
}
