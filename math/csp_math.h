#ifndef _MATHXD_H_
#define _MATHXD_H_

#define	MATH_EPSILON	1e-12f
#define	MATH_PI			3.1415926535898f
#define	MATH_PIOVER2	1.5707963267949f

#define MATH_DEG_TO_RAD(x)	((x) * 0.0174532925f)
#define MATH_RAD_TO_DEG(x)	((x) * 57.29577951f)

// 2d float
struct _float_2d
{
public:
	union {
		float m[2];
		struct { float x, y; };
		struct { float u, v; };
	};
} ;

// 3d float
struct _float_3d {
	union {
		float m[3];
		struct { float x, y, z; };
		struct { float r, g, b; };
	};
} ;

// 4d float
struct _float_4d {
	union {
		float m[4];
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
} ;

typedef _float_2d  point_2d, vector_2d;
typedef _float_3d  point_3d, vector_3d, color_3f;
typedef _float_4d  point_4d, vector_4d, color_4f;

// matrix
typedef struct { float m[3][3]; } matrix_3d;
typedef struct { float m[4][4]; } matrix_4d;

float interp(float x1, float x2, float t);
void tri_interp(vector_2d * _a, vector_2d * _b, vector_2d * _c, vector_2d * _p, vector_3d * dt);
void tri_interp(vector_2d * v1, vector_2d * v2, vector_2d * v3, vector_2d * _p, vector_3d * k, vector_3d * dt);

bool float_equal(float x1, float x2);

inline int cmid(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }
template <class T>inline T min(T x1, T x2, T x3) { return (x1 < x2) ? (x1 < x3 ? x1 : x3) : (x2 < x3 ? x2 : x3); }
template <class T>inline T max(T x1, T x2, T x3) { return (x1 > x2) ? (x1 > x3 ? x1 : x3) : (x2 > x3 ? x2 : x3); }
template <class T>inline void exchange(T& left, T& right) { T tmp = left; left = right; right = tmp; }

// vector's length
float vector_length(vector_2d* v2d);
float vector_length(vector_3d* v3d);
float vector_length(vector_4d* v4d);

// vector's length square
float vector_length2(vector_2d* v2d);
float vector_length2(vector_3d* v3d);
float vector_length2(vector_4d* v4d);

// vector - vector
void vector_add(vector_2d* result, const vector_2d* v1, const vector_2d* v2);
void vector_add(vector_3d* result, const vector_3d* v1, const vector_3d* v2);
void vector_add(vector_4d* result, const vector_4d* v1, const vector_4d* v2);
void vector_sub(vector_2d* result, const vector_2d* v1, const vector_2d* v2);
void vector_sub(vector_3d* result, const vector_3d* v1, const vector_3d* v2);
void vector_sub(vector_4d* result, const vector_4d* v1, const vector_4d* v2);
void vector_cross(vector_3d* result, const vector_3d* v1, const vector_3d* v2);
void vector_cross(vector_4d* result, const vector_4d* v1, const vector_4d* v2);
float vector_dot(const vector_2d* v1, const vector_2d* v2d2);
float vector_dot(const vector_3d* v1, const vector_3d* v2d2);
float vector_dot(const vector_4d* v1, const vector_4d* v2d2);

// vecotr interpolation
void vector_interp(vector_2d* result, const vector_2d* v1, const vector_2d* v2, float t);
void vector_interp(vector_3d* result, const vector_3d* v1, const vector_3d* v2, float t);
void vector_interp(vector_4d* result, const vector_4d* v1, const vector_4d* v2, float t);

// vector normalize
void vector_normalize(vector_2d* v);
void vector_normalize(vector_3d* v);
void vector_normalize(vector_4d* v);

// vector negate
void vector_negate(vector_2d* v);
void vector_negate(vector_3d* v);
void vector_negate(vector_4d* v);

// vector - scalar
void vector_add(vector_2d* v, float value);
void vector_add(vector_3d* v, float value);
void vector_add(vector_4d* v, float value);
void vector_sub(vector_2d* v, float value);
void vector_sub(vector_3d* v, float value);
void vector_sub(vector_4d* v, float value);
void vector_scale(vector_2d* v, float value);
void vector_scale(vector_3d* v, float value);
void vector_scale(vector_4d* v, float value);
void vector_div(vector_2d* v, float value);
void vector_div(vector_3d* v, float value);
void vector_div(vector_4d* v, float value);
void vector_scale(vector_2d* result, const vector_2d* v, float value);
void vector_scale(vector_3d* result, const vector_3d* v, float value);
void vector_scale(vector_4d* result, const vector_4d* v, float value);

// matrix - matrix
void matrix_add(matrix_3d* result, const matrix_3d* m1, const matrix_3d* m2);
void matrix_add(matrix_4d* result, const matrix_4d* m1, const matrix_4d* m2);
void matrix_sub(matrix_3d* result, const matrix_3d* m1, const matrix_3d* m2);
void matrix_sub(matrix_4d* result, const matrix_4d* m1, const matrix_4d* m2);
void matrix_mul(matrix_3d* result, const matrix_3d* m1, const matrix_3d* m2);
void matrix_mul(matrix_4d* result, const matrix_4d* m1, const matrix_4d* m2);

// matrix - scalar
void matrix_add(matrix_3d* result, const matrix_3d* m1, float value);
void matrix_add(matrix_4d* result, const matrix_4d* m1, float value);
void matrix_sub(matrix_3d* result, const matrix_3d* m1, float value);
void matrix_sub(matrix_4d* result, const matrix_4d* m1, float value);
void matrix_scale(matrix_3d* result, const matrix_3d* m1, float value);
void matrix_scale(matrix_4d* result, const matrix_4d* m1, float value);

// matrix - vector
// result = m * col_vector
void matrix_apply_col(vector_2d* result, const matrix_3d* m, const vector_2d* col_vector);
void matrix_apply_col(vector_3d* result, const matrix_3d* m, const vector_3d* col_vector);
void matrix_apply_col(vector_3d* result, const matrix_4d* m, const vector_3d* col_vector);
void matrix_apply_col(vector_4d* result, const matrix_4d* m, const vector_4d* col_vector);

// result = row_vector * m
void matrix_apply_row(vector_2d* result, const vector_2d* row_vector, const matrix_3d* m);
void matrix_apply_row(vector_3d* result, const vector_3d* row_vector, const matrix_3d* m);
void matrix_apply_row(vector_3d* result, const vector_3d* row_vector, const matrix_4d* m);
void matrix_apply_row(vector_4d* result, const vector_4d* row_vector, const matrix_4d* m);

void matrix_identity(matrix_3d* m1);
void matrix_identity(matrix_4d* m1);
void matrix_zero(matrix_3d* m1);
void matrix_zero(matrix_4d* m1);

unsigned int color_to_argb(color_4f* color);
unsigned int color_to_argb(color_3f* color);
unsigned int color_to_rgba(color_4f* color);
unsigned int color_to_rgba(color_3f* color);

void argb_to_color(color_4f* color, unsigned int argb);
void argb_to_color(color_3f* color, unsigned int argb);
void rgba_to_color(color_4f* color, unsigned int rgba);
void rgba_to_color(color_3f* color, unsigned int rgba);

// opengl's type
void create_perspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane, matrix_4d* dst);
void create_orthographic(float width, float height, float zNearPlane, float zFarPlane, matrix_4d* dst);
void create_orthographic_offcenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane, matrix_4d* dst);
void create_lookat_rh(float eyePositionX, float eyePositionY, float eyePositionZ,
	float targetPositionX, float targetPositionY, float targetPositionZ,
	float upX, float upY, float upZ, matrix_4d* dst);

// transform helper
void create_scale(const vector_3d& scale, matrix_4d* dst);
void create_scale(float xScale, float yScale, float zScale, matrix_4d* dst);
void create_rotation(const vector_3d& axis, float angle, matrix_4d* dst);
void create_rotation_x(float angle, matrix_4d* dst);
void create_rotation_y(float angle, matrix_4d* dst);
void create_rotation_z(float angle, matrix_4d* dst);
void create_translation(const vector_3d& translation, matrix_4d* dst);
void create_translation(float xTranslation, float yTranslation, float zTranslation, matrix_4d* dst);

#endif