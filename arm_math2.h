#ifndef ARM_MATH_H
#define ARM_MATH_H

#include <math.h>

#define MAX_VECTOR 4
#define FRACTION 256

#define ABS(x) ((x) > 0 ? (x) : (-(x)))
#define TO_RAD(x) (((float)(x)) * 2 * M_PI / 360)
#define TO_DEG(x) ((x) * 360 / 2 / M_PI)


typedef struct
{
	int size;
	union
	{
		struct
		{
			float values[MAX_VECTOR];
		};
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float p, q, r;
		};
		struct
		{
			float a, b, c, d;
		};
	};
} vector_t;


typedef struct
{
	int rows;
	int columns;
// each row is a vector
	union
	{
		struct
		{
			vector_t values[MAX_VECTOR];
		};
		struct
		{
			vector_t x;
			vector_t y;
			vector_t z;
			vector_t w;
		};
		struct
		{
			vector_t a;
			vector_t b;
			vector_t c;
			vector_t d;
		};
	};
} matrix_t;


// This structure automatically calculates derivatives for us
typedef struct
{
	float *prev;
	int index;
	int total;
	int filled;
} derivative_t;

// Zero derivative structure
void init_derivative(derivative_t *ptr, int buffer_size);
// change size of derivative
void resize_derivative(derivative_t *ptr, int buffer_size);
// Push new value on derivative buffer
void update_derivative(derivative_t *ptr, float value);
// Get derivative
float get_derivative(derivative_t *ptr);
// get sum of all values
float get_sum(derivative_t *ptr);
// get average of all values
float get_avg(derivative_t *ptr);
void reset_derivative(derivative_t *ptr);
#define derivative_size(x) ((x)->total)





// For creating a new vector
#define NEW_VECTOR(name, size_) \
vector_t (name); \
(name).size = (size_);


// For creating a new matrix
#define NEW_MATRIX(name, rows_, columns_) \
matrix_t (name); \
init_matrix(&name, rows_, columns_);

#define INIT_MATRIX(name, rows_, columns_) \
init_matrix(&name, rows_, columns_);

#define VECTOR_ENTRY(vector, entry) ((vector).values[(entry)])

#define MATRIX_ROW(matrix, row) (&(matrix).values[(row)])
#define MATRIX_ENTRY(matrix, row, column) ((matrix).values[(row)].values[(column)])


void init_matrix(matrix_t *matrix, int rows, int columns);
void print_matrix(matrix_t *mat);
void print_vector(vector_t *vec);
// angles are radians
float get_angle_change(float old_angle, float new_angle);
// angles are degrees
float get_angle_change_deg(float old_angle, float new_angle);
float compass_heading(int mag_x, 
	int mag_y, 
	int mag_z,
	int mag_x_min,
	int mag_y_min,
	int mag_z_min,
	int mag_x_max,
	int mag_y_max,
	int mag_z_max,
	float roll,
	float pitch,
	int compass_sign,
	float compass_offset);
float fix_angle(float angle);
void distance_angle(float *distance, 
	float *angle, 
	float x1,
	float y1,
	float x2, 
	float y2);

// arducopter
#ifdef USE_DCM
// create a rotation matrix from Euler angles
void euler_to_dcm(matrix_t *matrix, float roll, float pitch, float yaw);

// create eulers from a rotation matrix
void dcm_to_euler(matrix_t *matrix, float *roll, float *pitch, float *yaw);

// apply an additional rotation from a body frame gyro vector
// to a rotation matrix.
void rotate_dcm(matrix_t *matrix, vector_t *rotation);
void normalize_dcm(matrix_t *matrix);
void cross_product3(vector_t *result, vector_t *a, vector_t *b);
float vector_length(vector_t *vector);
float array_length(float *values);
#endif // USE_DCM

void multiply_vector(vector_t *vector, float value);
//float fabs(float x);






#endif

