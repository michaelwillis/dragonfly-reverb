
#include "optimization.hpp"
#include <stdint.h>

#include "default.hpp"
#include "avx.hpp"

typedef struct 
{
    void (*VSUM32FLOAT)(const float* op1, const float* op2, float* result, int32_t length);    
    void (*VMUL32FLOAT)(const float* op1, const float* op2, float* result, int32_t length);    
    void (*VMUL32FLOAT_V)(const float value, const float* vec, float* result, int32_t length);
} _CpuOptimization;

int8_t OptimizationInitialized = 0;
_CpuOptimization CpuOptimization;

void SetupOptimization()
{
    __builtin_cpu_init();
    if ( __builtin_cpu_supports("avx") )
    {
        CpuOptimization.VSUM32FLOAT = VSUM32FLOAT_avx;
        CpuOptimization.VMUL32FLOAT = VMUL32FLOAT_avx;
        CpuOptimization.VMUL32FLOAT_V = VMUL32FLOAT_V_avx;
    }
    else
    {
        CpuOptimization.VSUM32FLOAT = VSUM32FLOAT_default;
        CpuOptimization.VMUL32FLOAT = VMUL32FLOAT_default;
        CpuOptimization.VMUL32FLOAT_V = VMUL32FLOAT_V_default;
    }
    OptimizationInitialized = 1;
}


void VSUM32FLOAT(const float* op1, const float* op2, float* result, int32_t length)
{
    if (!OptimizationInitialized)
    {
        SetupOptimization();
    }

    CpuOptimization.VSUM32FLOAT(op1,op2,result,length);
}

void VMUL32FLOAT(const float* op1, const float* op2, float* result, int32_t length)
{
    if (!OptimizationInitialized)
    {
        SetupOptimization();
    }

    CpuOptimization.VMUL32FLOAT(op1,op2,result,length);
}

void VMUL32FLOAT_V(const float value, const float* vec, float* result, int32_t length)
{
    if (!OptimizationInitialized)
    {
        SetupOptimization();
    }

    CpuOptimization.VMUL32FLOAT_V(value,vec,result,length);
}