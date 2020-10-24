/*
 * Filename: ppm_reader.c
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : Implementation of the ppm_reader.h interface; reads in a ppm
 *             image and trims it to be even, or writes a ppm image
 */

#include "ppm_reader.h"

void trim_ppm(Pnm_ppm image, int width, int height, int size);
void copy_trimmed (int i, int j, A2Methods_UArray2 array2,
                                 A2Methods_Object *ptr, 
                                 void *cl);

/*
 * read_ppm(FILE *fp, A2Methods_T methods)
 * 
 * Parameters: FILE *fp: Pointer to a ppm image file, 
 *             A2Methods_T methods: Method suite containing functions to
 *             manipulate 2D arrays
 * Returns   : Pnm_ppm: Pnm_ppm containing the ppm representation of the image
 * Does      : Reads the file from the first argument and creates a
 *             Pnm_ppm representation of the image. Trims the width
 *             and height by 1 if they are not even.
 */
Pnm_ppm read_ppm (FILE *fp, A2Methods_T methods) 
{
    assert(fp != NULL);
    assert(methods != NULL);
    
        Pnm_ppm image = Pnm_ppmread(fp, methods);
    int width  = image -> width, /* obtain information about the image */
        height = image -> height,
        size   = image -> methods -> size(image -> pixels);

    bool need_trim = false;
    if (width % 2 != 0) {
        width--;
        need_trim = true;
    }
    if (height % 2 != 0) {
        height--;
        need_trim = true;
    }
    if (need_trim) { /* trim if necessary */
        trim_ppm(image, width, height, size);
        return image;
    } else {
        return image;
    }
}

/*
 * write_ppm
 * 
 * Parameters: Pnm_ppm image: Pnm_ppm representation of an image
 * Returns   : Nothing
 * Does      : Writes the ppm image representation to standard out in the
 *             desired format.
 */
void write_ppm (Pnm_ppm image) 
{
    assert(image != NULL);
        Pnm_ppmwrite(stdout, image);
}

/*
 * trim_ppm(Pnm_ppm image, int width, int height, int size)
 * 
 * Parameters: Pnm_ppm image: Pnm_ppm representation of an image,
 *             int width: Integer width of image in pixels,
 *             int height: Integer height of image in pixels,
 *             int size: Integer size of representation of each pixel in bytes
 * Returns   : Nothing
 * Does      : Using the new width and height given as arguments, creates a
 *             new 2D array representation of the image according to the
 *             new dimensions. Does so by creating a new, smaller array (if
 *             necessary), and copies the RGB values from the original array
 *             into the new one
 */
void trim_ppm (Pnm_ppm image, int width, int height, int size) 
{
        A2Methods_UArray2 original = image -> pixels; /* keep copy of the original
                                                   * image */
    image -> pixels = image -> methods -> new(width, height, size);
    image -> width  = width;
    image -> height = height;

    image -> methods -> map_row_major(original, copy_trimmed, &image);
    
    image -> methods -> free(&original);
}

/*
 * copy_trimmed(int i, int j, A2Methods_UArray2 array2,
 *                            A2Methods_Object *ptr, 
 *                            void *cl)
 * 
 * Parameters: Integer current column,
 *             Integer current row,
 *             UArray2 representation of the pixel array,
 *             Pointer to the current element in the array (in this case a 
 *                 Pnm_rgb),
 *             Pointer to a closure, which is a closure structure in this case
 * Returns   : Nothing
 * Does      : Copies the elements from the original array into the new, 
 *             trimmed array.
 */
void copy_trimmed (int i, int j, A2Methods_UArray2 array2,
                                 A2Methods_Object *ptr, 
                                 void *cl) 
{    
    (void) array2;

    Pnm_ppm image = *((Pnm_ppm *) cl);
    if (i > (int) (image -> width) - 1) {
        return;
    } else if (j > (int) (image -> height) - 1) {
        return;
    }

    Pnm_rgb old_array_rgb     = (Pnm_rgb) ptr;
    Pnm_rgb trimmed_array_rgb = (Pnm_rgb) image->methods->at(image->pixels,
                                                             i, 
                                                             j);
    /* make deep copy of rgb values */
    trimmed_array_rgb -> red   = old_array_rgb -> red; 
    trimmed_array_rgb -> green = old_array_rgb -> green;
    trimmed_array_rgb -> blue  = old_array_rgb -> blue;
}