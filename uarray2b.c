/************************************************************
 * Vichka Fonarev and Robert Lester
 * 2/20/20
 * Locality
 * uarray2b.c
 * 
 * This file is the implementation of the blocked 2d array implementation
 * provided for us
 * 
 * For storing the blocked array in memory we are using a single 2d array that
   in turn stores everything in an unblocked hanson array. This structure
   increases the cache hit rate by storing memoery in discrete blocks that 
   will fit into at least one cache block. In order to access the blocks
   we convert the coordinates of the visual representation to their 'actual'
   position in the virtual 2d array and then the 2d array implementations
   handles storing/accessing them in a linear 1d hanson array.
 * 
 * Invariants   : array2D should never be null
 *                width, height, size, and blocksize > 0
 *                contentWidth and contentHeight is always smaller than
 *                actualWidth and actualHeight respectively. 
 * *********************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "assert.h"
#include "uarray2b.h"
#include "uarray2.h"
#include "math.h"

struct UArray2b_T {
        int width;
        int height;
        int contentHeight;
        int contentWidth;
        int size;
        int blocksize;
        UArray2_T array2D;
};

struct Coordinates {
    int col;
    int row;
};

struct Coordinates convert_coordinates(UArray2b_T array2D, int col, int row);
struct Coordinates simple_index_to_coordinates(int index, int width);
struct Coordinates index_to_coordinates(UArray2b_T array2D, int index);

UArray2b_T UArray2b_new (int width, int height, int size, int blocksize)
{
    assert(width > 0);
    assert(height > 0);
    assert(size > 0);
    assert(blocksize > 0);

    UArray2b_T uarray2b = malloc(sizeof(struct UArray2b_T));
    assert(uarray2b);

    int blockHeight = (int) ceil((double)height / (double)blocksize);
    int blockWidth = (int) ceil((double)width / (double)blocksize);

    int fullHeight = blocksize * blockHeight;
    int fullWidth = blocksize * blockWidth;

    uarray2b -> contentWidth = width;
    uarray2b -> contentHeight = height;
    uarray2b -> width = fullWidth;
    uarray2b -> height = fullHeight;
    uarray2b -> size = size;
    uarray2b -> blocksize = blocksize;
    uarray2b -> array2D = UArray2_new(fullWidth, fullHeight, size);

    return uarray2b;
}

/* new blocked 2d array: blocksize as large as possible provided
* block occupies at most 64KB (if possible)
*/
UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
    assert(width > 0);
    assert(height > 0);
    assert(size > 0);

    int blocksize = 0;
        double num_cells;
        
        if (size > 64000) {
        blocksize = 1; 
        } else {
        num_cells = 64000 / size;
        blocksize = (int) floor(sqrt(num_cells));
        }

        if (width < blocksize || height < blocksize) {
                if (height > width) {
            blocksize = width;
        } else {
            blocksize = height;
        }
        }
        
        UArray2b_T array2D = UArray2b_new(width, height, size, blocksize);

        return array2D;
}

/*******************************************
 *  Name        :   UArray2b_free
 * 
 *  Params      :   UArray2b_T*  :   double pointer to a UArray2b_T struct
 *  Returns     :   None
 * 
 *  Function    :   Frees the dynamically allocated memory associated with
 *                  the class
 * 
 ******************************************/
void UArray2b_free (UArray2b_T *array2D)
{
    assert(array2D != NULL && *array2D != NULL);
        UArray2_free(&(*array2D) -> array2D);
        free(*array2D);
}

/*******************************************
 *  Name        :   UArray2b_width 
 * 
 *  Params      :   UArray2b_T uarray2b :   pointer to UArray2b_T struct
 *  Returns     :   int                 :   Width of the array
 * 
 *  Function    :   Returns width of the UArray2b_T
 * 
 ******************************************/
int UArray2b_width (UArray2b_T array2D)
{
    assert(array2D);
    return array2D -> contentWidth;
}

/*******************************************
 *  Name        :   UArray2b_height
 * 
 *  Params      :   UArray2b_T uarray2b :   pointer to UArray2b_T struct
 * 
 *  Returns     :   int                 :   height of the array
 * 
 *  Function    :   Returns height of the UArray2b_T
 * 
 ******************************************/
int UArray2b_height (UArray2b_T array2D)
{
    assert(array2D);
    return array2D -> contentHeight;
}

/*******************************************
 *  Name        :   UArray2b_size
 * 
 *  Params      :   UArray2b_T uarray2b :   pointer to UArray2bT struct
 * 
 *  Returns     :   int                 :   the size of an individual element
 * 
 *  Function    :   returns  element size of the an elemnt in UArray2b_T
 * 
 ******************************************/
int UArray2b_size (UArray2b_T array2D)
{
    assert(array2D);
    return array2D -> size;
}

/*******************************************
 *  Name        :   UArray2b_size
 * 
 *  Params      :   UArray2b_T uarray2b :   pointer to UArray2bT struct
 * 
 *  Returns     :   int                 :   the size of an individual element
 * 
 *  Function    :   returns  element size of the an elemnt in UArray2b_T
 * 
 ******************************************/
