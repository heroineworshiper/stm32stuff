/*
 * STM32F4 Wifi flight controller
 * Copyright (C) 2012-2014 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
 
 
#include "settings.h"
#include "uart.h"
#include "linux.h"
#include "arm_math2.h"
//#include "nav.h"
//#include "mpu9150.h"

#include <math.h>


void euler_dc(matrix_t *dst, vector_t *euler)
{
	float	cpsi;
	float	cphi;
	float	ctheta;

	float	spsi;
	float	sphi;
	float	stheta;

	if(euler->x == 0 && euler->y == 0)
	{
		cpsi	= 1;
		cphi	= 1;
		ctheta	= 1;

		spsi	= 0;
		sphi	= 0;
		stheta	= 0;
	}
	else
	{
		float phi = -euler->y;
		float theta	= euler->x;
		float psi = euler->z;

		cpsi	= cosf(psi);
		cphi	= cosf(phi);
		ctheta	= cosf(theta);

		spsi	= sinf(psi);
		sphi	= sinf(phi);
		stheta	= sinf(theta);
	}


	MATRIX_ENTRY(*dst, 0, 0) = cpsi * ctheta;
	MATRIX_ENTRY(*dst, 0, 1) = spsi * ctheta;
	MATRIX_ENTRY(*dst, 0, 2) = -stheta;

	MATRIX_ENTRY(*dst, 1, 0) = -spsi * cphi + (cpsi * stheta) * sphi;
	MATRIX_ENTRY(*dst, 1, 1) = cpsi * cphi + (spsi * stheta) * sphi;
	MATRIX_ENTRY(*dst, 1, 2) = ctheta * sphi;

	MATRIX_ENTRY(*dst, 2, 0) = spsi * sphi + (cpsi * stheta) * cphi;
	MATRIX_ENTRY(*dst, 2, 1) = -cpsi * sphi + (spsi * stheta) * cphi;
	MATRIX_ENTRY(*dst, 2, 2) = ctheta * cphi;
}

void init_matrix(matrix_t *matrix, int rows, int columns)
{
	matrix->rows = rows;
	matrix->columns = columns;
	int i;
	for(i = 0; i < rows; i++)
	{
		matrix->values[i].size = columns;
	}
}

void transpose_matrix(matrix_t *dst, 
	matrix_t *src)
{
	int i, j;

	if(dst->rows != src->columns || dst->columns != src->rows)
	{
		TRACE
		print_text("transpose_matrix: size mismatch");
		return;
	}

	for(i = 0; i < src->rows; i++)
	{
		float *src_row = MATRIX_ROW(*src, i)->values;
		for(j = 0; j < src->columns; j++)
			MATRIX_ENTRY(*dst, j, i) = src_row[j];
	}
}

void multiply_matrix_vector(vector_t *dst, matrix_t *mat, vector_t *vec)
{
	int i, j;
	int n = mat->rows;
	int m = mat->columns;

	if(m != vec->size || dst->size != n)
	{
		TRACE
		print_text("multiply_matrix_vector: size mismatch");
		return;
	}

	for(i = 0; i < n; i++)
	{
		float s = 0;
		for(j = 0; j < m; j++)
			s += MATRIX_ENTRY(*mat, i, j) * VECTOR_ENTRY(*vec, j);
		VECTOR_ENTRY(*dst, i) = s;
	}
}

void print_matrix(matrix_t *mat)
{
	int i, j;
	print_lf();
	for(i = 0; i < mat->rows; i++)
	{
		for(j = 0; j < mat->rows; j++)
		{
			print_float(MATRIX_ENTRY(*mat, i, j));
		}
		print_lf();
	}
}

void print_vector(vector_t *vec)
{
	int i, j;
	for(i = 0; i < vec->size; i++)
	{
		print_float(VECTOR_ENTRY(*vec, i));
	}
	print_lf();
}



#if defined(USE_NAV) && defined(COPTER_MODE) || defined(STANDALONE_MODE)

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
	float compass_offset)
{
	float mag_x_center = (float)(mag_x_max + mag_x_min) / 2;
	float mag_y_center = (float)(mag_y_max + mag_y_min) / 2;
	float mag_z_center = (float)(mag_z_max + mag_z_min) / 2;
	float mag_x_scale = (float)(mag_x_max - mag_x_min) / 2;
	float mag_y_scale = (float)(mag_y_max - mag_y_min) / 2;
	float mag_z_scale = (float)(mag_z_max - mag_z_min) / 2;


	NEW_VECTOR(mag_vector, 3);
	VECTOR_ENTRY(mag_vector, 0) = 
		(mag_x - mag_x_center) / mag_x_scale;
	VECTOR_ENTRY(mag_vector, 1) = 
		(mag_y - mag_y_center) / mag_y_scale;
	VECTOR_ENTRY(mag_vector, 2) = 
		(mag_z - mag_z_center) / mag_z_scale;
// fudge factors
	VECTOR_ENTRY(mag_vector, 0) *= -1;
	VECTOR_ENTRY(mag_vector, 2) *= -1;



/*
 * TRACE2
 * print_number(mag_y_center);
 * print_number(mag_y_scale);
 * print_fixed(VECTOR_ENTRY(mag_vector, 1));
 */


	
	NEW_MATRIX(dcm, 3, 3);
	NEW_VECTOR(euler, 3);
	VECTOR_ENTRY(euler, 0) = roll;
	VECTOR_ENTRY(euler, 1) = pitch;
	VECTOR_ENTRY(euler, 2) = 0;
	NEW_MATRIX(dcm_transpose, 3, 3);
	NEW_VECTOR(mag_ned, 3);
	euler_dc(&dcm, &euler);

	transpose_matrix(&dcm_transpose, &dcm);
