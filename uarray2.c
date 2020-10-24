#include <uarray2.h>
#include <uarray.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

struct UArray2_T {
    int DIM1;
    int DIM2;
    int ELEMENT_SIZE;
    UArray_T array;
};

UArray2_T UArray2_new(int dim1, int dim2, int elemSize) {
    UArray2_T uarray2 = malloc(sizeof(struct UArray2_T));
    assert(uarray2 != NULL);
    
    uarray2 -> DIM1 = dim1; /* width */
    uarray2 -> DIM2 = dim2; /* height */
    uarray2 -> ELEMENT_SIZE = elemSize;
    uarray2 -> array = UArray_new(dim1*dim2, elemSize);

    return uarray2;
}

int UArray2_width(UArray2_T uarray2) {
    assert(uarray2 != NULL);
    return uarray2 -> DIM1;
}

int UArray2_height(UArray2_T uarray2) {
    assert(uarray2 != NULL);
    return uarray2 -> DIM2;
}

int UArray2_size(UArray2_T uarray2) {
    assert(uarray2 != NULL);
    return uarray2 -> ELEMENT_SIZE;
}

void* UArray2_at(UArray2_T uarray2, int col, int row) {
    assert(uarray2 != NULL);
    int index = col + uarray2 -> DIM1 * row;
    
    return UArray_at(uarray2 -> array, index);
}

void UArray2_map_row_major(UArray2_T uarray2, void func(int i, int j, 
                           UArray2_T a, void *p1, void *p2), void* cl) {
    assert(uarray2 != NULL);
    assert(uarray2 -> array != NULL);
    assert(func != NULL);
    
    for (int row = 0; row < uarray2 -> DIM2; row++) {
        for (int col = 0; col < uarray2 -> DIM1; col++) {
            func(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
        }
    }
}

void UArray2_map_col_major(UArray2_T uarray2, void func(int i, int j, 
                           UArray2_T a, void *p1, void *p2), void* cl) {
    assert(uarray2 != NULL);
    assert(uarray2 -> array != NULL);
    assert(func != NULL);
    
    for (int col = 0; col < uarray2 -> DIM1; col++) {
        for (int row = 0; row < uarray2 -> DIM2; row++) {
            func(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
        }
    }
}

void UArray2_free(UArray2_T* uarray2Ptr) {
    assert(uarray2Ptr != NULL);
    assert(*uarray2Ptr != NULL);
    assert((*uarray2Ptr) -> array);
    UArray_free(&(*uarray2Ptr) -> array);
    free(*uarray2Ptr);
}
