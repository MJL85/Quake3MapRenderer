/**
 *	@file Q3map.cpp
 *	@brief Load a Quake3 BSP map.
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "definitions.h"
#include "math/mat.h"
#include "str.h"
#include "engine/engine.h"
#include "engine/Q3map.h"


EQ3Map::EQ3Map() {
	static struct entity_loader_callbacks_t _entity_loader_callbacks[] = {
		{	"info_player_deathmatch",		&EQ3Map::load_entity_info_player_deathmatch	},
		{	NULL,							NULL										}
	};

	entity_loader_callbacks = _entity_loader_callbacks;

	memset(spawn_points, 0, sizeof(struct q3bsp_spawn_point_t) * Q3_MAX_SPAWN_POINTS);
	num_spawn_points = 0;
}


EQ3Map::~EQ3Map() {
	INFO("Unloading Quake3 map...");

	free(entities.ents);
	free(textures);
	free(planes);
	free(nodes);
	free(leafs);
	free(leaffaces);
	free(leafbrushes);
	free(models);
	free(brushes);
	free(brushsides);
	free(vertexes);
	free(meshverts);
	free(effects);
	free(faces);
	free(lightmaps);
	free(lightvols);
	free(visdata.vecs);
}


/**
 *	@brief Load the specified Quake3 map.
 */
int EQ3Map::load(char* file) {
	FILE* fptr = fopen(file, "rb");

	INFO("Q3Map: Loading Quake3 map \"%s\"...", file);

	if (!fptr) {
		ERROR("Q3Map: Failed to load map.");
		return 0;
	}

	/* Read the header */
	if (!load_header(fptr)) {
		fclose(fptr);
		return 0;
	}

	/* Load the lumps */
	if (!load_lumps(fptr)) {
		fclose(fptr);
		return 0;
	}

	fclose(fptr);

	/* Load the textures */
	load_textures();

	/* Load the light maps */
	load_lightmaps();

	/* Parse the entities */
	parse_entities();


	/* display the spawn points */
	int i = 0;
	for (; i < num_spawn_points; ++i) {
		INFO("Spawn Point %i: Angle = %f\t\tOrigin = (%f, %f, %f)",
			i,
			spawn_points[i].angle,
			spawn_points[i].origin.x,
			spawn_points[i].origin.y,
			spawn_points[i].origin.z
		);
	}


	INFO("Q3Map: Successfully loaded Quake3 map \"%s\".", file);
	return 1;
}


/**
 *	@brief Load the Quake3 map header.
 */
int EQ3Map::load_header(FILE* fptr) {
	/* load magic number */
	fread((void*)&header.magic, 4, 1, fptr);
	if (header.magic != 0x50534249) {
		/* invalid magic number */
		ERROR("Q3Map: Invalid magic number for BSP file (given %x).\n", header.magic);
		return 0;
	}

	/* load version */
	fread((void*)&header.version, 4, 1, fptr);
	INFO("Q3Map: Version = %i\n", header.version);

	/* load direntry array */
	int entry = 0;
	for (; entry < 17; ++entry)
		fread((header.direntry + entry), 4, 2, fptr);

	return 1;
}


/**
 *	@brief Load the Quake3 map lumps.
 */
