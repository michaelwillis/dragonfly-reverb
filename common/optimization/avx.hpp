
#ifndef OPTIMIZATION_AVX_H_INCLUDED
#define OPTIMIZATION_AVX_H_INCLUDED

#include <stdint.h>

void VSUM32FLOAT_avx(const float* op1, const float* op2, float* result, int32_t length);

void VMUL32FLOAT_avx(const float* op1, const float* op2, float* result, int32_t length);

void VMUL32FLOAT_V_avx(const float value, const float* vec, float* result, int32_t length);

#endif