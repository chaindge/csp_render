#ifndef CSP_DEVICE_H
#define CSP_DEVICE_H

#include "csp_raster.h"
#include "csp_buffer.h"
#include "csp_primitive.h"

enum { csp_use_color, csp_use_texture, };
enum { csp_front_face, csp_back_face };
enum { csp_ccw, csp_cw };

enum {
	sub_vertex,
	sub_color,
	sub_texture
};

enum {
	draw_triangle,
	draw_line,
	draw_point
};

enum {
	fill_mode_fill,
	fill_mode_frame
};

/**
 *	device flags
 */
enum
{
	csp_blend			= 0x0001,
	csp_depth_test		= 0x0002,
	csp_alpha_test		= 0x0004,
	csp_template_test	= 0x0008,
	csp_texture_mapping = 0x0010,
	csp_cull_face		= 0x0020,
};

/**
 *	blend equations
 */
enum {
	csp_zero,
	csp_one,
	csp_src_color,
	csp_one_minus_src_color,
	csp_dst_color,
	csp_one_minus_dst_color,
	csp_src_alpha,
	csp_one_dst_alpha,
	csp_one_minus_dst_alpha,
	csp_constant_color,
	csp_one_minus_const_color,
	csp_const_alpha,
	csp_one_minus_constant_alpha,
	csp_src_alpha_saturate
};


/**
 *	render device
 */
class csp_device
{
protected:
	unsigned int	_flags;
	unsigned int	_screen_width;
	unsigned int	_screen_height;

	float _near_plane;
	float _far_plane;

	csp_frame_buffer _frame_buffer;
	csp_rasterizer* m_raster;

	unsigned char m_color_piker;
	unsigned char m_front_face;
	unsigned char m_cull_face;
	unsigned char m_fill_mode;

	matrix_4d m_mat_model;
	matrix_4d m_mat_view;
	matrix_4d m_mat_project;
	matrix_4d m_mat_viewport;

	csp_stream m_vertex_stream;
	std::vector<unsigned int> m_index_stream;
public:
	csp_device();

	~csp_device();

	/**
	*	enable render device flag
	*/
	void enable(unsigned int flag);

	/**
	*	disable render device flag
	*/
	void disable(unsigned int flag);

	/**
	*	set which face to be cull
	*	csp_back_face: the back face will be culled
	*	csp_front_face : the front face will be culled
	*/
	void cull_face(unsigned char face);

	/**
	*	set which face is the front face
	*	csp_ccw: the counter clockwise face is front face
	*	csp_cw: the clock-wise face is front face
	*/
	void front_face(unsigned char mode);

	/**
	*	get frame buffer
	*/
	csp_frame_buffer* get_frame_buffer();

	/**
	*	init device's rasterizer,frame buffer,depth buffer
	*/
	void init_device(unsigned int screen_width, unsigned int screen_height);

	/**
	*	free device's resource
	*/
	void free_device();

	/**
	*	set framebuffer's clear color
	*/
	void set_clear_color(color_3f * color);

	/**
	*	clear frame buffer,depth buffer
	*/
	void clear_buffer();

	/**
	 *	clear vertex stream and index stream
	 */
	inline void clear_stream() {
		m_vertex_stream.clear();
		m_index_stream.clear();
	}

	/**
	 *	set Orthographic projection
	 */
	inline void setOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		_near_plane = zNear; _far_plane = zFar;
		create_orthographic_offcenter(left, right, bottom, top, zNear, zFar, &m_mat_project);
	}

	/**
	 *	set perspective projection
	 */
	inline void setPerspective(float fovy, float spect, float zNear, float zFar)
	{
		_near_plane = zNear; _far_plane = zFar;
		create_perspective(fovy, spect, zNear, zFar, &m_mat_project);
	}

	/**
	 *	set view port
	 */
	void setViewport(float x, float y, float width, float height);

	/**
	 *	set look at ( left hand coordinate )
	 */
	inline void setLookAt(float eyePositionX, float eyePositionY, float eyePositionZ,
		float targetPositionX, float targetPositionY, float targetPositionZ,
		float upX, float upY, float upZ)
	{
		create_lookat_rh(eyePositionX, eyePositionY, eyePositionZ, targetPositionX, targetPositionY, targetPositionZ, upX, upY, upZ, &m_mat_view);
	}

	/**
	 *	set model matrix
	 */
	inline void setModelMatrix(matrix_4d* mt)
	{
		m_mat_model = *mt;
	}

	/**
	 *	test flag enable state
	 */
	inline bool is_enable(unsigned int flag) { return (_flags & flag) != 0; }

	/**
	 *	get screen width
	 */
	inline int get_screen_width() { return _screen_width; }

	/**
	 *	get screen height
	 */
	inline int get_screen_height() { return _screen_height; }

	/**
	 *	set fill mode
	 *	fill_mode_fill
	 *	fill_mode_frame
	 */
	inline void set_fill_mode(int fill_mode) { m_fill_mode = fill_mode; }