int UArray2b_blocksize(UArray2b_T array2D)
{
    assert(array2D);
    return array2D -> blocksize;
}

/*******************************************
 *  Name        :   UArray2_map
 * 
 *  Params      :   UArray2b_T uarray2   :   pointer to UArray2b_T struct
 *                  func(int i, int j, UArray2b_T a, void *p1, void *p2) : 
 *                  function that is being mapped accross the UArray2 in 
 *                  row major order
 *                  cl                  :   the closure element for mapping
 * 
 *  Returns     :   None
 * 
 *  Function    :   Maps the function in block major order
 * 
 ******************************************/
void *UArray2b_at(UArray2b_T array2D, int column, int row)
{
    assert(array2D != NULL);
    assert(column >= 0 && column < (array2D -> contentWidth));
    assert(row >= 0 && row < (array2D -> contentHeight));
    struct Coordinates coords = convert_coordinates(array2D, column, row);
    return UArray2_at(array2D -> array2D, coords.col, coords.row);
}

/*******************************************
 *  Name        :   UArray2_map
 * 
 *  Params      :   UArray2b_T uarray2   :   pointer to UArray2b_T struct
 *                  func(int i, int j, UArray2b_T a, void *p1, void *p2) : 
 *                  function that is being mapped accross the UArray2 in 
 *                  row major order
 *                  cl                  :   the closure element for mapping
 * 
 *  Returns     :   None
 * 
 *  Function    :   Maps the function in block major order
 * 
 ******************************************/
void UArray2b_map(UArray2b_T array2D,
                  void apply(int col, int row, UArray2b_T array2D,
                  void *elem, void *cl),
                  void *cl)
{
    assert(array2D != NULL);
    int size = array2D -> height * array2D -> width;

    struct Coordinates coords;

    for (int index = 0; index < size; index++) {
        coords = index_to_coordinates(array2D, index);
        if (coords.row < array2D -> contentHeight && 
            coords.col < array2D -> contentWidth) {
            void* val = UArray2b_at(array2D, coords.col, coords.row);
            apply(coords.col, coords.row, array2D, val, cl);
        }
    }
}

/*******************************************
 *  Name        :   convert_coordinates
 * 
 *  Params      :   UArray2b_T uarray2b :   pointer to UArray2_T struct
 *                  int col             :   column value of coord to be 
 *                                          converted
 *                  int row             :   row value of coord to be converted

 * 
 *  Returns     :   struct C            :   pointer to a struct containing the 
 *                                          row and col values of the new coord
 * 
 *  Function    :   converts the coordinates of an element in the virtual array
 *                  to the coordinates of where they are actually stored in the
 *                  2d array in memory
 * 
 ******************************************/
struct Coordinates convert_coordinates(UArray2b_T array2D, int col, int row)
{
    assert(array2D);
    struct Coordinates coords;
    coords.col = col;
    coords.row = row;

    int blockSize = array2D -> blocksize;

    if (blockSize == 1) {
        return coords;
    }

    int widthInBlocks = (array2D -> width) / (blockSize);

    int blockIndex = (((row / blockSize) * widthInBlocks) + (col / blockSize));
    int blockElement = ((row % blockSize) * blockSize) + (col % blockSize);
    int index = blockIndex * (blockSize * blockSize) + blockElement;
    
    return simple_index_to_coordinates(index, array2D -> width);
}

/*******************************************
 *  Name        :   simple_index_to_coordinates
 * 
 *  Params      :   int index   :   index of element in linear implementation
 *                  int width   :   width of the 2d array being used
 * 
 *  Returns     :   struct C    :   pointer to a struct containing the 
 *                                          row and col values of the new coord
 * 
 *  Function    :   returns the coordinates of an element whose linear index
 *                  was given
 * 
 ******************************************/
struct Coordinates simple_index_to_coordinates(int index, int width)
{
    struct Coordinates coords;
    coords.col = index % width;
    coords.row = index / width;

    return coords;
}

/*******************************************
 *  Name        :   simple_index_to_coordinates
 * 
 *  Params      :   UArray2b_T uarray2b :   pointer to UArray2_T struct
 *                  int index           :   index of element in linear 
 *                                          implementation
 * 
 *  Returns     :   struct C            :   pointer to a struct containing the 
 *                                          row and col values of the new coord
 * 
 *  Function    :   returns the coordinates of an element whose linear index
 *                  was given, does math to account for the blocked array
 * 
 ******************************************/
struct Coordinates index_to_coordinates(UArray2b_T array2D, int index)
{
    assert(array2D);
    struct Coordinates coords;

    int blocksize = array2D -> blocksize;
    int widthInBlocks = (array2D -> width) / (blocksize);
    int blockElement = index % (blocksize * blocksize);
    int blockIndex = (index - blockElement) / (blocksize * blocksize);

    coords.col  =   (blockIndex % widthInBlocks) * blocksize
                    + blockElement % blocksize;

    coords.row  =   (blockIndex / widthInBlocks) * blocksize
                    + blockElement / blocksize;

    return coords;
}