int EQ3Map::load_lumps(FILE* fptr) {
	int i;

	/*
	 *	Lump 0 - Entities
	 */
	SEEK_LUMP(LUMP_ENTITIES);
	entities.ents = (char*)malloc(sizeof(char) * LUMP_LENGTH(LUMP_ENTITIES));
	fread(entities.ents, LUMP_LENGTH(LUMP_ENTITIES), 1, fptr);

	/*
	 *	Lump 1 - Textures
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_TEXTURES);
	num_textures = (LUMP_LENGTH(LUMP_TEXTURES) / SIZEOF_TEXTURE);
	textures = (struct q3bsp_texture_t*)malloc(sizeof(struct q3bsp_texture_t) * num_textures);
	for (i = 0; i < num_textures; ++i) {
		fread(textures[i].name, 64, 1, fptr);
		fread(&textures[i].flags, 4, 1, fptr);
		fread(&textures[i].contents, 4, 1, fptr);
	}

	/*
	 *	Lump 2 - Planes
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_PLANES);
	num_planes = (LUMP_LENGTH(LUMP_PLANES) / SIZEOF_PLANE);
	planes = (struct q3bsp_plane_t*)malloc(sizeof(struct q3bsp_plane_t) * num_planes);
	for (i = 0; i < num_planes; ++i) {
		fread(planes[i].normal, 4, 3, fptr);
		fread(&planes[i].dist, 4, 1, fptr);

		swizzle_coords_f(&planes[i].normal[0], &planes[i].normal[1], &planes[i].normal[2]);
	}

	/*
	 *	Lump 3 - Nodes
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_NODES);
	num_nodes = (LUMP_LENGTH(LUMP_NODES) / SIZEOF_NODE);
	nodes = (struct q3bsp_node_t*)malloc(sizeof(struct q3bsp_node_t) * num_nodes);
	for (i = 0; i < num_nodes; ++i) {
		fread(&nodes[i].plane, 4, 1, fptr);
		fread(nodes[i].children, 4, 2, fptr);
		fread(nodes[i].mins, 4, 3, fptr);
		fread(nodes[i].maxs, 4, 3, fptr);

		swizzle_coords_i(&nodes[i].mins[0], &nodes[i].mins[1], &nodes[i].mins[2]);
		swizzle_coords_i(&nodes[i].maxs[0], &nodes[i].maxs[1], &nodes[i].maxs[2]);
	}

	/*
	 *	Lump 4 - Leafs
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_LEAFS);
	num_leafs = (LUMP_LENGTH(LUMP_LEAFS) / SIZEOF_LEAF);
	leafs = (struct q3bsp_leaf_t*)malloc(sizeof(struct q3bsp_leaf_t) * num_leafs);
	for (i = 0; i < num_leafs; ++i) {
		fread(&leafs[i].cluster, 4, 1, fptr);
		fread(&leafs[i].area, 4, 1, fptr);
		fread(leafs[i].mins, 4, 3, fptr);
		fread(leafs[i].maxs, 4, 3, fptr);
		fread(&leafs[i].leafface, 4, 1, fptr);
		fread(&leafs[i].num_leaffaces, 4, 1, fptr);
		fread(&leafs[i].leafbrush, 4, 1, fptr);
		fread(&leafs[i].num_leafbrushes, 4, 1, fptr);

		swizzle_coords_i(&leafs[i].mins[0], &leafs[i].mins[1], &leafs[i].mins[2]);
		swizzle_coords_i(&leafs[i].maxs[0], &leafs[i].maxs[1], &leafs[i].maxs[2]);
	}

	/*
	 *	Lump 5 - Leaf faces
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_LEAFFACES);
	num_leaffaces = (LUMP_LENGTH(LUMP_LEAFS) / SIZEOF_LEAFFACE);
	leaffaces = (struct q3bsp_leafface_t*)malloc(sizeof(struct q3bsp_leafface_t) * num_leaffaces);
	for (i = 0; i < num_leaffaces; ++i) {
		fread(&leaffaces[i].face, 4, 1, fptr);
	}

	/*
	 *	Lump 6 - Leaf brushes
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_LEAFBRUSHES);
	num_leafbrushes = (LUMP_LENGTH(LUMP_LEAFBRUSHES) / SIZEOF_LEAFBRUSH);
	leafbrushes = (struct q3bsp_leafbrush_t*)malloc(sizeof(struct q3bsp_leafbrush_t) * num_leafbrushes);
	for (i = 0; i < num_leafbrushes; ++i) {
		fread(&leafbrushes[i].brush, 4, 1, fptr);
	}

	/*
	 *	Lump 7 - Models
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_MODELS);
	num_models = (LUMP_LENGTH(LUMP_MODELS) / SIZEOF_MODEL);
	models = (struct q3bsp_model_t*)malloc(sizeof(struct q3bsp_model_t) * num_models);
	for (i = 0; i < num_models; ++i) {
		fread(models[i].mins, 4, 3, fptr);
		fread(models[i].maxs, 4, 3, fptr);
		fread(&models[i].face, 4, 1, fptr);
		fread(&models[i].num_faces, 4, 1, fptr);
		fread(&models[i].brush, 4, 1, fptr);
		fread(&models[i].num_brushes, 4, 1, fptr);

		swizzle_coords_f(&models[i].mins[0], &models[i].mins[1], &models[i].mins[2]);
		swizzle_coords_f(&models[i].maxs[0], &models[i].maxs[1], &models[i].maxs[2]);
	}

	/*
	 *	Lump 8 - Brushes
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_BRUSHES);
	num_brushes = (LUMP_LENGTH(LUMP_BRUSHES) / SIZEOF_BRUSH);
	brushes = (struct q3bsp_brush_t*)malloc(sizeof(struct q3bsp_brush_t) * num_brushes);
	for (i = 0; i < num_brushes; ++i) {
		fread(&brushes[i].brushside, 4, 1, fptr);
		fread(&brushes[i].num_brushsides, 4, 1, fptr);
		fread(&brushes[i].texture, 4, 1, fptr);
	}

	/*
	 *	Lump 9 - Brush sides
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_BRUSHSIDES);
	num_brushsides = (LUMP_LENGTH(LUMP_BRUSHSIDES) / SIZEOF_BRUSHSIDE);
	brushsides = (struct q3bsp_brushside_t*)malloc(sizeof(struct q3bsp_brushside_t) * num_brushsides);
	for (i = 0; i < num_brushsides; ++i) {
		fread(&brushsides[i].plane, 4, 1, fptr);
		fread(&brushsides[i].texture, 4, 1, fptr);
	}

	/*
	 *	Lump 10 - Vertexes
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_VERTEXES);
	num_vertexes = (LUMP_LENGTH(LUMP_VERTEXES) / SIZEOF_VERTEX);
	vertexes = (struct q3bsp_vertex_t*)malloc(sizeof(struct q3bsp_vertex_t) * num_vertexes);
	for (i = 0; i < num_vertexes; ++i) {
		fread(vertexes[i].position, 4, 3, fptr);
		fread(vertexes[i].texcoord, 4, 4, fptr);
		fread(vertexes[i].normal, 4, 3, fptr);
		fread(vertexes[i].color, 1, 4, fptr);

		swizzle_coords_f(&vertexes[i].position[0], &vertexes[i].position[1], &vertexes[i].position[2]);
		swizzle_coords_f(&vertexes[i].normal[0], &vertexes[i].normal[1], &vertexes[i].normal[2]);
	}

	/*
	 *	Lump 11 - Mesh verts
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_MESHVERTS);
	num_meshverts = (LUMP_LENGTH(LUMP_MESHVERTS) / SIZEOF_MESHVERT);
	meshverts = (struct q3bsp_meshvert_t*)malloc(sizeof(struct q3bsp_meshvert_t) * num_meshverts);
	for (i = 0; i < num_meshverts; ++i) {
		fread(&meshverts[i].offset, 1, 4, fptr);
	}

	/*
	 *	Lump 12 - Effects
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_EFFECTS);
	num_effects = (LUMP_LENGTH(LUMP_EFFECTS) / SIZEOF_EFFECT);
	effects = (struct q3bsp_effect_t*)malloc(sizeof(struct q3bsp_effect_t) * num_effects);
	for (i = 0; i < num_effects; ++i) {
		fread(effects[i].name, 64, 1, fptr);
		fread(&effects[i].brush, 4, 1, fptr);
		fread(&effects[i].unknown, 4, 1, fptr);
	}

	/*
	 *	Lump 13 - Faces
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_FACES);
	num_faces = (LUMP_LENGTH(LUMP_FACES) / SIZEOF_FACE);
	faces = (struct q3bsp_face_t*)malloc(sizeof(struct q3bsp_face_t) * num_faces);
	for (i = 0; i < num_faces; ++i) {
		fread(&faces[i].texture, 4, 1, fptr);
		fread(&faces[i].effect, 4, 1, fptr);
		fread(&faces[i].type, 4, 1, fptr);
		fread(&faces[i].vertex, 4, 1, fptr);
		fread(&faces[i].num_vertexes, 4, 1, fptr);
		fread(&faces[i].meshvert, 4, 1, fptr);
		fread(&faces[i].num_meshverts, 4, 1, fptr);
		fread(&faces[i].lm_index, 4, 1, fptr);
		fread(faces[i].lm_start, 4, 2, fptr);
		fread(faces[i].lm_size, 4, 2, fptr);
		fread(faces[i].lm_origin, 4, 3, fptr);
		fread(faces[i].lm_vecs, 4, 6, fptr);
		fread(faces[i].normal, 4, 3, fptr);
		fread(faces[i].size, 4, 2, fptr);

		swizzle_coords_f(&faces[i].lm_origin[0], &faces[i].lm_origin[1], &faces[i].lm_origin[2]);
		swizzle_coords_f(&faces[i].normal[0], &faces[i].normal[1], &faces[i].normal[2]);
	}

	/*
	 *	Lump 14 - Light maps
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_LIGHTMAPS);
	num_lightmaps = (LUMP_LENGTH(LUMP_LIGHTMAPS) / SIZEOF_LIGHTMAP);
	lightmaps = (struct q3bsp_lightmap_t*)malloc(sizeof(struct q3bsp_lightmap_t) * num_lightmaps);
	for (i = 0; i < num_lightmaps; ++i) {
		fread(&lightmaps[i].map, 128*128*3, 1, fptr);
	}

	/*
	 *	Lump 15 - Light volumes
	 *
	 *	Number of entries = (lump length) / sizeof(struct)
	 */
	SEEK_LUMP(LUMP_LIGHTVOLS);
	num_lightvols = (LUMP_LENGTH(LUMP_LIGHTVOLS) / SIZEOF_LIGHTVOL);
	lightvols = (struct q3bsp_lightvol_t*)malloc(sizeof(struct q3bsp_lightvol_t) * num_lightvols);
	for (i = 0; i < num_lightvols; ++i) {
		fread(&lightvols[i].ambient, 3, 1, fptr);
		fread(&lightvols[i].directional, 3, 1, fptr);
		fread(&lightvols[i].dir, 2, 1, fptr);
	}

	/*
	 *	Lump 16 - Visual data
	 */
	SEEK_LUMP(LUMP_VISDATA);
	fread(&visdata.num_vecs, 4, 1, fptr);
	fread(&visdata.sz_vecs, 4, 1, fptr);
	visdata.vecs = (byte*)malloc(sizeof(byte) * visdata.num_vecs * visdata.sz_vecs);
	fread(visdata.vecs, (visdata.num_vecs * visdata.sz_vecs), 1, fptr);

	return 1;
}


