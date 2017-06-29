#ifndef CSP_RENDER_TYPE_H
#define CSP_RENDER_TYPE_H

//  ���ݻ�������
enum data_buffer_type
{
	buffer_vertex,	// this buffer is a vertex buffer
	buffer_index	// this buffer is a index buffer
};

// ��������
enum geometry_type
{
	geometry_point,
	geometry_line,
	geometry_triangle,
};

// ���ģʽ
enum fill_type
{
	draw_frame,	// �߿�
	draw_fill,	// ���
};


#endif