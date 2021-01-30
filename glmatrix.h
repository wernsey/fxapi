/* Single header library vector and matrix library.
 * You need to `#define GL_MATRIX_IMPLEMENTATION` in one of your source files
 * Single header version which can be obtained at <https://github.com/wernsey/gl-matrix.c>
 * Modified from the original C port by Marco Buono <https://github.com/coreh/gl-matrix.c>
 * Which was ported to C from Brandon Jones' original JavaScript version <https://github.com/toji/gl-matrix>

Copyright (c) 2011 Brandon Jones

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
 */
/*
https://github.com/wernsey/gl-matrix.c
This was forked from https://github.com/coreh/gl-matrix.c
with some additions for vec2_t and vec4_t types.

This is a fairly straightforward port of gl-matrix.js (https://github.com/toji/gl-matrix)
from JavaScript to C.
*/
#ifndef GL_MATRIX_H
#define GL_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * gl-matrix.c - High performance matrix and vector operations for OpenGL
 * Version 1.2.3
 */

#define GL_MATRIX_MAJOR_VERSION 1
#define GL_MATRIX_MINOR_VERSION 2
#define GL_MATRIX_MICRO_VERSION 3

#define GL_MATRIX_VERSION  "1.2.3"

/* Hex version number. A value of 0x010203 means version 1.2.3.
  useful for comparisons. e.g. GL_MATRIX_VERSION_HEX >= 0x010203 */
#define GL_MATRIX_VERSION_HEX  ((GL_MATRIX_MAJOR_VERSION << 16) | \
                              (GL_MATRIX_MINOR_VERSION << 8) | \
                              (GL_MATRIX_MICRO_VERSION))

typedef double numeric_t;

typedef numeric_t *vec2_t;
typedef numeric_t *vec3_t;
typedef numeric_t *vec4_t;
typedef numeric_t *mat3_t;
typedef numeric_t *mat4_t;
typedef numeric_t *quat_t;

/*
 * vec2_t - 2 Dimensional Vector
 */

/*
 * vec2_create
 * Creates a new instance of a vec2_t
 *
 * Params:
 * vec - Optional, vec2_t containing values to initialize with. If NULL, the
 * vector will be initialized with zeroes.
 *
 * Returns:
 * New vec2
 */
vec2_t vec2_create(vec2_t vec);

/*
 * vec2_set
 * Copies the values of one vec2_t to another
 *
 * Params:
 * vec - vec2_t containing values to copy
 * dest - vec2_t receiving copied values
 *
 * Returns:
 * dest
 */
vec2_t vec2_set(vec2_t vec, vec2_t dest);

/*
 * vec2_add
 * Performs a vector addition
 *
 * Params:
 * vec - vec2, first operand
 * vec2 - vec2, second operand
 * dest - Optional, vec2_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec2_t vec2_add(vec2_t vec, vec2_t vec2, vec2_t dest);

/*
 * vec2_subtract
 * Performs a vector subtraction
 *
 * Params:
 * vec - vec2, first operand
 * vec2 - vec2, second operand
 * dest - Optional, vec2_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec2_t vec2_subtract(vec2_t vec, vec2_t vec2, vec2_t dest);

/*
 * vec2_negate
 * Negates the components of a vec2
 *
 * Params:
 * vec - vec2_t to negate
 * dest - Optional, vec2_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec2_t vec2_negate(vec2_t vec, vec2_t dest);

/*
 * vec2_scale
 * Multiplies the components of a vec2_t by a scalar value
 *
 * Params:
 * vec - vec2_t to scale
 * val - Numeric value to scale by
 * dest - Optional, vec2_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec2_t vec2_scale(vec2_t vec, numeric_t val, vec2_t dest);

/*
 * vec2_normalize
 * Generates a unit vector of the same direction as the provided vec2
 * If vector length is 0, returns [0, 0]
 *
 * Params:
 * vec - vec2_t to normalize
 * dest - Optional, vec2_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec2_t vec2_normalize(vec2_t vec, vec2_t dest);

/*
 * vec2_length
 * Caclulates the length of a vec2
 *
 * Params:
 * vec - vec2_t to calculate length of
 *
 * Returns:
 * Length of vec
 */
numeric_t vec2_length(vec2_t vec);

/*
 * vec2_dot
 * Caclulates the dot product of two vec2s
 *
 * Params:
 * vec - vec2, first operand
 * vec2 - vec2, second operand
 *
 * Returns:
 * Dot product of vec and vec2
 */
numeric_t vec2_dot(vec2_t vec, vec2_t vec2);

/*
 * vec2_direction
 * Generates a unit vector pointing from one vector to another
 *
 * Params:
 * vec - origin vec2
 * vec2 - vec2_t to point to
 * dest - Optional, vec2_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec2_t vec2_direction (vec2_t vec, vec2_t vec2, vec2_t dest);

/*
 * vec2_lerp
 * Performs a linear interpolation between two vec2
 *
 * Params:
 * vec - vec2, first vector
 * vec2 - vec2, second vector
 * lerp - interpolation amount between the two inputs
 * dest - Optional, vec2_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec2_t vec2_lerp(vec2_t vec, vec2_t vec2, numeric_t lerp, vec2_t dest);

/*
 * vec2_dist
 * Calculates the euclidian distance between two vec2
 *
 * Params:
 * vec - vec2, first vector
 * vec2 - vec2, second vector
 *
 * Returns:
 * distance between vec and vec2
 */
numeric_t vec2_dist(vec2_t vec, vec2_t vec2);

/*
 * vec2_str
 * Writes a string representation of a vector
 *
 * Params:
 * vec - vec2_t to represent as a string
 * buffer - char * to store the results
 */
void vec2_str(vec2_t vec, char *buffer);

/*
 * vec3_t - 3 Dimensional Vector
 */

/*
 * vec3_create
 * Creates a new instance of a vec3_t
 *
 * Params:
 * vec - Optional, vec3_t containing values to initialize with. If NULL, the
 * vector will be initialized with zeroes.
 *
 * Returns:
 * New vec3
 */
vec3_t vec3_create(vec3_t vec);

/*
 * vec3_set
 * Copies the values of one vec3_t to another
 *
 * Params:
 * vec - vec3_t containing values to copy
 * dest - vec3_t receiving copied values
 *
 * Returns:
 * dest
 */
vec3_t vec3_set(vec3_t vec, vec3_t dest);

/*
 * vec3_add
 * Performs a vector addition
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_add(vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_subtract
 * Performs a vector subtraction
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_subtract(vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_multiply
 * Performs a vector multiplication
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_multiply(vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_negate
 * Negates the components of a vec3
 *
 * Params:
 * vec - vec3_t to negate
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_negate(vec3_t vec, vec3_t dest);

/*
 * vec3_scale
 * Multiplies the components of a vec3_t by a scalar value
 *
 * Params:
 * vec - vec3_t to scale
 * val - Numeric value to scale by
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_scale(vec3_t vec, numeric_t val, vec3_t dest);

/*
 * vec3_normalize
 * Generates a unit vector of the same direction as the provided vec3
 * If vector length is 0, returns [0, 0, 0]
 *
 * Params:
 * vec - vec3_t to normalize
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_normalize(vec3_t vec, vec3_t dest);

/*
 * vec3_cross
 * Generates the cross product of two vec3s
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_cross (vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_length
 * Caclulates the length of a vec3
 *
 * Params:
 * vec - vec3_t to calculate length of
 *
 * Returns:
 * Length of vec
 */
numeric_t vec3_length(vec3_t vec);

/*
 * vec3_dot
 * Caclulates the dot product of two vec3s
 *
 * Params:
 * vec - vec3, first operand
 * vec2 - vec3, second operand
 *
 * Returns:
 * Dot product of vec and vec2
 */
numeric_t vec3_dot(vec3_t vec, vec3_t vec2);

/*
 * vec3_direction
 * Generates a unit vector pointing from one vector to another
 *
 * Params:
 * vec - origin vec3
 * vec2 - vec3_t to point to
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_direction (vec3_t vec, vec3_t vec2, vec3_t dest);

/*
 * vec3_lerp
 * Performs a linear interpolation between two vec3
 *
 * Params:
 * vec - vec3, first vector
 * vec2 - vec3, second vector
 * lerp - interpolation amount between the two inputs
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */

vec3_t vec3_lerp(vec3_t vec, vec3_t vec2, numeric_t lerp, vec3_t dest);

/*
 * vec3_dist
 * Calculates the euclidian distance between two vec3
 *
 * Params:
 * vec - vec3, first vector
 * vec2 - vec3, second vector
 *
 * Returns:
 * distance between vec and vec2
 */
numeric_t vec3_dist(vec3_t vec, vec3_t vec2);

/*
 * vec3_unproject
 * Projects the specified vec3_t from screen space into object space
 * Based on Mesa gluUnProject implementation at:
 * http://webcvs.freedesktop.org/mesa/Mesa/src/glu/mesa/project.c?revision=1.4&view=markup
 *
 * Params:
 * vec - vec3, screen-space vector to project
 * view - mat4, View matrix
 * proj - mat4, Projection matrix
 * viewport - vec4, Viewport as given to gl.viewport [x, y, width, height]
 * dest - Optional, vec3_t receiving unprojected result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t vec3_unproject(vec3_t vec, mat4_t view, mat4_t proj, vec4_t viewport, vec3_t dest);

/*
 * vec3_str
 * Writes a string representation of a vector
 *
 * Params:
 * vec - vec3_t to represent as a string
 * buffer - char * to store the results
 */
void vec3_str(vec3_t vec, char *buffer);

/*
 * vec4_t - 4 Dimensional Vector
 */

/*
 * vec4_create
 * Creates a new instance of a vec4_t
 *
 * Params:
 * vec - Optional, vec4_t containing values to initialize with. If NULL, the
 * vector will be initialized with zeroes.
 *
 * Returns:
 * New vec4
 */
vec4_t vec4_create(vec4_t vec);

