#include "csp_device.h"
#include <math.h>

csp_device::csp_device()
	:_flags(0)
	, _screen_width(0)
	, _screen_height(0)
	, m_raster(nullptr)
	, m_front_face(csp_ccw)
	, m_cull_face(csp_back_face)
	, m_fill_mode(fill_mode_fill)
{
	enable(csp_blend | csp_depth_test);

	matrix_identity(&m_mat_model);
	matrix_identity(&m_mat_view);
	matrix_identity(&m_mat_project);

	_near_plane = 1.f; _far_plane = 100.f;
}

csp_device::~csp_device()
{
}

void csp_device::enable(unsigned int flag)
{
	_flags |= flag;
}

void csp_device::disable(unsigned int flag)
{
	_flags &= ~flag;
}

void csp_device::cull_face(unsigned char face)
{
	m_cull_face = face;
}

void csp_device::front_face(unsigned char mode)
{
	m_front_face = mode;
}

csp_frame_buffer* csp_device::get_frame_buffer()
{
	return &_frame_buffer;
}

void csp_device::init_device(unsigned int screen_width, unsigned int screen_height)
{
	_screen_width = screen_width;
	_screen_height = screen_height;

	_frame_buffer.init(screen_width, screen_height);

	m_raster = new csp_rasterizer(this, screen_width, screen_height);

	setViewport(0.f, 0.f, screen_width, screen_height);
}

void csp_device::free_device()
{
	_frame_buffer.free();
	delete m_raster; m_raster = nullptr;
}

void csp_device::set_clear_color(color_3f * color)
{
	_frame_buffer.set_clear_color(color);
}

void csp_device::clear_buffer()
{
	_frame_buffer.clear();
}

void csp_device::setViewport(float x, float y, float width, float height)
{
	float half_w = width * 0.5f;
	float half_h = height * 0.5f;

	m_mat_viewport = 
	{ 
		half_w,	0.f,	0.f,	x + half_w,
		0.f,	-half_h,0.f,	y + half_h,
		0.f,	0.f,	0.5f,	0.5f,
		0.f,	0.f,	0.f,	1.f
	};
}

inline void load_vertex(csp_stream& stream, int index, csp_raster_vertex* vertex)
{
	stream.get_vertex(index, &vertex->vertex, &vertex->color, &vertex->textcoord);
}

