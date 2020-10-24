/*
 * Authors: Robert Lester, Craig Cagner
 * Filename: ypp_dct.h
 * Assignment: Arith
 * Summary: The functions in this file perform the necessary operations to
 *          transform the y, pb, and pr values from our component_video struct
 *          at each point in the array of those component_video structs to 
 *          dctrans structs which hold the a, b, c, d, average pb, and average
 *          pr values for 2-by-2 blocks in the array of component_video structs
 */


#ifndef YPP_DCT_INCLUDED
#define YPP_DCT_INCLUDED

#include "a2methods.h"
#include "assert.h"
#include "a2plain.h"
#include <string.h>
#include <malloc.h>

/*
 * ypp_to_dct
 * 
 * returns a discrete cosine transformation 2D array representation of the
 * image represented by the given 2D array of component video elements
 * 
 * assumes the arguments are not NULL
 */
A2Methods_UArray2 ypp_to_dct (A2Methods_UArray2 array2, A2Methods_T methods);

/*
 * dct_to_ypp
 * 
 * returns a component video 2D array representation of the
 * image represented by the given 2D array of discrete cosine
 * transformation elements
 * 
 * assumes the arguments are not NULL
 */
A2Methods_UArray2 dct_to_ypp (A2Methods_UArray2 array2, A2Methods_T methods);

#endif