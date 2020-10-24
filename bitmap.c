/* 
 * Filename: bitmap.c
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : Functions in this file help map through the UArray2 of
 *             codewords, either packing them via bitpack.h from the 
 *             a, b, c, d, avg pb, and avg pr values in our array of
 *             dctrans structs, or unpacking the codewords and transforming
 *             them back into dctrans structs via bitpack.h
 */


#include "bitmap.h"

#define SIZE 64
#define SIGNED_T int64_t
#define UNSIGNED_T uint64_t

#define LSB_A 26 /* least significant bit values for codeword */
#define LSB_B 20
#define LSB_C 14
#define LSB_D 8
#define LSB_PB 4
#define LSB_PR 0

#define W_A 6 /* width values for codeword */
#define W_BCD 6
#define W_PBPR 4

/* struct containing discrete cosine information */
typedef struct dctrans {
             
    float avgpb,
          avgpr,
          a,
          b,
          c,
          d;

} *dctrans;

/* closure struct containing a 2D array representation of the image
 * and the proper method suite */
typedef struct closure_struct {

    A2Methods_UArray2 array2;
    A2Methods_T methods;

} *closure_struct;

void pack_words(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
                void *cl);
void unpack_words(int i, int j, A2Methods_UArray2 array2, 
                  A2Methods_Object *ptr, void *cl);

/*
 * bitmap_pack (A2Methods_T methods, A2Methods_UArray2 array2)
 * 
 * Parameters: A2Methods_UArray2 array2: array of dctrans structs
 *             A2Methods_T methods: methods for UArray2
 * Returns   : A2Methods_UArray2: array of codewords
 * Does      : maps through an array of scaled dctrans structs, uses functions 
 *             from bitpack.h to pack a codeword for each, and returns an
 *             array of those codewords
 */
A2Methods_UArray2 bitmap_pack (A2Methods_T methods, A2Methods_UArray2 array2)
{
    assert(array2 != NULL);
    assert(methods != NULL);
    A2Methods_UArray2 word_map = methods -> new(methods -> width(array2), 
                                                methods -> height(array2),
                                                sizeof(UNSIGNED_T));
    closure_struct cl = malloc(sizeof(*cl));
    assert(cl != NULL);
    cl -> array2 = word_map;
    cl -> methods = methods;

    methods -> map_row_major(array2, pack_words, cl);

    free(cl);

    return word_map;
}

/*
 * pack_words (int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr, 
 *                                                     void *cl)
 * Parameters: int i: index of column
 *             int j: index of row
 *             A2Methods_UArray2 array2: array of dctrans structs
 *             A2Methods_Object *ptr: the dctrans struct at the 
 *                                    current index
 *             void *cl: closure struct holding the array of codewords
 *                       and the methods for that array
 * Returns   : None
 * Does      : apply function called in map_row_major which calls functions
 *             from bitpack.h to do the actual packing of each codeword from
 *             a dctrans struct 
 */
void pack_words(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr, 
                                                        void *cl)
{
    (void) array2;

    closure_struct cl_struct = (closure_struct) cl;
    dctrans dct = (dctrans) ptr;

    UNSIGNED_T *word = (UNSIGNED_T *)(cl_struct -> methods -> 
                                                   at(cl_struct -> array2, 
                                                      i, 
                                                      j));
    /* pack the word with each separate value at its proper location */
    *word = Bitpack_newu(*word, W_A, LSB_A, dct -> a);
    *word = Bitpack_news(*word, W_BCD, LSB_B, dct -> b);
    *word = Bitpack_news(*word, W_BCD, LSB_C, dct -> c);
    *word = Bitpack_news(*word, W_BCD, LSB_D, dct -> d);
    *word = Bitpack_newu(*word, W_PBPR, LSB_PB, dct -> avgpb);
    *word = Bitpack_newu(*word, W_PBPR, LSB_PR, dct -> avgpr);
}

/*
 * bitmap_unpack (A2Methods_UArray2 methods, A2Methods_T array2)
 * 
 * Parameters: A2Methods_UArray2 array2: array of codewords
 *             A2Methods_T methods: methods for UArray2
 * Returns   : A2Methods_UArray2: array of scaled dctrans structs
 * Does      : maps through an array of codewords, uses functions from
 *             bitpack.h to get the values for a, b, c, d, avg pb, and avg 
 *             pr, from each codeword, and returns an array of those
 *             dctrans structs
 */
A2Methods_UArray2 bitmap_unpack(A2Methods_T methods, A2Methods_UArray2 array2)
{
    assert(array2 != NULL);
    assert(methods != NULL);
    A2Methods_UArray2 dct_rep = methods -> new(methods -> width(array2), 
                                               methods -> height(array2),
                                               sizeof(struct dctrans));
    closure_struct cl = malloc(sizeof(*cl));
    assert(cl != NULL);
    cl -> array2 = dct_rep;
    cl -> methods = methods;

    methods -> map_row_major(array2, unpack_words, cl);

    free(cl);

    return dct_rep;
}

/*
 * unpack_words(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr, 
 *                                                      void *cl)
 * 
 * Parameters: int i: index of column
 *             int j: index of row
 *             A2Methods_UArray2 array2: array of codewords
 *             A2Methods_Object *ptr: the codeword at the current index
 *             void *cl: closure struct holding the array of dctrans structs
 *                       and the methods for that array
 * Returns   : None
 * Does      : apply function called in map_row_major which calls functions
 *             from bitpack.h to do the actual unpacking of each codeword and
 *             setting a, b, c, d, avg pb, and avg pr to the corresponding
 *             values at each field in the codeword
 */
void unpack_words(int i, int j, A2Methods_UArray2 array2, 
                  A2Methods_Object *ptr, void *cl)
{
    (void) array2;

    closure_struct cl_struct = (closure_struct) cl;
    UNSIGNED_T *word = (UNSIGNED_T *) ptr;

    dctrans dct = (dctrans)(cl_struct -> methods -> at(cl_struct -> array2, 
                                                              i, j));
    /* extract each value separately from the compressed codeword */
    dct -> a = Bitpack_getu(*word, W_A, LSB_A);
    dct -> b = Bitpack_gets(*word, W_BCD, LSB_B);
    dct -> c = Bitpack_gets(*word, W_BCD, LSB_C);
    dct -> d = Bitpack_gets(*word, W_BCD, LSB_D);
    dct -> avgpb = Bitpack_getu(*word, W_PBPR, LSB_PB);
    dct -> avgpr = Bitpack_getu(*word, W_PBPR, LSB_PR);                                             
}