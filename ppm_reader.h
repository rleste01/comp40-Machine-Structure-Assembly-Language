/*
 * Filename: ppm_reader.h
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : Reads in a ppm image and trims it to be even, or writes a ppm
 *             image depending on the function called
 */


#ifndef PPM_READER_INCLUDED
#define PPM_READER_INCLUDED

#include <stdbool.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"

/*
 * read_ppm
 * 
 * returns a Pnm_ppm representation of a given ppm image file
 * 
 * assumes the arguments are not NULL
 */
Pnm_ppm read_ppm (FILE *fp, A2Methods_T methods);

/*
 * write_ppm
 * 
 * writes a ppm image file, given through a Pnm_ppm representation,
 * in the required format to standard output
 * 
 * assumes the argument is not NULL
 */
void write_ppm (Pnm_ppm image);

#endif