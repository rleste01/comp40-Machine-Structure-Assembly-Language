/*
 *
 * 
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "assert.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

int main (int argc, char *argv[]){
    assert(argc == 3);

    FILE *file1 = NULL;
    FILE *file2 = NULL;
    if (strcmp(argv[1], "-") == 0) {
        file1 = stdin;
    } else {
        file1 = fopen(argv[1], "rb");
    }
    if (strcmp(argv[2], "-") == 0) {
        file2 = stdin;
    } else {
        file2 = fopen(argv[2], "rb");
    }
    assert(file1 != NULL && file2 != NULL);

    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods != NULL);
    Pnm_ppm image1 = Pnm_ppmread(file1, methods);
    Pnm_ppm image2 = Pnm_ppmread(file2, methods);
    if (abs((int) (image1 -> height - image2 -> height)) > 1) {
        fprintf(stderr, "1.0\n");
        exit(EXIT_SUCCESS);
    }
    int height,
        width;
    if (image1 -> height > image2 -> height) {
        height = image2 -> height;
    } else {
        height = image1 -> height;
    }
    if (image1 -> width > image2 -> width) {
        width = image2 -> width;
    } else {
        width = image1 -> width;
    }
    double sum = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Pnm_rgb pix1 = (Pnm_rgb) (image1 -> methods -> at(image1 -> pixels, i, j));
            Pnm_rgb pix2 = (Pnm_rgb) (image2 -> methods -> at(image2 -> pixels, i, j));
            double red = (double)(pix1 -> red) / (image1 -> denominator) -
                         (double)(pix2 -> red) / (image2 -> denominator);
            red = red * red;
            double green = (double)(pix1 -> green) / (image1 -> denominator) -
                           (double)(pix2->green) / (image2 -> denominator);
            //fprintf(stdout, "%.4f\n", green);
            green = green * green;
            //fprintf(stdout, "%.4f\n", green);
            double blue = (double)(pix1 -> blue) / (image1 -> denominator) -
                          (double)(pix2 -> blue) / (image2 -> denominator);
            blue = blue * blue;
            sum += (red + blue + green);
        }
    }
    double total = sum / (3 * width *height);
    double root =  sqrt(total);
    fprintf(stdout, "%.4f\n", root);
    return 0;
}
