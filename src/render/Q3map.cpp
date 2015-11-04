/**
 *	@file Q3map.cpp
 *	@brief Render a Quake3 BSP map.
 */

#include <stdio.h>
#include "definitions.h"
#include "gl.h"

#include "math/vector.h"
#include "engine/Q3map.h"


/**
 *	@brief Render the full map from the camera's current position.
 *	@param camera	The camera to render from
 */
void EQ3Map::render(RCamera* camera) {
	vector3 pos;
	int leaf, cluster;

	/* find the leaf and cluster the camera is at */
	camera->get_position(&pos);
	leaf = find_leaf(&pos);
	cluster = leafs[leaf].cluster;
	//DEBUG("[Render::Q3Map] cluster = %i", cluster);

	#if 0

	/* render everything */
	int face = 0;

	for (; face < num_faces; ++face) {
		if (faces[face].type == Q3_FACETYPE_POLYGON)
			render_face(face);
	}

	#else

	int i = num_leafs;
	struct q3bsp_leaf_t* t_leaf = NULL;

	for (; i >= 0; --i) {
		t_leaf = &leafs[i];

		/* check if this leaf cluster is visable from here */
		if (!is_cluster_visable(cluster, t_leaf->cluster))
			continue;

		/* if this cluster is not in the camera frustum, skip it */
		if (!camera->is_box_visable(t_leaf->mins[0], t_leaf->mins[1], t_leaf->mins[2],
									t_leaf->maxs[0], t_leaf->maxs[1], t_leaf->maxs[2]))
			continue;

		/* render all the faces in this leaf */
		int f = t_leaf->num_leaffaces;
		for (; f >= 0; --f) {
			int f_index = leaffaces[t_leaf->leafface + f].face;
			render_face(f_index);
		}
	}

	#endif
}


/**
 *	@brief Render a face.
 *	@param face_index	The index of the face to render
 */
inline void EQ3Map::render_face(int face_index) {
	struct q3bsp_face_t* face = (faces + face_index);

	/* bind the texture */
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(struct q3bsp_vertex_t), vertexes[face->vertex].texcoord);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[face->texture].gl_text_id);

	/* bind the light map */
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(struct q3bsp_vertex_t), vertexes[face->vertex].lightmapcoord);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lightmaps[face->lm_index].gl_text_id);

	/* draw everything */
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(struct q3bsp_vertex_t), vertexes[face->vertex].position);
	glNormalPointer(GL_FLOAT, sizeof(struct q3bsp_vertex_t), vertexes[face->vertex].normal);
	//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct q3bsp_vertex_t), vertexes[face->vertex].color);

	glDrawRangeElements(GL_TRIANGLES, 0, face->num_meshverts - 1, face->num_meshverts, GL_UNSIGNED_INT, &meshverts[face->meshvert]);
}


/**
 *	@brief Find the leaf at a given position
 *	@param pos	The position of interest
 *	@return Offset of the leaf in the nodes vector
 */
int EQ3Map::find_leaf(vector3* pos) {
	int i = 0;
	float distance = 0.0f;
	struct q3bsp_node_t* node = NULL;
	struct q3bsp_plane_t* plane = NULL;

	while (i >= 0) {
		node = &nodes[i];
		plane = &planes[node->plane];

		/* calculate the distance Ax+By+Cz+d=0 */
		distance = (plane->normal[0] * pos->x +
					plane->normal[1] * pos->y +
					plane->normal[2] * pos->z -
					plane->dist);

		if (distance >= 0)
			/* go to the front node */
			i = node->children[0];
		else
			/* go to the back node */
			i = node->children[1];
	}

	return ~i;
}


/**
 *	@brief Check to see if a cluster is visable from the current cluster
 *	@param current	The current cluster index
 *	@param test		The index to the cluster to be tested
 *	@return Returns 1 if the test cluster is visable from the current, 0 if not.
 */
int EQ3Map::is_cluster_visable(int current, int test) {
	if (current < 0)
		return 1;

	/* get vector for current cluster */
	byte vec = visdata.vecs[(current * visdata.sz_vecs) + (test / 8)];

	return (vec & (1 << ((test) & 7)));
}
