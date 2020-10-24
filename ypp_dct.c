/*
 * Filename: ypp_dct.c
 * 
 * Authors   : Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary   : The functions in this file perform the necessary operations to
 *             transform the y, pb, and pr values from our component_video
 *             struct at each point in the array of those component_video
 *             structs to dctrans structs which hold the a, b, c, d, average
 *             pb, and average pr values for 2-by-2 blocks in the array of
 *             component_video structs
 */

#include "ypp_dct.h"

/* struct containing discrete cosine attributes */
typedef struct dctrans {
             
    float avgpb,
          avgpr,
          a,
          b,
          c,
          d;

} *dctrans;

/* struct containing the cv values y, pb, and pr */
typedef struct component_video {

    float y, 
          pb, 
          pr;

} *component_video;

/* closure struct containing a uarray2 and the methods for a uarray2 */
typedef struct closure_struct {

    A2Methods_UArray2 array2;
    A2Methods_T methods;

} *closure_struct;

void perform_dct (int i, int j, A2Methods_UArray2 array2,
                                A2Methods_Object *ptr, 
                                void *cl);
void reverse_dct (int i, int j, A2Methods_UArray2 array2, 
                                A2Methods_Object *ptr,
                                void *cl);
void reverse_dct_calc (component_video ypp_rep, dctrans dct_rep, 
                                                int block_counter);


/* 
 * A2Methods_UArray2 ypp_to_dct (A2Methods_UArray2 array2, A2Methods_T methods)
 * 
 * Parameters: A2Methods_UArray2 array2: array of component_video structs
 *             A2Methods_T methods: methods for UArray2
 * Returns   : A2Methods_UArray2: 1/2 size array of dctrans structs in place
 *                                of the component_video structs
 * Does      : Takes in an array of component_video structs and performs
 *             calculations to create an array of dctrans structs with
 *             dimensions half the size from the y, pb, pr values.
 */
A2Methods_UArray2 ypp_to_dct (A2Methods_UArray2 array2, A2Methods_T methods) 
{
    assert(array2 != NULL);
    assert(methods != NULL);
    /* 2x2 box representation -> half the width and height */
    int width  = (methods -> width(array2)) / 2,
        height = (methods -> height(array2)) / 2;
    A2Methods_UArray2 dct_rep = methods -> new(width, height, 
                                                      sizeof(struct dctrans));

    closure_struct cl = malloc(sizeof(*cl));
    assert(cl != NULL);
    cl -> array2 = dct_rep;
    cl -> methods = methods;

    methods -> map_row_major(array2, perform_dct, cl);

    free(cl);

    return dct_rep;
}


/* 
 * perform_dct (int i, int j, A2Methods_UArray2 array2,
 *                            A2Methods_Object *ptr, 
 *                            void *cl)
 * 
 * Parameters: int i: index of column of array2
 *             int j: index of row of array2
 *             A2Methods_UArray2 array2: array of component_video structs
 *             A2Methods_Object *ptr: the component_video struct at the 
 *                                    current index
 *             void *cl: closure struct holding the array of dctrans structs,
 *                       half the size of the component_video structs array,
 *                       and the methods for that array
 * Returns   : None
 * Does      : maps through the array of component_video structs by 2, always
 *             making sure we start at the top left corner of each block,
 *             extract the Y values, and uses Y1, Y2, Y3, and Y4 to calculate
 *             the a, b, c, and d values for the dctrans struct for that block.
 *             Also takes the average of the pb and pr values from those four
 *             blocks and sets avgpb and avgpr in the dctrans struct for that
 *             block.
 */
void perform_dct (int i, int j, A2Methods_UArray2 array2,
                                A2Methods_Object *ptr, 
                                void *cl) 
{
    (void) array2;

    if (i % 2 != 0 || j % 2 != 0) { /* start at top left corner of each
                                     * 2x2 box */
        return;
    }
    closure_struct cl_struct = (closure_struct) cl;
    component_video ypp_rep = (component_video) ptr;

    dctrans dct_rep = (dctrans) (cl_struct -> methods -> 
                                              at(cl_struct -> array2,
                                                 i / 2, 
                                                 j / 2));
    /* obtain info about the other pixel representations in the same 2x2 box */
    component_video tr = (component_video)(cl_struct -> methods -> at(array2,
                                                                      i + 1, 
                                                                      j));
    component_video bl = (component_video)(cl_struct -> methods -> at(array2,
                                                                      i, 
                                                                      j + 1));
    component_video br = (component_video)(cl_struct -> methods -> at(array2,
                                                                      i + 1, 
                                                                      j + 1));
    /* perform necessary calculations */
    dct_rep -> avgpb = (ypp_rep -> pb + tr -> pb + bl -> pb + br -> pb) / 4.0;
    dct_rep -> avgpr = (ypp_rep -> pr + tr -> pr + bl -> pr + br -> pr) / 4.0;
    dct_rep -> a = (br -> y + bl -> y + tr -> y + ypp_rep -> y) / 4.0;
    dct_rep -> b = (br -> y + bl -> y - tr -> y - ypp_rep -> y) / 4.0;
    dct_rep -> c = (br -> y - bl -> y + tr -> y - ypp_rep -> y) / 4.0;
    dct_rep -> d = (br -> y - bl -> y - tr -> y + ypp_rep -> y) / 4.0;
}


