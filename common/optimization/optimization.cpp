
#include "optimization.hpp"
#include <stdint.h>

#include "default.hpp"
#include "avx.hpp"

typedef struct 
{
    void (*VADD32FLOAT)(const float* op1, const float* op2, float* result, int32_t length);    
    void (*VMUL32FLOAT)(const float* op1, const float* op2, float* result, int32_t length);    
    void (*VMUL32FLOAT_V)(const float value, const float* vec, float* result, int32_t length);
    void (*VMADD32FLOAT_V)(const float value, const float* vmul, const float* vadd, float* result, int32_t length);
} _CpuOptimization;

int8_t OptimizationInitialized = 0;
_CpuOptimization CpuOptimization;

void SetupOptimization()
{
    __builtin_cpu_init();
#if defined(__x86_64__) 
    if ( __builtin_cpu_supports("avx") )
    {
        CpuOptimization.VADD32FLOAT = VADD32FLOAT_avx;
        CpuOptimization.VMUL32FLOAT = VMUL32FLOAT_avx;
        CpuOptimization.VMUL32FLOAT_V = VMUL32FLOAT_V_avx;
        CpuOptimization.VMADD32FLOAT_V = VMADD32FLOAT_V_avx;
    }
    else
#endif
    {
        CpuOptimization.VADD32FLOAT = VADD32FLOAT_default;
        CpuOptimization.VMUL32FLOAT = VMUL32FLOAT_default;
        CpuOptimization.VMUL32FLOAT_V = VMUL32FLOAT_V_default;
        CpuOptimization.VMADD32FLOAT_V = VMADD32FLOAT_V_default;
    }
    OptimizationInitialized = 1;
}


void VADD32FLOAT(const float* op1, const float* op2, float* result, int32_t length)
{
    if (!OptimizationInitialized)
    {
        SetupOptimization();
    }

    CpuOptimization.VADD32FLOAT(op1,op2,result,length);
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

void VMADD32FLOAT_V(const float value, const float* vmul, const float* vadd, float* result, int32_t length)
{
    if (!OptimizationInitialized)
    {
        SetupOptimization();
    }

    CpuOptimization.VMADD32FLOAT_V(value,vmul,vadd,result,length);
}