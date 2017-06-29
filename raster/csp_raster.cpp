#include "csp_raster.h"
#include "csp_device.h"

#include <math.h>

csp_rasterizer::csp_rasterizer(csp_device* device, int clip_w, int clip_h)
{
	m_device = device;

	m_min_clip_x = m_min_clip_y = 0;
	m_max_clip_x = clip_w;
	m_max_clip_y = clip_h;
}

csp_rasterizer::~csp_rasterizer() {}

void csp_rasterizer::put_pixel(int x, int y, unsigned int color)
{
	m_device->get_frame_buffer()->get_color_buffer()->set_value(x, y, color);
}

void csp_rasterizer::draw_triangle_fill(csp_raster_vertex * v1, csp_raster_vertex * v2, csp_raster_vertex * v3)
{
	float label = (v2->vertex.x - v1->vertex.x) * (v3->vertex.y - v1->vertex.y) - (v2->vertex.y - v1->vertex.y) * (v3->vertex.x - v1->vertex.x);
	if (float_equal(label, 0.f)) return;
	if (label < 0)
	{
		exchange(v2, v3);
	}

	// 28.4 fixed-point coordinates
	const int Y1 = lroundf(16.0f * v1->vertex.y);
	const int Y2 = lroundf(16.0f * v2->vertex.y);
	const int Y3 = lroundf(16.0f * v3->vertex.y);

	const int X1 = lroundf(16.0f * v1->vertex.x);
	const int X2 = lroundf(16.0f * v2->vertex.x);
	const int X3 = lroundf(16.0f * v3->vertex.x);

	// Deltas
	const int DX12 = X2 - X1;
	const int DX23 = X3 - X2;
	const int DX31 = X1 - X3;

	const int DY12 = Y2 - Y1;
	const int DY23 = Y3 - Y2;
	const int DY31 = Y1 - Y3;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx = (min(X1, X2, X3) + 0xF) >> 4;
	int maxx = (max(X1, X2, X3) + 0xF) >> 4;
	int miny = (min(Y1, Y2, Y3) + 0xF) >> 4;
	int maxy = (max(Y1, Y2, Y3) + 0xF) >> 4;

	if (minx < m_min_clip_x) minx = m_min_clip_x;
	if (miny < m_min_clip_y) miny = m_min_clip_y;
	if (maxx > m_max_clip_x) maxx = m_max_clip_x;
	if (maxy > m_max_clip_y) maxy = m_max_clip_y;

	// Block size, standard 8x8 (must be power of two)
	const int q = 8;

	// Start in corner of 8x8 block
	minx &= ~(q - 1);
	miny &= ~(q - 1);

	auto stride = m_device->get_frame_buffer()->get_color_buffer()->get_stride();
	auto colorBuffer = (unsigned int*)m_device->get_frame_buffer()->get_color_buffer_data();
	(char*&)colorBuffer += miny * stride;

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	if (DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
	if (DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
	if (DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	color_4f color, tc1, tc2, tc3;
	vector_3d tri_dt;
	vector_3d point_p;
	vector_3d k;

	// caculate Slope
	k.x = ((v1->vertex.y - v2->vertex.y) * v3->vertex.x + (v2->vertex.x - v1->vertex.x) * v3->vertex.y + v1->vertex.x * v2->vertex.y - v2->vertex.x * v1->vertex.y);
	k.y = ((v2->vertex.y - v3->vertex.y) * v1->vertex.x + (v3->vertex.x - v2->vertex.x) * v1->vertex.y + v2->vertex.x * v3->vertex.y - v3->vertex.x * v2->vertex.y);
	k.z = ((v3->vertex.y - v1->vertex.y) * v2->vertex.x + (v1->vertex.x - v3->vertex.x) * v2->vertex.y + v3->vertex.x * v1->vertex.y - v1->vertex.x * v3->vertex.y);

	k.x = 1 / k.x;
	k.y = 1 / k.y;
	k.z = 1 / k.z;

	// Loop through blocks
	for (int y = miny; y < maxy; y += q)
	{
		for (int x = minx; x < maxx; x += q)
		{
			// Corners of block
			int x0 = x << 4;
			int x1 = (x + q - 1) << 4;
			int y0 = y << 4;
			int y1 = (y + q - 1) << 4;

			// Evaluate half-space functions
			bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
			bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
			bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
			bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
			int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

			bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
			bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
			bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
			bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
			int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

			bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
			bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
			bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
			bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
			int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

			// Skip block when outside an edge
			if (a == 0x0 || b == 0x0 || c == 0x0) continue;

			unsigned int *buffer = colorBuffer;

			// Accept whole block when totally covered
			if (a == 0xF && b == 0xF && c == 0xF)
			{
				//for (int iy = 0; iy < q; iy++)
				for (int iy = y; iy < y + q; iy++)
				{
					for (int ix = x; ix < x + q; ix++)
					{
						//buffer[ix] = 0xff7f0000; continue;

						point_p.x = ix; point_p.y = iy;

						// interp in triangle v1,v2,v3, and p
						//tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &tri_dt);
						tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &k, &tri_dt);

						// caculate point p's depth value
						point_p.z = m_cur_depth = v1->vertex.z * tri_dt.x + v2->vertex.z * tri_dt.y + v3->vertex.z * tri_dt.z;

						// depth test
						if (m_device->is_enable(csp_depth_test))
							if (!m_device->get_frame_buffer()->depth_test(ix, iy, m_cur_depth))
								continue;

						// interp color inner triangle v1,v2,v3
						vector_scale(&tc1, &v1->color, tri_dt.x);
						vector_scale(&tc2, &v2->color, tri_dt.y);
						vector_scale(&tc3, &v3->color, tri_dt.z);

						vector_add(&color, &tc1, &tc2);
						vector_add(&color, &color, &tc3);

						buffer[ix] = color_to_argb(&color);
					}

					(char*&)buffer += stride;
				}
			}
			else  // Partially covered block
			{
				int CY1 = C1 + DX12 * y0 - DY12 * x0;
				int CY2 = C2 + DX23 * y0 - DY23 * x0;
				int CY3 = C3 + DX31 * y0 - DY31 * x0;

				for (int iy = y; iy < y + q; iy++)
				{
					int CX1 = CY1;
					int CX2 = CY2;
					int CX3 = CY3;

					for (int ix = x; ix < x + q; ix++)
					{
						if (CX1 > 0 && CX2 > 0 && CX3 > 0)
						{
							//buffer[ix] = 0xff00007f; continue;

							point_p.x = ix; point_p.y = iy;

							// interp in triangle v1,v2,v3, and p
							//tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &tri_dt);
							tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &k, &tri_dt);

							// caculate point p's depth value
							point_p.z = m_cur_depth = v1->vertex.z * tri_dt.x + v2->vertex.z * tri_dt.y + v3->vertex.z * tri_dt.z;

							if (m_device->is_enable(csp_depth_test))
								if (!m_device->get_frame_buffer()->depth_test(ix, iy, m_cur_depth))
									continue;

							// interp color inner triangle v1,v2,v3
							//fragment_shader(&point_p, &tri_dt, &color);
							vector_scale(&tc1, &v1->color, tri_dt.x);
							vector_scale(&tc2, &v2->color, tri_dt.y);
							vector_scale(&tc3, &v3->color, tri_dt.z);

							vector_add(&color, &tc1, &tc2);
							vector_add(&color, &color, &tc3);

							buffer[ix] = color_to_argb(&color);
						}

						CX1 -= FDY12;
						CX2 -= FDY23;
						CX3 -= FDY31;
					}

					CY1 += FDX12;
					CY2 += FDX23;
					CY3 += FDX31;

					(char*&)buffer += stride;
				}
			}
		}

		(char*&)colorBuffer += q * stride;
	}
}

void csp_rasterizer::draw_line_fill(csp_raster_vertex * v1, csp_raster_vertex * v2)
{
	int x1 = ceilf(v1->vertex.x);
	int y1 = ceilf(v1->vertex.y);
	int x2 = ceilf(v2->vertex.x);
	int y2 = ceilf(v2->vertex.y);

	int dx, dy;
	int tx, ty;
	int inc1, inc2;
	int d, iTag;
	int x, y;

	

	m_cur_depth = v1->vertex.z;
	bool depth_test = m_device->is_enable(csp_depth_test);

	if (!(depth_test && !m_device->get_frame_buffer()->depth_test(x1, y1, m_cur_depth)))
	{
		put_pixel(x1, y1, color_to_argb(&v1->color));
	}


	/*
	如果两点重合，结束后面的动作
	*/
	if (x1 == x2 && y1 == y2)
		return;


	iTag = 0;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	float fdx = fabs(v1->vertex.x - v2->vertex.x);
	float fdy = fabs(v1->vertex.y - v2->vertex.y);

	/*
	如果dy为较长长方向，则交换纵横坐标。
	*/
	if (dx < dy)
	{
		iTag = 1;
		exchange(x1, y1);
		exchange(x2, y2);
		exchange(dx, dy);
		exchange(fdx, fdy);
	}

	/*确定是增1还是减1*/
	tx = (x2 - x1) > 0 ? 1 : -1;
	ty = (y2 - y1) > 0 ? 1 : -1;

	x = x1;
	y = y1;

	inc1 = 2 * dy;
	inc2 = 2 * (dy - dx);
	d = inc1 - dx;

	csp_raster_vertex interp_vertex;
	float t = 0.f;
	int pix_x, pix_y;

	/*循环画点*/
	while (x != x2)
	{
		if (d < 0)
			d += inc1;
		else
		{
			y += ty;
			d += inc2;
		}

		float t = fabs(float(x - x1)) / fdx;
		interp(&interp_vertex, v1, v2, t);
		m_cur_depth = interp_vertex.vertex.z;

		iTag ? (pix_x = y, pix_y = x) : (pix_x = x, pix_y = y);

		// depth test
		if (!(depth_test && !m_device->get_frame_buffer()->depth_test(pix_x, pix_y, m_cur_depth)))
		{
			put_pixel(pix_x, pix_y, color_to_argb(&interp_vertex.color));
		}
		
		x += tx;
	}
}


void csp_rasterizer::triangle(vector_4d *v1, vector_4d *v2, vector_4d *v3, unsigned int color)
{
	// value = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1)
	// value = 0 pt(x,y) in line 
	// value > 0 on right side of the line
	// value < 0 on left side of the line

	// label = yc * xb - yb * xc
	// label > 0 counter-clockwise
	// label < 0 clockwise
	// label = 0 abc is a line

	float y1 = v1->y;
	float y2 = v2->y;
	float y3 = v3->y;

	float x1 = v1->x;
	float x2 = v2->x;
	float x3 = v3->x;

	// Bounding rectangle
	int minx = (int)min(x1, x2, x3);
	int maxx = (int)max(x1, x2, x3);
	int miny = (int)min(y1, y2, y3);
	int maxy = (int)max(y1, y2, y3);

	auto stride = m_device->get_frame_buffer()->get_color_buffer()->get_stride();
	auto colorBuffer = (unsigned int*)m_device->get_frame_buffer()->get_color_buffer_data();
	(char*&)colorBuffer += miny * stride;

	// Scan through bounding rectangle
	for (int y = miny; y < maxy; y++)
	{
		for (int x = minx; x < maxx; x++)
		{
			float c1 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
			float c2 = (x3 - x2) * (y - y2) - (y3 - y2) * (x - x2);
			float c3 = (x1 - x3) * (y - y3) - (y1 - y3) * (x - x3);

			// When all half-space functions positive, pixel is in triangle
			if (c1 > 0 && c2 > 0 && c3 > 0)
			{
				colorBuffer[x] = color; // White
			}
		}

		(char*&)colorBuffer += stride;
	}
}

void csp_rasterizer::triangle_opt(vector_4d *v1, vector_4d *v2, vector_4d *v3, unsigned int color)
{
	float y1 = v1->y;
	float y2 = v2->y;
	float y3 = v3->y;

	float x1 = v1->x;
	float x2 = v2->x;
	float x3 = v3->x;

	// Deltas
	float Dx12 = x2 - x1;
	float Dx23 = x3 - x2;
	float Dx31 = x1 - x3;

	float Dy12 = y2 - y1;
	float Dy23 = y3 - y2;
	float Dy31 = y1 - y3;

	// Bounding rectangle
	int minx = (int)min(x1, x2, x3);
	int maxx = (int)max(x1, x2, x3);
	int miny = (int)min(y1, y2, y3);
	int maxy = (int)max(y1, y2, y3);

	auto stride = m_device->get_frame_buffer()->get_color_buffer()->get_stride();
	auto colorBuffer = (unsigned int*)m_device->get_frame_buffer()->get_color_buffer_data();
	(char*&)colorBuffer += miny * stride;

	// Constant part of half-edge functions
	float C1 = Dy12 * x1 - Dx12 * y1;
	float C2 = Dy23 * x2 - Dx23 * y2;
	float C3 = Dy31 * x3 - Dx31 * y3;

	float Cy1 = C1 + Dx12 * miny - Dy12 * minx;
	float Cy2 = C2 + Dx23 * miny - Dy23 * minx;
	float Cy3 = C3 + Dx31 * miny - Dy31 * minx;

	// Scan through bounding rectangle
	for (int y = miny; y < maxy; y++)
	{
		// Start value for horizontal scan
		float Cx1 = Cy1;
		float Cx2 = Cy2;
		float Cx3 = Cy3;

		for (int x = minx; x < maxx; x++)
		{
			if (Cx1 > 0 && Cx2 > 0 && Cx3 > 0)
			{
				colorBuffer[x] = color; // White
			}

			Cx1 -= Dy12;
			Cx2 -= Dy23;
			Cx3 -= Dy31;
		}

		Cy1 += Dx12;
		Cy2 += Dx23;
		Cy3 += Dx31;

		(char*&)colorBuffer += stride;
	}
}

void csp_rasterizer::triangle_opt1(vector_4d *v1, vector_4d *v2, vector_4d *v3, unsigned int color)
{
	// 28.4 fixed-point coordinates
	const int Y1 = lroundf(16.0f * v1->y);
	const int Y2 = lroundf(16.0f * v2->y);
	const int Y3 = lroundf(16.0f * v3->y);

	const int X1 = lroundf(16.0f * v1->x);
	const int X2 = lroundf(16.0f * v2->x);
	const int X3 = lroundf(16.0f * v3->x);

	// Deltas
	const int DX12 = X2 - X1;
	const int DX23 = X3 - X2;
	const int DX31 = X1 - X3;

	const int DY12 = Y2 - Y1;
	const int DY23 = Y3 - Y2;
	const int DY31 = Y1 - Y3;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx = (min(X1, X2, X3) + 0xF) >> 4;
	int maxx = (max(X1, X2, X3) + 0xF) >> 4;
	int miny = (min(Y1, Y2, Y3) + 0xF) >> 4;
	int maxy = (max(Y1, Y2, Y3) + 0xF) >> 4;

	auto stride = m_device->get_frame_buffer()->get_color_buffer()->get_stride();
	auto colorBuffer = (unsigned int*)m_device->get_frame_buffer()->get_color_buffer_data();
	(char*&)colorBuffer += miny * stride;

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	if (DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
	if (DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
	if (DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
	int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
	int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

	for (int y = miny; y < maxy; y++)
	{
		int CX1 = CY1;
		int CX2 = CY2;
		int CX3 = CY3;

		for (int x = minx; x < maxx; x++)
		{
			if (CX1 > 0 && CX2 > 0 && CX3 > 0)
			{
				colorBuffer[x] = color;
			}

			CX1 -= FDY12;
			CX2 -= FDY23;
			CX3 -= FDY31;
		}

		CY1 += FDX12;
		CY2 += FDX23;
		CY3 += FDX31;

		(char*&)colorBuffer += stride;
	}
}

void csp_rasterizer::triangle_opt2(vector_4d *v1, vector_4d *v2, vector_4d *v3)
{
	float label = (v2->x - v1->x) * (v3->y - v1->y) - (v2->y - v1->y) * (v3->x - v1->x);
	if (float_equal(label, 0.f)) return;
	if (label < 0) exchange(v2, v3);

	// 28.4 fixed-point coordinates
	const int Y1 = lroundf(16.0f * v1->y);
	const int Y2 = lroundf(16.0f * v2->y);
	const int Y3 = lroundf(16.0f * v3->y);

	const int X1 = lroundf(16.0f * v1->x);
	const int X2 = lroundf(16.0f * v2->x);
	const int X3 = lroundf(16.0f * v3->x);

	// Deltas
	const int DX12 = X2 - X1;
	const int DX23 = X3 - X2;
	const int DX31 = X1 - X3;

	const int DY12 = Y2 - Y1;
	const int DY23 = Y3 - Y2;
	const int DY31 = Y1 - Y3;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx = (min(X1, X2, X3) + 0xF) >> 4;
	int maxx = (max(X1, X2, X3) + 0xF) >> 4;
	int miny = (min(Y1, Y2, Y3) + 0xF) >> 4;
	int maxy = (max(Y1, Y2, Y3) + 0xF) >> 4;

	if (minx < m_min_clip_x) minx = m_min_clip_x;
	if (miny < m_min_clip_y) miny = m_min_clip_y;
	if (maxx > m_max_clip_x) maxx = m_max_clip_x;
	if (maxy > m_max_clip_y) maxy = m_max_clip_y;

	// Block size, standard 8x8 (must be power of two)
	const int q = 8;

	// Start in corner of 8x8 block
	minx &= ~(q - 1);
	miny &= ~(q - 1);

	auto stride = m_device->get_frame_buffer()->get_color_buffer()->get_stride();
	auto colorBuffer = (unsigned int*)m_device->get_frame_buffer()->get_color_buffer_data();
	(char*&)colorBuffer += miny * stride;

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	if (DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
	if (DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
	if (DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	color_4f color;
	vector_3d tri_dt;
	vector_3d point_p;
	vector_3d k;

	// caculate Slope
	k.x = ((v1->y - v2->y) * v3->x + (v2->x - v1->x) * v3->y + v1->x * v2->y - v2->x * v1->y);
	k.y = ((v2->y - v3->y) * v1->x + (v3->x - v2->x) * v1->y + v2->x * v3->y - v3->x * v2->y);
	k.z = ((v3->y - v1->y) * v2->x + (v1->x - v3->x) * v2->y + v3->x * v1->y - v1->x * v3->y);

	k.x = 1 / k.x;
	k.y = 1 / k.y;
	k.z = 1 / k.z;

	// Loop through blocks
	for (int y = miny; y < maxy; y += q)
	{
		for (int x = minx; x < maxx; x += q)
		{
			// Corners of block
			int x0 = x << 4;
			int x1 = (x + q - 1) << 4;
			int y0 = y << 4;
			int y1 = (y + q - 1) << 4;

			// Evaluate half-space functions
			bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
			bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
			bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
			bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
			int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

			bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
			bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
			bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
			bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
			int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

			bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
			bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
			bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
			bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
			int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

			// Skip block when outside an edge
			if (a == 0x0 || b == 0x0 || c == 0x0) continue;

			unsigned int *buffer = colorBuffer;

			// Accept whole block when totally covered
			if (a == 0xF && b == 0xF && c == 0xF)
			{
				//for (int iy = 0; iy < q; iy++)
				for (int iy = y; iy < y + q; iy++)
				{
					for (int ix = x; ix < x + q; ix++)
					{
						//buffer[ix] = 0xff7f0000; continue;

						point_p.x = ix; point_p.y = iy;

						// interp in triangle v1,v2,v3, and p
						//tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &tri_dt);
						tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &k, &tri_dt);

						// caculate point p's depth value
						point_p.z = m_cur_depth = v1->z * tri_dt.x + v2->z * tri_dt.y + v3->z * tri_dt.z;

						// depth test
						if (m_device->is_enable(csp_depth_test))
							if (!m_device->get_frame_buffer()->depth_test(ix, iy, m_cur_depth))
								continue;

						// interp color inner triangle v1,v2,v3
						fragment_shader(&point_p, &tri_dt, &color);
						buffer[ix] = color_to_argb(&color);
					}

					(char*&)buffer += stride;
				}
			}
			else  // Partially covered block
			{
				int CY1 = C1 + DX12 * y0 - DY12 * x0;
				int CY2 = C2 + DX23 * y0 - DY23 * x0;
				int CY3 = C3 + DX31 * y0 - DY31 * x0;

				for (int iy = y; iy < y + q; iy++)
				{
					int CX1 = CY1;
					int CX2 = CY2;
					int CX3 = CY3;

					for (int ix = x; ix < x + q; ix++)
					{
						if (CX1 > 0 && CX2 > 0 && CX3 > 0)
						{
							//buffer[ix] = 0xff00007f; continue;

							point_p.x = ix; point_p.y = iy;

							// interp in triangle v1,v2,v3, and p
							//tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &tri_dt);
							tri_interp((vector_2d*)v1, (vector_2d*)v2, (vector_2d*)v3, (vector_2d*)&point_p, &k, &tri_dt);

							// caculate point p's depth value
							point_p.z = m_cur_depth = v1->z * tri_dt.x + v2->z * tri_dt.y + v3->z * tri_dt.z;

							if (m_device->is_enable(csp_depth_test))
								if (!m_device->get_frame_buffer()->depth_test(ix, iy, m_cur_depth))
									continue;

							// interp color inner triangle v1,v2,v3
							fragment_shader(&point_p, &tri_dt, &color);
							buffer[ix] = color_to_argb(&color);
						}

						CX1 -= FDY12;
						CX2 -= FDY23;
						CX3 -= FDY31;
					}

					CY1 += FDX12;
					CY2 += FDX23;
					CY3 += FDX31;

					(char*&)buffer += stride;
				}
			}
		}

		(char*&)colorBuffer += q * stride;
	}
}

void csp_rasterizer::BresenhamLine(vector_4d *v1, vector_4d *v2, unsigned int color)
{
	int x1 = ceilf(v1->x);
	int y1 = ceilf(v1->y);
	int x2 = ceilf(v2->x);
	int y2 = ceilf(v2->y);

	int dx, dy;
	int tx, ty;
	int inc1, inc2;
	int d, iTag;
	int x, y;

	put_pixel(x1, y1, color);

	/*
	如果两点重合，结束后面的动作
	*/
	if (x1 == x2 && y1 == y2)
		return;


	iTag = 0;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	/*
	如果dy为计长方向，则交换纵横坐标。
	*/
	if (dx < dy)
	{
		iTag = 1;
		exchange(x1, y1);
		exchange(x2, y2);
		exchange(dx, dy);
	}

	/*确定是增1还是减1*/
	tx = (x2 - x1) > 0 ? 1 : -1;
	ty = (y2 - y1) > 0 ? 1 : -1;

	x = x1;
	y = y1;

	inc1 = 2 * dy;
	inc2 = 2 * (dy - dx);
	d = inc1 - dx;

	/*循环画点*/
	while (x != x2)
	{
		if (d < 0)
			d += inc1;
		else
		{
			y += ty;
			d += inc2;
		}


		if (iTag)
			put_pixel(y, x, color);
		else
			put_pixel(x, y, color);

		x += tx;
	}
}

