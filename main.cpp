#include <iostream>

#include "raster/csp_device.h"
#include "SOIL.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "SOIL.lib")

#include <time.h>
#include "wnd.h"

#include "Console/ConsoleSingleton.h"

#include "timer/SimpleTimer.h"

void save_file(csp_device* dc, const std::string& path)
{
	auto fb = dc->get_frame_buffer();

	int chanels = fb->get_color_bits() >> 3;
	int width = fb->get_width();
	int height = fb->get_height();

	unsigned char* buffer = (unsigned char*)fb->get_color_buffer_data();
	SOIL_save_image(path.c_str(), SOIL_SAVE_TYPE_BMP, width, height, chanels, buffer);
}


void bind_device(window* wnd, csp_device* dc)
{
	wnd->framebuffer = dc->get_frame_buffer()->get_color_buffer_data();
}

void offset(vector_4d* v, float x, float y)
{
	v->x += x;
	v->y += y;
}

// void draw(csp_device* dc )
// {
// 	csp_raster_vertex v1, v2, v3, v4, v5, v6;
// 	color_4f toy_color = { 1.f,0.5f,0.31f,0.5f };
// 	
// 	float alpha = 0.8f;
// 
// 	v1._vertex.x = 0;
// 	v1._vertex.y = 0;
// 	v1._vertex.z = 1;
// 	v1._vertex.w = 1;
// 	v1._color = { 1.0f,0.f,0.f,alpha };
// 
// 	v2._vertex.x = 800;
// 	v2._vertex.y = 600;
// 	v2._vertex.z = 1;
// 	v2._vertex.w = 1;
// 	v2._color = { 0.0f,1.f,0.f,alpha };
// 
// 	v3._vertex.x = 12;
// 	v3._vertex.y = 500;
// 	v3._vertex.z = 1;
// 	v3._vertex.w = 1;
// 	v3._color = { 0.f,0.f,1.f,alpha };
// 
// 	v4._vertex.x = 100;
// 	v4._vertex.y = 300;
// 	v4._vertex.z = 1;
// 	v4._vertex.w = 1;
// 	v4._color = toy_color;
// 
// 	v5._vertex.x = 300;
// 	v5._vertex.y = 100;
// 	v5._vertex.z = 1;
// 	v5._vertex.w = 1;
// 	v5._color = toy_color;
// 
// 	v6._vertex.x = 100;
// 	v6._vertex.y = 100;
// 	v6._vertex.z = 1;
// 	v6._vertex.w = 1;
// 	v6._color = toy_color;
// 
// 	
// 	int d_width = 800, d_height = 600;
// 
// 	dc->clear_buffer();
// 
// // 	for (int i = 0; i < 1; ++i)
// // 	{
// // 		v1._vertex.x = rand() % (d_width - 1);
// // 		v1._vertex.y = rand() % (d_height - 1);
// // 		v2._vertex.x = rand() % (d_width - 1);
// // 		v2._vertex.y = rand() % (d_height - 1);
// // 		v3._vertex.x = rand() % (d_width - 1);
// // 		v3._vertex.y = rand() % (d_height - 1);
// // 
// // 		dc->triangle(&v1, &v2, &v3, csp_use_color);
// // 	}
// 
// 	dc->triangle(&v1, &v2, &v3, csp_use_color,true);
// 
// // 	for (int i = 0; i < 10; i++)
// // 	{
// // 		dc->triangle(&v5, &v4, &v6, csp_use_color);
// // 		offset(&v4._vertex, 40, 20);
// // 		offset(&v5._vertex, 40, 20);
// // 		offset(&v6._vertex, 40, 20);
// // 	}
// 
// 	save_file(dc, "triangle.png");
// }

void draw(csp_device* dc)
{
	dc->clear_buffer();
	dc->clear_stream();

	float vertex[] = 
	{
		-1, 0, 1, 1, 0, 0, 1,
		 1, 0, 1, 0, 1, 0, 1,
		 0, 0,-1, 0, 0, 1, 1,
		 0, 1, 0, 1, 1, 0, 1
	};

	unsigned int index[] =
	{
		0,2,1,
		3,0,1,
		3,1,2,
		3,2,0
	};
	
	dc->sub_data(sub_vertex, vertex, 4, sizeof(float) * 7);
	dc->sub_data(sub_color, vertex + 3, 4, sizeof(float) * 7);
	dc->sub_index(index, sizeof(index), sizeof(index[0]));

	dc->draw_index(draw_triangle, 4);
}

bool key_down[256] = { false };

