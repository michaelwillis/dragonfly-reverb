#include "avx.hpp"

#include <immintrin.h>

void VSUM32FLOAT_avx(const float* op1, const float* op2, float* result, int32_t length)
{
    float extracted[8];
    int32_t processed = 0;
    
    while ( processed < length )
    {
        int processing = ((length - processed) % 8) + 1;

        const __m256 mm_op1 = _mm256_loadu_ps(op1 + processed);
        const __m256 mm_op2 = _mm256_loadu_ps(op2 + processed );
        __m256 mm_res = _mm256_add_ps(mm_op1, mm_op2);
        
        _mm256_storeu_ps(extracted, mm_res);
        for ( int i = 0 ; i < processing ; i++ )
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
        int processing = ((length - processed) % 8) + 1;
        const __m256 mm_op1 = _mm256_loadu_ps(op1 + processed);
        const __m256 mm_op2 = _mm256_loadu_ps(op2 + processed);
        __m256 mm_res = _mm256_mul_ps(mm_op1, mm_op2);
        
        _mm256_storeu_ps(extracted, mm_res);
        for ( int i = 0 ; i < processing ; i++ )
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
        
    int processed = 0;
    while ( processed < length )
    {
        int processing = ((length - processed) % 8) + 1;

        const __m256 mm_op2 = _mm256_loadu_ps(vec + processed);
        __m256 mm_res = _mm256_mul_ps(mm_op1, mm_op2);
      
        _mm256_storeu_ps(extracted, mm_res);
        for ( int i = 0 ; i < processing ; i++ )
        {
            result[processed + i] = extracted[i];
        }

        processed += processing;
    }
}

