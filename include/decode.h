/**
 * @file decode.h
 * @brief Header file for the decoding operations.
 */

#ifndef DECODE_H
#define DECODE_H

#include "quadtree.h"
#include "image.h"
#include "bit.h" 

/**
 * @brief Constructs a quadtree from a BitStream.
 * @param stream BitStream to read from.
 * @return Pointer to the constructed Quadtree.
 */
Quadtree * decode(BitStream * stream);

/**
 * @brief Builds an image from a quadtree.
 * @param quadtree Quadtree representation of the Image. 
 * @return Pointer to the constructed Image.
 */
Image * build_image_from_quadtree(Quadtree * quadtree);

#endif // DECODE_H