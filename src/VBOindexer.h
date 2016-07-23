#ifndef _VBOINDEXER_H_
#define _VBOINDEXER_H_

void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	//std::vector<glm::vec3> & in_normals,

	std::vector<unsigned short> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs//,
	//std::vector<glm::vec3> & out_normals
);
#endif