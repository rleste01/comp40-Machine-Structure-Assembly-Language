/*
 * Filename: compress40.c
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Does      : Holds functions compress and decompress; compress will utilize
 *             functions from all of our helper files in order to compress an
 *             image; decompress will utilize functions from all of our 
 *             helper files in order to decompress an image
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "compress40.h"
#include "bitmap.h"
#include "ppm_reader.h"
#include "quantization.h"
#include "rgb_ypp.h"
#include "ypp_dct.h"
#include "read_bitfile.h"

#define DENOMINATOR 255 /* ppm denominator */

A2Methods_UArray2 codeword_info_array (A2Methods_T methods); 

/* 
 * compress40(FILE *inputfp)
 * 
 * Parameters: FILE *fp: input file, ppm image
 * Returns   : None
 * Does      : compresses an image, and writes the compressed image
 */ 
extern void compress40 (FILE *inputfp)
{
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    Pnm_ppm rgb_rep = read_ppm(inputfp, methods);
    A2Methods_UArray2 ypp_rep = rgb_to_ypp(rgb_rep -> pixels, 
                                           methods,
                                           rgb_rep -> denominator);
    A2Methods_UArray2 dct_rep = ypp_to_dct(ypp_rep, methods);
    quantize_c(dct_rep, methods);   
    A2Methods_UArray2 word_map = bitmap_pack(methods, dct_rep);//, codeword_info);
    write_bitfile(methods, word_map);

    methods -> free(&ypp_rep);
    methods -> free(&dct_rep);
    methods -> free(&word_map);
    Pnm_ppmfree(&rgb_rep);
}


/* 
 * decompress40(FILE *inputfp)
 * 
 * Parameters: FILE *fp: input file, ppm image
 * Returns   : None
 * Does      : compresses an image, and writes the decompressed image
 * 
 */ 
extern void decompress40(FILE *inputfp)
{
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    A2Methods_UArray2 word_map = read_bitfile(inputfp, methods);
    A2Methods_UArray2 dct_rep2 = bitmap_unpack(methods, word_map);//, 
                                                        //codeword_info);
    quantize_d(dct_rep2, methods);
    A2Methods_UArray2 ypp_rep2 = dct_to_ypp(dct_rep2, methods);

    Pnm_ppm pixmap = malloc(sizeof(struct Pnm_ppm));
    assert(pixmap != NULL);
    pixmap -> width = methods -> width(ypp_rep2);
    pixmap -> height = methods -> height(ypp_rep2);
    pixmap -> denominator = DENOMINATOR;
    pixmap -> methods = methods;
    pixmap -> pixels = ypp_to_rgb(ypp_rep2, methods);
    
    write_ppm(pixmap);

    methods -> free(&dct_rep2);
    methods -> free(&word_map);
    methods -> free(&ypp_rep2);
    Pnm_ppmfree(&pixmap);
}

/*
 * codeword_info_array (A2Methods_T methods)
 * 
 * Parameters: A2Methods_T methods: method suite to manipulate 2D arrays
 * Returns   : A2Methods_UArray 2 containing information about width and
 *             least significant bit values for compressed codeword
 * Does      : Creates and returns an array containing information about the
 *             compressed codeword specifications
 */
// A2Methods_UArray2 codeword_info_array (A2Methods_T methods) 
// {
//     A2Methods_UArray2 codeword_info = methods -> new(6, 2, sizeof(int));

//     int *info_holder = (int *) methods -> at(codeword_info, 0, 1);
//     *info_holder = LSB_A;
//     info_holder = (int *) methods -> at(codeword_info, 1, 1);
//     *info_holder = LSB_B;
//     info_holder = (int *) methods -> at(codeword_info, 2, 1);
//     *info_holder = LSB_C;
//     info_holder = (int *) methods -> at(codeword_info, 3, 1);
//     *info_holder = LSB_D;
//     info_holder = (int *) methods -> at(codeword_info, 4, 1);
//     *info_holder = LSB_PB;
//     info_holder = (int *) methods -> at(codeword_info, 5, 1);
//     *info_holder = LSB_PR;
//     info_holder = (int *) methods -> at(codeword_info, 0, 0);
//     *info_holder = W_A;
//     info_holder = (int *) methods -> at(codeword_info, 1, 0);
//     *info_holder = W_BCD;
//     info_holder = (int *) methods -> at(codeword_info, 2, 0);
//     *info_holder = W_BCD;
//     info_holder = (int *) methods -> at(codeword_info, 3, 0);
//     *info_holder = W_BCD;
//     info_holder = (int *) methods -> at(codeword_info, 4, 0);
//     *info_holder = W_PBPR;
//     info_holder = (int *) methods -> at(codeword_info, 5, 0);
//     *info_holder = W_PBPR;

//     return codeword_info;
// }
