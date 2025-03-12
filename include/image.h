/**
 * @file image.h
 * @brief Header file for an image.
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Quadtree;

typedef struct {
    int width;              // Image's width (we work on square images so width = height)
    int image_size;         // Number of pixels in the image (width * width)
    int max_val;            // Maximum grayscale value
    unsigned char * image;  // Pixel array of the image
}Image;


/**
 * @brief Allocates memory for an image structure.
 * @param widthWidth of the image (assumes the image to be square, width = height).
 * @param image_size Total number of pixel in the image (width * height).
 * @param max_val Maximum grayscale value for the Image (max 255).
 * @return Pointer to the allocated Image.
 */
Image* allocate_image(int width, int image_size, int max_val);

/**
 * @brief Frees allocated memory for an Image.
 * @param image Pointer to the Image to be freed.
 */
void free_image(Image * image);

#endif // IMAGE_H