/*
 * vec4_set
 * Copies the values of one vec4_t to another
 *
 * Params:
 * vec - vec4_t containing values to copy
 * dest - vec4_t receiving copied values
 *
 * Returns:
 * dest
 */
vec4_t vec4_set(vec4_t vec, vec4_t dest);

/*
 * vec4_add
 * Performs a vector addition
 *
 * Params:
 * vec - vec4, first operand
 * vec2 - vec4, second operand
 * dest - Optional, vec4_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t vec4_add(vec4_t vec, vec4_t vec2, vec4_t dest);

/*
 * vec4_subtract
 * Performs a vector subtraction
 *
 * Params:
 * vec - vec4, first operand
 * vec2 - vec4, second operand
 * dest - Optional, vec4_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t vec4_subtract(vec4_t vec, vec4_t vec2, vec4_t dest);

/*
 * vec4_negate
 * Negates the components of a vec4
 *
 * Params:
 * vec - vec4_t to negate
 * dest - Optional, vec4_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t vec4_negate(vec4_t vec, vec4_t dest);

/*
 * vec4_scale
 * Multiplies the components of a vec4_t by a scalar value
 *
 * Params:
 * vec - vec4_t to scale
 * val - Numeric value to scale by
 * dest - Optional, vec4_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t vec4_scale(vec4_t vec, numeric_t val, vec4_t dest);

/*
 * vec4_normalize
 * Generates a unit vector of the same direction as the provided vec4
 * If vector length is 0, returns [0, 0, 0, 0]
 *
 * Params:
 * vec - vec4_t to normalize
 * dest - Optional, vec4_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t vec4_normalize(vec4_t vec, vec4_t dest);

/*
 * vec4_length
 * Caclulates the length of a vec4
 *
 * Params:
 * vec - vec4_t to calculate length of
 *
 * Returns:
 * Length of vec
 */
numeric_t vec4_length(vec4_t vec);

/*
 * vec4_dot
 * Caclulates the dot product of two vec4s
 *
 * Params:
 * vec - vec4, first operand
 * vec2 - vec4, second operand
 *
 * Returns:
 * Dot product of vec and vec2
 */
numeric_t vec4_dot(vec4_t vec, vec4_t vec2);

/*
 * vec4_direction
 * Generates a unit vector pointing from one vector to another
 *
 * Params:
 * vec - origin vec4
 * vec2 - vec4_t to point to
 * dest - Optional, vec4_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t vec4_direction (vec4_t vec, vec4_t vec2, vec4_t dest);

/*
 * vec4_lerp
 * Performs a linear interpolation between two vec4
 *
 * Params:
 * vec - vec4, first vector
 * vec2 - vec4, second vector
 * lerp - interpolation amount between the two inputs
 * dest - Optional, vec4_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t vec4_lerp(vec4_t vec, vec4_t vec2, numeric_t lerp, vec4_t dest);

/*
 * vec4_dist
 * Calculates the euclidian distance between two vec4
 *
 * Params:
 * vec - vec4, first vector
 * vec2 - vec4, second vector
 *
 * Returns:
 * distance between vec and vec2
 */
numeric_t vec4_dist(vec4_t vec, vec4_t vec2);

/*
 * vec4_str
 * Writes a string representation of a vector
 *
 * Params:
 * vec - vec4_t to represent as a string
 * buffer - char * to store the results
 */
void vec4_str(vec4_t vec, char *buffer);

/*
 * mat3_t - 3x3 Matrix
 */

/*
 * mat3_create
 * Creates a new instance of a mat3_t
 *
 * Params:
 * mat - Optional, mat3_t containing values to initialize with. If NULL the result
 * will be initialized with zeroes.
 *
 * Returns:
 * New mat3
 */
mat3_t mat3_create(mat3_t mat);

/*
 * mat3_set
 * Copies the values of one mat3_t to another
 *
 * Params:
 * mat - mat3_t containing values to copy
 * dest - mat3_t receiving copied values
 *
 * Returns:
 * dest
 */
mat3_t mat3_set(mat3_t mat, mat3_t dest);

/*
 * mat3_identity
 * Sets a mat3_t to an identity matrix
 *
 * Params:
 * dest - mat3_t to set
 *
 * Returns:
 * dest
 */
 mat3_t mat3_identity(mat3_t dest);

/*
 * mat4.transpose
 * Transposes a mat3_t (flips the values over the diagonal)
 *
 * Params:
 * mat - mat3_t to transpose
 * dest - Optional, mat3_t receiving transposed values. If NULL, result is written to mat
 *
 * Returns:
 * dest is specified, mat otherwise
 */
mat3_t mat3_transpose(mat3_t mat, mat3_t dest);

/*
 * mat3_toMat4
 * Copies the elements of a mat3_t into the upper 3x3 elements of a mat4
 *
 * Params:
 * mat - mat3_t containing values to copy
 * dest - Optional, mat4_t receiving copied values
 *
 * Returns:
 * dest if not NULL, a new mat4_t otherwise
 */
mat4_t mat3_toMat4(mat3_t mat, mat4_t dest);

/*
 * mat3_str
 * Writes a string representation of a mat3
 *
 * Params:
 * mat - mat3_t to represent as a string
 * buffer - char * to store the results
 */
void mat3_str(mat3_t mat, char *buffer);

/*
 * mat4_t - 4x4 Matrix
 */

/*
 * mat4_create
 * Creates a new instance of a mat4_t
 *
 * Params:
 * mat - Optional, mat4_t containing values to initialize with
 *
 * Returns:
 * New mat4
 */
mat4_t mat4_create(mat4_t mat);

/*
 * mat4_set
 * Copies the values of one mat4_t to another
 *
 * Params:
 * mat - mat4_t containing values to copy
 * dest - mat4_t receiving copied values
 *
 * Returns:
 * dest
 */
mat4_t mat4_set(mat4_t mat, mat4_t dest);

/*
 * mat4_identity
 * Sets a mat4_t to an identity matrix
 *
 * Params:
 * dest - mat4_t to set
 *
 * Returns:
 * dest
 */
mat4_t mat4_identity(mat4_t dest);

/*
 * mat4_transpose
 * Transposes a mat4_t (flips the values over the diagonal)
 *
 * Params:
 * mat - mat4_t to transpose
 * dest - Optional, mat4_t receiving transposed values. If NULL, result is written to mat
 *
 * Returns:
 * dest is specified, mat otherwise
 */
mat4_t mat4_transpose(mat4_t mat, mat4_t dest);

/*
 * mat4_determinant
 * Calculates the determinant of a mat4
 *
 * Params:
 * mat - mat4_t to calculate determinant of
 *
 * Returns:
 * determinant of mat
 */
numeric_t mat4_determinant(mat4_t mat);

/*
 * mat4_inverse
 * Calculates the inverse matrix of a mat4
 *
 * Params:
 * mat - mat4_t to calculate inverse of
 * dest - Optional, mat4_t receiving inverse matrix. If NULL, result is written to mat
 *
 * Returns:
 * dest is specified, mat otherwise, NULL if matrix cannot be inverted
 */
mat4_t mat4_inverse(mat4_t mat, mat4_t dest);

/*
 * mat4_toRotationMat
 * Copies the upper 3x3 elements of a mat4_t into another mat4
 *
 * Params:
 * mat - mat4_t containing values to copy
 * dest - Optional, mat4_t receiving copied values
 *
 * Returns:
 * dest is specified, a new mat4_t otherwise
 */
mat4_t mat4_toRotationMat(mat4_t mat, mat4_t dest);

/*
 * mat4_toMat3
 * Copies the upper 3x3 elements of a mat4_t into a mat3
 *
 * Params:
 * mat - mat4_t containing values to copy
 * dest - Optional, mat3_t receiving copied values
 *
 * Returns:
 * dest is specified, a new mat3_t otherwise
 */
mat3_t mat4_toMat3(mat4_t mat, mat3_t dest);

/*
 * mat4_toInverseMat3
 * Calculates the inverse of the upper 3x3 elements of a mat4_t and copies the result into a mat3
 * The resulting matrix is useful for calculating transformed normals
 *
 * Params:
 * mat - mat4_t containing values to invert and copy
 * dest - Optional, mat3_t receiving values
 *
 * Returns:
 * dest is specified, a new mat3_t otherwise, NULL if the matrix cannot be inverted
 */
mat3_t mat4_toInverseMat3(mat4_t mat, mat3_t dest);

/*
 * mat4_multiply
 * Performs a matrix multiplication
 *
 * Params:
 * mat - mat4, first operand
 * mat2 - mat4, second operand
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_multiply(mat4_t mat, mat4_t mat2, mat4_t dest);

/*
 * mat4_multiplyVec3
 * Transforms a vec3_t with the given matrix
 * 4th vector component is implicitly '1'
 *
 * Params:
 * mat - mat4_t to transform the vector with
 * vec - vec3_t to transform
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec3_t mat4_multiplyVec3(mat4_t mat, vec3_t vec, vec3_t dest);

/*
 * mat4_multiplyVec4
 * Transforms a vec4 with the given matrix
 *
 * Params:
 * mat - mat4_t to transform the vector with
 * vec - vec4 to transform
 * dest - Optional, vec4 receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
vec4_t mat4_multiplyVec4(mat4_t mat, vec4_t vec, vec4_t dest);

/*
 * mat4_translate
 * Translates a matrix by the given vector
 *
 * Params:
 * mat - mat4_t to translate
 * vec - vec3_t specifying the translation
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_translate(mat4_t mat, vec3_t vec, mat4_t dest);

/*
 * mat4_scale
 * Scales a matrix by the given vector
 *
 * Params:
 * mat - mat4_t to scale
 * vec - vec3_t specifying the scale for each axis
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_scale(mat4_t mat, vec3_t vec, mat4_t dest);

/*
 * mat4_scale_scalar
 * Scales a matrix by the given scalar
 *
 * Params:
 * mat - mat4_t to scale
 * scalar - scalar specifying the scale for each axis
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_scale_scalar(mat4_t mat, numeric_t scalar, mat4_t dest);

/*
 * mat4_rotate
 * Rotates a matrix by the given angle around the specified axis
 * If rotating around a primary axis (X,Y,Z) one of the specialized rotation functions should be used instead for performance
 *
 * Params:
 * mat - mat4_t to rotate
 * angle - angle (in radians) to rotate
 * axis - vec3_t representing the axis to rotate around
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_rotate(mat4_t mat, numeric_t angle, vec3_t axis, mat4_t dest);

/*
 * mat4_rotateX
 * Rotates a matrix by the given angle around the X axis
 *
 * Params:
 * mat - mat4_t to rotate
 * angle - angle (in radians) to rotate
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_rotateX(mat4_t mat, numeric_t angle, mat4_t dest);

/*
 * mat4_rotateY
 * Rotates a matrix by the given angle around the Y axis
 *
 * Params:
 * mat - mat4_t to rotate
 * angle - angle (in radians) to rotate
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_rotateY(mat4_t mat, numeric_t angle, mat4_t dest);

/*
 * mat4_rotateZ
 * Rotates a matrix by the given angle around the Z axis
 *
 * Params:
 * mat - mat4_t to rotate
 * angle - angle (in radians) to rotate
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to mat
 *
 * Returns:
 * dest if not NULL, mat otherwise
 */