/* 
 * A2Methods_UArray2 dct_to_ypp (A2Methods_UArray2 array2, A2Methods_T methods)
 * 
 * Parameters: A2Methods_UArray2 array2: array of dctrans structs
 *             A2Methods_T methods: methods for UArray2
 * Returns   : A2Methods_UArray2: UArray2 of component_video objects 2x the 
 *                                height and width of the UArray2 of dctrans
 *                                structs
 * Does      : Takes in an array of dctrans structs and performs calculations
 *             to create an array of component_video structs with dimensions
 *             double the size from the a, b, c, and d values.
 */
A2Methods_UArray2 dct_to_ypp (A2Methods_UArray2 array2, A2Methods_T methods) 
{
    assert(array2 != NULL);
    assert(methods != NULL);
    /* each element in dct array corresponds to a 2x2 box of pixels */
    int width  = (methods -> width(array2)) * 2,
        height = (methods -> height(array2)) * 2;
    A2Methods_UArray2 ypp_rep = methods -> new(width, 
                                               height,
                                               sizeof(struct component_video));
    closure_struct cl = malloc(sizeof(*cl));
    assert(cl != NULL);
    cl -> array2 = ypp_rep;
    cl -> methods = methods;

    methods -> map_row_major(array2, reverse_dct, cl);

    free(cl);

    return ypp_rep;
}

/* 
 * reverse_dct (int i, int j, A2Methods_UArray2 array2,
 *                            A2Methods_Object *ptr, 
 *                            void *cl)
 * 
 * Parameters: int i: index of column of array2
 *             int j: index of row of array2
 *             A2Methods_UArray2 array2: array of dctrans structs
 *             A2Methods_Object *ptr: the dctrans struct at the current index
 *             void *cl: closure struct holding the array of component_video
 *                       structs, double the size of the dctrans structs array,
 *                       and the methods for that array
 * Returns   : None
 * Does      : maps through the array of dctrans structs, extracts the 
 *             a, b, c, d, avgpr, and avgpb values, and uses them to calculate
 *             Y1, Y2, Y3, and Y4 as well as set pb and pr.
 */
void reverse_dct (int i, int j, A2Methods_UArray2 array2,
                                A2Methods_Object *ptr, 
                                void *cl) 
{
    (void) array2;

    closure_struct cl_struct = (closure_struct) cl;
    dctrans dct_rep = (dctrans) ptr;

    int block_counter = 1; /* to check if we need to calculate Y1,2,3, or 4 */
    for (int iteratorR = j * 2; iteratorR <= (j * 2) + 1; iteratorR++) {
        for (int iteratorC = i * 2; iteratorC <= (i * 2) + 1; iteratorC++) {
            component_video ypp_rep = (component_video) cl_struct -> 
                                                        methods ->
                                                        at(cl_struct -> array2, 
                                                           iteratorC, 
                                                           iteratorR);
            reverse_dct_calc(ypp_rep, dct_rep, block_counter);
            block_counter++;
        }
    }
}

/* 
 * reverse_dct_calc (component_video ypp_rep, dctrans dct_rep,
 *                                            int block_counter)
 * Parameters: component_video ypp_rep: component_video struct at current
 *                                      spot
 *             dctrans dct_rep: dctrans struct at current block
 *             int block_counter: indicator of which spot is current in the
 *                                block
 * Returns   : None
 * Does      : Performs calculations necessary to transform a dctrans block
 *             back into a component_video struct for each of the four spots
 *             in the block
 */
void reverse_dct_calc (component_video ypp_rep, dctrans dct_rep,
                                                int block_counter) 
{
    ypp_rep -> pb = dct_rep -> avgpb;
    ypp_rep -> pr = dct_rep -> avgpr;
    
    if(block_counter == 1) { /* y1 */
        ypp_rep -> y = (dct_rep -> a - dct_rep -> b - dct_rep -> c
                                                    + dct_rep -> d);
    } else if (block_counter == 2) { /* y2 */
        ypp_rep -> y = (dct_rep -> a - dct_rep -> b + dct_rep -> c
                                                    - dct_rep -> d);
    } else if (block_counter == 3) { /* y3 */
        ypp_rep -> y = (dct_rep -> a + dct_rep -> b - dct_rep -> c
                                                    - dct_rep -> d);
    } else if (block_counter == 4) { /* y4 */
        ypp_rep -> y = (dct_rep -> a + dct_rep -> b + dct_rep -> c
                                                    + dct_rep -> d);
    }
}