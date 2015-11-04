#ifndef TEXTURE_MANAGER_H_INCLUDED
#define TEXTURE_MANAGER_H_INCLUDED

#include "gl.h"

/**
 *	@file texture_manager.h
 *	@brief Texture manager.
 */


/**
 *	@struct texture_t
 *	@brief Linked list of textures
 */
typedef struct _texture_t {
	struct _texture_t* next;

	char* file;
	GLuint gl_id;
} texture_t;


/**
 *	@class ETexture_Manager
 *	@brief Manages loaded textures
 */
class ETextureManager {
	public:
		~ETextureManager();

		void init();

		unsigned int load(char* file);
		unsigned int try_load(char* file, char* extensions[]);

		static void modify_gamma(byte* data, int width, int height, int bbp, float factor);

		int get_num_loaded() const;

	private:
		texture_t* textures;

		texture_t* cached(char* file);

		int num_loaded;
};


#endif // TEXTURE_MANAGER_H_INCLUDED