mat4_t mat4_rotateZ(mat4_t mat, numeric_t angle, mat4_t dest);

/*
 * mat4_frustum
 * Generates a frustum matrix with the given bounds
 *
 * Params:
 * left, right - scalar, left and right bounds of the frustum
 * bottom, top - scalar, bottom and top bounds of the frustum
 * near, far - scalar, near and far bounds of the frustum
 * dest - Optional, mat4_t frustum matrix will be written into
 *
 * Returns:
 * dest if not NULL, a new mat4_t otherwise
 */
mat4_t mat4_frustum(numeric_t left, numeric_t right, numeric_t bottom, numeric_t top, numeric_t near, numeric_t far, mat4_t dest);

/*
 * mat4_perspective
 * Generates a perspective projection matrix with the given bounds
 *
 * Params:
 * fovy - scalar, vertical field of view
 * aspect - scalar, aspect ratio. typically viewport width/height
 * near, far - scalar, near and far bounds of the frustum
 * dest - Optional, mat4_t frustum matrix will be written into
 *
 * Returns:
 * dest if not NULL, a new mat4_t otherwise
 */
mat4_t mat4_perspective(numeric_t fovy, numeric_t aspect, numeric_t near, numeric_t far, mat4_t dest);

/*
 * mat4_ortho
 * Generates a orthogonal projection matrix with the given bounds
 *
 * Params:
 * left, right - scalar, left and right bounds of the frustum
 * bottom, top - scalar, bottom and top bounds of the frustum
 * near, far - scalar, near and far bounds of the frustum
 * dest - Optional, mat4_t frustum matrix will be written into
 *
 * Returns:
 * dest if not NULL, a new mat4_t otherwise
 */
mat4_t mat4_ortho(numeric_t left, numeric_t right, numeric_t bottom, numeric_t top, numeric_t near, numeric_t far, mat4_t dest);

/*
 * mat4_lookAt
 * Generates a look-at matrix with the given eye position, focal point, and up axis
 *
 * Params:
 * eye - vec3, position of the viewer
 * center - vec3, point the viewer is looking at
 * up - vec3_t pointing "up"
 * dest - Optional, mat4_t frustum matrix will be written into
 *
 * Returns:
 * dest if not NULL, a new mat4_t otherwise
 */
mat4_t mat4_lookAt(vec3_t eye, vec3_t center, vec3_t up, mat4_t dest);

/*
 * mat4_fromRotationTranslation
 * Creates a matrix from a quaternion rotation and vector translation
 * This is equivalent to (but much faster than):
 *
 *     mat4_identity(dest);
 *     mat4_translate(dest, vec);
 *     mat4_t quatMat = mat4_create();
 *     quat_toMat4(quat, quatMat);
 *     mat4_multiply(dest, quatMat);
 *
 * Params:
 * quat - quat specifying the rotation by
 * vec - vec3_t specifying the translation
 * dest - Optional, mat4_t receiving operation result. If NULL, result is written to a new mat4
 *
 * Returns:
 * dest if not NULL, a new mat4_t otherwise
 */
mat4_t mat4_fromRotationTranslation(quat_t quat, vec3_t vec, mat4_t dest);

/*
 * mat4_str
 * Writes a string representation of a mat4
 *
 * Params:
 * mat - mat4_t to represent as a string
 * buffer - char * to store the results
 */
void mat4_str(mat4_t mat, char *buffer);

/*
 * quat - Quaternions
 */

/*
 * quat_create
 * Creates a new instance of a quat_t
 *
 * Params:
 * quat - Optional, quat_t containing values to initialize with
 *
 * Returns:
 * New quat_t
 */
quat_t quat_create(quat_t quat);

/*
 * quat_set
 * Copies the values of one quat_t to another
 *
 * Params:
 * quat - quat_t containing values to copy
 * dest - quat_t receiving copied values
 *
 * Returns:
 * dest
 */
quat_t quat_set(quat_t quat, quat_t dest);

/*
 * quat_calculateW
 * Calculates the W component of a quat_t from the X, Y, and Z components.
 * Assumes that quaternion is 1 unit in length.
 * Any existing W component will be ignored.
 *
 * Params:
 * quat - quat_t to calculate W component of
 * dest - Optional, quat_t receiving calculated values. If NULL, result is written to quat
 *
 * Returns:
 * dest if not NULL, quat otherwise
 */
quat_t quat_calculateW(quat_t quat, quat_t dest);

/**
 * quat_dot
 * Calculates the dot product of two quaternions
 *
 * @param {quat4} quat First operand
 * @param {quat4} quat2 Second operand
 *
 * @return {number} Dot product of quat and quat2
 */
numeric_t quat_dot(quat_t quat, quat_t quat2);

/*
 * quat_inverse
 * Calculates the inverse of a quat_t
 *
 * Params:
 * quat - quat_t to calculate inverse of
 * dest - Optional, quat_t receiving inverse values. If NULL, result is written to quat
 *
 * Returns:
 * dest if not NULL, quat otherwise
 */
quat_t quat_inverse(quat_t quat, quat_t dest);

/*
 * quat_conjugate
 * Calculates the conjugate of a quat_t
 *
 * Params:
 * quat - quat_t to calculate conjugate of
 * dest - Optional, quat_t receiving conjugate values. If NULL, result is written to quat
 *
 * Returns:
 * dest if not NULL, quat otherwise
 */
quat_t quat_conjugate(quat_t quat, quat_t dest);

/*
 * quat_length
 * Calculates the length of a quat_t
 *
 * Params:
 * quat - quat_t to calculate length of
 *
 * Returns:
 * Length of quat
 */
numeric_t quat_length(quat_t quat);

/*
 * quat_normalize
 * Generates a unit quaternion of the same direction as the provided quat_t
 * If quaternion length is 0, returns [0, 0, 0, 0]
 *
 * Params:
 * quat - quat_t to normalize
 * dest - Optional, quat_ receiving operation result. If NULL, result is written to quat
 *
 * Returns:
 * dest if not NULL, quat otherwise
 */
quat_t quat_normalize(quat_t quat, quat_t dest);

/*
 * quat_multiply
 * Performs a quaternion multiplication
 *
 * Params:
 * quat - quat_t, first operand
 * quat2 - quat_t, second operand
 * dest - Optional, quat_t receiving operation result. If NULL, result is written to quat
 *
 * Returns:
 * dest if not NULL, quat otherwise
 */
quat_t quat_multiply(quat_t quat, quat_t quat2, quat_t dest);

/*
 * quat_multiplyVec3
 * Transforms a vec3_t with the given quaternion
 *
 * Params:
 * quat - quat_t to transform the vector with
 * vec - vec3_t to transform
 * dest - Optional, vec3_t receiving operation result. If NULL, result is written to vec
 *
 * Returns:
 * dest if not NULL, vec otherwise
 */
quat_t quat_multiplyVec3(quat_t quat, vec3_t vec, vec3_t dest);

/*
 * quat_toMat3
 * Calculates a 3x3 matrix from the given quat_t
 *
 * Params:
 * quat - quat_t to create matrix from
 * dest - Optional, mat3_t receiving operation result
 *
 * Returns:
 * dest if not NULL, a new mat3_t otherwise
 */
mat3_t quat_toMat3(quat_t quat, mat3_t dest);

/*
 * quat_toMat4
 * Calculates a 4x4 matrix from the given quat_t
 *
 * Params:
 * quat - quat_t to create matrix from
 * dest - Optional, mat4_t receiving operation result
 *
 * Returns:
 * dest if not NULL, a new mat4_t otherwise
 */
quat_t quat_toMat4(quat_t quat, mat4_t dest);

/*
 * quat_slerp
 * Performs a spherical linear interpolation between two quat_t
 *
 * Params:
 * quat - quat_t, first quaternion
 * quat2 - quat_t, second quaternion
 * slerp - interpolation amount between the two inputs
 * dest - Optional, quat_t receiving operation result. If NULL, result is written to quat
 *
 * Returns:
 * dest if not NULL, quat otherwise
 */
quat_t quat_slerp(quat_t quat, quat_t quat2, numeric_t slerp, quat_t dest);

/*
 * quat_rotate
 * Rotates a point around a quaternion by applying the formula
 *
 *    r = q.p.q*
 *
 * where q is the quaternion, q* is its conjugate, and p is the
 * point to be rotated.
 *
 * Params:
 * quat - quat_t, first quaternion
 * p - vec3_t, the point to rotate
 * dest - Optional, quat_t receiving operation result. If NULL, result is written to quat
 *
 * Returns:
 * dest if not NULL, quat otherwise
 */
quat_t quat_rotate(quat_t quat, vec3_t p, quat_t dest);

/*
 * quat_str
 * Writes a string representation of a quaternion
 *
 * Params:
 * quat - quat_t to represent as a string
 * buffer - char * to store the results
 */
