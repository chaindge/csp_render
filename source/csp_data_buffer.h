#ifndef CSP_DATA_BUFFER_H
#define CSP_DATA_BUFFER_H
#include <vector>
#include <iostream>
#include "csp_render_type.h"

class csp_data_buffer
{
public:
	csp_data_buffer(float* vertex, unsigned int vertex_size,
		unsigned int * index, unsigned int index_size);

	void set_vertex(float* data, unsigned int data_size);
	void add_vertex(float* data, unsigned int data_size);
	void set_index(unsigned int* data, unsigned int data_size);
	void add_index(unsigned int* data, unsigned int data_size);

	bool get_data_from_vertex(std::vector<float>& data, unsigned int begin, unsigned int count);
	bool get_data_from_index(std::vector<float>& data, unsigned int begin, unsigned int count);

	std::vector<float>& get_vertex();
	std::vector<unsigned int>& get_index();
protected:
	std::vector<float> m_vertex;
	std::vector<unsigned int> m_index;
};

template<class T>
void print_vector(std::ostream& out,const std::vector<T>& vec)
{
	for (auto& item : vec)
		out << item << " ";
}

inline void print(std::ostream& out,csp_data_buffer& buffer)
{
	out << "vertex :";
	print_vector(out,buffer.get_vertex());
	out << std::endl;

	out << "index :";
	print_vector(out,buffer.get_index());
	out << std::endl;
}

inline std::ostream& operator<<(std::ostream& out,csp_data_buffer& buffer)
{
	print(out, buffer);
	return out;
}

#endif // !CSP_DATA_BUFFER_H
