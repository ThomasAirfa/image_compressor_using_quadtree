/**
 * @file encode.h
 * @brief Header file for encoding operations.
 */

#ifndef ENCODE_H
#define ENCODE_H

#include "quadtree.h"
#include "image.h"
#include "bit.h"

/**
 * @brief Encodes a Quadtree into a BitStream.
 * @param quadtree Quadtree to encode.
 * @return BitStream representation of the Quadtree.
 */
BitStream * encode(Quadtree * quadtree);

/**
 * @brief Build a Quadtree from an Image.
 * @param image Image to build Quadtree from.
 * @return Quadtree representation of the given Image.
 */
Quadtree* build_quadtree_from_image(Image *image);

/**
 * @brief Filtering Quadtree (lossy compression).
 * @param quadtree Quadtree to filter.
 * @param node Current node.
 * @param index Index of the current node.
 * @param sigma medvar/maxvar of the Quadtree.
 * @param alpha Given value to filter based on.
 */
int filtrage(Quadtree * quadtree, Node * node, int index, double sigma, double alpha);

#endif // ENCODE_H