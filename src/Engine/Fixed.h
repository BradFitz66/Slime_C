#pragma once
#include <stdint.h>
#include <stdio.h>

// NDS-style fixed-point: 20 integer bits, 12 fractional bits (20.12)
typedef int32_t fx32;

#define FX32_SHIFT        12
#define FX32_ONE          (1 << FX32_SHIFT)  // 4096 == 1.0

#define INT_TO_FX32(x)    ((fx32)((x) * FX32_ONE))
#define FX32_TO_INT(x)    ((int)((x) >> FX32_SHIFT))
#define FLOAT_TO_FX32(x)  ((fx32)((x) * (float)FX32_ONE))
#define FX32_TO_FLOAT(x)  ((float)(x) / (float)FX32_ONE)
// Converts fx32 to a decimal string "[-]int.frac" written into buf.
// buf must be at least 16 bytes.
#define FX32_TO_STRING(buf, x) do { \
    fx32 _v = (x); \
    int _neg = (_v < 0); \
    if (_neg) _v = -_v; \
    int _i = (int)(_v >> FX32_SHIFT); \
    int _f = (int)(((_v & (FX32_ONE - 1)) * 1000) >> FX32_SHIFT); \
    sprintf((buf), "%s%d.%03d", _neg ? "-" : "", _i, _f); \
} while(0)


// Multiply two fx32 values: (a * b) keeping 20.12 scale
#define FX32_MUL(a, b)    ((fx32)(((int64_t)(a) * (b)) >> FX32_SHIFT))
// Divide two fx32 values: (a / b) keeping 20.12 scale
#define FX32_DIV(a, b)    ((fx32)(((int64_t)(a) << FX32_SHIFT) / (b)))
// Simple fixed-point square root using float math for simplicity
#define FX32_SQRT(x)     ((fx32)(sqrtf(FX32_TO_FLOAT(x)) * (float)FX32_ONE))