void quat_str(quat_t quat, char *buffer);

#ifdef __cplusplus
}
#endif

#endif
#ifdef GL_MATRIX_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


vec2_t vec2_create(vec2_t vec) {
    vec2_t dest = calloc(2, sizeof(numeric_t));

    if (vec) {
        dest[0] = vec[0];
        dest[1] = vec[1];
    } else {
        dest[0] = dest[1] = 0;
    }

    return dest;
}

vec2_t vec2_set(vec2_t vec, vec2_t dest) {
    dest[0] = vec[0];
    dest[1] = vec[1];
    return dest;
}

vec2_t vec2_add(vec2_t vec, vec2_t vec2, vec2_t dest) {
    if (!dest || vec == dest) {
        vec[0] += vec2[0];
        vec[1] += vec2[1];
        return vec;
    }

    dest[0] = vec[0] + vec2[0];
    dest[1] = vec[1] + vec2[1];

    return dest;
}

vec2_t vec2_subtract(vec2_t vec, vec2_t vec2, vec2_t dest) {
    if (!dest || vec == dest) {
        vec[0] -= vec2[0];
        vec[1] -= vec2[1];
        return vec;
    }

    dest[0] = vec[0] - vec2[0];
    dest[1] = vec[1] - vec2[1];
    return dest;
}

vec2_t vec2_negate(vec2_t vec, vec2_t dest) {
    if (!dest) { dest = vec; }

    dest[0] = -vec[0];
    dest[1] = -vec[1];
    return dest;
}

vec2_t vec2_scale(vec2_t vec, numeric_t val, vec2_t dest) {
    if (!dest || vec == dest) {
        vec[0] *= val;
        vec[1] *= val;
        return vec;
    }

    dest[0] = vec[0] * val;
    dest[1] = vec[1] * val;
    return dest;
}

vec2_t vec2_normalize(vec2_t vec, vec2_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0], y = vec[1],
        len = sqrt(x * x + y * y);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        return dest;
    } else if (len == 1) {
        dest[0] = x;
        dest[1] = y;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    return dest;
}

numeric_t vec2_length(vec2_t vec) {
    numeric_t x = vec[0], y = vec[1];
    return sqrt(x * x + y * y);
}

numeric_t vec2_dot(vec2_t vec, vec2_t vec2) {
    return vec[0] * vec2[0] + vec[1] * vec2[1];
}

vec2_t vec2_direction (vec2_t vec, vec2_t vec2, vec2_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0] - vec2[0],
        y = vec[1] - vec2[1],
        len = sqrt(x * x + y * y);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    return dest;
}

vec2_t vec2_lerp(vec2_t vec, vec2_t vec2, numeric_t lerp, vec2_t dest) {
    if (!dest) { dest = vec; }
    dest[0] = vec[0] + lerp * (vec2[0] - vec[0]);
    dest[1] = vec[1] + lerp * (vec2[1] - vec[1]);
    return dest;
}

numeric_t vec2_dist(vec2_t vec, vec2_t vec2) {
    numeric_t x = vec2[0] - vec[0],
        y = vec2[1] - vec[1];

    return sqrt(x*x + y*y);
}


vec3_t vec3_create(vec3_t vec) {
    vec3_t dest = calloc(3, sizeof(numeric_t));

    if (vec) {
        dest[0] = vec[0];
        dest[1] = vec[1];
        dest[2] = vec[2];
    } else {
        dest[0] = dest[1] = dest[2] = 0;
    }

    return dest;
}

vec3_t vec3_set(vec3_t vec, vec3_t dest) {
    dest[0] = vec[0];
    dest[1] = vec[1];
    dest[2] = vec[2];

    return dest;
}

vec3_t vec3_add(vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] += vec2[0];
        vec[1] += vec2[1];
        vec[2] += vec2[2];
        return vec;
    }

    dest[0] = vec[0] + vec2[0];
    dest[1] = vec[1] + vec2[1];
    dest[2] = vec[2] + vec2[2];

    return dest;
}

vec3_t vec3_subtract(vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] -= vec2[0];
        vec[1] -= vec2[1];
        vec[2] -= vec2[2];
        return vec;
    }

    dest[0] = vec[0] - vec2[0];
    dest[1] = vec[1] - vec2[1];
    dest[2] = vec[2] - vec2[2];
    return dest;
}

vec3_t vec3_multiply(vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] *= vec2[0];
        vec[1] *= vec2[1];
        vec[2] *= vec2[2];
        return vec;
    }

    dest[0] = vec[0] * vec2[0];
    dest[1] = vec[1] * vec2[1];
    dest[2] = vec[2] * vec2[2];
    return dest;
}

vec3_t vec3_negate(vec3_t vec, vec3_t dest) {
    if (!dest) { dest = vec; }

    dest[0] = -vec[0];
    dest[1] = -vec[1];
    dest[2] = -vec[2];
    return dest;
}

vec3_t vec3_scale(vec3_t vec, numeric_t val, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] *= val;
        vec[1] *= val;
        vec[2] *= val;
        return vec;
    }

    dest[0] = vec[0] * val;
    dest[1] = vec[1] * val;
    dest[2] = vec[2] * val;
    return dest;
}

vec3_t vec3_normalize(vec3_t vec, vec3_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0], y = vec[1], z = vec[2],
        len = sqrt(x * x + y * y + z * z);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        return dest;
    } else if (len == 1) {
        dest[0] = x;
        dest[1] = y;
        dest[2] = z;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    dest[2] = z * len;
    return dest;
}

vec3_t vec3_cross (vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0], y = vec[1], z = vec[2],
        x2 = vec2[0], y2 = vec2[1], z2 = vec2[2];

    dest[0] = y * z2 - z * y2;
    dest[1] = z * x2 - x * z2;
    dest[2] = x * y2 - y * x2;
    return dest;
}

numeric_t vec3_length(vec3_t vec) {
    numeric_t x = vec[0], y = vec[1], z = vec[2];
    return sqrt(x * x + y * y + z * z);
}

numeric_t vec3_dot(vec3_t vec, vec3_t vec2) {
    return vec[0] * vec2[0] + vec[1] * vec2[1] + vec[2] * vec2[2];
}

vec3_t vec3_direction (vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0] - vec2[0],
        y = vec[1] - vec2[1],
        z = vec[2] - vec2[2],
        len = sqrt(x * x + y * y + z * z);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    dest[2] = z * len;
    return dest;
}

vec3_t vec3_lerp(vec3_t vec, vec3_t vec2, numeric_t lerp, vec3_t dest) {
    if (!dest) { dest = vec; }

    dest[0] = vec[0] + lerp * (vec2[0] - vec[0]);
    dest[1] = vec[1] + lerp * (vec2[1] - vec[1]);
    dest[2] = vec[2] + lerp * (vec2[2] - vec[2]);

    return dest;
}

numeric_t vec3_dist(vec3_t vec, vec3_t vec2) {
    numeric_t x = vec2[0] - vec[0],
        y = vec2[1] - vec[1],
        z = vec2[2] - vec[2];

    return sqrt(x*x + y*y + z*z);
}

vec3_t vec3_unproject(vec3_t vec, mat4_t view, mat4_t proj, vec4_t viewport, vec3_t dest) {
    if (!dest) { dest = vec; }

    mat4_t m = mat4_create(NULL);
    numeric_t *v = malloc(sizeof(numeric_t) * 4);

    v[0] = (vec[0] - viewport[0]) * 2.0 / viewport[2] - 1.0;
    v[1] = (vec[1] - viewport[1]) * 2.0 / viewport[3] - 1.0;
    v[2] = 2.0 * vec[2] - 1.0;
    v[3] = 1.0;

    mat4_multiply(proj, view, m);
    if(!mat4_inverse(m, NULL)) { return NULL; }

    mat4_multiplyVec4(m, v, NULL);
    if(v[3] == 0.0) { return NULL; }

    dest[0] = v[0] / v[3];
    dest[1] = v[1] / v[3];
    dest[2] = v[2] / v[3];

    return dest;
}


vec4_t vec4_create(vec4_t vec) {
    vec4_t dest = calloc(4, sizeof(numeric_t));

    if (vec) {
        dest[0] = vec[0];
        dest[1] = vec[1];
        dest[2] = vec[2];
        dest[3] = vec[3];
    } else {
        dest[0] = dest[1] = dest[2] = dest[3] = 0;
    }

    return dest;
}

vec4_t vec4_set(vec4_t vec, vec4_t dest) {
    dest[0] = vec[0];
    dest[1] = vec[1];
    dest[2] = vec[2];
    dest[3] = vec[3];
    return dest;
}

vec4_t vec4_add(vec4_t vec, vec4_t vec2, vec4_t dest) {
    if (!dest || vec == dest) {
        vec[0] += vec2[0];
        vec[1] += vec2[1];
        vec[2] += vec2[2];
        vec[3] += vec2[3];
        return vec;
    }

    dest[0] = vec[0] + vec2[0];
    dest[1] = vec[1] + vec2[1];
    dest[2] = vec[2] + vec2[2];
    dest[3] = vec[3] + vec2[3];

    return dest;
}

vec4_t vec4_subtract(vec4_t vec, vec4_t vec2, vec4_t dest) {
    if (!dest || vec == dest) {
        vec[0] -= vec2[0];
        vec[1] -= vec2[1];
        vec[2] -= vec2[2];
        vec[3] -= vec2[3];
        return vec;
    }

    dest[0] = vec[0] - vec2[0];
    dest[1] = vec[1] - vec2[1];
    dest[2] = vec[2] - vec2[2];
    dest[3] = vec[3] - vec2[3];
    return dest;
}

vec4_t vec4_negate(vec4_t vec, vec4_t dest) {
    if (!dest) { dest = vec; }

    dest[0] = -vec[0];
    dest[1] = -vec[1];
    dest[2] = -vec[2];
    dest[3] = -vec[3];
    return dest;
}

