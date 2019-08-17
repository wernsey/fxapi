#include <stdlib.h>
#include <math.h>

#include "gl-matrix.h"

vec2_t vec2_create(vec2_t vec) {
    vec2_t dest = calloc(2, sizeof(double));

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

vec2_t vec2_scale(vec2_t vec, double val, vec2_t dest) {
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

    double x = vec[0], y = vec[1],
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

double vec2_length(vec2_t vec) {
    double x = vec[0], y = vec[1];
    return sqrt(x * x + y * y);
}

double vec2_dot(vec2_t vec, vec2_t vec2) {
    return vec[0] * vec2[0] + vec[1] * vec2[1];
}

vec2_t vec2_direction (vec2_t vec, vec2_t vec2, vec2_t dest) {
    if (!dest) { dest = vec; }

    double x = vec[0] - vec2[0],
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

vec2_t vec2_lerp(vec2_t vec, vec2_t vec2, double lerp, vec2_t dest) {
    if (!dest) { dest = vec; }
    dest[0] = vec[0] + lerp * (vec2[0] - vec[0]);
    dest[1] = vec[1] + lerp * (vec2[1] - vec[1]);
    return dest;
}

double vec2_dist(vec2_t vec, vec2_t vec2) {
    double x = vec2[0] - vec[0],
        y = vec2[1] - vec[1];

    return sqrt(x*x + y*y);
}