void on_keydown(WPARAM wParam, LPARAM lParam)
{
	key_down[wParam] = true;
}

void on_keyup(WPARAM wParam, LPARAM lParam)
{
	key_down[wParam] = false;
}

void init(csp_device* dc, window* wnd)
{
	dc->init_device(wnd->w, wnd->h);

	color_3f clear_color = { 0.65f,0.65f,0.65f };
	dc->set_clear_color(&clear_color);

	//dc.disable(csp_blend);
	//dc->disable(csp_depth_test);

	dc->front_face(csp_ccw);
	dc->cull_face(csp_back_face);
	dc->enable(csp_cull_face);
	//dc->disable(csp_cull_face);
	dc->set_fill_mode(fill_mode_frame);

	dc->setPerspective(MATH_PIOVER2 * 0.5, wnd->w / wnd->h, 1.f, 100.f);

	bind_device(wnd, dc);

	register_msg(wnd, WM_KEYDOWN, on_keydown);
	register_msg(wnd, WM_KEYUP, on_keyup);

	debug_con << "init device succeed! screen_width=" << wnd->w << ",screen_height=" << wnd->h << "";
}


int main()
{
	float eye_angle = 0.f;
	int d_width = 800, d_height = 600;

	// create window 
	auto wnd = create_window(d_width, d_height, "test_wnd");
	if (wnd == nullptr)
	{
		debug_con << "create window failed!" << "";
		return 0;
	}

	// create render device
	csp_device dc;
	init(&dc, wnd);

	vector_3d eye = { 0.f,0.f,5.f };
	vector_3d targt = { 0.f,0.f,-1.f };

	vector_3d up = { 0.f,1.f,0.f };
	vector_3d right = { 1.f,0.f,0.f };
	vector_3d forward = { 0.f,0.f,-1.f };

	//dc.setLookAt(eye.x, eye.y, eye.z, targt.x, targt.y, targt.z, up.x, up.y, up.z);


	MSG msg;
	memset(&msg, 0, sizeof(msg));

	matrix_4d transform = { 0.f };

	_SimpleTimer->reset();
	_SimpleTimer->set_updator_fps(120);

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (_SimpleTimer->ready_for_next_update())
			{
				auto dt = _SimpleTimer->time_elapsed();
				//debug_con << "dt=" << dt << "s" << "";

				eye_angle += dt * 1.5;
				if (eye_angle > MATH_PI * 2)
					eye_angle = 0.f;

				auto x = sinf(eye_angle) * 5;
				auto z = cosf(eye_angle) * 5;

				matrix_4d translate, rotation;
				create_translation(x, 0.f, z, &translate);
				create_rotation({ 1.f,1.f,1.f }, eye_angle, &rotation);
				matrix_mul(&transform, &translate, &rotation);
				dc.setModelMatrix(&rotation);


				if (key_down[VK_UP]) {
					eye.z -= dt;
					vector_add(&targt, &eye, &forward);
					dc.setLookAt(eye.x, eye.y, eye.z, targt.x, targt.y, targt.z, up.x, up.y, up.z);
				}

				if (key_down[VK_DOWN]) {
					eye.z += dt;
					vector_add(&targt, &eye, &forward);
					dc.setLookAt(eye.x, eye.y, eye.z, targt.x, targt.y, targt.z, up.x, up.y, up.z);
				}

				if (key_down['A']) {
					eye.x -= dt;
					vector_add(&targt, &eye, &forward);
					dc.setLookAt(eye.x, eye.y, eye.z, targt.x, targt.y, targt.z, up.x, up.y, up.z);
				}

				if (key_down['D']) {
					eye.x += dt;
					vector_add(&targt, &eye, &forward);
					dc.setLookAt(eye.x, eye.y, eye.z, targt.x, targt.y, targt.z, up.x, up.y, up.z);
				}

				if (key_down['W']) {
					eye.y += dt;
					vector_add(&targt, &eye, &forward);
					dc.setLookAt(eye.x, eye.y, eye.z, targt.x, targt.y, targt.z, up.x, up.y, up.z);
				}

				if (key_down['S']) {
					eye.y -= dt;
					vector_add(&targt, &eye, &forward);
					dc.setLookAt(eye.x, eye.y, eye.z, targt.x, targt.y, targt.z, up.x, up.y, up.z);
				}

				draw(&dc);
				update_window(wnd);
			}
		}
	}

	save_file(&dc, "triangle.png");

	close_window(wnd);
	dc.free_device();

	debug_con << "exit!" << "";

	return 0;
}