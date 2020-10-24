/*
 * Filename  : read_bitfile.h
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : Interface for printing and reading in bit files
 *             for compression and decompression
 */

#ifndef READ_BITFILE_INCLUDED
#define READ_BITFILE_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "a2methods.h"
#include "malloc.h"
#include "a2plain.h"
#include "assert.h"
#include "bitpack.h"
#include "except.h"

/*
 * read_bitfile
 * 
 * reads a file and populates a 2D array of 32 bit codewords
 * 
 * assumes the arguments are not NULL
 */
A2Methods_UArray2 read_bitfile (FILE *fp, A2Methods_T methods);

/*
 * write_bitfile
 * 
 * using the given representation of the image, write the bit file
 * to standard output
 * 
 * assumes the arguments are not NULL
 */
void write_bitfile (A2Methods_T methods, A2Methods_UArray2 array2);

#endif