void load_primitive(std::vector<csp_primitive*>& result,
	int primitive_count, primitive_type _type, int vertex_count, csp_stream& stream)
{
	for (int i = 0; i < primitive_count; ++i) 
	{
		auto p_primitive = new csp_primitive(_type);

		for (int vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
			load_vertex(stream, i * vertex_count + vertex_index, p_primitive->get_vertex(vertex_index));
	
		result.push_back(p_primitive);
	}
}

void load_primitive_by_index(std::vector<csp_primitive*>& result,
	int primitive_count, primitive_type _type, int vertex_count, csp_stream& stream, std::vector<unsigned int>& index_stream)
{
	for (int i = 0; i < primitive_count; ++i)
	{
		auto p_primitive = new csp_primitive(_type);

		for (int vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
			load_vertex(stream, index_stream[i * vertex_count + vertex_index], p_primitive->get_vertex(vertex_index));

		result.push_back(p_primitive);
	}
}

void clear_primitive(std::vector<csp_primitive*>& result)
{
	for (auto item : result)
		delete item;

	result.clear();
}


void csp_device::draw_primitive_from_vertex(unsigned int count, primitive_type _type, int vertex_count)
{
	std::vector<csp_primitive*> primitives;

	load_primitive(primitives, count, _type, vertex_count, m_vertex_stream);

	enter_pipeline(primitives);

	clear_primitive(primitives);
}



void csp_device::draw_primitive_from_index(unsigned int count, primitive_type _type, int vertex_count)
{
	if (count == 0 || count * vertex_count > m_index_stream.size())
		return;

	std::vector<csp_primitive*> primitives;

	load_primitive_by_index(primitives, count, _type, vertex_count, m_vertex_stream, m_index_stream);

	enter_pipeline(primitives);

	clear_primitive(primitives);
}


void csp_device::blend(color_4f* result, color_4f* src, color_4f* dist)
{
	float f_src = src->a;
	float f_dist = 1 - f_src;

	vector_scale(src, f_src);
	vector_scale(dist, f_dist);

	vector_add(result, src, dist);
}

// void csp_device::device_triangle(vector_4d* v1, vector_4d* v2, vector_4d*v3)
// {
// 	// cull face
// 	if ((_flags & csp_cull_face) && is_culled(v1, v2, v3)) return;
// 
// 	// transform to cvv
// 	transform_vector(v1);
// 	transform_vector(v2);
// 	transform_vector(v3);
// 
// 	if (v1->w == 0 || v2->w == 0 || v3->w == 0) return;
// 
// 	// transform to ndc
// 	transform_homogenize(v1);
// 	transform_homogenize(v2);
// 	transform_homogenize(v3);
// 
// 	// clip test
// 	int a1 = transform_check_cvv(v1);
// 	int a2 = transform_check_cvv(v2);
// 	int a3 = transform_check_cvv(v3);
// 
// 	bool b1 = (a1 & 1) != 0;
// 	bool b2 = (a2 & 1) != 0;
// 	bool b3 = (a3 & 1) != 0;
// 
// 	// all vertex out of cvv
// 	if (a1 != 0 && a2 != 0 && a3 != 0) return;
// 
// 	std::vector<vector_4d*> in;
// 	std::vector<vector_4d*> out;
// 
// 	b1 ? out.push_back(v1) : in.push_back(v1);
// 	b2 ? out.push_back(v2) : in.push_back(v2);
// 	b3 ? out.push_back(v3) : in.push_back(v3);
// 
// 	// all vertex in cvv
// 	if (out.empty())
// 	{
// 		// transfrom to screen coordination
// 		matrix_apply_col(v1, &m_mat_viewport, v1);
// 		matrix_apply_col(v2, &m_mat_viewport, v2);
// 		matrix_apply_col(v3, &m_mat_viewport, v3);
// 
// 		m_raster->triangle_opt2(v1, v2, v3);
// 
// 		return;
// 	}
// 
// 	// need clip
// 	if (out.size() == 1) {
// 		vector_4d clip_0, clip_1;
// 		clip_triangle_2(in[0], in[1], out[0], &clip_0, &clip_1);
// 
// 		matrix_apply_col(in[0], &m_mat_viewport, in[0]);
// 		matrix_apply_col(in[1], &m_mat_viewport, in[1]);
// 		matrix_apply_col(&clip_0, &m_mat_viewport, &clip_0);
// 		matrix_apply_col(&clip_1, &m_mat_viewport, &clip_1);
// 
// 		m_raster->triangle_opt2(in[0], &clip_0, &clip_1);
// 		m_raster->triangle_opt2(in[0], &clip_1, in[1]);
// 		return;
// 	}
// 
// 	if (out.size() == 2) {
// 		clip_triangle_1(in[0], out[0], out[1]);
// 
// 		matrix_apply_col(in[0], &m_mat_viewport, in[0]);
// 		matrix_apply_col(out[0], &m_mat_viewport, out[0]);
// 		matrix_apply_col(out[1], &m_mat_viewport, out[1]);
// 
// 		m_raster->triangle_opt2(in[0], out[0], out[1]);
// 		return;
// 	}
// }

void csp_device::sub_data(int flag, void * data, unsigned int count, int offset)
{
	if (flag == sub_vertex)
		m_vertex_stream.sub_vertex_3d(data, count, offset);
	else if (flag == sub_color)
		m_vertex_stream.sub_color_4f(data, count, offset);
	else if (flag == sub_texture)
		m_vertex_stream.sub_texture_2d(data, count, offset);
}

void csp_device::sub_index(void * pdata, int count, int offset)
{
	unsigned char* pbuf = (unsigned char*)pdata;
	for (int i = 0; i < count; ++i) {
		auto p_cur = (unsigned int*)(pbuf + offset * i);
		m_index_stream.push_back(p_cur[0]);
	}
}

void csp_device::draw_vertex(int flag, unsigned int count)
{
	if (flag == draw_triangle) {
		draw_primitive_from_vertex(count, primitive_triangle, 3);
	}
	else if (flag == draw_line) {
		draw_primitive_from_vertex(count, primitive_line, 2);
	}
	else if (flag == draw_point) {
		draw_primitive_from_vertex(count, primitive_point, 1);
	}
}

void csp_device::draw_index(int flag, unsigned int count)
{
	if (flag == draw_triangle) {
		draw_primitive_from_index(count, primitive_triangle, 3);
	}
	else if (flag == draw_line) {
		draw_primitive_from_index(count, primitive_line, 2);
	}
	else if (flag == draw_point) {
		draw_primitive_from_index(count, primitive_point, 1);
	}
}

void csp_device::device_draw(std::vector<csp_primitive*>& primitives)
{
	std::vector<csp_primitive*> frame_primitives;
	for (auto primitive : primitives)
	{
		if (primitive->is_skiped())continue;

		if (m_fill_mode == fill_mode_frame &&
			primitive->get_type() >= primitive_triangle)
		{
			frame_primitives.push_back(primitive);
			continue;
		}

		device_draw_primitive(primitive);
	}

	device_draw_primtive_frame(frame_primitives);
}

void csp_device::device_draw_primitive(csp_primitive * primitive)
{
	auto type = primitive->get_type();

	switch (type) 
	{
	// draw triangle
	case primitive_triangle:
	{
		m_raster->draw_triangle_fill(primitive->get_vertex(0), primitive->get_vertex(1), primitive->get_vertex(2));

		break;
	}
	
	// draw line
	case primitive_line:
	{
		m_raster->draw_line_fill(primitive->get_vertex(0), primitive->get_vertex(1));

		break;
	}

	// draw point
	case primitive_point:
	{

		break;
	}

	default:break;
	}// end switch
}

struct frame_line
{
	csp_raster_vertex* v1;
	csp_raster_vertex* v2;
};

inline bool operator==(const vector_4d& v1, const vector_4d& v2) 
{
	return float_equal(v1.x, v2.x) &&
		float_equal(v1.y, v2.y) &&
		float_equal(v1.z, v2.z) &&
		float_equal(v1.w, v2.w);
}

inline bool is_exist(const std::vector<frame_line>& lines, csp_raster_vertex* v1, csp_raster_vertex* v2) 
{
	for (auto& line : lines) 
	{
		if (line.v1->vertex == v1->vertex && line.v2->vertex == v2->vertex ||
			line.v1->vertex == v2->vertex && line.v2->vertex == v1->vertex)
			return true;
	}

	return false;
}

void csp_device::device_draw_primtive_frame(std::vector<csp_primitive*>& primitives)
{
	std::vector<frame_line> lines;

	int count = 0;
	for (auto primitive : primitives) 
	{
		count = primitive->get_type();
		for (int i = 0; i < count; ++i) {
			auto v1 = primitive->get_vertex(i);
			auto v2 = primitive->get_vertex((i + 1) % count);

			if (!is_exist(lines, v1, v2))
				lines.push_back({ v1,v2 });
		}
	}

	for (auto& line : lines)
		m_raster->draw_line_fill(line.v1, line.v2);
}

void csp_device::enter_pipeline(std::vector<csp_primitive*>& primitives)
{
	// pipeline step 1
	// transform to view world
	// ready to clip
	transform_cvv(primitives);

	// pipeline step 2
	// clip
	cvv_clip(primitives);

	// pipeline step 3
	// do projective transform
	projection(primitives);

	// pipeline step 4
	// perspective divide 
	// and transform to screen coordination
	transform_homogenize(primitives);

	// pipeline step 5
	// cull back face
	if (_flags & csp_cull_face)
		cull_face(primitives);

	// pipeline step 6
	// rasteration
	device_draw(primitives);
}

void csp_device::transform_cvv(std::vector<csp_primitive*>& primitives)
{
	int count = 0;
	for (auto primitive : primitives)
	{
		if (primitive->is_skiped())continue;

		count = primitive->get_type();

		// transform to cvv
		for (int i = 0; i < count; ++i)
			transform_vector(&primitive->get_vertex(i)->vertex);
	}
}

void csp_device::cvv_clip(std::vector<csp_primitive*>& primitives)
{
	// at right hand coordination
	// -z is visible
	// so the [-far,-near] will be project to [-1,1]

	std::vector<csp_primitive*> insert_;

	for (auto primitive : primitives)
	{
		if (primitive->is_skiped())continue;

		cvv_clip_primitive(primitive, insert_);

	}// end for

	primitives.insert(primitives.end(), insert_.begin(), insert_.end());
}



void csp_device::cvv_clip_primitive(csp_primitive * primitive, std::vector<csp_primitive*>& insert_)
{
	auto type = primitive->get_type();

	switch (type)
	{
		// triangle
	case primitive_triangle:
	{
		auto p_v1 = primitive->get_vertex(0);
		auto p_v2 = primitive->get_vertex(1);
		auto p_v3 = primitive->get_vertex(2);

		// near plane clip
		bool _v1_z_out = p_v1->vertex.z > -_near_plane;
		bool _v2_z_out = p_v2->vertex.z > -_near_plane;
		bool _v3_z_out = p_v3->vertex.z > -_near_plane;

		std::vector<csp_raster_vertex*> in;
		std::vector<csp_raster_vertex*> out;

		_v1_z_out ? out.push_back(p_v1) : in.push_back(p_v1);
		_v2_z_out ? out.push_back(p_v2) : in.push_back(p_v2);
		_v3_z_out ? out.push_back(p_v3) : in.push_back(p_v3);

		// need clip
		if (out.size() == 1) {
			primitive->set_skiped(true);

			csp_raster_vertex clip_0, clip_1;
			clip_triangle_2(in[0], in[1], out[0], &clip_0, &clip_1);


			if (!(cvv_clip_test_x(in[0]->vertex, clip_0.vertex, clip_1.vertex) ||
				cvv_clip_test_y(in[0]->vertex, clip_0.vertex, clip_1.vertex))) {

				// insert new triangle 1
				auto p_new1 = new csp_primitive(primitive_triangle);
				p_new1->set_vertex(0, *in[0]);
				p_new1->set_vertex(1, clip_0);
				p_new1->set_vertex(2, clip_1);
				p_new1->set_normal(primitive->get_normal());

				insert_.push_back(p_new1);
			}

			if (!(cvv_clip_test_x(in[0]->vertex, clip_1.vertex, in[1]->vertex) ||
				cvv_clip_test_y(in[0]->vertex, clip_1.vertex, in[1]->vertex))) {

				// insert two new triangle 2
				auto p_new2 = new csp_primitive(primitive_triangle);
				p_new2->set_vertex(0, *in[0]);
				p_new2->set_vertex(1, clip_1);
				p_new2->set_vertex(2, *in[1]);
				p_new2->set_normal(primitive->get_normal());

				insert_.push_back(p_new2);
			}
		}

		// need clip
		else if (out.size() == 2) {
			clip_triangle_1(in[0], out[0], out[1]);
		}

		// if all three vertex less than project near plane
		// cull
		else if (out.size() == 3) {
			primitive->set_skiped(true);
			return;
		}

		// far plane cull
		_v1_z_out = p_v1->vertex.z < -_far_plane;
		_v2_z_out = p_v2->vertex.z < -_far_plane;
		_v3_z_out = p_v3->vertex.z < -_far_plane;

		if (_v1_z_out && _v2_z_out && _v3_z_out)
		{
			primitive->set_skiped(true);
			return;
		}

		// x direction cull
		if (cvv_clip_test_x(p_v1->vertex, p_v2->vertex, p_v3->vertex)) {
			primitive->set_skiped(true);
			return;
		}

		// y direction cull
		if (cvv_clip_test_y(p_v1->vertex, p_v2->vertex, p_v3->vertex)) {
			primitive->set_skiped(true);
			return;
		}

		break;
	}

	// line
	case primitive_line:
	{
		auto p_v1 = primitive->get_vertex(0);
		auto p_v2 = primitive->get_vertex(1);

		// near plane clip
		bool _v1_z_out = p_v1->vertex.z > -_near_plane;
		bool _v2_z_out = p_v2->vertex.z > -_near_plane;

		std::vector<csp_raster_vertex*> in;
		std::vector<csp_raster_vertex*> out;

		_v1_z_out ? out.push_back(p_v1) : in.push_back(p_v1);
		_v2_z_out ? out.push_back(p_v2) : in.push_back(p_v2);

		// need clip
		if (out.size() == 1) {
			clip_line(in[0], out[0]);
		}

		// if all three vertex less than project near plane
		// cull
		else if (out.size() == 2) {
			primitive->set_skiped(true);
			return;
		}

		// far plane cull
		_v1_z_out = p_v1->vertex.z < -_far_plane;
		_v2_z_out = p_v2->vertex.z < -_far_plane;

		if (_v1_z_out && _v2_z_out)
		{
			primitive->set_skiped(true);
			return;
		}

		// x or y direction cull
		if (cvv_clip_test_x(p_v1->vertex, p_v2->vertex) ||
			cvv_clip_test_y(p_v1->vertex, p_v2->vertex)) {
			primitive->set_skiped(true);
			return;
		}

		break;
	}

	// point
	case primitive_point:
	{
		auto p_v1 = primitive->get_vertex(0);

		float x_test = cvv_clip_text_x(p_v1->vertex);
		float y_text = cvv_clip_text_y(p_v1->vertex);

		if (p_v1->vertex.z > -_near_plane ||
			p_v1->vertex.z < -_far_plane ||
			x_test < -1.f || x_test > 1.f ||
			y_text < -1.f || y_text > 1.f)
		{
			primitive->set_skiped(true);
			return;
		}

		break;
	}

	default:break;

	}// end switch
}

void csp_device::cull_face(std::vector<csp_primitive*>& primitives)
{
	for (auto primitive : primitives)
	{
		if (primitive->is_skiped())continue;

		if (primitive->get_type() == primitive_triangle &&
			is_culled(&primitive->get_vertex(0)->vertex,
				&primitive->get_vertex(1)->vertex,
				&primitive->get_vertex(2)->vertex))
			primitive->set_skiped(true);
	}
}

void csp_device::projection(std::vector<csp_primitive*>& primitives)
{
	int count = 0;
	for (auto primitive : primitives)
	{
		if (primitive->is_skiped())continue;

		count = primitive->get_type();

		for (int i = 0; i < count; ++i)
			projection(&primitive->get_vertex(i)->vertex);
	}
}

void csp_device::transform_homogenize(std::vector<csp_primitive*>& primitives)
{
	int count = 0;
	for (auto primitive : primitives)
	{
		if (primitive->is_skiped())continue;

		count = primitive->get_type();

		for (int i = 0; i < count; ++i)
			transform_homogenize(&primitive->get_vertex(i)->vertex);
	}
}
