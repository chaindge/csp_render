#include "csp_data_buffer.h"
#include <algorithm>

csp_data_buffer::csp_data_buffer(float * vertex, unsigned int vertex_size, unsigned int * index, unsigned int index_size)
{
	set_vertex(vertex, vertex_size);
	set_index(index, index_size);
}

std::vector<float>& csp_data_buffer::get_vertex()
{
	return m_vertex;
}

std::vector<unsigned int>& csp_data_buffer::get_index()
{
	return m_index;
}

void csp_data_buffer::set_vertex(float* data, unsigned int data_size)
{
	if (data)m_vertex.assign(data, data + data_size);
}
void csp_data_buffer::add_vertex(float* data, unsigned int data_size)
{
	if (data)m_vertex.insert(m_vertex.end(), data, data + data_size);
}
void csp_data_buffer::set_index(unsigned int* data, unsigned int data_size)
{
	if (data)m_index.assign(data, data + data_size);
}
void csp_data_buffer::add_index(unsigned int* data, unsigned int data_size)
{
	if (data)m_index.insert(m_index.end(), data, data + data_size);
}

bool csp_data_buffer::get_data_from_vertex(std::vector<float>& data, unsigned int begin, unsigned int count)
{
	if (begin > m_vertex.size() - count)return false;

	auto iter_begin = m_vertex.begin() + begin;
	auto iter_end = iter_begin + count;

	data.assign(iter_begin, iter_end);
	return true;
}

bool csp_data_buffer::get_data_from_index(std::vector<float>& data, unsigned int begin, unsigned int count)
{
	if (begin > m_index.size() - count) return false;

	auto iter_begin = m_index.begin() + begin;
	auto iter_end = iter_begin + count;

	data.clear();

	while (iter_begin != iter_end)
	{
		if (*iter_begin >= m_vertex.size())return false;

		data.push_back(m_vertex[*iter_begin]);

		iter_begin++;
	}
	return true;
}