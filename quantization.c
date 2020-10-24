/*
 * Filename  : quantization.c
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : The functions in this file perform the necessary operations to
 *             quantize the a, b, c, d, pb, and pr values into scaled integers
 *             of the correct size for bitpacking
 */

#include "quantization.h"

#define fit_bit_a 63.0 /* turns into unsigned 9 bit */
#define fit_bit_bcd 100.0 /* turns into signed 5 bit */

/* struct containing discrete cosine information */
typedef struct dctrans {
             
    float avgpb,
          avgpr,
          a,
          b,
          c,
          d;

} *dctrans;

void check_positive (dctrans dct);
void check_negative (dctrans dct);
void floats_to_ints (dctrans dct);
void ints_to_floats (dctrans dct);
void perform_quantization (int i, int j, A2Methods_UArray2 array2,
                                         A2Methods_Object *ptr, 
                                         void *cl);
void reverse_quantization (int i, int j, A2Methods_UArray2 array2,
                                         A2Methods_Object *ptr, 
                                         void *cl);

/* 
 * quantize_c (A2Methods_UArray2 array2, A2Methods_T methods)
 * 
 * Parameters: A2Methods_UArray2 array2: array of dctrans structs
 *             A2Methods_T methods: methods for UArray
 * Returns   : None
 * Does      : Maps through the array, performing the necessary quantization on
 *             a, b, c, d, pb, and pr, in order to turn them into the necessary
 *             scaled values
 */ 
void quantize_c (A2Methods_UArray2 array2, A2Methods_T methods) 
{
    assert(array2 != NULL);
    assert(methods != NULL);
    methods -> map_row_major(array2, perform_quantization, methods);
}

/* 
 * quantize_d (A2Methods_UArray2 array2, A2Methods_T methods)
 * 
 * Parameters: A2Methods_UArray2 array2: array of dctrans structs
 *             A2Methods_T methods: methods for UArray2
 * Returns   : None
 * Does      : Maps through the array, performing the necessary de-quantization
 *             on scaled values a, b, c, d, pb, and pr, in order to turn them
 *             into the necessary decimal values
 */ 
void quantize_d (A2Methods_UArray2 array2, A2Methods_T methods) 
{    
    assert(array2 != NULL);
    assert(methods != NULL);
    methods -> map_row_major(array2, reverse_quantization, methods);
}

/* 
 * perform_quantization (int i, int j, A2Methods_UArray2 array2, 
 *                                     A2Methods_Object *ptr, 
 *                                     void *cl)
 * 
 * Parameters: int i: index of column
 *             int j: index of row
 *             A2Methods_UArray2 array2: array of dctrans structs
 *             A2Methods_Object *ptr: the dctrans struct at the current index
 *             void *cl: closure struct holding the array of dctrans
 *                       structs and the methods for that array
 * Returns   : None
 * Does      : apply function which calls helper functions to quantize the a,
 *             b, c, d, pb, and pr values to scaled values in their respective
 *             ranges
 */ 
void perform_quantization (int i, int j, A2Methods_UArray2 array2,
                                         A2Methods_Object *ptr, 
                                         void *cl) 
{
    (void) cl;
    (void) i;
    (void) j;
    (void) array2;
    
    dctrans dct = (dctrans) ptr;

    check_positive(dct);
    check_negative(dct);
    floats_to_ints(dct);
}

/* 
 * reverse_quantization (int i, int j, A2Methods_UArray2 array2, 
 *                                     A2Methods_Object *ptr, 
 *                                     void *cl)
 * 
 * Parameters: int i: index of column
 *             int j: index of row
 *             A2Methods_UArray2 array2: array of scaled dctrans structs
 *             A2Methods_Object *ptr: the dctrans struct at the current index
 *             void *cl: closure struct holding the array of dctrans
 *                        structs and the methods for that array
 * Returns   : None
 * Does      : apply function which calls helper functions to de-quantize the
 *             a, b, c, d, pb, and pr values to decimal float values in their
 *             respective ranges
 */ 
void reverse_quantization (int i, int j, A2Methods_UArray2 array2,
                                         A2Methods_Object *ptr, 
                                         void *cl) 
{
    (void) cl;
    (void) i;
    (void) j;
    (void) array2;

    dctrans dct = (dctrans) ptr;

    ints_to_floats(dct);
    check_positive(dct);
    check_negative(dct);
}


/* 
 * check_positive (dctrans dct)
 * 
 * Parameters: dctrans dct: dctrans struct at the current index in array
 * Returns   : None
 * Does      : Makes sure that there are no b, c, or d values above 0.3
 */
void check_positive (dctrans dct) 
{
    if (dct -> b > 0.3) {
        dct -> b = 0.3;
    }
    if (dct -> c > 0.3) {
        dct -> c = 0.3;
    }
    if (dct -> d > 0.3) {
        dct -> d = 0.3;
    }
}

/* 
 * check_negative (dctrans dct)
 * 
 * Parameters: dctrans dct: dctrans struct at the current index in array
 * Returns   : None
 * Does      : Makes sure that there are no b, c, or d values below -0.3
 */
void check_negative (dctrans dct) 
{
    if (dct -> b < -0.3) {
        dct -> b = -0.3;
    }
    if (dct -> c < -0.3) {
        dct -> c = -0.3;
    }
    if (dct -> d < -0.3) {
        dct -> d = -0.3;
    }
}


/* 
 * floats_to_ints (dctrans dct)
 * 
 * Parameters: dctrans dct: dctrans struct at the current index in array
 * Returns   : None
 * Does      : performs the actual quantization of a, b, c, and d values,
 *             transforming a into a 9-bit scaled integer, b, c, and d into
 *             5-bit scaled integers, and calling Arith_40_index_of_chroma
 *             on the pb and pr values to get 4-bit scaled integers.
 */
void floats_to_ints (dctrans dct) 
{   
    dct -> a = round((dct -> a) * fit_bit_a);
    dct -> b = round((dct -> b) * fit_bit_bcd);
    dct -> c = round((dct -> c) * fit_bit_bcd);
    dct -> d = round((dct -> d) * fit_bit_bcd);
    dct -> avgpb = Arith40_index_of_chroma(dct -> avgpb);
    dct -> avgpr = Arith40_index_of_chroma(dct -> avgpr);
}


/* 
 * floats_to_ints (dctrans dct)
 * 
 * Parameters: dctrans dct: dctrans struct at the current index in array
 * Returns   : None
 * Does      : performs the reverse of floats_to_ints, transforming the scaled
 *             integer values back into unscaled decimals
 */
void ints_to_floats (dctrans dct) 
{
    dct -> a = (dct -> a) / fit_bit_a;
    dct -> b = (dct -> b) / fit_bit_bcd;
    dct -> c = (dct -> c) / fit_bit_bcd;
    dct -> d = (dct -> d) / fit_bit_bcd;
    dct -> avgpb = Arith40_chroma_of_index(dct -> avgpb);
    dct -> avgpr = Arith40_chroma_of_index(dct -> avgpr);
}