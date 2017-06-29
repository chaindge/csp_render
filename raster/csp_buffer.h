#ifndef CSP_BUFFER_H
#define CSP_BUFFER_H
#include <stdlib.h>
#include <functional>

/**
 *	render buffer
 */
template <typename T> class csp_buffer
{
protected:
	T * data;
	unsigned int width;
	unsigned int height;
	unsigned int stride;
	bool inner_buffer;

public:
	csp_buffer()
		:data(nullptr)
		, width(0)
		, height(0)
		, inner_buffer(false)
	{}

	void init_with_exist_buffer(void* buffer, unsigned int _width, unsigned int _height, unsigned int _stride)
	{
		data = (T*)buffer;
		width = _width;
		height = _height;
		stride = _stride;

		inner_buffer = false;
	}

	void init_buffer(unsigned int width, unsigned int height)
	{
		this->stride = width * sizeof(T);
		this->width = width;
		this->height = height;

		this->data = (T *)malloc(height * this->stride);
		inner_buffer = true;
	}

	void free_buffer()
	{
		if (inner_buffer)
		{
			free(this->data);
			this->data = 0;
			this->width = 0;
			this->height = 0;
		}
	}

	void clear_buffer(T value)
	{
		unsigned int count = this->width * this->height;
		for (unsigned int i = 0; i < count; ++i)
			this->data[i] = value;
	}

	void set_value(unsigned int x, unsigned int y, T color)
	{
		if (x < this->width && y < this->height)
			this->data[y * this->width + x] = color;
	}

	T* get_value(unsigned int x, unsigned int y)
	{
		if (x < this->width && y < this->height)
			return &(this->data[y * this->width + x]);
		return nullptr;
	}

	void* get_data() { return (void*)data; }
	unsigned int get_stride() { return stride; }
};

typedef csp_buffer<float       > csp_depth_buffer;
typedef csp_buffer<unsigned int> csp_color_buffer;
typedef csp_buffer<unsigned int> csp_stencil_buffer;

#define  MAX_DEPTH 1.f

class csp_frame_buffer
{
public:
	csp_frame_buffer(){
		m_width = 0.f;
		m_height = 0.f;
		m_bits = 0;

		m_clear_color = 0xff000000;
	}

	~csp_frame_buffer(){

	}

	void init(float width, float height){
		m_color_buffer.init_buffer(width, height);
		m_stencil_buffer.init_buffer(width, height);
		m_depth_buffer.init_buffer(width, height);

		m_width = width;
		m_height = height;
		m_bits = 32;
	}

	void free(){
		m_width = 0.f;
		m_height = 0.f;
		m_bits = 0;

		m_color_buffer.free_buffer();
		m_stencil_buffer.free_buffer();
		m_depth_buffer.free_buffer();
	}

	void clear() {
		m_color_buffer.clear_buffer(m_clear_color);
		m_depth_buffer.clear_buffer(MAX_DEPTH);
		m_stencil_buffer.clear_buffer(0xff000000);
	}

	inline bool depth_test(unsigned int x, unsigned int y, float depth) {
		if (auto p_depth = m_depth_buffer.get_value(x, y)) {
			if ((depth <= *p_depth)) {
				*p_depth = depth;
				return true;
			}
		}
		return false;
	}

	void set_clear_color(color_3f* color) { m_clear_color = color_to_argb(color); }

	int get_color_bits() { return m_bits; }
	void* get_color_buffer_data() { return m_color_buffer.get_data(); }

	float get_width() { return m_width; }
	float get_height() { return m_height; }

	csp_color_buffer* get_color_buffer() { return &m_color_buffer; }
	csp_depth_buffer* get_depth_buffer() { return &m_depth_buffer; }
	csp_stencil_buffer* get_stencil_buffer() { return &m_stencil_buffer; }
protected:
	csp_color_buffer m_color_buffer;
	csp_depth_buffer m_depth_buffer;
	csp_stencil_buffer m_stencil_buffer;

	unsigned int m_clear_color;
	float m_width;
	float m_height;
	char m_bits;
};

class csp_stream
{
public:
	csp_stream() {}
	~csp_stream() {}

	unsigned int size() { return m_vertex_stream.size(); }

	void clear() {
		m_vertex_stream.clear();
		m_color_stream.clear();
		m_texture_stream.clear();
	}

	void sub_vertex_3d(void* pdata, int vertex_count, int offset)
	{
		unsigned char* pbuf = (unsigned char*)pdata;
		for (int i = 0; i < vertex_count; ++i) {
			auto p_cur = (float*)(pbuf + offset * i);
			m_vertex_stream.push_back({ p_cur[0],p_cur[1],p_cur[2],1.f });
		}
	}

	void sub_color_4f(void* pdata, int color_count, int offset) {
		unsigned char* pbuf = (unsigned char*)pdata;
		for (int i = 0; i < color_count; ++i) {
			auto p_cur = (float*)(pbuf + offset * i);
			m_color_stream.push_back({ p_cur[0],p_cur[1],p_cur[2],p_cur[3] });
		}
	}

	void sub_texture_2d(void* pdata, int coord_count, int offset) {
		unsigned char* pbuf = (unsigned char*)pdata;
		for (int i = 0; i < coord_count; ++i) {
			auto p_cur = (float*)(pbuf + offset * i);
			m_texture_stream.push_back({ p_cur[0],p_cur[1] });
		}
	}

	void get_vertex(unsigned int index, vector_4d* vertex, color_4f* color, vector_2d* texture) {
		if (vertex && index < m_vertex_stream.size())
			*vertex = m_vertex_stream[index];

		if (color && index < m_color_stream.size())
			*color = m_color_stream[index];

		if (texture && index < m_texture_stream.size())
			*texture = m_texture_stream[index];
	}

protected:
	std::vector<vector_4d> m_vertex_stream;
	std::vector<color_4f> m_color_stream;
	std::vector<vector_2d> m_texture_stream;
};

#endif