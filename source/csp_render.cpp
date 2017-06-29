#include "csp_render.h"

csp_render::csp_render()
	:m_fill_type(fill_type::draw_fill)
{
}

csp_render::~csp_render()
{
}

void csp_render::draw_vertex(const csp_data_buffer & data_buffer, geometry_type _type, unsigned int geometry_count, unsigned int elem_size)
{
	csp_data_buffer work_buffer = data_buffer;

	unsigned int index = 0;

	while (index <= work_buffer.get_vertex().size() - elem_size)
	{
		float* data = &work_buffer.get_vertex()[index];
		run_vertex_shader(data, elem_size);
		index += elem_size;
	}

	std::cout << work_buffer;
}

void csp_render::draw_element(const csp_data_buffer & data_buffer, geometry_type _type, unsigned int geometry_count, unsigned int elem_size)
{
	
}

bool csp_render::run_vertex_shader(float* data, unsigned int data_size)
{
	if (data_size >= 3)
	{
		data[0] += 10;
		data[1] += 10;
		data[2] += 10;
	}
	return true;
}