//TRACE
//print_matrix(&dcm_transpose);


//TRACE2
//print_vector(&mag_vector);

	multiply_matrix_vector(&mag_ned, &dcm_transpose, &mag_vector);
//TRACE2
//print_fixed(VECTOR_ENTRY(mag_ned, 1));
//print_fixed(VECTOR_ENTRY(mag_ned, 0));

	float result = -atan2f(VECTOR_ENTRY(mag_ned, 1), VECTOR_ENTRY(mag_ned, 0));
  	result *= compass_sign;
  	result += compass_offset;
//TRACE2
//print_fixed(result);
	return result;
}

#endif // defined(USE_NAV) && defined(COPTER_MODE) || defined(STANDALONE_MODE)


float get_angle_change(float old_angle, float new_angle)
{
	float result = new_angle - old_angle;
	if(result > M_PI) result -= M_PI * 2;
	else
	if(result < -M_PI) result += M_PI * 2;
	return result;
}

float get_angle_change_deg(float old_angle, float new_angle)
{
	float result = new_angle - old_angle;
    if(result > 180) result -= 360;
    else
    if(result < -180) result += 360;
	return result;
}


float fix_angle(float angle)
{
	while(angle > M_PI) angle -= M_PI * 2;
	while(angle < -M_PI) angle += M_PI * 2;
	return angle;
}

void distance_angle(float *distance, 
	float *angle, 
	float x1,
	float y1,
	float x2, 
	float y2)
{
	*distance = 0;
	*angle = 0;

	if(x1 == x2 && y1 == y2)
	{
		return;
	}

	*angle = atan2f(y2 - y1, x2 - x1);
	if(ABS(x2 - x1) > ABS(y2 - y1))
	{
		*distance = (x2 - x1) / cosf(*angle);
	}
	else
	{
		*distance = (y2 - y1) / sinf(*angle);
	}
	
}


// Zero derivative structure
void init_derivative(derivative_t *ptr, int buffer_size)
{
	ptr->prev = kmalloc(buffer_size * sizeof(float), 1);
	bzero(ptr->prev, buffer_size * sizeof(float));
	ptr->total = buffer_size;
	ptr->index = 0;
}

void resize_derivative(derivative_t *ptr, int buffer_size)
{
	kfree(ptr->prev);
	init_derivative(ptr, buffer_size);
}

void reset_derivative(derivative_t *ptr)
{
//printf("reset_derivative %d: allocated=%d\n", __LINE__, ptr->allocated);
	bzero(ptr->prev, ptr->total * sizeof(int));
	ptr->index = 0;
}


