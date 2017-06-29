#ifndef CSP_RENDER_TYPE_H
#define CSP_RENDER_TYPE_H

//  数据缓存类型
enum data_buffer_type
{
	buffer_vertex,	// this buffer is a vertex buffer
	buffer_index	// this buffer is a index buffer
};

// 几何类型
enum geometry_type
{
	geometry_point,
	geometry_line,
	geometry_triangle,
};

// 填充模式
enum fill_type
{
	draw_frame,	// 边框
	draw_fill,	// 填充
};


#endif