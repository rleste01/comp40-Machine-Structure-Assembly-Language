#ifndef QUANTIZATION_INCLUDED
#define QUANTIZATION_INCLUDED
/*
 * Authors: Robert Lester, Craig Cagner
 * Filename: quantization.h
 * Assignment: Arith
 * Summary: The functions in this file perform the necessary operations to
 *          quantize the a, b, c, d, pb, and pr values into scaled integers
 *          of the correct size for bitpacking
 * Invariants: 
 *
 */

#include <stdlib.h>
#include <math.h>
#include "arith40.h"
#include "pnm.h"
#include "assert.h"

/*
 * quantize_c
 * 
 * scales the discrete cosine values in the given array2 so the 
 * values are represented as both signed and unsigned integers
 * 
 * assumes the arguments are not NULL
 */
void quantize_c (A2Methods_UArray2 array2, A2Methods_T methods);

/*
 * quantize_d
 * 
 * scales the discrete cosine values in the given array2 so the 
 * values are represented as floating point values
 * 
 * assumes the arguments are not NULL
 */
void quantize_d (A2Methods_UArray2 array2, A2Methods_T methods);

#endif