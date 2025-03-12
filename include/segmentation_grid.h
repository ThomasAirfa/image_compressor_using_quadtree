/**
 * @file segmentation_grid.h
 * @brief Header file for segmentation_grid operations.
 */

#ifndef SEGMENTATION_GRID_H
#define SEGMENTATION_GRID_H

#include "quadtree.h"
#include "image.h"
#include <string.h>

/**
 * @brief Generates a segmentation grid from a Quadtree.
 * @param quadtree Pointer to the Quadtree.
 * @return Pointer to the generated semgentation grid as an Image.
 */
Image * generate_segmentation_grid(Quadtree * quadtree);

#endif // SEGMENTATION_GRID_H