public:

	/**
	 *	sub data to render device
	 *  flag :	sub_vertex,
	 *			sub_color,
	 *			sub_texture
	 */
	void sub_data(int flag, void* data, unsigned int count, int offset);

	/**
	 *	sub indexs to index buffer
	 */
	void sub_index(void* pdata, int count, int offset);

	/**
	 *	draw by vertex
	 *  flag :	draw_triangle,
	 *			draw_line,
	 *			draw_point
	 */
	void draw_vertex(int flag, unsigned int count);

	/**
	 *	draw by index
	 *  flag :	draw_triangle,
	 *			draw_line,
	 *			draw_point
	 */
	void draw_index(int flag, unsigned int count);

protected:

	/**
	 *	rasteration
	 */
	void device_draw(std::vector<csp_primitive*>& primitives);
	void device_draw_primitive(csp_primitive* primitive);
	void device_draw_primtive_frame(std::vector<csp_primitive*>& primitives);

	/**
	 *	enter render pipeline
	 */
	void enter_pipeline(std::vector<csp_primitive*>& primitives);

	/**
	*	transform vertex to cvv
	*/
	void transform_cvv(std::vector<csp_primitive*>& primitives);

	/**
	 *	clip
	 */
	void cvv_clip(std::vector<csp_primitive*>& primitives);
	void cvv_clip_primitive(csp_primitive* primitive, std::vector<csp_primitive*>& insert_);

	inline float cvv_clip_text_x(vector_4d& v1) { return v1.x / (v1.z) * m_mat_project.m[0][0]; }
	inline float cvv_clip_text_y(vector_4d& v1) { return v1.y / (v1.z) * m_mat_project.m[1][1]; }

	inline bool cvv_clip_test_x(vector_4d& v1, vector_4d& v2)
	{
		float test_1 = v1.x / (v1.z) * m_mat_project.m[0][0];
		float test_2 = v2.x / (v2.z) * m_mat_project.m[0][0];

		// out of cvv in x direction
		return (test_1 > 1.f && test_2 > 1.f ||
			test_1 < -1.f && test_2 < -1.f);
	}

	inline bool cvv_clip_test_x(vector_4d& v1, vector_4d& v2, vector_4d& v3)
	{
		float test_1 = v1.x / (v1.z) * m_mat_project.m[0][0];
		float test_2 = v2.x / (v2.z) * m_mat_project.m[0][0];
		float test_3 = v3.x / (v3.z) * m_mat_project.m[0][0];

		// out of cvv in x direction
		return (test_1 > 1.f && test_2 > 1.f && test_3 > 1.f ||
			test_1 < -1.f && test_2 < -1.f && test_3 < -1.f);
	}

	inline bool cvv_clip_test_y(vector_4d& v1, vector_4d& v2, vector_4d& v3)
	{
		float test_1 = v1.y / (v1.z) * m_mat_project.m[1][1];
		float test_2 = v2.y / (v2.z) * m_mat_project.m[1][1];
		float test_3 = v3.y / (v3.z) * m_mat_project.m[1][1];

		// out of cvv in y direction
		return (test_1 > 1.f && test_2 > 1.f && test_3 > 1.f ||
			test_1 < -1.f && test_2 < -1.f && test_3 < -1.f);
	}

	inline bool cvv_clip_test_y(vector_4d& v1, vector_4d& v2)
	{
		float test_1 = v1.y / (v1.z) * m_mat_project.m[1][1];
		float test_2 = v2.y / (v2.z) * m_mat_project.m[1][1];

		// out of cvv in y direction
		return (test_1 > 1.f && test_2 > 1.f ||
			test_1 < -1.f && test_2 < -1.f);
	}

	/**
	 *	cull back face
	 */
	void cull_face(std::vector<csp_primitive*>& triangles);

	/**
	*	projective transform
	*/
	void projection(std::vector<csp_primitive*>& triangles);

	/**
	*	perspective divide, and transform to screen world
	*/
	void transform_homogenize(std::vector<csp_primitive*>& triangles);


	/**
	*	return true ,when give a dir[ccw or cw], if this kind of dir's face is culled
	*/
	inline bool is_culled(unsigned char dir)
	{
		// if cull front face and current face is front face or if cull back face and current face is back face
		return (m_cull_face == csp_front_face && m_front_face == dir) || (m_cull_face == csp_back_face && m_front_face != dir);
	}

	/**
	 *	back face cull will run at screen space
	 *	so the click circle is reversed direction
	 */
	inline bool is_culled(vector_4d* v1, vector_4d* v2, vector_4d* v3)
	{
		//计算绕向
		//float label = v3->y * v2->x - v2->y * v3->x;
		float label = (v2->x - v1->x) * (v3->y - v1->y) - (v2->y - v1->y) * (v3->x - v1->x);

		// clock-wise
		if (label < 0)
			return is_culled(csp_ccw); // 屏幕空间下，绕向相反
		
		// counter clock-wise
		if(label > 0)
			return is_culled(csp_cw);
	
		// line
		return false;
	}

	/**
	 *	transform to view world
	 */
	inline void transform_vector(vector_4d* v1)
	{
		matrix_apply_col(v1, &m_mat_model, v1);
		matrix_apply_col(v1, &m_mat_view, v1);
	}

	/**
	 *	project to near plane
	 */
	inline void projection(vector_4d* v1)
	{
		matrix_apply_col(v1, &m_mat_project, v1);
	}
	
	/**
	 *	perspective divide, and transform to screen world
	 */
	inline void transform_homogenize(vector_4d* v1)
	{
		float w = v1->w;
		float rhw = 1 / w;
		
		v1->x *= rhw;
		v1->y *= rhw;
		v1->z *= rhw;
		v1->w = 1.f;

		matrix_apply_col(v1, &m_mat_viewport, v1);
	}


	/**
	 *  type 1: two vertex out of near plane
	 *	at right hand coordinate, z in range [-far_plane,-near_plane] is visible,
	 *	so all z greate than -near_plane will be clippd.
	 */
	inline void clip_triangle_1(csp_raster_vertex * v_in, csp_raster_vertex* v_out1, csp_raster_vertex * v_out2)
	{
		float near_plane_to_in_z = -_near_plane - v_in->vertex.z; // z - ( -_near_plane )
		float dt1 = near_plane_to_in_z / (v_out1->vertex.z - v_in->vertex.z);
		float dt2 = near_plane_to_in_z / (v_out2->vertex.z - v_in->vertex.z);

		interp(v_out1, v_in, v_out1, dt1);
		interp(v_out2, v_in, v_out2, dt2);
	}

	/**
	 *  type 2: one vertex out of near plane
	 *	at right hand coordinate, z in range [-far_plane,-near_plane] is visible,
	 *	so all z greate than -near_plane will be clippd.
	 */
	inline void clip_triangle_2(csp_raster_vertex * v_in1, csp_raster_vertex * v_in2, csp_raster_vertex * v_out, csp_raster_vertex* v_clip1, csp_raster_vertex* v_clip2)
	{
		float near_plane_to_in1_z = -_near_plane - v_in1->vertex.z;
		float near_plane_to_in2_z = -_near_plane - v_in2->vertex.z;

		float dt1 = near_plane_to_in1_z / (v_out->vertex.z - v_in1->vertex.z);
		float dt2 = near_plane_to_in2_z / (v_out->vertex.z - v_in2->vertex.z);

		interp(v_clip1, v_in1, v_out, dt1);
		interp(v_clip2, v_in2, v_out, dt2);
	}


	/**
	 *  clip line
	 *	at right hand coordinate, z in range [-far_plane,-near_plane] is visible,
	 *	so all z greate than -near_plane will be clippd.
	 */
	inline void clip_line(csp_raster_vertex* v_in, csp_raster_vertex* v_out)
	{
		float near_plane_to_in_z = -_near_plane - v_in->vertex.z;
		float dt1 = near_plane_to_in_z / (v_out->vertex.z - v_in->vertex.z);

		interp(v_out, v_in, v_out, dt1);
	}

	/**
	 *	prepare primitives before enter render pipeline
	 */
	void draw_primitive_from_vertex(unsigned int count, primitive_type _type, int vertex_count);
	//void draw_triangles_from_vertex(unsigned int count);
	//void draw_line_from_vertex(unsigned int count);
	//void draw_point_from_vertex(unsigned int count);

	/**
	*	prepare primitives before enter render pipeline according to index buffer
	*/
	void draw_primitive_from_index(unsigned int count, primitive_type _type, int vertex_count);
	//void draw_triangles_from_index(unsigned int count);
	//void draw_line_from_index(unsigned int count);
	//void draw_point_from_index(unsigned int count);

	// not use
private:

	void blend(color_4f* result, color_4f* src, color_4f* dist);
};

#endif