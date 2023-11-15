#include "default.hpp"

void VADD32FLOAT_default(const float* op1, const float* op2, float* result, int32_t length)
{
    for ( int32_t i = 0 ; i < length ; i++ )
    {
        result[i] = op1[i] + op2[i];
    }
}

void VMUL32FLOAT_default(const float* op1, const float* op2, float* result, int32_t length)
{
    for ( int32_t i = 0 ; i < length ; i++ )
    {
        result[i] = op1[i] * op2[i];
    }
}

void VMUL32FLOAT_V_default(const float value, const float* vec, float* result, int32_t length)
{
    for ( int32_t i = 0 ; i < length ; i++ )
    {
        result[i] = value * vec[i];
    }
}

void VMADD32FLOAT_V_default(const float value, const float* vmul, const float* vadd, float* result, int32_t length)
{
    for ( int32_t i = 0 ; i < length ; i++ )
    {
        result[i] = (value * vmul[i]) + vadd[i];
    }
}

