
#ifndef OPTIMIZATION_DEFAULT_H_INCLUDED
#define OPTIMIZATION_DEFAULT_H_INCLUDED

#include <stdint.h>

void VSUM32FLOAT_default(const float* op1, const float* op2, float* result, int32_t length);

void VMUL32FLOAT_default(const float* op1, const float* op2, float* result, int32_t length);

void VMUL32FLOAT_V_default(const float value, const float* vec, float* result, int32_t length);


#endif