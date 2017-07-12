#include "csp_math.h"
#include <math.h>
#include <memory.h>


float interp(float x1, float x2, float t) 
{ 
	return x1 + (x2 - x1) * t; 
}

inline float fast_mul(float f1, float f2)
{
	float ret = 0.f;
// 	__asm
// 	{
// 		fld	f1
// 		fld	f2
// 		fmul
// 		fst ret
// 	}
	return ret;
}

inline float fast_div(float f1, float f2)
{
	float ret = 0.f;
// 	__asm
// 	{
// 		fld	f1
// 		fld	f2
// 		fdiv
// 		fst ret
// 	}
	return ret;
}

void tri_interp(vector_2d * _a, vector_2d * _b, vector_2d * _c, vector_2d * _p, vector_3d * dt)
{
	float u, v;
 	float ax = _c->x - _a->x;
 	float ay = _c->y - _a->y;
 	float bx = _b->x - _a->x;
 	float by = _b->y - _a->y;
 	float cx = _p->x - _a->x;
 	float cy = _p->y - _a->y;

	v = (cy * bx - by * cx) / (ay * bx - by * ax);
	u = (cx - ax * v) / bx;

	dt->x = 1 - u - v;
	dt->y = u;
	dt->z = v;
}

void tri_interp(vector_2d * v1, vector_2d * v2, vector_2d * v3, vector_2d * _p, vector_3d * k, vector_3d * dt)
{
	dt->x = ((v2->y - v3->y) * _p->x + (v3->x - v2->x) * _p->y + v2->x * v3->y - v3->x * v2->y) * k->y;
	dt->y =	((v3->y - v1->y) * _p->x + (v1->x - v3->x) * _p->y + v3->x * v1->y - v1->x * v3->y) * k->z;
	dt->z = 1 - dt->x - dt->y;
}

bool float_equal(float x1, float x2)
{
	return fabs(x1 - x2) < MATH_EPSILON;
}

float vector_length(vector_2d* v2d)
{
	return sqrtf(v2d->x * v2d->x + v2d->y * v2d->y);
}

float vector_length(vector_3d* v3d)
{
	return sqrtf(v3d->x * v3d->x + v3d->y * v3d->y + v3d->z * v3d->z);
}

float vector_length(vector_4d* v4d)
{
	return sqrtf(v4d->x * v4d->x + v4d->y * v4d->y + v4d->z * v4d->z);
}

float vector_length2(vector_2d* v2d)
{
	return v2d->x * v2d->x + v2d->y * v2d->y;
}

float vector_length2(vector_3d* v3d)
{
	return v3d->x * v3d->x + v3d->y * v3d->y + v3d->z * v3d->z;
}

float vector_length2(vector_4d* v4d)
{
	return v4d->x * v4d->x + v4d->y * v4d->y + v4d->z * v4d->z;
}

void vector_add(vector_2d* result, const vector_2d* v1, const vector_2d* v2)
{
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
}

void vector_add(vector_3d* result, const vector_3d* v1, const vector_3d* v2)
{
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
	result->z = v1->z + v2->z;
}

void vector_add(vector_4d* result, const vector_4d* v1, const vector_4d* v2)
{
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
	result->z = v1->z + v2->z;
	result->w = v1->w + v2->w;
}

void vector_sub(vector_2d* result, const vector_2d* v1, const vector_2d* v2)
{
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
}

void vector_sub(vector_3d* result, const vector_3d* v1, const vector_3d* v2)
{
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
	result->z = v1->z - v2->z;
}
void vector_sub(vector_4d* result, const vector_4d* v1, const vector_4d* v2)
{
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
	result->z = v1->z - v2->z;
	result->w = v1->w - v2->w;
}

void vector_cross(vector_3d* result, const vector_3d* v1, const vector_3d* v2)
{
	float x = v1->y * v2->z - v1->z * v2->y;
	float y = v1->z * v2->x - v1->x * v2->z;
	float z = v1->x * v2->y - v1->y * v2->x;

	result->x = x;
	result->y = y;
	result->z = z;
}

void vector_cross(vector_4d* result, const vector_4d* v1, const vector_4d* v2)
{
	float x = v1->y * v2->z - v1->z * v2->y;
	float y = v1->z * v2->x - v1->x * v2->z;
	float z = v1->x * v2->y - v1->y * v2->x;

	result->x = x;
	result->y = y;
	result->z = z;
	result->w = 1.0f;
}

