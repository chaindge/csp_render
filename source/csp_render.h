#ifndef CSP_RENDER_H
#define CSP_RENDER_H
#include <vector>
#include "csp_render_type.h"
#include "csp_data_buffer.h"

//class frame_buffer;
class csp_data_buffer;

class csp_render
{
public:
	csp_render();
	virtual ~csp_render();
	
	void draw_vertex(
		const csp_data_buffer& data_buffer,
		geometry_type _type,
		unsigned int geometry_count, 
		unsigned int elem_size);

	void draw_element(
		const csp_data_buffer& data_buffer,
		geometry_type _type,
		unsigned int geometry_count,
		unsigned int elem_size);
protected:
	virtual bool run_vertex_shader(float* data,unsigned int data_size);		// �������ݴ���	
// 	virtual bool run_geometry_shader(geometry_type _type);		// ������װ
// 	virtual bool run_clip();				// �ü�
// 	virtual bool run_rasterizer();			// դ��
// 	virtual bool run_fragment_shader();		// ������ɫ
public:
	// getter
	//frame_buffer* get_frame_buffer();
	
	// setter
//	void set_fill_type(fill_type _type);
	//void set_data_buffer(const csp_data_buffer& data_buffer);
protected:
	fill_type		m_fill_type;	//

	//csp_data_buffer m_data_buffer;
	//frame_buffer*	m_frame_buffer;
};

#endif