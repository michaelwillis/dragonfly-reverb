#include "avx.hpp"

#include <immintrin.h>

int8_t AVXInitialized = 0;
int8_t FMASupported = 0;

void CheckFMASupport()
{
    if ( !AVXInitialized )
    {
        __builtin_cpu_init();
        if ( __builtin_cpu_supports("fma") )
        {
            FMASupported = 1;
        }

        AVXInitialized = 1;
    }
}

void VADD32FLOAT_avx(const float* op1, const float* op2, float* result, int32_t length)
{
    float extracted[8];
    int32_t processed = 0;
    
    while ( processed < length )
    {
        int32_t processing = ((length - processed) % 8) + 1;

        const __m256 mm_op1 = _mm256_loadu_ps(op1 + processed);
        const __m256 mm_op2 = _mm256_loadu_ps(op2 + processed );
        __m256 mm_res = _mm256_add_ps(mm_op1, mm_op2);
        
        _mm256_storeu_ps(extracted, mm_res);
        for ( int32_t i = 0 ; i < processing ; i++ )
        {
            result[processed + i] = extracted[i];
        }

        processed += processing;
    }
}

void VMUL32FLOAT_avx(const float* op1, const float* op2, float* result, int32_t length)
{
    float extracted[8];
    int32_t processed = 0;
 
    while ( processed < length )
    {
        int32_t processing = ((length - processed) % 8) + 1;
        const __m256 mm_op1 = _mm256_loadu_ps(op1 + processed);
        const __m256 mm_op2 = _mm256_loadu_ps(op2 + processed);
        __m256 mm_res = _mm256_mul_ps(mm_op1, mm_op2);
        
        _mm256_storeu_ps(extracted, mm_res);
        for ( int32_t i = 0 ; i < processing ; i++ )
        {
            result[processed + i] = extracted[i];
        }

        processed += processing;
    }
}

void VMUL32FLOAT_V_avx(const float value, const float* vec, float* result, int32_t length)
{    
    float extracted[8];
    const __m256 mm_op1 = _mm256_set1_ps(value);
        
    int32_t processed = 0;
    while ( processed < length )
    {
        int32_t processing = ((length - processed) % 8) + 1;

        const __m256 mm_op2 = _mm256_loadu_ps(vec + processed);
        __m256 mm_res = _mm256_mul_ps(mm_op1, mm_op2);
      
        _mm256_storeu_ps(extracted, mm_res);
        for ( int32_t i = 0 ; i < processing ; i++ )
        {
            result[processed + i] = extracted[i];
        }

        processed += processing;
    }
}

void VMADD32FLOAT_V_avx(const float value, const float* vmul, const float* vadd, float* result, int32_t length)
{
    float extracted[8];
    const __m256 mm_val = _mm256_set1_ps(value);

    CheckFMASupport();
        
    int32_t processed = 0;
    while ( processed < length )
    {
        int32_t processing = ((length - processed) % 8) + 1;

        const __m256 mm_mul = _mm256_loadu_ps(vmul + processed);
        const __m256 mm_add = _mm256_loadu_ps(vadd + processed);

        __m256 mm_res;
        if ( FMASupported )
        {
            mm_res = _mm256_fmadd_ps(mm_val, mm_mul, mm_add);
        }
        else
        {
            mm_res = _mm256_mul_ps(mm_val, mm_mul);
            mm_res = _mm256_add_ps(mm_res, mm_add);
        }
      
        _mm256_storeu_ps(extracted, mm_res);
        for ( int32_t i = 0 ; i < processing ; i++ )
        {
            result[processed + i] = extracted[i];
        }

        processed += processing;
    }
}