float vector_dot(const vector_2d* v1, const vector_2d* v2)
{
	return v1->x * v2->x + v1->y * v2->y;
}

float vector_dot(const vector_3d* v1, const vector_3d* v2)
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

float vector_dot(const vector_4d* v1, const vector_4d* v2)
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

void vector_interp(vector_2d* result, const vector_2d* v1, const vector_2d* v2, float t)
{
	result->x = interp(v1->x, v2->x, t);
	result->y = interp(v1->y, v2->y, t);
}

void vector_interp(vector_3d* result, const vector_3d* v1, const vector_3d* v2, float t)
{
	result->x = interp(v1->x, v2->x, t);
	result->y = interp(v1->y, v2->y, t);
	result->z = interp(v1->z, v2->z, t);
}

void vector_interp(vector_4d* result, const vector_4d* v1, const vector_4d* v2, float t)
{
	result->x = interp(v1->x, v2->x, t);
	result->y = interp(v1->y, v2->y, t);
	result->z = interp(v1->z, v2->z, t);
	result->w = interp(v1->w, v2->w, t);
}

void vector_normalize(vector_2d* v)
{
	float length = vector_length(v);
	if (!float_equal(length, 0.0f)) {
		float length_r = 1 / length;
		v->x *= length_r;
		v->y *= length_r;
	}
}

void vector_normalize(vector_3d* v)
{
	float length = vector_length(v);
	if (!float_equal(length, 0.0f)) {
		float length_r = 1 / length;
		v->x *= length_r;
		v->y *= length_r;
		v->z *= length_r;
	}
}

void vector_normalize(vector_4d* v)
{
	float length = vector_length(v);
	if (!float_equal(length, 0.0f)) {
		float length_r = 1 / length;
		v->x *= length_r;
		v->y *= length_r;
		v->z *= length_r;
	}
}

void vector_negate(vector_2d* v)
{
	v->x = -v->x;
	v->y = -v->y;
}

void vector_negate(vector_3d* v)
{
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}

void vector_negate(vector_4d* v)
{
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
	v->w = 1.0f;
}

void vector_add(vector_2d* v, float value)
{
	v->x += value;
	v->y += value;
}

void vector_add(vector_3d* v, float value)
{
	v->x += value;
	v->y += value;
	v->z += value;
}

void vector_add(vector_4d* v, float value)
{
	v->x += value;
	v->y += value;
	v->z += value;
	v->w += value;
}

void vector_sub(vector_2d* v, float value)
{
	v->x -= value;
	v->y -= value;
}

void vector_sub(vector_3d* v, float value)
{
	v->x -= value;
	v->y -= value;
	v->z -= value;
}

void vector_sub(vector_4d* v, float value)
{
	v->x -= value;
	v->y -= value;
	v->z -= value;
	v->w -= value;
}

void vector_scale(vector_2d* v, float value)
{
	v->x *= value;
	v->y *= value;
}

void vector_scale(vector_3d* v, float value)
{
	v->x *= value;
	v->y *= value;
	v->z *= value;
}

void vector_scale(vector_4d* v, float value)
{
	v->x *= value;
	v->y *= value;
	v->z *= value;
	v->w *= value;
}

void vector_scale(vector_2d* result, const vector_2d* v, float value)
{
	result->x = v->x * value;
	result->y = v->y * value;
}

void vector_scale(vector_3d* result, const vector_3d* v, float value)
{
	result->x = v->x * value;
	result->y = v->y * value;
	result->z = v->z * value;
}

void vector_scale(vector_4d* result, const vector_4d* v, float value)
{
	result->x = v->x * value;
	result->y = v->y * value;
	result->z = v->z * value;
	result->w = v->w * value;
}

void vector_div(vector_2d* v, float value)
{
	if (!float_equal(value, 0.0f)) {
		float value_r = 1 / value;
		v->x *= value_r;
		v->y *= value_r;
	}
}

void vector_div(vector_3d* v, float value)
{
	if (!float_equal(value, 0.0f)) {
		float value_r = 1 / value;
		v->x *= value_r;
		v->y *= value_r;
		v->z *= value_r;
	}
}

void vector_div(vector_4d* v, float value)
{
	if (!float_equal(value, 0.0f)) {
		float value_r = 1 / value;
		v->x *= value_r;
		v->y *= value_r;
		v->z *= value_r;
		v->w *= value_r;
	}
}