/**
 *	@brief Load the textures from the texture lump into the texture manager.
 */
void EQ3Map::load_textures() {
	int i;
	ETextureManager* tm = g_engine.get_texture_manager();
	assert(tm);

	char* img_ext[] = {
		".jpg",
		".tga",
		NULL
	};

	for (i = 0; i < num_textures; ++i) {
		/* cache this texture */
		textures[i].gl_text_id = tm->try_load(textures[i].name, img_ext);
	}
}


/**
 *	@brief Load the lightmap textures from the lightmaps lump.
 */
void EQ3Map::load_lightmaps() {
	int i = 0;

	for (; i < num_lightmaps; ++i) {
		unsigned int* text_id = &(lightmaps[i].gl_text_id);

		glGenTextures(1, text_id);

		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

		glBindTexture(GL_TEXTURE_2D, *text_id);

		ETextureManager::modify_gamma((byte*)lightmaps[i].map, 128, 128, 3, 4.0f);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, lightmaps[i].map);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
}


/**
 *	@brief Parse the entities lump
 */
void EQ3Map::parse_entities() {
	const char* s = entities.ents;
	const char* b = NULL;

	char key[256];
	char value[256];

	if (!s || !*s)
		return;

	/* find and parse each block */
	while (*s) {
		/* scan until we hit the beginning of a block */
		for (; (*s && (*s != '{')); ++s);

		if (!*s)
			break;

		/* save the position beginning of the block */
		b = s;

		/* find the "classname" section of this block */
		while (*s && (*s != '}')) {
			s = get_next_token(s, key, 256);
			s = get_next_token(s, value, 256);

			/* if this is the block classname, do something with it */
			if (!cstrcmp("classname", key)) {
				/*
				 *	call the callback to handle this type.
				 *	the callback will get a pointer to the
				 *	beginning of the entity block.
				 */

				DEBUG("Found entity classname \"%s\".", value);

				int i = 0;
				for (; entity_loader_callbacks[i].classname; ++i) {
					if (!cstrcmp(entity_loader_callbacks[i].classname, value))
						(this->*(entity_loader_callbacks[i].cb))(b);
				}
			}
		}

	}
}


