/* 
 * Authors: Robert Lester, Craig Cagner
 * Filename: rgb_ypp.h
 * Assignment: Arith
 * Summary: In compression, transforms an array of Pnm_rgb structs to an array
 *          of component_video structs. In decompression, transforms an array 
 *          of component_video structs to an array of Pnm_rgb structs.
 */

#ifndef RGB_TO_YPP_INCLUDED
#define RGB_TO_YPP_INCLUDED

#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include <malloc.h>

/*
 * rgb_to_ypp
 * 
 * returns a 2D array of component video elements whose values are equivalent
 * to those of the given 2D array of RGB values
 * 
 * assumes the first 2 arguments are not NULL
 */
A2Methods_UArray2 rgb_to_ypp (A2Methods_UArray2 array2, A2Methods_T methods,
                                                        unsigned denominator);

/* 
 * ypp_to_rgb
 * 
 * returns a 2D array of Pnm_rgb elements whose values are equivalent to those
 * of the given 2D array of component video elements
 * 
 * assumes the arguments are not NULL
 */
A2Methods_UArray2 ypp_to_rgb (A2Methods_UArray2 array2, A2Methods_T methods);

#endif