// matrix - matrix
void matrix_add(matrix_3d* result, const matrix_3d* m1, const matrix_3d* m2)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result->m[i][j] = m1->m[i][j] + m2->m[i][j];
}

void matrix_add(matrix_4d* result, const matrix_4d* m1, const matrix_4d* m2)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result->m[i][j] = m1->m[i][j] + m2->m[i][j];
}

void matrix_sub(matrix_3d* result, const matrix_3d* m1, const matrix_3d* m2)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result->m[i][j] = m1->m[i][j] - m2->m[i][j];
}

void matrix_sub(matrix_4d* result, const matrix_4d* m1, const matrix_4d* m2)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result->m[i][j] = m1->m[i][j] - m2->m[i][j];
}

void matrix_mul(matrix_3d* result, const matrix_3d* m1, const matrix_3d* m2)
{
	matrix_3d temp;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			temp.m[i][j] = m1->m[i][0] * m2->m[0][j]
			+ m1->m[i][1] * m2->m[1][j] 
			+ m1->m[i][2] * m2->m[2][j];
	*result = temp;
}

void matrix_mul(matrix_4d* result, const matrix_4d* m1, const matrix_4d* m2)
{
	matrix_4d temp;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			temp.m[i][j]	= m1->m[i][0] * m2->m[0][j]
			+ m1->m[i][1] * m2->m[1][j]
			+ m1->m[i][2] * m2->m[2][j]
			+ m1->m[i][3] * m2->m[3][j];
	*result = temp;
}

// matrix - scalar
void matrix_add(matrix_3d* result, const matrix_3d* m1, float value)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result->m[i][j] = m1->m[i][j] + value;
}

void matrix_add(matrix_4d* result, const matrix_4d* m1, float value)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result->m[i][j] = m1->m[i][j] + value;
}

void matrix_sub(matrix_3d* result, const matrix_3d* m1, float value)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result->m[i][j] = m1->m[i][j] - value;
}

void matrix_sub(matrix_4d* result, const matrix_4d* m1, float value)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result->m[i][j] = m1->m[i][j] - value;
}

void matrix_scale(matrix_3d* result, const matrix_3d* m1, float value)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result->m[i][j] = m1->m[i][j] * value;
}

void matrix_scale(matrix_4d* result, const matrix_4d* m1, float value)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result->m[i][j] = m1->m[i][j] * value;
}

// matrix - vector
// result = m * col_vector
void matrix_apply_col(vector_2d* result, const matrix_3d* m, const vector_2d* col_vector)
{
	float x = col_vector->x, y = col_vector->y, z = 1.0f;
	result->x = m->m[0][0] * x + m->m[0][1] * y + m->m[0][2] * z;
	result->y = m->m[1][0] * x + m->m[1][1] * y + m->m[1][2] * z;
	//result->z = m->m[2][0] * x + m->m[2][1] * y + m->m[2][2] * z;
}

void matrix_apply_col(vector_3d * result, const matrix_3d * m, const vector_3d * col_vector)
{
	float x = col_vector->x, y = col_vector->y, z = col_vector->z;
	result->x = m->m[0][0] * x + m->m[0][1] * y + m->m[0][2] * z;
	result->y = m->m[1][0] * x + m->m[1][1] * y + m->m[1][2] * z;
	result->z = m->m[2][0] * x + m->m[2][1] * y + m->m[2][2] * z;
}

void matrix_apply_col(vector_3d* result, const matrix_4d* m, const vector_3d* col_vector)
{
	float x = col_vector->x, y = col_vector->y, z = col_vector->z, w = 1.0f;
	result->x = m->m[0][0] * x + m->m[0][1] * y + m->m[0][2] * z + m->m[0][3] * w;
	result->y = m->m[1][0] * x + m->m[1][1] * y + m->m[1][2] * z + m->m[1][3] * w;
	result->z = m->m[2][0] * x + m->m[2][1] * y + m->m[2][2] * z + m->m[2][3] * w;
	//result->w = m->m[3][0] * x + m->m[3][1] * y + m->m[3][2] * z + m->m[3][3] * w;
}

void matrix_apply_col(vector_4d* result, const matrix_4d* m, const vector_4d* col_vector)
{
	float x = col_vector->x, y = col_vector->y, z = col_vector->z, w = col_vector->w;
	result->x = m->m[0][0] * x + m->m[0][1] * y + m->m[0][2] * z + m->m[0][3] * w;
	result->y = m->m[1][0] * x + m->m[1][1] * y + m->m[1][2] * z + m->m[1][3] * w;
	result->z = m->m[2][0] * x + m->m[2][1] * y + m->m[2][2] * z + m->m[2][3] * w;
	result->w = m->m[3][0] * x + m->m[3][1] * y + m->m[3][2] * z + m->m[3][3] * w;
}