/**
 *	@brief Get the next token in the string
 *	@param str		Where the string begins
 *	@param buf		Where to store the extracted token
 *	@param buf_size	The size of buf
 *	@return Returns the the character after the token ends in str
 *
 *	A token is in the form:
 *		"token"
 *
 *	If get_next_token("asdf \"foo\" bar", buf, 20) is called,
 *	buf = "foo"
 *	The return is pointing to " bar"
 */
char* EQ3Map::get_next_token(const char* str, char* buf, int buf_size) {
	assert(str && buf && (buf_size > 0));

	memset(buf, 0, buf_size);

	/* find the next quote */
	for (; (*str && (*str != '"') && (*str != '}')); ++str);

	if (!*str || (*str == '}'))
		return (char*)str;

	/* skip the quote */
	++str;

	/* read in everything up to the next quote or buf_size */
	for (; (buf_size && *str && (*str != '"')); --buf_size, ++str, ++buf)
		*buf = *str;

	return (char*)(str + 1);
}


/**
 *	@brief Load the info_player_deathmatch entity block
 */
void EQ3Map::load_entity_info_player_deathmatch(const char* ent) {
	/*
	 *	ent points to the beginning of a string that looks like this:
	 *
	 *	{
	 *	"classname" "info_player_deathmatch"
	 *	"angle" "360"
	 *	"origin" "216 1328 24"
	 *	}
	 */

	/* if we are beyond the limit of spawn points then ignore this one */
	if (num_spawn_points > Q3_MAX_SPAWN_POINTS)
		return;

	++ent;
	char key[256];
	char value[256];

	/* put this spawn data in the next available slot */
	float* angle = &(spawn_points[num_spawn_points].angle);
	vector3* origin = &(spawn_points[num_spawn_points].origin);
	++num_spawn_points;

	while (*ent && (*ent != '}')) {
		ent = get_next_token(ent, key, 256);
		ent = get_next_token(ent, value, 256);

		if (!cstrcmp("angle", key))
			sscanf(value, "%f", angle);
		else if (!cstrcmp("origin", key)) {
			sscanf(value, "%f %f %f", &origin->x, &origin->y, &origin->z);

			/* convert the coordinate system */
			swizzle_coords_f(&origin->x, &origin->y, &origin->z);
		}
	}
}


/**
 *	@breif Get the index'th spawn point information
 *	@param index	The spawn point index
 *	@param angle	Pointer to where the angle should be stored
 *	@param position	Pointer to a vector3 where the position should be stored
 *	@return 1 if successful, 0 if the index is out of range
 */
int EQ3Map::get_spawn_point(int index, float* angle, vector3* position) {
	assert(angle && position);

	if ((index < 0) || (index > num_spawn_points))
		return 0;

	*angle = spawn_points[index].angle;
	position->x = spawn_points[index].origin.x;
	position->y = spawn_points[index].origin.y;
	position->z = spawn_points[index].origin.z;

	return 1;
}
