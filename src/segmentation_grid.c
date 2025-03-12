/**
 * @file segmentation_grid.c
 * @brief Implementation of a segmentation grid.
 * 
 * This file provides functions to generate a segmentation grid from a Quadtre.
 * The segmentation grid is reprensented as an image with visible boundaries of uniform blocs.
 */

#include "segmentation_grid.h"

/**
 * @brief Draws the borders of a uniform bloc using a specific grayscale value (190).
 * 
 * @param image Pointer to the image where the grid is being drawn.
 * @param x X coordinate of the top-left corner of a uniform bloc.
 * @param y Y coordinate of the top-left corner of a uniform bloc.
 * @param size Size of the uniform bloc.
 */
static void build_grid(Image * image, int x, int y, int size) {
    // Draw the top border of the bloc
    if (y > 0) {
        for (int i = 0; i < size; i++) {
            if (x + i < image->width) {
                image->image[(y - 1) * image->width + x + i] = 190;
            }
        }
    }
    // Draw the left border of the bloc
    if (x > 0) {
        for (int i = 0; i < size; i++) {
            if (y + i < image->width) {
                image->image[(y + i) * image->width + x - 1] = 190;
            }
        }
    }
}

/**
 * @brief Recursively builds the segmentation grid by traversing the quadtree.
 * 
 * Helper function,
 * Traverses the quadtree to identify unifrom bloc and draw them in the segmentation grid.
 *
 * @param quadtree Pointer to the Quadtree.
 * @param image Pointer to the image representing the grid.
 * @param size Size of the current bloc.
 * @param index Index of the current node.
 * @param x X coordinate of the top-left corner of the current bloc.
 * @param y Y coordinate of the top-left corner of the current bloc.
 */
static void build_segmentation_grid(Quadtree * quadtree, Image * image, int size, int index, int x, int y) {
    Node * node = &quadtree->nodes[index];
    // If the node is uniform, draw its corresponding bloc
    if (node->u) {
        build_grid(image, x, y, size);
        return;
    }

    // Same logic as the Quadtree construction and image reconstruction
    int child_size = size / 2;
    build_segmentation_grid(quadtree, image, child_size, 4 * index + 1, x, y);
    build_segmentation_grid(quadtree, image, child_size, 4 * index + 2, x + child_size, y);
    build_segmentation_grid(quadtree, image, child_size, 4 * index + 3, x + child_size, y + child_size);
    build_segmentation_grid(quadtree, image, child_size, 4 * index + 4, x, y + child_size);
}

/**
 * @brief Generates a segmentation grid from a Quadtree.
 * 
 * Generates a semgention grid by identifying uniform blocs in the Quadtree.
 * 
 * @param quadtree Pointer to the Quadtree.
 * @return Pointer to the generated semgentation grid as an Image.
 */
Image * generate_segmentation_grid(Quadtree * quadtree) {
    int width = 1 << quadtree->levels; // équivalant à pow(2, quadtree->levels) ou simplement 2^quadtree->levels
    // Allocation de mémoire pour l'image
    Image * image = allocate_image(width, width * width, 255);
    // initialise l'image avec des pixels blancs
    memset(image->image, 255, image->image_size);
    build_segmentation_grid(quadtree, image, width, 0, 0, 0);
    return image;
}