// Push new value on derivative buffer
void update_derivative(derivative_t *ptr, float value)
{
	ptr->prev[ptr->index] = value;
	ptr->index++;
	if(ptr->index >= ptr->total) ptr->index = 0;
	ptr->filled++;
	if(ptr->filled > ptr->total) ptr->filled = ptr->total;
}


// Get derivative
float get_derivative(derivative_t *ptr)
{
	int current_index = ptr->index - 1;
	if(current_index < 0) current_index = ptr->total - 1;
	return ptr->prev[current_index] - ptr->prev[ptr->index];
}

// get sum of all values
float get_sum(derivative_t *ptr)
{
	int i;
	float sum = 0;

	for(i = 0; i < ptr->total; i++)
	{
		sum += ptr->prev[i];
	}
	
	return sum;
}

// get average of all values
float get_avg(derivative_t *ptr)
{
	int i;
	float sum = 0;

	for(i = 0; i < ptr->total; i++)
	{
		sum += ptr->prev[i];
	}
	
	return sum / ptr->total;
}

/*
 * float fabs(float x)
 * {
 * 	if(x < 0) return -x;
 * 	return x;
 * }
 */


void multiply_vector(vector_t *vector, float value)
{
	int i;
	for(i = 0; i < vector->size; i++)
	{
		VECTOR_ENTRY(*vector, i) *= value;
	}
}

#ifdef USE_DCM

// a varient of asin() that checks the input ranges and ensures a
// valid angle as output. If nan is given as input then zero is
// returned.
float safe_asin(float v)
{
	if (isnan(v)) {
		return 0.0;
	}
	if (v >= 1.0) {
		return M_PI/2;
	}
	if (v <= -1.0) {
		return -M_PI/2;
	}
	return asinf(v);
}

// create a rotation matrix from Euler angles
void euler_to_dcm(matrix_t *matrix, float roll, float pitch, float yaw)
{
	float cp = cosf(pitch);
	float sp = sinf(pitch);
	float sr = sinf(roll);
	float cr = cosf(roll);
	float sy = sinf(yaw);
	float cy = cosf(yaw);

	MATRIX_ENTRY(*matrix, 0, 0) = cp * cy;
	MATRIX_ENTRY(*matrix, 0, 1) = (sr * sp * cy) - (cr * sy);
	MATRIX_ENTRY(*matrix, 0, 2) = (cr * sp * cy) + (sr * sy);
	MATRIX_ENTRY(*matrix, 1, 0) = cp * sy;
	MATRIX_ENTRY(*matrix, 1, 1) = (sr * sp * sy) + (cr * cy);
	MATRIX_ENTRY(*matrix, 1, 2) = (cr * sp * sy) - (sr * cy);
	MATRIX_ENTRY(*matrix, 2, 0) = -sp;
	MATRIX_ENTRY(*matrix, 2, 1) = sr * cp;
	MATRIX_ENTRY(*matrix, 2, 2) = cr * cp;

}

// create eulers from a rotation matrix
void dcm_to_euler(matrix_t *matrix, float *roll, float *pitch, float *yaw)
{
	if (pitch != NULL) {
		*pitch = -safe_asin(MATRIX_ENTRY(*matrix, 2, 0));
	}
	if (roll != NULL) {
		*roll = atan2f(MATRIX_ENTRY(*matrix, 2, 1), MATRIX_ENTRY(*matrix, 2, 2));
	}
	if (yaw != NULL) {
		*yaw = atan2f(MATRIX_ENTRY(*matrix, 1, 0), MATRIX_ENTRY(*matrix, 0, 0));
	}
}


void add_matrix(matrix_t *dst, matrix_t *arg1, matrix_t *arg2)
{
	int i, j;
	if(dst->rows != arg1->rows ||
		dst->columns != arg1->columns ||
		dst->rows != arg2->rows ||
		dst->columns != arg2->columns)
	{
		TRACE
		print_text("add_matrix_const: size mismatch");
		return;
	}

	for(i = 0; i < arg1->rows; i++)
		for(j = 0; j < arg1->columns; j++)
			MATRIX_ENTRY(*dst, i, j) = 
				MATRIX_ENTRY(*arg1, i, j) + 
				MATRIX_ENTRY(*arg2, i, j);
}