vec4_t vec4_scale(vec4_t vec, numeric_t val, vec4_t dest) {
    if (!dest || vec == dest) {
        vec[0] *= val;
        vec[1] *= val;
        vec[2] *= val;
        vec[3] *= val;
        return vec;
    }

    dest[0] = vec[0] * val;
    dest[1] = vec[1] * val;
    dest[2] = vec[2] * val;
    dest[3] = vec[3] * val;
    return dest;
}

vec4_t vec4_normalize(vec4_t vec, vec4_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0], y = vec[1], z = vec[2], w = vec[3],
        len = sqrt(x * x + y * y + z * z + w * w);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        dest[3] = 0;
        return dest;
    } else if (len == 1) {
        dest[0] = x;
        dest[1] = y;
        dest[2] = z;
        dest[3] = z;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    dest[2] = z * len;
    dest[3] = w * len;
    return dest;
}

numeric_t vec4_length(vec4_t vec) {
    numeric_t x = vec[0], y = vec[1], z = vec[2], w = vec[3];
    return sqrt(x * x + y * y + z * z + w * w);
}

numeric_t vec4_dot(vec4_t vec, vec4_t vec2) {
    return vec[0] * vec2[0] + vec[1] * vec2[1] + vec[2] * vec2[2] + vec[3] * vec2[3];
}

vec4_t vec4_direction (vec4_t vec, vec4_t vec2, vec4_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0] - vec2[0],
        y = vec[1] - vec2[1],
        z = vec[2] - vec2[2],
        w = vec[3] - vec2[3],
        len = sqrt(x * x + y * y + z * z + w * w);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        dest[3] = 0;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    dest[2] = z * len;
    dest[3] = w * len;
    return dest;
}

vec4_t vec4_lerp(vec4_t vec, vec4_t vec2, numeric_t lerp, vec4_t dest) {
    if (!dest) { dest = vec; }

    dest[0] = vec[0] + lerp * (vec2[0] - vec[0]);
    dest[1] = vec[1] + lerp * (vec2[1] - vec[1]);
    dest[2] = vec[2] + lerp * (vec2[2] - vec[2]);
    dest[3] = vec[3] + lerp * (vec2[3] - vec[3]);

    return dest;
}

numeric_t vec4_dist(vec4_t vec, vec4_t vec2) {
    numeric_t x = vec2[0] - vec[0],
        y = vec2[1] - vec[1],
        z = vec2[2] - vec[2],
        w = vec2[3] - vec[3];

    return sqrt(x*x + y*y + z*z + w*w);
}


mat3_t mat3_create(mat3_t mat) {
    mat3_t dest = calloc(9, sizeof(numeric_t));

    if (mat) {
        dest[0] = mat[0];
        dest[1] = mat[1];
        dest[2] = mat[2];
        dest[3] = mat[3];
        dest[4] = mat[4];
        dest[5] = mat[5];
        dest[6] = mat[6];
        dest[7] = mat[7];
        dest[8] = mat[8];
    }

    return dest;
}

mat3_t mat3_set(mat3_t mat, mat3_t dest) {
    dest[0] = mat[0];
    dest[1] = mat[1];
    dest[2] = mat[2];
    dest[3] = mat[3];
    dest[4] = mat[4];
    dest[5] = mat[5];
    dest[6] = mat[6];
    dest[7] = mat[7];
    dest[8] = mat[8];
    return dest;
}

mat3_t mat3_identity(mat3_t dest) {
    if (!dest) { dest = mat3_create(NULL); }
    dest[0] = 1;
    dest[1] = 0;
    dest[2] = 0;
    dest[3] = 0;
    dest[4] = 1;
    dest[5] = 0;
    dest[6] = 0;
    dest[7] = 0;
    dest[8] = 1;
    return dest;
}

mat3_t mat3_transpose(mat3_t mat, mat3_t dest) {
    // If we are transposing ourselves we can skip a few steps but have to cache some values
    if (!dest || mat == dest) {
        numeric_t a01 = mat[1], a02 = mat[2],
            a12 = mat[5];

        mat[1] = mat[3];
        mat[2] = mat[6];
        mat[3] = a01;
        mat[5] = mat[7];
        mat[6] = a02;
        mat[7] = a12;
        return mat;
    }

    dest[0] = mat[0];
    dest[1] = mat[3];
    dest[2] = mat[6];
    dest[3] = mat[1];
    dest[4] = mat[4];
    dest[5] = mat[7];
    dest[6] = mat[2];
    dest[7] = mat[5];
    dest[8] = mat[8];
    return dest;
}

mat4_t mat3_toMat4(mat3_t mat, mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }

    dest[15] = 1;
    dest[14] = 0;
    dest[13] = 0;
    dest[12] = 0;

    dest[11] = 0;
    dest[10] = mat[8];
    dest[9] = mat[7];
    dest[8] = mat[6];

    dest[7] = 0;
    dest[6] = mat[5];
    dest[5] = mat[4];
    dest[4] = mat[3];

    dest[3] = 0;
    dest[2] = mat[2];
    dest[1] = mat[1];
    dest[0] = mat[0];

    return dest;
}


mat4_t mat4_create(mat4_t mat) {
    mat4_t dest = calloc(16, sizeof(numeric_t));

    if (mat) {
        dest[0] = mat[0];
        dest[1] = mat[1];
        dest[2] = mat[2];
        dest[3] = mat[3];
        dest[4] = mat[4];
        dest[5] = mat[5];
        dest[6] = mat[6];
        dest[7] = mat[7];
        dest[8] = mat[8];
        dest[9] = mat[9];
        dest[10] = mat[10];
        dest[11] = mat[11];
        dest[12] = mat[12];
        dest[13] = mat[13];
        dest[14] = mat[14];
        dest[15] = mat[15];
    }

    return dest;
}

mat4_t mat4_set(mat4_t mat, mat4_t dest) {
    dest[0] = mat[0];
    dest[1] = mat[1];
    dest[2] = mat[2];
    dest[3] = mat[3];
    dest[4] = mat[4];
    dest[5] = mat[5];
    dest[6] = mat[6];
    dest[7] = mat[7];
    dest[8] = mat[8];
    dest[9] = mat[9];
    dest[10] = mat[10];
    dest[11] = mat[11];
    dest[12] = mat[12];
    dest[13] = mat[13];
    dest[14] = mat[14];
    dest[15] = mat[15];
    return dest;
}

mat4_t mat4_identity(mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }
    dest[0] = 1;
    dest[1] = 0;
    dest[2] = 0;
    dest[3] = 0;
    dest[4] = 0;
    dest[5] = 1;
    dest[6] = 0;
    dest[7] = 0;
    dest[8] = 0;
    dest[9] = 0;
    dest[10] = 1;
    dest[11] = 0;
    dest[12] = 0;
    dest[13] = 0;
    dest[14] = 0;
    dest[15] = 1;
    return dest;
}

mat4_t mat4_transpose(mat4_t mat, mat4_t dest) {
    // If we are transposing ourselves we can skip a few steps but have to cache some values
    if (!dest || mat == dest) {
        numeric_t a01 = mat[1], a02 = mat[2], a03 = mat[3],
            a12 = mat[6], a13 = mat[7],
            a23 = mat[11];

        mat[1] = mat[4];
        mat[2] = mat[8];
        mat[3] = mat[12];
        mat[4] = a01;
        mat[6] = mat[9];
        mat[7] = mat[13];
        mat[8] = a02;
        mat[9] = a12;
        mat[11] = mat[14];
        mat[12] = a03;
        mat[13] = a13;
        mat[14] = a23;
        return mat;
    }

    dest[0] = mat[0];
    dest[1] = mat[4];
    dest[2] = mat[8];
    dest[3] = mat[12];
    dest[4] = mat[1];
    dest[5] = mat[5];
    dest[6] = mat[9];
    dest[7] = mat[13];
    dest[8] = mat[2];
    dest[9] = mat[6];
    dest[10] = mat[10];
    dest[11] = mat[14];
    dest[12] = mat[3];
    dest[13] = mat[7];
    dest[14] = mat[11];
    dest[15] = mat[15];
    return dest;
}

numeric_t mat4_determinant(mat4_t mat) {
    // Cache the matrix values (makes for huge speed increases!)
    numeric_t a00 = mat[0], a01 = mat[1], a02 = mat[2], a03 = mat[3],
        a10 = mat[4], a11 = mat[5], a12 = mat[6], a13 = mat[7],
        a20 = mat[8], a21 = mat[9], a22 = mat[10], a23 = mat[11],
        a30 = mat[12], a31 = mat[13], a32 = mat[14], a33 = mat[15];

    return (a30 * a21 * a12 * a03 - a20 * a31 * a12 * a03 - a30 * a11 * a22 * a03 + a10 * a31 * a22 * a03 +
            a20 * a11 * a32 * a03 - a10 * a21 * a32 * a03 - a30 * a21 * a02 * a13 + a20 * a31 * a02 * a13 +
            a30 * a01 * a22 * a13 - a00 * a31 * a22 * a13 - a20 * a01 * a32 * a13 + a00 * a21 * a32 * a13 +
            a30 * a11 * a02 * a23 - a10 * a31 * a02 * a23 - a30 * a01 * a12 * a23 + a00 * a31 * a12 * a23 +
            a10 * a01 * a32 * a23 - a00 * a11 * a32 * a23 - a20 * a11 * a02 * a33 + a10 * a21 * a02 * a33 +
            a20 * a01 * a12 * a33 - a00 * a21 * a12 * a33 - a10 * a01 * a22 * a33 + a00 * a11 * a22 * a33);
}