// result = row_vector * m
void matrix_apply_row(vector_2d* result, const vector_2d* row_vector, const matrix_3d* m)
{
	float x = row_vector->x, y = row_vector->y, z = 1.0f;
	result->x = x * m->m[0][0] + y * m->m[1][0] + m->m[2][0] * z;
	result->y = x * m->m[0][1] + y * m->m[1][1] + m->m[2][1] * z;
	//result->z = x * m->m[0][2] + y * m->m[1][2] + m->m[2][2] * z;
}

void matrix_apply_row(vector_3d * result, const vector_3d * row_vector, const matrix_3d * m)
{
	float x = row_vector->x, y = row_vector->y, z = row_vector->z;
	result->x = x * m->m[0][0] + y * m->m[1][0] + m->m[2][0] * z;
	result->y = x * m->m[0][1] + y * m->m[1][1] + m->m[2][1] * z;
	result->z = x * m->m[0][2] + y * m->m[1][2] + m->m[2][2] * z;
}

void matrix_apply_row(vector_3d* result, const vector_3d* row_vector, const matrix_4d* m)
{
	float x = row_vector->x, y = row_vector->y, z = row_vector->z, w = 1.0f;
	result->x = x * m->m[0][0] + y * m->m[1][0] + m->m[2][0] * z + w * m->m[3][0];
	result->y = x * m->m[0][1] + y * m->m[1][1] + m->m[2][1] * z + w * m->m[3][1];
	result->z = x * m->m[0][2] + y * m->m[1][2] + m->m[2][2] * z + w * m->m[3][2];
	//result->w = x * m->m[0][3] + y * m->m[1][3] + m->m[2][3] * z + w * m->m[3][3];
}

void matrix_apply_row(vector_4d* result, const vector_4d* row_vector, const matrix_4d* m)
{
	float x = row_vector->x, y = row_vector->y, z = row_vector->z, w = row_vector->w;
	result->x = x * m->m[0][0] + y * m->m[1][0] + m->m[2][0] * z + w * m->m[3][0];
	result->y = x * m->m[0][1] + y * m->m[1][1] + m->m[2][1] * z + w * m->m[3][1];
	result->z = x * m->m[0][2] + y * m->m[1][2] + m->m[2][2] * z + w * m->m[3][2];
	result->w = x * m->m[0][3] + y * m->m[1][3] + m->m[2][3] * z + w * m->m[3][3];
}

void matrix_identity(matrix_3d* m1)
{
	memset(m1->m, 0, sizeof(m1->m));
	m1->m[0][0] = m1->m[1][1] = m1->m[2][2] = 1.0f;
}

void matrix_identity(matrix_4d* m1)
{
	memset(m1->m, 0, sizeof(m1->m));
	m1->m[0][0] = m1->m[1][1] = m1->m[2][2] = m1->m[3][3] = 1.0f;
}

void matrix_zero(matrix_3d* m1)
{
	memset(m1->m, 0, sizeof(m1->m));
}

void matrix_zero(matrix_4d* m1)
{
	memset(m1->m, 0, sizeof(m1->m));
}


unsigned int color_to_argb(color_4f* color)
{
	int r = cmid(color->r * 255.f, 0, 255);
	int g = cmid(color->g * 255.f, 0, 255);
	int b = cmid(color->b * 255.f, 0, 255);
	int a = cmid(color->a * 255.f, 0, 255);

	unsigned int argb = (a << 24) | (r << 16) | (g << 8) | (b);
	return argb;
}

unsigned int color_to_argb(color_3f* color)
{
	int r = cmid(color->r * 255.f, 0, 255);
	int g = cmid(color->g * 255.f, 0, 255);
	int b = cmid(color->b * 255.f, 0, 255);
	int a = 255;

	unsigned int argb = (a << 24) | (r << 16) | (g << 8) | (b);
	return argb;
}

unsigned int color_to_rgba(color_4f* color)
{
	int r = cmid(color->r * 255.f, 0, 255);
	int g = cmid(color->g * 255.f, 0, 255);
	int b = cmid(color->b * 255.f, 0, 255);
	int a = cmid(color->a * 255.f, 0, 255);

	unsigned int rgba = (a << 24) | (b << 16) | (g << 8) | (r);
	return rgba;
}