// apply an additional rotation from a body frame gyro vector
// to a rotation matrix.
void rotate_dcm(matrix_t *matrix, vector_t *rotation)
{
	NEW_MATRIX(temp_matrix, 3, 3);
	
	MATRIX_ENTRY(temp_matrix, 0, 0) = 
		MATRIX_ENTRY(*matrix, 0, 1) * rotation->z - MATRIX_ENTRY(*matrix, 0, 2) * rotation->y;
	MATRIX_ENTRY(temp_matrix, 0, 1) = 
		MATRIX_ENTRY(*matrix, 0, 2) * rotation->x - MATRIX_ENTRY(*matrix, 0, 0) * rotation->z;
	MATRIX_ENTRY(temp_matrix, 0, 2) = 
		MATRIX_ENTRY(*matrix, 0, 0) * rotation->y - MATRIX_ENTRY(*matrix, 0, 1) * rotation->x;
	MATRIX_ENTRY(temp_matrix, 1, 0) = 
		MATRIX_ENTRY(*matrix, 1, 1) * rotation->z - MATRIX_ENTRY(*matrix, 1, 2) * rotation->y;
	MATRIX_ENTRY(temp_matrix, 1, 1) = 
		MATRIX_ENTRY(*matrix, 1, 2) * rotation->x - MATRIX_ENTRY(*matrix, 1, 0) * rotation->z;
	MATRIX_ENTRY(temp_matrix, 1, 2) = 
		MATRIX_ENTRY(*matrix, 1, 0) * rotation->y - MATRIX_ENTRY(*matrix, 1, 1) * rotation->x;
	MATRIX_ENTRY(temp_matrix, 2, 0) = 
		MATRIX_ENTRY(*matrix, 2, 1) * rotation->z - MATRIX_ENTRY(*matrix, 2, 2) * rotation->y;
	MATRIX_ENTRY(temp_matrix, 2, 1) = 
		MATRIX_ENTRY(*matrix, 2, 2) * rotation->x - MATRIX_ENTRY(*matrix, 2, 0) * rotation->z;
	MATRIX_ENTRY(temp_matrix, 2, 2) = 
		MATRIX_ENTRY(*matrix, 2, 0) * rotation->y - MATRIX_ENTRY(*matrix, 2, 1) * rotation->x;

	add_matrix(matrix, matrix, &temp_matrix);
}


void renorm(vector_t *a, vector_t *row)
{
	float length = vector_length(a);
	
	if(length > 0)
	{
		float renorm_val = 1.0 / length;
		row->x = a->x * renorm_val;
		row->y = a->y * renorm_val;
		row->z = a->z * renorm_val;
	}
}

void cross_product3(vector_t *result, vector_t *a, vector_t *b)
{
	result->x = a->y * b->z - a->z * b->y;
	result->y = a->z * b->x - a->x * b->z;
	result->z = a->x * b->y - a->y * b->x;
}

float vector_length(vector_t *vector)
{
	return sqrtf(SQR(vector->x) + SQR(vector->y) + SQR(vector->z));	
}

void normalize_dcm(matrix_t *matrix)
{
	float error = 0;
	int i, j;
	NEW_VECTOR(t0, 3);
	NEW_VECTOR(t1, 3);
	NEW_VECTOR(t2, 3);

	for(i = 0; i < 3; i++)
	{
		error += MATRIX_ENTRY(*matrix, 0, i) * MATRIX_ENTRY(*matrix, 1, i);
	}

	for(i = 0; i < 3; i++)
	{
		VECTOR_ENTRY(t0, i) = 
			MATRIX_ENTRY(*matrix, 0, i) - 
			MATRIX_ENTRY(*matrix, 1, i) * 0.5f * error;
	}
	
	for(i = 0; i < 3; i++)
	{
		VECTOR_ENTRY(t1, i) = 
			MATRIX_ENTRY(*matrix, 1, i) - 
			MATRIX_ENTRY(*matrix, 0, i) * 0.5f * error;
	}
	

	cross_product3(&t2, &t0, &t1);
	
	renorm(&t0, &matrix->x);
	renorm(&t1, &matrix->y);
	renorm(&t2, &matrix->z);

}

#endif // USE_DCM


