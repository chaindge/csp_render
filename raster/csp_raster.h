#ifndef CSP_RASTER_H
#define CSP_RASTER_H

#include "../math/csp_math.h"
#include <vector>
#include "csp_primitive.h"

class csp_device;

/**
 *	reasterizer
 */
class csp_rasterizer
{
public:
	csp_rasterizer(csp_device* device, int clip_w, int clip_h);

	~csp_rasterizer();

	void triangle(vector_4d *v1, vector_4d *v2, vector_4d *v3, unsigned int color = 0xFFFFFFFF);

	void triangle_opt(vector_4d *v1, vector_4d *v2, vector_4d *v3, unsigned int color = 0xFFFFFFFF);

	void triangle_opt1(vector_4d *v1, vector_4d *v2, vector_4d *v3, unsigned int color = 0xFFFFFFFF);

	void triangle_opt2(vector_4d *v1, vector_4d *v2, vector_4d *v3);

	void BresenhamLine(vector_4d *v1, vector_4d *v2, unsigned int color = 0xFFFFFFFF);

	void put_pixel(int x, int y, unsigned int color);

	inline void fragment_shader(vector_3d* vertex, vector_3d* tri_interp_dt, color_4f* out)
	{
		color_4f c1 = { 1.f,0.f,0.f,1.f };
		color_4f c2 = { 0.f,1.f,0.f,1.f };
		color_4f c3 = { 0.f,0.f,1.f,1.f };

		vector_scale(&c1, tri_interp_dt->x);
		vector_scale(&c2, tri_interp_dt->y);
		vector_scale(&c3, tri_interp_dt->z);

		vector_add(out, &c1, &c2);
		vector_add(out, out, &c3);
	}

	void blend(color_4f*out, color_4f* src, color_4f * dst, int eqation)
	{

	}

	
	void draw_triangle_fill(csp_raster_vertex *v1, csp_raster_vertex *v2, csp_raster_vertex *v3);
	
	void draw_line_fill(csp_raster_vertex* v1, csp_raster_vertex* v2);
protected:	
	csp_device* m_device;

	int m_min_clip_x;
	int m_min_clip_y;
	int m_max_clip_x;
	int m_max_clip_y;

	float m_cur_depth;
};

#endif