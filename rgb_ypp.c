/* 
 * Filename: rgb_ypp.c
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : In compression, transforms an array of Pnm_rgb structs to an
 *             array of component_video structs. In decompression, transforms
 *             an array of component_video structs to an array of Pnm_rgb
 *             structs. 
 */

#include "rgb_ypp.h"

/* struct created to hold the y, pb, and pr values of a cv per pixel,
 * converted from rgb pixels */
typedef struct component_video {

    float y, 
          pb, 
          pr;

} *component_video;


/* closure struct which holds a 2d uarray and the methods for a 2d uarray 
 * as well as an unsigned integer representing the ppm image's denominator */
typedef struct closure_struct {

    A2Methods_UArray2 array2;
    A2Methods_T methods;
    unsigned denominator;

} *closure_struct;

void convert_to_cv (int i, int j, A2Methods_UArray2 array2,
                                  A2Methods_Object *ptr, 
                                  void *cl);
void convert_to_rgb (int i, int j, A2Methods_UArray2 array2,
                                   A2Methods_Object *ptr, 
                                   void *cl);

/* 
 * rgb_to_ypp (A2Methods_UArray2 array2, A2Methods_T methods,
 *                                       unsigned denominator)
 * 
 * Parameters: A2Methods_UArray2 array2: array of rgb pixels from ppm image
 *             A2Methods_T methods: methods for UArray2
 *             unsigned denominator: denominator used to scale rgb values
 * Returns   : A2Methods_UArray2: array of component_video structs in place
 *                                of the rgb pixels 
 * Does      : Takes in a Pnm_ppm pixel map and overwrites the Pnm_rgb
 *             structs with component_video structs
 */
A2Methods_UArray2 rgb_to_ypp (A2Methods_UArray2 array2, A2Methods_T methods,
                                                        unsigned denominator) 
{
    assert(array2 != NULL);
    assert(methods != NULL);
    A2Methods_UArray2 ypp_rep = methods -> new(methods -> width(array2), 
                                               methods -> height(array2),
                                               sizeof(struct component_video));
    closure_struct cl = malloc(sizeof(*cl));
    assert(cl != NULL);
    cl -> array2 = ypp_rep;
    cl -> methods = methods;
    cl -> denominator = denominator;

    methods -> map_row_major(array2, convert_to_cv, cl);

    free(cl);

    return ypp_rep;
}

/* 
 * ypp_to_rgb (A2Methods_UArray2 array2, A2Methods_T methods)
 * 
 * Parameters: A2Methods_UArray2 array2: array of component_video structs
 *             A2Methods_T methods: methods for UArray2
 * Returns   : A2Methods_UArray2: array of Pnm_rgb structs for a Pnm_ppm 
 *                                pixel map
 * Does      : Takes in a UArray2 of component_video structs, converts the 
 *             values to red, green, and blue pixel values for Pnm_rgb structs
 *             and overwrites the array with Pnm_rgb structs in place of
 *             component_video structs
 */
A2Methods_UArray2 ypp_to_rgb (A2Methods_UArray2 array2, A2Methods_T methods) 
{
    assert(array2 != NULL);
    assert(methods != NULL);
    A2Methods_UArray2 rgb_rep = methods -> new(methods -> width(array2), 
                                               methods -> height(array2),
                                               sizeof(struct Pnm_rgb));
    closure_struct cl = malloc(sizeof(*cl));
    assert(cl != NULL);
    cl -> array2 = rgb_rep;
    cl -> methods = methods;
    cl -> denominator = (unsigned) 0; /* don't need denominator at this point
                                       * for decompression */
    methods -> map_row_major(array2, convert_to_rgb, cl);

    free(cl);

    return rgb_rep;
}

/* 
 * void convert_to_cv (int i, int j, A2Methods_UArray2 array2, 
 *                                   A2Methods_Object *ptr, 
 *                                   void *cl)
 * 
 * Parameters: int i: index of column
 *             int j: index of row
 *             A2Methods_UArray2 array2: pixel map for ppm image
 *             A2Methods_Object *ptr: the Pnm_rgb struct at the current index
 *             void *cl: closure struct holding the array of component_video
 *                       structs and the methods for that array
 * Returns   : None
 * Does      : apply function performs the actual calculations to transform
 *             the red, green, and blue values into the y, pb, and pr values
 *             for a component_video struct, and overwrites the array of 
 *             Pnm_rgb structs
 */
void convert_to_cv (int i, int j, A2Methods_UArray2 array2,
                                  A2Methods_Object *ptr, 
                                  void *cl) 
{    
    (void) array2;
    
    closure_struct cl_struct = (closure_struct) cl;
    Pnm_rgb rgb_rep = (Pnm_rgb) ptr;
    
    /* obtain values */
    float red   = (float) rgb_rep -> red / (float) cl_struct -> denominator,
          green = (float) rgb_rep -> green / (float) cl_struct -> denominator,
          blue  = (float) rgb_rep -> blue / (float) cl_struct -> denominator;
    
    component_video ypp_rep = (component_video) cl_struct -> 
                                                methods -> 
                                                at(cl_struct -> array2, i, j);
    /* make necessary calculations */
    ypp_rep -> y  = 0.299 * red + 0.587 * green + 0.114 * blue;
    ypp_rep -> pb = -0.168736 * red - 0.331264 * green + 0.5 * blue;
    ypp_rep -> pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;
}


/* 
 * void convert_to_rgb (int i, int j, A2Methods_UArray2 array2, 
 *                                    A2Methods_Object *ptr, 
 *                                    void *cl)
 * 
 * Parameters: int i: index of column
 *             int j: index of row
 *             A2Methods_UArray2 array2: array of component_video structs
 *             A2Methods_Object *ptr: the component_video struct at the 
 *                                    current index
 *             void *cl: closure struct holding the array of Pnm_rgb
 *                       structs and the methods for that array
 * Returns   : None
 * Does      : apply function performs the actual calculations to transform
 *             the y, pb, and pr values into the red, green, and blue values
 *             for a Pnm_rgb struct, and overwrites the array of 
 *             component_video structs
 */
void convert_to_rgb (int i, int j, A2Methods_UArray2 array2,
                                   A2Methods_Object *ptr, 
                                   void *cl) 
{
    (void) array2;

    closure_struct cl_struct = (closure_struct) cl;
    component_video ypp_rep = (component_video) ptr;

    /* obtain values */
    float y  = ypp_rep -> y,
          pb = ypp_rep -> pb,
          pr = ypp_rep -> pr;

    /* make necessary calculations */
    float red   = (float) (1.0 * y + 0.0 * pb + 1.402 * pr),
          green = (float) (1.0 * y - 0.344136 * pb - 0.714136 * pr),
          blue  = (float) (1.0 * y + 1.772 * pb + 0.0 * pr);

    /* force values into proper bounds */
    if (red < 0) {
        red = 0;
    } else if (red > 1) {
        red = 1;
    }
    if (green < 0) {
        green = 0;
    } else if (green > 1) {
         green = 1;
    }
    if (blue < 0) {
        blue = 0;
    } else if (blue > 1)
        blue = 1;
    
    Pnm_rgb rgb_rep = (Pnm_rgb) (cl_struct -> methods -> at(cl_struct -> array2, 
                                                            i, j));
    /* scale values */
    rgb_rep -> red = red * 255;
    rgb_rep -> green = green * 255;
    rgb_rep -> blue = blue * 255;
}