unsigned int color_to_rgba(color_3f* color)
{
	int r = cmid(color->r * 255.f, 0, 255);
	int g = cmid(color->g * 255.f, 0, 255);
	int b = cmid(color->b * 255.f, 0, 255);
	int a = 255;

	unsigned int rgba = (a << 24) | (b << 16) | (g << 8) | (r);
	return rgba;
}

void argb_to_color(color_4f* color, unsigned int argb)
{
	unsigned char* p_color = (unsigned char*)&argb;
	float div = 0.0039215686f; // 1 / 255;

	color->b = p_color[0] * div;
	color->g = p_color[1] * div;
	color->r = p_color[2] * div;
	color->a = p_color[3] * div;
}

void argb_to_color(color_3f* color, unsigned int argb)
{
	unsigned char* p_color = (unsigned char*)&argb;
	float div = 0.0039215686f; // 1 / 255;

	color->b = p_color[0] * div;
	color->g = p_color[1] * div;
	color->r = p_color[2] * div;
}

void rgba_to_color(color_4f* color, unsigned int rgba)
{
	unsigned char* p_color = (unsigned char*)&rgba;
	float div = 0.0039215686f; // 1 / 255;

	color->r = p_color[0] * div;
	color->g = p_color[1] * div;
	color->b = p_color[2] * div;
	color->a = p_color[3] * div;
}

void rgba_to_color(color_3f* color, unsigned int rgba)
{
	unsigned char* p_color = (unsigned char*)&rgba;
	float div = 0.0039215686f; // 1 / 255;

	color->r = p_color[0] * div;
	color->g = p_color[1] * div;
	color->b = p_color[2] * div;
}

void create_perspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane, matrix_4d* dst)
{
	float f_n = 1.0f / (zFarPlane - zNearPlane);
	float theta = (fieldOfView) * 0.5f;
	if (fabs(fmodf(theta, MATH_PIOVER2)) < MATH_EPSILON)
	{
		//CCLOGERROR("Invalid field of view value (%f) causes attempted calculation tan(%f), which is undefined.", fieldOfView, theta);
		return;
	}

	float divisor = tanf(theta);
	float factor = 1.0f / divisor;

	memset(dst, 0, sizeof(matrix_4d));

	dst->m[0][0] = (1.0f / aspectRatio) * factor;
	dst->m[1][1] = factor;
	dst->m[2][2] = (-(zFarPlane + zNearPlane)) * f_n;
	dst->m[3][2] = -1.0f;
	dst->m[2][3] = -2.0f * zFarPlane * zNearPlane * f_n;
}

void create_orthographic(float width, float height, float zNearPlane, float zFarPlane, matrix_4d* dst)
{
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	create_orthographic_offcenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, dst);
}

void create_orthographic_offcenter(float left, float right, float bottom, float top,
	float zNearPlane, float zFarPlane, matrix_4d* dst)
{
// 	GP_ASSERT(dst);
// 	GP_ASSERT(right != left);
// 	GP_ASSERT(top != bottom);
// 	GP_ASSERT(zFarPlane != zNearPlane);

	memset(dst, 0, sizeof(matrix_4d));

	dst->m[0][0] = 2 / (right - left);
	dst->m[1][1] = 2 / (top - bottom);
	dst->m[2][2] = 2 / (zNearPlane - zFarPlane);

	dst->m[0][3] = (left + right) / (left - right);
	dst->m[1][3] = (top + bottom) / (bottom - top);
	dst->m[2][3] = (zNearPlane + zFarPlane) / (zNearPlane - zFarPlane);
	dst->m[3][3] = 1;
}

