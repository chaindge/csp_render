#ifndef CSP_PRIMITIVE_H
#define CSP_PRIMITIVE_H

#include "../math/csp_math.h"

/**
*	vertex
*/
struct csp_raster_vertex
{
	vector_4d	vertex;
	color_4f	color;
	vector_2d	textcoord;
	vector_4d   normal; // vertex normal
	vector_4d	light_color;
};


inline void interp(csp_raster_vertex* result, csp_raster_vertex* a, csp_raster_vertex* b, float delta)
{
	vector_interp(&result->vertex, &a->vertex, &b->vertex, delta);
	vector_interp(&result->color, &a->color, &b->color, delta);
	vector_interp(&result->textcoord, &a->textcoord, &b->textcoord, delta);
	vector_interp(&result->light_color, &a->light_color, &b->light_color, delta);
	vector_interp(&result->normal, &a->normal, &b->normal, delta);
}

enum primitive_type {
	primitive_unknow	= 0x00,
	primitive_point		= 0x01,
	primitive_line		= 0x02,
	primitive_triangle	= 0x03,
	primitive_quad		= 0x04
};

class csp_primitive
{
public:
	csp_primitive(primitive_type type)
	{
		_type = type;
		_skip = false;
		_normal = { 0.f,0.f,0.f,0.f };

		if (_type != 0)
			_vertexs.resize(_type);
	}
	
	inline csp_raster_vertex* get_vertex(unsigned int pt_index) 
	{
		if (pt_index < _type)
			return &_vertexs[pt_index];
		return nullptr;
	}

	inline void set_vertex(unsigned int pt_index, const csp_raster_vertex& vertex) {
		if (pt_index < _type)
			_vertexs[pt_index] = vertex;
	}

	inline void set_normal(vector_4d* normal) { _normal = *normal; }

	inline void set_skiped(bool skip = true) { _skip = true; }
	inline bool is_skiped() { return _skip; }
	inline vector_4d* get_normal() { return &_normal; }
	inline primitive_type get_type() { return (primitive_type)_type; }
private:
	unsigned char _type;
	bool _skip;
	vector_4d _normal; // primitive normal
	std::vector<csp_raster_vertex> _vertexs;
};

// struct csp_primitive
// {
// 	csp_raster_vertex v1, v2, v3, v4;
// 	vector_4d face_n; // face normal
// 	bool skiped;
// 
// 	// triangle use v1,v2,v3
// 	// line use v1,v2,
// 	// point use v1
// 	unsigned char type;
// };


#endif