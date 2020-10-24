/* 
 * Authors: Robert Lester, Craig Cagner
 * Filename: bitmap.h
 * Assignment: Arith
 * Summary: Functions in this file help map through the UArray2 of codewords,
 *          either packing them via bitpack.h from the a, b, c, d, avg pb, 
 *          and avg pr values in our array of dctrans structs, or unpacking
 *          the codewords and transforming them back into dctrans structs via
 *          bitpack.h
 * Invariants: 
 *              
 */

#ifndef BITMAP_INCLUDED
#define BITMAP_INCLUDED

#include "bitpack.h"
#include "uarray2.h"
#include "quantization.h"
#include "assert.h"
#include <malloc.h>
#include "a2methods.h"
#include <stdint.h>

/*
 * bitmap_pack
 *
 * packs each discrete cosine element from the given 2D array into a 32 bit 
 * codeword according to the desired specifications
 * 
 * assumes the arguments are not NULL
 */
A2Methods_UArray2 bitmap_pack(A2Methods_T methods, A2Methods_UArray2 array2);

/*
 * bitmap_unpack
 *
 * obtains each individual value in the compressed codeword separately and
 * returns a 2D array of containing the discrete cosine transformation elements
 * 
 * assumes the arguments are not NULL
 */
A2Methods_UArray2 bitmap_unpack(A2Methods_T methods, A2Methods_UArray2 array2);

#endif