void create_lookat_rh(float eyePositionX, float eyePositionY, float eyePositionZ,
	float targetPositionX, float targetPositionY, float targetPositionZ,
	float upX, float upY, float upZ, matrix_4d* dst)
{
	vector_3d eye = { eyePositionX, eyePositionY, eyePositionZ };
	vector_3d target = { targetPositionX, targetPositionY, targetPositionZ };
	vector_3d up = { upX, upY, upZ };

	vector_normalize(&up);

	vector_3d zaxis;
	vector_sub(&zaxis, &eye, &target);
	vector_normalize(&zaxis);

	vector_3d xaxis;
	vector_cross(&xaxis, &up, &zaxis);
	vector_normalize(&xaxis);


	vector_3d yaxis;
	vector_cross(&yaxis, &zaxis, &xaxis);
	vector_normalize(&yaxis);


	dst->m[0][0] = xaxis.x;
	dst->m[1][0] = yaxis.x;
	dst->m[2][0] = zaxis.x;
	dst->m[3][0] = 0.0f;

	dst->m[0][1] = xaxis.y;
	dst->m[1][1] = yaxis.y;
	dst->m[2][1] = zaxis.y;
	dst->m[3][1] = 0.0f;

	dst->m[0][2] = xaxis.z;
	dst->m[1][2] = yaxis.z;
	dst->m[2][2] = zaxis.z;
	dst->m[3][2] = 0.0f;

	dst->m[0][3] = -vector_dot(&xaxis, &eye);
	dst->m[1][3] = -vector_dot(&yaxis, &eye);
	dst->m[2][3] = -vector_dot(&zaxis, &eye);
	dst->m[3][3] = 1.0f;
}

void create_scale(const vector_3d& scale, matrix_4d* dst)
{
	matrix_identity(dst);

	dst->m[0][0] = scale.x;
	dst->m[1][1] = scale.y;
	dst->m[2][2] = scale.z;
}

void create_scale(float xScale, float yScale, float zScale, matrix_4d* dst)
{
	matrix_identity(dst);

	dst->m[0][0] = xScale;
	dst->m[1][1] = yScale;
	dst->m[2][2] = zScale;
}

void create_rotation(const vector_3d& axis, float angle, matrix_4d* dst)
{
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	// Make sure the input axis is normalized.
	float n = x*x + y*y + z*z;
	if (n != 1.0f)
	{
		// Not normalized.
		n = sqrt(n);
		// Prevent divide too close to zero.
		if (n > 0.000001f)
		{
			n = 1.0f / n;
			x *= n;
			y *= n;
			z *= n;
		}
	}

	float c = cos(angle);
	float s = sin(angle);

	float t = 1.0f - c;
	float tx = t * x;
	float ty = t * y;
	float tz = t * z;
	float txy = tx * y;
	float txz = tx * z;
	float tyz = ty * z;
	float sx = s * x;
	float sy = s * y;
	float sz = s * z;

	dst->m[0][0] = c + tx*x;
	dst->m[1][0] = txy + sz;
	dst->m[2][0] = txz - sy;
	dst->m[3][0] = 0.0f;

	dst->m[0][1] = txy - sz;
	dst->m[1][1] = c + ty*y;
	dst->m[2][1] = tyz + sx;
	dst->m[3][1] = 0.0f;

	dst->m[0][2] = txz + sy;
	dst->m[1][2] = tyz - sx;
	dst->m[2][2] = c + tz*z;
	dst->m[3][2] = 0.0f;

	dst->m[0][3] = 0.0f;
	dst->m[1][3] = 0.0f;
	dst->m[2][3] = 0.0f;
	dst->m[3][3] = 1.0f;
}

void create_rotation_x(float angle, matrix_4d* dst)
{
	matrix_identity(dst);

	float c = cos(angle);
	float s = sin(angle);

	dst->m[1][1] = c;
	dst->m[2][1] = s;
	dst->m[1][2] = -s;
	dst->m[2][2] = c;
}

void create_rotation_y(float angle, matrix_4d* dst)
{
	matrix_identity(dst);

	float c = cos(angle);
	float s = sin(angle);

	dst->m[0][0] = c;
	dst->m[2][0] = -s;
	dst->m[0][2] = s;
	dst->m[2][2] = c;
}

void create_rotation_z(float angle, matrix_4d* dst)
{
	matrix_identity(dst);

	float c = cos(angle);
	float s = sin(angle);

	dst->m[0][0] = c;
	dst->m[1][0] = s;
	dst->m[0][1] = -s;
	dst->m[1][1] = c;
}

void create_translation(const vector_3d& translation, matrix_4d* dst)
{
	matrix_identity(dst);

	dst->m[0][3] = translation.x;
	dst->m[1][3] = translation.y;
	dst->m[2][3] = translation.z;
}

void create_translation(float xTranslation, float yTranslation, float zTranslation, matrix_4d* dst)
{
	matrix_identity(dst);

	dst->m[0][3] = xTranslation;
	dst->m[1][3] = yTranslation;
	dst->m[2][3] = zTranslation;
}