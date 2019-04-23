/*
 * Free FFT and convolution (C)
 *
 * Copyright (c) 2017 Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/free-small-fft-in-multiple-languages
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fft.h"


// Private function prototypes
static size_t reverse_bits(size_t x, int n);
static void *memdup(const void *src, size_t n);


bool Fft_transform(double real[], double imag[], size_t n) {
	if (n == 0)
		return true;
	else if ((n & (n - 1)) == 0)  // Is power of 2
		return Fft_transformRadix2(real, imag, n);
	else  // More complicated algorithm for arbitrary sizes
		return Fft_transformBluestein(real, imag, n);
}


bool Fft_inverseTransform(double real[], double imag[], size_t n) {
	return Fft_transform(imag, real, n);
}


bool Fft_transformRadix2(double real[], double imag[], size_t n) {
	// Length variables
	bool status = false;
	int levels = 0;  // Compute levels = floor(log2(n))
	for (size_t temp = n; temp > 1U; temp >>= 1)
		levels++;
	if ((size_t)1U << levels != n)
		return false;  // n is not a power of 2

	// Trignometric tables
	if (SIZE_MAX / sizeof(double) < n / 2)
		return false;
	size_t size = (n / 2) * sizeof(double);
	double *cos_table = malloc(size);
	double *sin_table = malloc(size);
	if (cos_table == NULL || sin_table == NULL)
		goto cleanup;
	for (size_t i = 0; i < n / 2; i++) {
		cos_table[i] = cos(2 * M_PI * i / n);
		sin_table[i] = sin(2 * M_PI * i / n);
	}

	// Bit-reversed addressing permutation
	for (size_t i = 0; i < n; i++) {
		size_t j = reverse_bits(i, levels);
		if (j > i) {
			double temp = real[i];
			real[i] = real[j];
			real[j] = temp;
			temp = imag[i];
			imag[i] = imag[j];
			imag[j] = temp;
		}
	}

	// Cooley-Tukey decimation-in-time radix-2 FFT
	for (size_t size = 2; size <= n; size *= 2) {
		size_t halfsize = size / 2;
		size_t tablestep = n / size;
		for (size_t i = 0; i < n; i += size) {
			for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
				size_t l = j + halfsize;
				double tpre =  real[l] * cos_table[k] + imag[l] * sin_table[k];
				double tpim = -real[l] * sin_table[k] + imag[l] * cos_table[k];
				real[l] = real[j] - tpre;
				imag[l] = imag[j] - tpim;
				real[j] += tpre;
				imag[j] += tpim;
			}
		}
		if (size == n)  // Prevent overflow in 'size *= 2'
			break;
	}
	status = true;

cleanup:
	free(cos_table);
	free(sin_table);
	return status;
}


bool Fft_transformBluestein(double real[], double imag[], size_t n) {
	bool status = false;

	// Find a power-of-2 convolution length m such that m >= n * 2 + 1
	size_t m = 1;
	while (m / 2 <= n) {
		if (m > SIZE_MAX / 2)
			return false;
		m *= 2;
	}

	// Allocate memory
	if (SIZE_MAX / sizeof(double) < n || SIZE_MAX / sizeof(double) < m)
		return false;
	size_t size_n = n * sizeof(double);
	size_t size_m = m * sizeof(double);
	double *cos_table = malloc(size_n);
	double *sin_table = malloc(size_n);
	double *areal = calloc(m, sizeof(double));
	double *aimag = calloc(m, sizeof(double));
	double *breal = calloc(m, sizeof(double));
	double *bimag = calloc(m, sizeof(double));
	double *creal = malloc(size_m);
	double *cimag = malloc(size_m);
	if (cos_table == NULL || sin_table == NULL
			|| areal == NULL || aimag == NULL
			|| breal == NULL || bimag == NULL
			|| creal == NULL || cimag == NULL)
		goto cleanup;

	// Trignometric tables
	for (size_t i = 0; i < n; i++) {
		unsigned long long temp = (unsigned long long)i * i;
		temp %= (unsigned long long)n * 2;
		double angle = M_PI * temp / n;
		// Less accurate version if long long is unavailable: double angle = M_PI * i * i / n;
		cos_table[i] = cos(angle);
		sin_table[i] = sin(angle);
	}

	// Temporary vectors and preprocessing
	for (size_t i = 0; i < n; i++) {
		areal[i] =  real[i] * cos_table[i] + imag[i] * sin_table[i];
		aimag[i] = -real[i] * sin_table[i] + imag[i] * cos_table[i];
	}
	breal[0] = cos_table[0];
	bimag[0] = sin_table[0];
	for (size_t i = 1; i < n; i++) {
		breal[i] = breal[m - i] = cos_table[i];
		bimag[i] = bimag[m - i] = sin_table[i];
	}

	// Convolution
	if (!Fft_convolveComplex(areal, aimag, breal, bimag, creal, cimag, m))
		goto cleanup;

	// Postprocessing
	for (size_t i = 0; i < n; i++) {
		real[i] =  creal[i] * cos_table[i] + cimag[i] * sin_table[i];
		imag[i] = -creal[i] * sin_table[i] + cimag[i] * cos_table[i];
	}
	status = true;

	// Deallocation
cleanup:
	free(cimag);
	free(creal);
	free(bimag);
	free(breal);
	free(aimag);
	free(areal);
	free(sin_table);
	free(cos_table);
	return status;
}


bool Fft_convolveReal(const double x[], const double y[], double out[], size_t n) {
	bool status = false;
	double *ximag = calloc(n, sizeof(double));
	double *yimag = calloc(n, sizeof(double));
	double *zimag = calloc(n, sizeof(double));
	if (ximag == NULL || yimag == NULL || zimag == NULL)
		goto cleanup;

	status = Fft_convolveComplex(x, ximag, y, yimag, out, zimag, n);
cleanup:
	free(zimag);
	free(yimag);
	free(ximag);
	return status;
}


bool Fft_convolveComplex(
		const double xreal[], const double ximag[],
		const double yreal[], const double yimag[],
		double outreal[], double outimag[], size_t n) {

	bool status = false;
	if (SIZE_MAX / sizeof(double) < n)
		return false;
	size_t size = n * sizeof(double);

	double *xr = memdup(xreal, size);
	double *xi = memdup(ximag, size);
	double *yr = memdup(yreal, size);
	double *yi = memdup(yimag, size);
	if (xr == NULL || xi == NULL || yr == NULL || yi == NULL)
		goto cleanup;

	if (!Fft_transform(xr, xi, n))
		goto cleanup;
	if (!Fft_transform(yr, yi, n))
		goto cleanup;

	for (size_t i = 0; i < n; i++) {
		double temp = xr[i] * yr[i] - xi[i] * yi[i];
		xi[i] = xi[i] * yr[i] + xr[i] * yi[i];
		xr[i] = temp;
	}
	if (!Fft_inverseTransform(xr, xi, n))
		goto cleanup;

	for (size_t i = 0; i < n; i++) {  // Scaling (because this FFT implementation omits it)
		outreal[i] = xr[i] / n;
		outimag[i] = xi[i] / n;
	}
	status = true;

cleanup:
	free(yi);
	free(yr);
	free(xi);
	free(xr);
	return status;
}


static size_t reverse_bits(size_t x, int n) {
	size_t result = 0;
	for (int i = 0; i < n; i++, x >>= 1)
		result = (result << 1) | (x & 1U);
	return result;
}


static void *memdup(const void *src, size_t n) {
	void *dest = malloc(n);
	if (n > 0 && dest != NULL)
		memcpy(dest, src, n);
	return dest;
}