mat4_t mat4_inverse(mat4_t mat, mat4_t dest) {
    if (!dest) { dest = mat; }

    // Cache the matrix values (makes for huge speed increases!)
    numeric_t a00 = mat[0], a01 = mat[1], a02 = mat[2], a03 = mat[3],
        a10 = mat[4], a11 = mat[5], a12 = mat[6], a13 = mat[7],
        a20 = mat[8], a21 = mat[9], a22 = mat[10], a23 = mat[11],
        a30 = mat[12], a31 = mat[13], a32 = mat[14], a33 = mat[15],

        b00 = a00 * a11 - a01 * a10,
        b01 = a00 * a12 - a02 * a10,
        b02 = a00 * a13 - a03 * a10,
        b03 = a01 * a12 - a02 * a11,
        b04 = a01 * a13 - a03 * a11,
        b05 = a02 * a13 - a03 * a12,
        b06 = a20 * a31 - a21 * a30,
        b07 = a20 * a32 - a22 * a30,
        b08 = a20 * a33 - a23 * a30,
        b09 = a21 * a32 - a22 * a31,
        b10 = a21 * a33 - a23 * a31,
        b11 = a22 * a33 - a23 * a32,

        d = (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06),
        invDet;

        // Calculate the determinant
        if (!d) { return NULL; }
        invDet = 1 / d;

    dest[0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    dest[1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    dest[2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    dest[3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    dest[4] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    dest[5] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    dest[6] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    dest[7] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    dest[8] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    dest[9] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    dest[10] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    dest[11] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    dest[12] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    dest[13] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    dest[14] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    dest[15] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;

    return dest;
}

mat4_t mat4_toRotationMat(mat4_t mat, mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }

    dest[0] = mat[0];
    dest[1] = mat[1];
    dest[2] = mat[2];
    dest[3] = mat[3];
    dest[4] = mat[4];
    dest[5] = mat[5];
    dest[6] = mat[6];
    dest[7] = mat[7];
    dest[8] = mat[8];
    dest[9] = mat[9];
    dest[10] = mat[10];
    dest[11] = mat[11];
    dest[12] = 0;
    dest[13] = 0;
    dest[14] = 0;
    dest[15] = 1;

    return dest;
}

mat3_t mat4_toMat3(mat4_t mat, mat3_t dest) {
    if (!dest) { dest = mat3_create(NULL); }

    dest[0] = mat[0];
    dest[1] = mat[1];
    dest[2] = mat[2];
    dest[3] = mat[4];
    dest[4] = mat[5];
    dest[5] = mat[6];
    dest[6] = mat[8];
    dest[7] = mat[9];
    dest[8] = mat[10];

    return dest;
}

mat3_t mat4_toInverseMat3(mat4_t mat, mat3_t dest) {
    // Cache the matrix values (makes for huge speed increases!)
    numeric_t a00 = mat[0], a01 = mat[1], a02 = mat[2],
        a10 = mat[4], a11 = mat[5], a12 = mat[6],
        a20 = mat[8], a21 = mat[9], a22 = mat[10],

        b01 = a22 * a11 - a12 * a21,
        b11 = -a22 * a10 + a12 * a20,
        b21 = a21 * a10 - a11 * a20,

        d = a00 * b01 + a01 * b11 + a02 * b21,
        id;

    if (!d) { return NULL; }
    id = 1 / d;

    if (!dest) { dest = mat3_create(NULL); }

    dest[0] = b01 * id;
    dest[1] = (-a22 * a01 + a02 * a21) * id;
    dest[2] = (a12 * a01 - a02 * a11) * id;
    dest[3] = b11 * id;
    dest[4] = (a22 * a00 - a02 * a20) * id;
    dest[5] = (-a12 * a00 + a02 * a10) * id;
    dest[6] = b21 * id;
    dest[7] = (-a21 * a00 + a01 * a20) * id;
    dest[8] = (a11 * a00 - a01 * a10) * id;

    return dest;
}

mat4_t mat4_multiply(mat4_t mat, mat4_t mat2, mat4_t dest) {
    if (!dest) { dest = mat; }

    // Cache the matrix values (makes for huge speed increases!)
    numeric_t a00 = mat[0], a01 = mat[1], a02 = mat[2], a03 = mat[3],
        a10 = mat[4], a11 = mat[5], a12 = mat[6], a13 = mat[7],
        a20 = mat[8], a21 = mat[9], a22 = mat[10], a23 = mat[11],
        a30 = mat[12], a31 = mat[13], a32 = mat[14], a33 = mat[15],

        b00 = mat2[0], b01 = mat2[1], b02 = mat2[2], b03 = mat2[3],
        b10 = mat2[4], b11 = mat2[5], b12 = mat2[6], b13 = mat2[7],
        b20 = mat2[8], b21 = mat2[9], b22 = mat2[10], b23 = mat2[11],
        b30 = mat2[12], b31 = mat2[13], b32 = mat2[14], b33 = mat2[15];

    dest[0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
    dest[1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
    dest[2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
    dest[3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
    dest[4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
    dest[5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
    dest[6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
    dest[7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
    dest[8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
    dest[9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
    dest[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
    dest[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
    dest[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
    dest[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
    dest[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
    dest[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;

    return dest;
}

vec3_t mat4_multiplyVec3(mat4_t mat, vec3_t vec, vec3_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0], y = vec[1], z = vec[2];

    dest[0] = mat[0] * x + mat[4] * y + mat[8] * z + mat[12];
    dest[1] = mat[1] * x + mat[5] * y + mat[9] * z + mat[13];
    dest[2] = mat[2] * x + mat[6] * y + mat[10] * z + mat[14];

    return dest;
}

vec4_t mat4_multiplyVec4(mat4_t mat, vec4_t vec, vec4_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0], y = vec[1], z = vec[2], w = vec[3];

    dest[0] = mat[0] * x + mat[4] * y + mat[8] * z + mat[12] * w;
    dest[1] = mat[1] * x + mat[5] * y + mat[9] * z + mat[13] * w;
    dest[2] = mat[2] * x + mat[6] * y + mat[10] * z + mat[14] * w;
    dest[3] = mat[3] * x + mat[7] * y + mat[11] * z + mat[15] * w;

    return dest;
}

mat4_t mat4_translate(mat4_t mat, vec3_t vec, mat4_t dest) {
    numeric_t x = vec[0], y = vec[1], z = vec[2],
        a00, a01, a02, a03,
        a10, a11, a12, a13,
        a20, a21, a22, a23;

    if (!dest || mat == dest) {
        mat[12] = mat[0] * x + mat[4] * y + mat[8] * z + mat[12];
        mat[13] = mat[1] * x + mat[5] * y + mat[9] * z + mat[13];
        mat[14] = mat[2] * x + mat[6] * y + mat[10] * z + mat[14];
        mat[15] = mat[3] * x + mat[7] * y + mat[11] * z + mat[15];
        return mat;
    }

    a00 = mat[0]; a01 = mat[1]; a02 = mat[2]; a03 = mat[3];
    a10 = mat[4]; a11 = mat[5]; a12 = mat[6]; a13 = mat[7];
    a20 = mat[8]; a21 = mat[9]; a22 = mat[10]; a23 = mat[11];

    dest[0] = a00; dest[1] = a01; dest[2] = a02; dest[3] = a03;
    dest[4] = a10; dest[5] = a11; dest[6] = a12; dest[7] = a13;
    dest[8] = a20; dest[9] = a21; dest[10] = a22; dest[11] = a23;

    dest[12] = a00 * x + a10 * y + a20 * z + mat[12];
    dest[13] = a01 * x + a11 * y + a21 * z + mat[13];
    dest[14] = a02 * x + a12 * y + a22 * z + mat[14];
    dest[15] = a03 * x + a13 * y + a23 * z + mat[15];
    return dest;
}

mat4_t mat4_scale(mat4_t mat, vec3_t vec, mat4_t dest) {
    numeric_t x = vec[0], y = vec[1], z = vec[2];

    if (!dest || mat == dest) {
        mat[0] *= x;
        mat[1] *= x;
        mat[2] *= x;
        mat[3] *= x;
        mat[4] *= y;
        mat[5] *= y;
        mat[6] *= y;
        mat[7] *= y;
        mat[8] *= z;
        mat[9] *= z;
        mat[10] *= z;
        mat[11] *= z;
        return mat;
    }

    dest[0] = mat[0] * x;
    dest[1] = mat[1] * x;
    dest[2] = mat[2] * x;
    dest[3] = mat[3] * x;
    dest[4] = mat[4] * y;
    dest[5] = mat[5] * y;
    dest[6] = mat[6] * y;
    dest[7] = mat[7] * y;
    dest[8] = mat[8] * z;
    dest[9] = mat[9] * z;
    dest[10] = mat[10] * z;
    dest[11] = mat[11] * z;
    dest[12] = mat[12];
    dest[13] = mat[13];
    dest[14] = mat[14];
    dest[15] = mat[15];
    return dest;
}

mat4_t mat4_scale_scalar(mat4_t mat, numeric_t scalar, mat4_t dest) {
    if (!dest || mat == dest) {
        mat[0] *= scalar;
        mat[1] *= scalar;
        mat[2] *= scalar;
        mat[3] *= scalar;
        mat[4] *= scalar;
        mat[5] *= scalar;
        mat[6] *= scalar;
        mat[7] *= scalar;
        mat[8] *= scalar;
        mat[9] *= scalar;
        mat[10] *= scalar;
        mat[11] *= scalar;
        return mat;
    }

    dest[0] = mat[0] * scalar;
    dest[1] = mat[1] * scalar;
    dest[2] = mat[2] * scalar;
    dest[3] = mat[3] * scalar;
    dest[4] = mat[4] * scalar;
    dest[5] = mat[5] * scalar;
    dest[6] = mat[6] * scalar;
    dest[7] = mat[7] * scalar;
    dest[8] = mat[8] * scalar;
    dest[9] = mat[9] * scalar;
    dest[10] = mat[10] * scalar;
    dest[11] = mat[11] * scalar;
    dest[12] = mat[12];
    dest[13] = mat[13];
    dest[14] = mat[14];
    dest[15] = mat[15];
    return dest;
}

mat4_t mat4_rotate(mat4_t mat, numeric_t angle, vec3_t axis, mat4_t dest) {
    numeric_t x = axis[0], y = axis[1], z = axis[2],
        len = sqrt(x * x + y * y + z * z),
        s, c, t,
        a00, a01, a02, a03,
        a10, a11, a12, a13,
        a20, a21, a22, a23,
        b00, b01, b02,
        b10, b11, b12,
        b20, b21, b22;

    if (!len) { return NULL; }
    if (len != 1) {
        len = 1 / len;
        x *= len;
        y *= len;
        z *= len;
    }

    s = sin(angle);
    c = cos(angle);
    t = 1 - c;

    a00 = mat[0]; a01 = mat[1]; a02 = mat[2]; a03 = mat[3];
    a10 = mat[4]; a11 = mat[5]; a12 = mat[6]; a13 = mat[7];
    a20 = mat[8]; a21 = mat[9]; a22 = mat[10]; a23 = mat[11];

    // Construct the elements of the rotation matrix
    b00 = x * x * t + c; b01 = y * x * t + z * s; b02 = z * x * t - y * s;
    b10 = x * y * t - z * s; b11 = y * y * t + c; b12 = z * y * t + x * s;
    b20 = x * z * t + y * s; b21 = y * z * t - x * s; b22 = z * z * t + c;

    if (!dest) {
        dest = mat;
    } else if (mat != dest) { // If the source and destination differ, copy the unchanged last row
        dest[12] = mat[12];
        dest[13] = mat[13];
        dest[14] = mat[14];
        dest[15] = mat[15];
    }

    // Perform rotation-specific matrix multiplication
    dest[0] = a00 * b00 + a10 * b01 + a20 * b02;
    dest[1] = a01 * b00 + a11 * b01 + a21 * b02;
    dest[2] = a02 * b00 + a12 * b01 + a22 * b02;
    dest[3] = a03 * b00 + a13 * b01 + a23 * b02;

    dest[4] = a00 * b10 + a10 * b11 + a20 * b12;
    dest[5] = a01 * b10 + a11 * b11 + a21 * b12;
    dest[6] = a02 * b10 + a12 * b11 + a22 * b12;
    dest[7] = a03 * b10 + a13 * b11 + a23 * b12;

    dest[8] = a00 * b20 + a10 * b21 + a20 * b22;
    dest[9] = a01 * b20 + a11 * b21 + a21 * b22;
    dest[10] = a02 * b20 + a12 * b21 + a22 * b22;
    dest[11] = a03 * b20 + a13 * b21 + a23 * b22;
    return dest;
}

mat4_t mat4_rotateX(mat4_t mat, numeric_t angle, mat4_t dest) {
    numeric_t s = sin(angle),
        c = cos(angle),
        a10 = mat[4],
        a11 = mat[5],
        a12 = mat[6],
        a13 = mat[7],
        a20 = mat[8],
        a21 = mat[9],
        a22 = mat[10],
        a23 = mat[11];

    if (!dest) {
        dest = mat;
    } else if (mat != dest) { // If the source and destination differ, copy the unchanged rows
        dest[0] = mat[0];
        dest[1] = mat[1];
        dest[2] = mat[2];
        dest[3] = mat[3];

        dest[12] = mat[12];
        dest[13] = mat[13];
        dest[14] = mat[14];
        dest[15] = mat[15];
    }

    // Perform axis-specific matrix multiplication
    dest[4] = a10 * c + a20 * s;
    dest[5] = a11 * c + a21 * s;
    dest[6] = a12 * c + a22 * s;
    dest[7] = a13 * c + a23 * s;

    dest[8] = a10 * -s + a20 * c;
    dest[9] = a11 * -s + a21 * c;
    dest[10] = a12 * -s + a22 * c;
    dest[11] = a13 * -s + a23 * c;
    return dest;
}

mat4_t mat4_rotateY(mat4_t mat, numeric_t angle, mat4_t dest) {
    numeric_t s = sin(angle),
        c = cos(angle),
        a00 = mat[0],
        a01 = mat[1],
        a02 = mat[2],
        a03 = mat[3],
        a20 = mat[8],
        a21 = mat[9],
        a22 = mat[10],
        a23 = mat[11];

    if (!dest) {
        dest = mat;
    } else if (mat != dest) { // If the source and destination differ, copy the unchanged rows
        dest[4] = mat[4];
        dest[5] = mat[5];
        dest[6] = mat[6];
        dest[7] = mat[7];

        dest[12] = mat[12];
        dest[13] = mat[13];
        dest[14] = mat[14];
        dest[15] = mat[15];
    }

    // Perform axis-specific matrix multiplication
    dest[0] = a00 * c + a20 * -s;
    dest[1] = a01 * c + a21 * -s;
    dest[2] = a02 * c + a22 * -s;
    dest[3] = a03 * c + a23 * -s;

    dest[8] = a00 * s + a20 * c;
    dest[9] = a01 * s + a21 * c;
    dest[10] = a02 * s + a22 * c;
    dest[11] = a03 * s + a23 * c;
    return dest;
}

mat4_t mat4_rotateZ(mat4_t mat, numeric_t angle, mat4_t dest) {
    numeric_t s = sin(angle),
        c = cos(angle),
        a00 = mat[0],
        a01 = mat[1],
        a02 = mat[2],
        a03 = mat[3],
        a10 = mat[4],
        a11 = mat[5],
        a12 = mat[6],
        a13 = mat[7];

    if (!dest) {
        dest = mat;
    } else if (mat != dest) { // If the source and destination differ, copy the unchanged last row
        dest[8] = mat[8];
        dest[9] = mat[9];
        dest[10] = mat[10];
        dest[11] = mat[11];

        dest[12] = mat[12];
        dest[13] = mat[13];
        dest[14] = mat[14];
        dest[15] = mat[15];
    }

    // Perform axis-specific matrix multiplication
    dest[0] = a00 * c + a10 * s;
    dest[1] = a01 * c + a11 * s;
    dest[2] = a02 * c + a12 * s;
    dest[3] = a03 * c + a13 * s;

    dest[4] = a00 * -s + a10 * c;
    dest[5] = a01 * -s + a11 * c;
    dest[6] = a02 * -s + a12 * c;
    dest[7] = a03 * -s + a13 * c;

    return dest;
}

mat4_t mat4_frustum(numeric_t left, numeric_t right, numeric_t bottom, numeric_t top, numeric_t near, numeric_t far, mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }
    numeric_t rl = (right - left),
        tb = (top - bottom),
        fn = (far - near);
    dest[0] = (near * 2) / rl;
    dest[1] = 0;
    dest[2] = 0;
    dest[3] = 0;
    dest[4] = 0;
    dest[5] = (near * 2) / tb;
    dest[6] = 0;
    dest[7] = 0;
    dest[8] = (right + left) / rl;
    dest[9] = (top + bottom) / tb;
    dest[10] = -(far + near) / fn;
    dest[11] = -1;
    dest[12] = 0;
    dest[13] = 0;
    dest[14] = -(far * near * 2) / fn;
    dest[15] = 0;
    return dest;
}

mat4_t mat4_perspective(numeric_t fovy, numeric_t aspect, numeric_t near, numeric_t far, mat4_t dest) {
    numeric_t top = near * tan(fovy * 3.14159265358979323846 / 360.0),
        right = top * aspect;
    return mat4_frustum(-right, right, -top, top, near, far, dest);
}

mat4_t mat4_ortho(numeric_t left, numeric_t right, numeric_t bottom, numeric_t top, numeric_t near, numeric_t far, mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }
    numeric_t rl = (right - left),
        tb = (top - bottom),
        fn = (far - near);
    dest[0] = 2 / rl;
    dest[1] = 0;
    dest[2] = 0;
    dest[3] = 0;
    dest[4] = 0;
    dest[5] = 2 / tb;
    dest[6] = 0;
    dest[7] = 0;
    dest[8] = 0;
    dest[9] = 0;
    dest[10] = -2 / fn;
    dest[11] = 0;
    dest[12] = -(left + right) / rl;
    dest[13] = -(top + bottom) / tb;
    dest[14] = -(far + near) / fn;
    dest[15] = 1;
    return dest;
}

mat4_t mat4_lookAt(vec3_t eye, vec3_t center, vec3_t up, mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }

    numeric_t x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
        eyex = eye[0],
        eyey = eye[1],
        eyez = eye[2],
        upx = up[0],
        upy = up[1],
        upz = up[2],
        centerx = center[0],
        centery = center[1],
        centerz = center[2];

    if (eyex == centerx && eyey == centery && eyez == centerz) {
        return mat4_identity(dest);
    }

    //vec3.direction(eye, center, z);
    z0 = eyex - centerx;
    z1 = eyey - centery;
    z2 = eyez - centerz;

    // normalize (no check needed for 0 because of early return)
    len = 1 / sqrt(z0 * z0 + z1 * z1 + z2 * z2);
    z0 *= len;
    z1 *= len;
    z2 *= len;

    //vec3.normalize(vec3.cross(up, z, x));
    x0 = upy * z2 - upz * z1;
    x1 = upz * z0 - upx * z2;
    x2 = upx * z1 - upy * z0;
    len = sqrt(x0 * x0 + x1 * x1 + x2 * x2);
    if (!len) {
        x0 = 0;
        x1 = 0;
        x2 = 0;
    } else {
        len = 1 / len;
        x0 *= len;
        x1 *= len;
        x2 *= len;
    }

    //vec3.normalize(vec3.cross(z, x, y));
    y0 = z1 * x2 - z2 * x1;
    y1 = z2 * x0 - z0 * x2;
    y2 = z0 * x1 - z1 * x0;

    len = sqrt(y0 * y0 + y1 * y1 + y2 * y2);
    if (!len) {
        y0 = 0;
        y1 = 0;
        y2 = 0;
    } else {
        len = 1 / len;
        y0 *= len;
        y1 *= len;
        y2 *= len;
    }

    dest[0] = x0;
    dest[1] = y0;
    dest[2] = z0;
    dest[3] = 0;
    dest[4] = x1;
    dest[5] = y1;
    dest[6] = z1;
    dest[7] = 0;
    dest[8] = x2;
    dest[9] = y2;
    dest[10] = z2;
    dest[11] = 0;
    dest[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
    dest[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
    dest[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
    dest[15] = 1;

    return dest;
}

mat4_t mat4_fromRotationTranslation(quat_t quat, vec3_t vec, mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }

    // Quaternion math
    numeric_t x = quat[0], y = quat[1], z = quat[2], w = quat[3],
        x2 = x + x,
        y2 = y + y,
        z2 = z + z,

        xx = x * x2,
        xy = x * y2,
        xz = x * z2,
        yy = y * y2,
        yz = y * z2,
        zz = z * z2,
        wx = w * x2,
        wy = w * y2,
        wz = w * z2;

    dest[0] = 1 - (yy + zz);
    dest[1] = xy + wz;
    dest[2] = xz - wy;
    dest[3] = 0;
    dest[4] = xy - wz;
    dest[5] = 1 - (xx + zz);
    dest[6] = yz + wx;
    dest[7] = 0;
    dest[8] = xz + wy;
    dest[9] = yz - wx;
    dest[10] = 1 - (xx + yy);
    dest[11] = 0;
    dest[12] = vec[0];
    dest[13] = vec[1];
    dest[14] = vec[2];
    dest[15] = 1;

    return dest;
}


quat_t quat_create(quat_t quat) {
    quat_t dest = calloc(4, sizeof(numeric_t));

    if (quat) {
        dest[0] = quat[0];
        dest[1] = quat[1];
        dest[2] = quat[2];
        dest[3] = quat[3];
    }

    return dest;
}

quat_t quat_set(quat_t quat, quat_t dest) {
    dest[0] = quat[0];
    dest[1] = quat[1];
    dest[2] = quat[2];
    dest[3] = quat[3];

    return dest;
}

quat_t quat_calculateW(quat_t quat, quat_t dest) {
    numeric_t x = quat[0], y = quat[1], z = quat[2];

    if (!dest || quat == dest) {
        quat[3] = -sqrt(fabs(1.0 - x * x - y * y - z * z));
        return quat;
    }
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
    dest[3] = -sqrt(fabs(1.0 - x * x - y * y - z * z));
    return dest;
}

numeric_t quat_dot(quat_t quat, quat_t quat2) {
    return quat[0]*quat2[0] + quat[1]*quat2[1] + quat[2]*quat2[2] + quat[3]*quat2[3];
}

quat_t quat_inverse(quat_t quat, quat_t dest) {
    numeric_t dot = quat_dot(quat,quat),
        invDot = 1.0/dot;
    if(!dest || quat == dest) {
        quat[0] *= -invDot;
        quat[1] *= -invDot;
        quat[2] *= -invDot;
        quat[3] *= invDot;
        return quat;
    }
    dest[0] = -quat[0]*invDot;
    dest[1] = -quat[1]*invDot;
    dest[2] = -quat[2]*invDot;
    dest[3] = quat[3]*invDot;
    return dest;
}

quat_t quat_conjugate(quat_t quat, quat_t dest) {
    if (!dest || quat == dest) {
        quat[0] *= -1;
        quat[1] *= -1;
        quat[2] *= -1;
        return quat;
    }
    dest[0] = -quat[0];
    dest[1] = -quat[1];
    dest[2] = -quat[2];
    dest[3] = quat[3];
    return dest;
}

numeric_t quat_length(quat_t quat) {
    numeric_t x = quat[0], y = quat[1], z = quat[2], w = quat[3];
    return sqrt(x * x + y * y + z * z + w * w);
}

quat_t quat_normalize(quat_t quat, quat_t dest) {
    if (!dest) { dest = quat; }

    numeric_t x = quat[0], y = quat[1], z = quat[2], w = quat[3],
        len = sqrt(x * x + y * y + z * z + w * w);
    if (len == 0) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        dest[3] = 0;
        return dest;
    }
    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    dest[2] = z * len;
    dest[3] = w * len;

    return dest;
}

quat_t quat_multiply(quat_t quat, quat_t quat2, quat_t dest) {
    if (!dest) { dest = quat; }

    numeric_t qax = quat[0], qay = quat[1], qaz = quat[2], qaw = quat[3],
        qbx = quat2[0], qby = quat2[1], qbz = quat2[2], qbw = quat2[3];

    dest[0] = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
    dest[1] = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
    dest[2] = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
    dest[3] = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;

    return dest;
}

quat_t quat_multiplyVec3(quat_t quat, vec3_t vec, vec3_t dest) {
    if (!dest) { dest = vec; }

    numeric_t x = vec[0], y = vec[1], z = vec[2],
        qx = quat[0], qy = quat[1], qz = quat[2], qw = quat[3],

        // calculate quat * vec
        ix = qw * x + qy * z - qz * y,
        iy = qw * y + qz * x - qx * z,
        iz = qw * z + qx * y - qy * x,
        iw = -qx * x - qy * y - qz * z;

    // calculate result * inverse quat
    dest[0] = ix * qw + iw * -qx + iy * -qz - iz * -qy;
    dest[1] = iy * qw + iw * -qy + iz * -qx - ix * -qz;
    dest[2] = iz * qw + iw * -qz + ix * -qy - iy * -qx;

    return dest;
}

mat3_t quat_toMat3(quat_t quat, mat3_t dest) {
    if (!dest) { dest = mat3_create(NULL); }

    numeric_t x = quat[0], y = quat[1], z = quat[2], w = quat[3],
        x2 = x + x,
        y2 = y + y,
        z2 = z + z,

        xx = x * x2,
        xy = x * y2,
        xz = x * z2,
        yy = y * y2,
        yz = y * z2,
        zz = z * z2,
        wx = w * x2,
        wy = w * y2,
        wz = w * z2;

    dest[0] = 1 - (yy + zz);
    dest[1] = xy + wz;
    dest[2] = xz - wy;

    dest[3] = xy - wz;
    dest[4] = 1 - (xx + zz);
    dest[5] = yz + wx;

    dest[6] = xz + wy;
    dest[7] = yz - wx;
    dest[8] = 1 - (xx + yy);

    return dest;
}

quat_t quat_toMat4(quat_t quat, mat4_t dest) {
    if (!dest) { dest = mat4_create(NULL); }

    numeric_t x = quat[0], y = quat[1], z = quat[2], w = quat[3],
        x2 = x + x,
        y2 = y + y,
        z2 = z + z,

        xx = x * x2,
        xy = x * y2,
        xz = x * z2,
        yy = y * y2,
        yz = y * z2,
        zz = z * z2,
        wx = w * x2,
        wy = w * y2,
        wz = w * z2;

    dest[0] = 1 - (yy + zz);
    dest[1] = xy + wz;
    dest[2] = xz - wy;
    dest[3] = 0;

    dest[4] = xy - wz;
    dest[5] = 1 - (xx + zz);
    dest[6] = yz + wx;
    dest[7] = 0;

    dest[8] = xz + wy;
    dest[9] = yz - wx;
    dest[10] = 1 - (xx + yy);
    dest[11] = 0;

    dest[12] = 0;
    dest[13] = 0;
    dest[14] = 0;
    dest[15] = 1;

    return dest;
}

quat_t quat_slerp(quat_t quat, quat_t quat2, numeric_t slerp, quat_t dest) {
    if (!dest) { dest = quat; }

    numeric_t cosHalfTheta = quat[0] * quat2[0] + quat[1] * quat2[1] + quat[2] * quat2[2] + quat[3] * quat2[3],
        halfTheta,
        sinHalfTheta,
        ratioA,
        ratioB;

    if (fabs(cosHalfTheta) >= 1.0) {
        if (dest != quat) {
            dest[0] = quat[0];
            dest[1] = quat[1];
            dest[2] = quat[2];
            dest[3] = quat[3];
        }
        return dest;
    }

    halfTheta = acos(cosHalfTheta);
    sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);

    if (fabs(sinHalfTheta) < 0.001) {
        dest[0] = (quat[0] * 0.5 + quat2[0] * 0.5);
        dest[1] = (quat[1] * 0.5 + quat2[1] * 0.5);
        dest[2] = (quat[2] * 0.5 + quat2[2] * 0.5);
        dest[3] = (quat[3] * 0.5 + quat2[3] * 0.5);
        return dest;
    }

    ratioA = sin((1 - slerp) * halfTheta) / sinHalfTheta;
    ratioB = sin(slerp * halfTheta) / sinHalfTheta;

    dest[0] = (quat[0] * ratioA + quat2[0] * ratioB);
    dest[1] = (quat[1] * ratioA + quat2[1] * ratioB);
    dest[2] = (quat[2] * ratioA + quat2[2] * ratioB);
    dest[3] = (quat[3] * ratioA + quat2[3] * ratioB);

    return dest;
}

quat_t quat_rotate(quat_t q, vec3_t p, quat_t dest) {

    if(!dest) {
        dest = q;
    }
    numeric_t t[4] = {p[0], p[1], p[2], 0}, r[4];

    quat_multiply(q, t, r);
    quat_multiply(r, quat_conjugate(q, t), dest);

    return dest;
}


void vec2_str(vec2_t vec, char *buffer) {
    sprintf(buffer, "[%f, %f]", vec[0], vec[1]);
}

void vec3_str(vec3_t vec, char *buffer) {
    sprintf(buffer, "[%f, %f, %f]", vec[0], vec[1], vec[2]);
}

void vec4_str(vec4_t vec, char *buffer) {
    sprintf(buffer, "[%f, %f, %f, %f]", vec[0], vec[1], vec[2], vec[3]);
}

void mat3_str(mat3_t mat, char *buffer) {
    sprintf(buffer, "[%f, %f, %f, %f, %f, %f, %f, %f, %f]", mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8]);
}

void mat4_str(mat4_t mat, char *buffer) {
    sprintf(buffer, "[%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f]",
        mat[0], mat[1], mat[2], mat[3],
        mat[4], mat[5], mat[6], mat[7],
        mat[8], mat[9], mat[10], mat[11],
        mat[12], mat[13], mat[14], mat[15]);
}

void quat_str(quat_t quat, char *buffer) {
    sprintf(buffer, "[%f, %f, %f, %f]", quat[0], quat[1], quat[2], quat[3]);
}
#endif /* GL_MATRIX_IMPLEMENTATION */
