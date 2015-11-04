#ifndef Q3MAP_H_INCLUDED
#define Q3MAP_H_INCLUDED

#include "definitions.h"
#include "math/vector.h"
#include "render/camera.h"
#include "engine/map.h"


#define Q3BSP_XYZ_SCALE		(1.0 / 64.0)


/*
 *	Maximum spawn points per map
 */
#define Q3_MAX_SPAWN_POINTS		50


/*
 *	Lump indicies.
 */
#define LUMP_ENTITIES		0
#define LUMP_TEXTURES		1
#define LUMP_PLANES			2
#define LUMP_NODES			3
#define LUMP_LEAFS			4
#define LUMP_LEAFFACES		5
#define LUMP_LEAFBRUSHES	6
#define LUMP_MODELS			7
#define LUMP_BRUSHES		8
#define LUMP_BRUSHSIDES		9
#define LUMP_VERTEXES		10
#define LUMP_MESHVERTS		11
#define LUMP_EFFECTS		12
#define LUMP_FACES			13
#define LUMP_LIGHTMAPS		14
#define LUMP_LIGHTVOLS		15
#define LUMP_VISDATA		16


/*
 *	Face types.
 */
#define Q3_FACETYPE_POLYGON		1
#define Q3_FACETYPE_PATCH		2
#define Q3_FACETYPE_MESH		3
#define Q3_FACETYPE_BILLBOARD	4


/*
 *	Useful macros for loading.
 */
#define SEEK_LUMP(lump_type)			fseek(fptr, header.direntry[lump_type].offset, SEEK_SET)
#define LUMP_LENGTH(lump_type)			header.direntry[lump_type].length
#define LUMP_OFFSET(lump_type)			header.direntry[lump_type].offset


struct q3bsp_direntry_t {
	int offset;
	int length;
};


struct q3bsp_header_t {
	int magic;					/* Always "IBSP" (0x50534249) [little endian] */
	int version;
	struct q3bsp_direntry_t direntry[17];
};


/*
 *	Lump structures.
 */
struct q3bsp_entity_t {
	char* ents;
};

#define SIZEOF_TEXTURE		72
struct q3bsp_texture_t {
	char name[64];
	int flags;
	int contents;

	unsigned int gl_text_id;
};

#define SIZEOF_PLANE		16
struct q3bsp_plane_t {
	float normal[3];
	float dist;
};

#define SIZEOF_NODE			32
struct q3bsp_node_t {
	int plane;
	int children[2];
	int mins[3];
	int maxs[3];
};

#define SIZEOF_LEAF			48
struct q3bsp_leaf_t {
	int cluster;
	int area;
	int mins[3];
	int maxs[3];
	int leafface;
	int num_leaffaces;
	int leafbrush;
	int num_leafbrushes;
};

#define SIZEOF_LEAFFACE		4
struct q3bsp_leafface_t {
	int face;
};

#define SIZEOF_LEAFBRUSH	4
struct q3bsp_leafbrush_t {
	int brush;
};

#define SIZEOF_MODEL		80
struct q3bsp_model_t {
	float mins[3];
	float maxs[3];
	int face;
	int num_faces;
	int brush;
	int num_brushes;
};

#define SIZEOF_BRUSH		12
struct q3bsp_brush_t {
	int brushside;
	int num_brushsides;
	int texture;
};

#define SIZEOF_BRUSHSIDE	8
struct q3bsp_brushside_t {
	int plane;
	int texture;
};

#define SIZEOF_VERTEX		44
struct q3bsp_vertex_t {
	float position[3];
	float texcoord[2];
	float lightmapcoord[2];
	float normal[3];
	byte color[4];
};

#define SIZEOF_MESHVERT		4
struct q3bsp_meshvert_t {
	int offset;
};

#define SIZEOF_EFFECT		72
struct q3bsp_effect_t {
	char name[64];
	int brush;
	int unknown;
};

#define SIZEOF_FACE			104
struct q3bsp_face_t {
	int texture;
	int effect;
	int type;
	int vertex;
	int num_vertexes;
	int meshvert;
	int num_meshverts;
	int lm_index;
	int lm_start[2];
	int lm_size[2];
	float lm_origin[3];
	float lm_vecs[2];
	float normal[3];
	int size[2];
};

#define SIZEOF_LIGHTMAP		49152
struct q3bsp_lightmap_t {
	byte map[128][128][3];

	unsigned int gl_text_id;
};

#define SIZEOF_LIGHTVOL		8
struct q3bsp_lightvol_t {
	byte ambient[3];
	byte directional[3];
	byte dir[2];
};

struct q3bsp_visdata_t {
	int num_vecs;
	int sz_vecs;
	byte* vecs;
};


/*
 *	Callback structures for
 *	parsing the entities lump.
 */
class EQ3Map;
struct entity_loader_callbacks_t {
	char* classname;
	void (EQ3Map::*cb)(const char*);
};


/*
 *	Spawn point structure
 */
struct q3bsp_spawn_point_t {
	float angle;
	vector3 origin;
};


/**
 *	@class EQ3Map
 *	@brief A quake3 BSP map
 */
class EQ3Map : public EMap {
	public:
		EQ3Map();
		~EQ3Map();

		int load(char* file);

		void render(RCamera* camera);
		void render_face(int face_index);

		int get_spawn_point(int index, float* angle, vector3* position);

	private:
		int load_header(FILE* fptr);
		int load_lumps(FILE* fptr);

		void load_textures();
		void load_lightmaps();
		void parse_entities();

		char* get_next_token(const char* str, char* buf, int buf_size);

		void load_entity_info_player_deathmatch(const char* ent);


		int find_leaf(vector3* pos);
		int is_cluster_visable(int current, int test);

		int num_textures;
		int num_planes;
		int num_nodes;
		int num_leafs;
		int num_leaffaces;
		int num_leafbrushes;
		int num_models;
		int num_brushes;
		int num_brushsides;
		int num_vertexes;
		int num_meshverts;
		int num_effects;
		int num_faces;
		int num_lightmaps;
		int num_lightvols;

		struct q3bsp_header_t header;
		struct q3bsp_entity_t entities;
		struct q3bsp_texture_t* textures;
		struct q3bsp_plane_t* planes;
		struct q3bsp_node_t* nodes;
		struct q3bsp_leaf_t* leafs;
		struct q3bsp_leafface_t* leaffaces;
		struct q3bsp_leafbrush_t* leafbrushes;
		struct q3bsp_model_t* models;
		struct q3bsp_brush_t* brushes;
		struct q3bsp_brushside_t* brushsides;
		struct q3bsp_vertex_t* vertexes;
		struct q3bsp_meshvert_t* meshverts;
		struct q3bsp_effect_t* effects;
		struct q3bsp_face_t* faces;
		struct q3bsp_lightmap_t* lightmaps;
		struct q3bsp_lightvol_t* lightvols;
		struct q3bsp_visdata_t visdata;

		struct entity_loader_callbacks_t* entity_loader_callbacks;

		struct q3bsp_spawn_point_t spawn_points[Q3_MAX_SPAWN_POINTS];
		int num_spawn_points;
};


#endif // Q3MAP_H_INCLUDED
