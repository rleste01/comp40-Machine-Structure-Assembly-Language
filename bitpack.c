/* 
 * Filename: bitpack.c
 * 
 * Authors: Robert Lester, Craig Cagner
 * Assignment: Arith
 * Summary: Implementation of bitpack.h; performs necessary operations to
 *          check if a value fits in a given field of a codeword, place new
 *          values in a field in a codeword, or get values from a field in a
 *          codeword. 
 */

#include <stdint.h>
#include "bitpack.h"
#include "except.h"
#include "stdio.h"
#include "assert.h"

#define SIZE 64
#define SIGNED_T int64_t
#define UNSIGNED_T uint64_t

/* excepton to be raised if a Bitpack_fitss or Bitpack_fitsu returns false */
Except_T Bitpack_Overflow = { "Overflow packing bits" };


/* 
 * Bitpack_fitsu(UNSIGNED_T n, unsgined with)
 * 
 * Parameters: 
 *              UNSIGNED_T n: unsigned value to be checked for its ability to 
 *                            fit in a field of specified width
 *              unsigned width: width of the field which n is being checked
 *                              against
 * Returns: 
 *          bool: 1 or 0
 * Does: returns true if an unsigned value can fit in the specified 
 *       width, returns false if it cannot
 * 
 */
bool Bitpack_fitsu (UNSIGNED_T n, unsigned width) 
{
    assert(width <= SIZE);
    n = n >> width;
    if (n)
        return false;
    else
        return true;
}


/* 
 * Bitpack_fitss(SIGNED_T n, unsgined with)
 * 
 * Parameters: 
 *              SIGNED_T n: signed value to be checked for its ability to fit
 *                          in a field of specified width
 *              unsigned width: width of the field which n is being checked
 *                              against
 * Returns: 
 *          bool: 1 or 0
 * Does: returns true if an signed value can fit in the specified 
 *       width, returns false if it cannot
 * 
 */
bool Bitpack_fitss (SIGNED_T n, unsigned width) 
{
    width = width - 1;
    if (n < 0) {
        n = ((-1) * n) - 1;
        return Bitpack_fitsu(n, width);
    }
    else {
        return Bitpack_fitsu(n, width); 
    }
    
}


/*
 * Bitpack_getu (UNSIGNED_T word, unsigned width, unsigned lsb)
 * 
 * Parameters:
 *              UNSIGNED_T word: codeword which getu is extracting the value
 *                               from
 *              unsigned width: width of the value that getu is looking for
 *              unsigned lsb: least significant bit of the value that getu is 
 *                            looking for
 * Returns: 
 *          UNSIGNED_T: unsigned value in field of "width" width starting at 
 *                      the lsb provided
 * Does: Returns the unsigned value of the field in word which is designated
 *       by width and lsb
 */
UNSIGNED_T Bitpack_getu (UNSIGNED_T word, unsigned width, unsigned lsb)
{
    assert(width <= SIZE);
    assert((width + lsb) <= SIZE);
    
    if (width == 0)
        return 0;

    UNSIGNED_T temp = word;
    
    assert((SIZE - (width + lsb)) <= SIZE);
    assert((SIZE - width) <= SIZE);
    
    temp = temp << (SIZE - (width + lsb));
    temp = temp >> (SIZE - width);
    return temp;
}


/*
 * Bitpack_getu (UNSIGNED_T word, unsigned width, unsigned lsb)
 * 
 * Parameters:
 *              UNSIGNED_T word: codeword which gets is extracting the value
 *                             from
 *              unsigned width: width of the value that gets is looking for
 *              unsigned lsb: least significant bit of the value that gets is 
 *                            looking for
 * Returns: 
 *          SIGNED_T: signed value in field of "width" width starting at 
 *                    the lsb provided
 * Does: Returns the signed value of the field in word which is designated by
 *       width and lsb
 */
SIGNED_T Bitpack_gets(UNSIGNED_T word, unsigned width, unsigned lsb)
{
    assert(width <= SIZE);
    assert((width + lsb) <= SIZE);
    
    if (width == 0)
        return 0;

    SIGNED_T temp = word;

    assert((SIZE - (width + lsb)) <= SIZE);
    assert((SIZE - width) <= SIZE);
    
    temp = temp << (SIZE - (lsb + width));
    temp = temp >> (SIZE - width);
    return temp;
}


/*
 * Bitpack_newu(UNSIGNED_T word, unsigned width, unsigned lsb, 
 *                                               UNSIGNED_T value)
 * 
 * Parameters:
 *              UNSIGNED_T word: codeword to be modified by newu
 *              unsigned width: width of the field into which the value should
 *                              be placed
 *              unsigned lsb: lowest significant bit of the field into which
 *                            the value should be placed
 *              UNSIGNED_T value: unsigned value to be placed into a specific
 *                                field of the provided codeword
 * Returns:
 *              UNSIGNED_T: the updated codeword
 * Does: Takes in a new unsigned value, checks if it can fit into the specified
 *       field, and if so, uses a mask to clear and then fill the specified 
 *       field with the value.
 * 
 */
UNSIGNED_T Bitpack_newu(UNSIGNED_T word, unsigned width, unsigned lsb,
                                                         UNSIGNED_T value)
{
    if (!(Bitpack_fitsu(value, width))) {
        RAISE(Bitpack_Overflow);
    }

    assert((width + lsb) <= SIZE);
    
    UNSIGNED_T mask = ~0;
    mask = mask << (SIZE - (width + lsb));
    mask = mask >> (SIZE - width);
    mask = mask << lsb;
    mask = ~mask;

    UNSIGNED_T temp = word;
    temp = temp & mask;


    value = value << lsb;

    temp = temp | value;

    return temp;
    
}


/*
 * Bitpack_news(UNSIGNED_T word, unsigned width, unsigned lsb, SIGNED_T value)
 * 
 * Parameters:
 *              SIGNED_T word: codeword to be modified by newu
 *              unsigned width: width of the field into which the value should
 *                              be placed
 *              unsigned lsb: lowest significant bit of the field into which
 *                            the value should be placed
 *              SIGNED_T value: signed value to be placed into a specific
 *                                field of the provided codeword
 * Returns:
 *              UNSIGNED_T: the updated codeword
 * Does: Takes in a new signed value, checks if it can fit into the specified
 *       field, and if so, uses a mask to clear and then fill the specified 
 *       field with the value.
 * 
 */
UNSIGNED_T Bitpack_news(UNSIGNED_T word, unsigned width, unsigned lsb,
                                                         SIGNED_T value)
{

    if (!(Bitpack_fitss(value, width))) {
        RAISE(Bitpack_Overflow);
    }

    assert((width + lsb) <= SIZE);

    UNSIGNED_T mask = ~0;
    mask = mask << (SIZE - (width + lsb));
    mask = mask >> (SIZE - width);
    mask = mask << lsb;
    mask = ~mask;

    UNSIGNED_T temp = word;
    temp = temp & mask;

    value = value << (SIZE - (width));
    value = value >> (SIZE - (width + lsb));
    UNSIGNED_T val_mask = ~0;
    val_mask = val_mask << (SIZE - (width));
    val_mask = val_mask >> (SIZE - (width + lsb));
    val_mask = val_mask & value;


    temp = temp | val_mask;

    return temp;
}
