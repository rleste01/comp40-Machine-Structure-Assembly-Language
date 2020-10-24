/*
 * Filename  : read_bitfile.c
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : Implementation of the read_bitfile.h interface
 */

#include "read_bitfile.h"

#define PRINT_TYPE uint32_t
#define SIZE 64
#define SIGNED_T int64_t
#define UNSIGNED_T uint64_t

/* exception to be raised when not enough codewords are read */
Except_T NO_CODEWORDS_LEFT = { "Not enough words to complete the image" };

/* closure struct containing a uarray2 and the methods for a uarray2 */
typedef struct closure_struct {

    FILE *fp;
    A2Methods_T methods;

} *closure_struct;

void print_codewords (int i, int j, A2Methods_UArray2 array2,
                                 A2Methods_Object *ptr, 
                                 void *cl);
void populate_word_array (int i, int j, A2Methods_UArray2 array2,
                                        A2Methods_Object *ptr, 
                                        void *cl);

/*
 * read_bitfile(FILE *fp, A2Methods_T)
 * 
 * Parameters: FILE *fp: pointer to an image file conaining 32 bit codewords,
 *             A2Methods_T methods: method suite containing functions to 
 *                                  manipulate 2D arrays
 * Returns   : A2Methods_UArray2 of codewords
 * Does      : Reads the given file and returns a 2D array where each block
 *             of pixels is represented by a 32 bit codeword
 */
A2Methods_UArray2 read_bitfile (FILE *fp, A2Methods_T methods)
{
    assert(fp != NULL);
    assert(methods != NULL);
    unsigned height, width;
    int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", &width, 
                                                                     &height);
    assert(read == 2);
    int c = getc(fp);
    assert(c == '\n');
    width = width / 2; /* we got the width of the image, not the blocked
                        * representation */
    height = height / 2;
    A2Methods_UArray2 codeword_rep = methods -> new(width, height, 
                                                           sizeof(UNSIGNED_T));
    
    closure_struct cl = malloc(sizeof(*cl));
    cl -> fp = fp;
    cl -> methods = methods;

    methods -> map_row_major(codeword_rep, populate_word_array, cl);

    free(cl);

    return codeword_rep;
}

/*
 * populate_word_array (int i, int j, A2Methods_UArray2 array2,
 *                                    A2Methods_Object *ptr, 
 *                                    void *cl)
 * 
 * Parameters: int i: current col
 *             int j: current row
 *             A2Methods_UArray2 array2: codeword array being mapped through
 *             A2Methods_Object *ptr: pointer to the current codeword in the
 *                                    array
 *             void *cl: closure
 * Returns   : Nothing
 * Does      : Reads the next 32 bits in the file to obtain the codeword for 
 *             the current position in the 2D array representation
 */
void populate_word_array (int i, int j, A2Methods_UArray2 array2,
                                        A2Methods_Object *ptr, 
                                        void *cl)
{
    (void) i;
    (void) j;
    (void) array2;

    closure_struct cl_struct = (closure_struct) cl;
    UNSIGNED_T *word = (UNSIGNED_T *) ptr;
    
    int c = getc(cl_struct -> fp);
    if (c == EOF) {
        if((i + 1) * (j + 1) != (cl_struct -> methods -> width(array2) *
                                 cl_struct -> methods -> height(array2))) {
            RAISE(NO_CODEWORDS_LEFT);
        }
    }
    *word = Bitpack_newu(*word, 8, 24, c);
    c = getc(cl_struct -> fp);
    if (c == EOF) {
        if((i + 1) * (j + 1) != (cl_struct -> methods -> width(array2) *
                                 cl_struct -> methods -> height(array2))) {
            RAISE(NO_CODEWORDS_LEFT);
        }
    }
    *word = Bitpack_newu(*word, 8, 16, c);
    c = getc(cl_struct -> fp);
    if (c == EOF) {
        if((i + 1) * (j + 1) != (cl_struct -> methods -> width(array2) *
                                 cl_struct -> methods -> height(array2))) {
            RAISE(NO_CODEWORDS_LEFT);
        }
    }
    *word = Bitpack_newu(*word, 8, 8, c);
    c = getc(cl_struct -> fp);
    if (c == EOF) {
        if((i + 1) * (j + 1) != (cl_struct -> methods -> width(array2) *
                                 cl_struct -> methods -> height(array2))) {
            RAISE(NO_CODEWORDS_LEFT);
        }
    }
    *word = Bitpack_newu(*word, 8, 0, c);
}

/*
 * write_bitfile (A2Methods_T methods, A2Methods_UArray2 array2)
 * 
 * Parameters: A2Methods_T methods: method suite to manipulate 2D arrays
 *             A2Methods_UArray2 array2: 2D array of codewords
 * Returns   : Nothing
 * Does      : Rights the bitfile represented by the given 2D array to
 *             stdout according to the specifications
 */
void write_bitfile (A2Methods_T methods, A2Methods_UArray2 array2)
{
    assert(methods != NULL);
    assert(array2 != NULL);
    int width  = methods -> width(array2),
        height = methods -> height(array2);
    fprintf(stdout, "COMP40 Compressed image format 2\n%u %u", width * 2, height * 2);
    fprintf(stdout, "\n");
    methods -> map_row_major(array2, print_codewords, methods);
    
}

/*
 * print_codewords (int i, int j, A2Methods_UArray2 array2,
 *                                A2Methods_Object *ptr, 
 *                                void *cl)
 * 
 * Parameters: int i: current col
 *             int j: current row
 *             A2Methods_UArray2 array2: codeword array being mapped through
 *             A2Methods_Object *ptr: pointer to the current codeword in the
 *                                    array
 *             void *cl: closure
 * Returns   : Nothing
 * Does      : Prints the codewords in big endian order to standard output
 */
void print_codewords (int i, int j, A2Methods_UArray2 array2,
                                 A2Methods_Object *ptr, 
                                 void *cl) 
{
    (void) i;
    (void) j;
    (void) array2;
    (void) cl;

    PRINT_TYPE codeword = *((PRINT_TYPE *) ptr);

    putchar(Bitpack_getu(codeword, 8, 24));
    putchar(Bitpack_gets(codeword, 8, 16));
    putchar(Bitpack_getu(codeword, 8, 8));
    putchar(Bitpack_getu(codeword, 8, 0));
}