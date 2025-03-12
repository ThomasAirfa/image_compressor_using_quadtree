/**
 * @file image.c
 * @brief Implementation of an image.
 * 
 * This file provides functions for allocating and freeing memory of an Image structure.
 */
#include "image.h"

// Fonction d'allocation de mémoire pour une image
/**
 * @brief Allocates memory for an image structure.
 * 
 * Allocates memory for an Image reprensentation and its pixel data.
 * 
 * @param widthWidth of the image (assumes the image to be square, width = height).
 * @param image_size Total number of pixel in the image (width * height).
 * @param max_val Maximum grayscale value for the Image (max 255).
 * @return Pointer to the allocated Image.
 * 
 * @note Exits program with an error message if memory allocation fails.
 */
Image* allocate_image(int width, int image_size, int max_val) {
    Image *image = (Image*) malloc(sizeof(Image));
    if (!image) {
        fprintf(stderr, "Error while allocating memory for the image structure.\n");
        exit(EXIT_FAILURE);
    }
    image->width = width;
    image->image_size = image_size;
    image->max_val = max_val;
    image->image = (unsigned char*) malloc(image_size * sizeof(unsigned char));
    if (!image->image) {
        fprintf(stderr, "Error while allocating memory for the image's pixel data.\n");
        exit(EXIT_FAILURE);
    }
    return image;
}

/**
 * @brief Frees allocated memory for an Image.
 * 
 * Frees allocated memory for an Image structure.
 * 
 * @param image Pointer to the Image to be freed.
 */
void free_image(Image * image) {
    free(image->image);
    free(image);
}