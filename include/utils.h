/**
 * @file utils.h
 * @brief Header file for utily functions such as read and write on file.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <time.h>
#include "quadtree.h"
#include "image.h"
#include "bit.h"

/**
 * @brief Reads a PGM image from a file.
 * @param filename Path to the PGM file.
 * @return Pointer to the allocated Image structure.
 */
Image * read_pgm(const char *filename);

/**
 * @brief Writes a BitStream to a QTC file.
 * @param filename Path to the output QTC file.
 * @param stream Pointer to the BitStream o write.
 * @param quadtree Pointer to the associaed Quadtree.
 */
void write_qtc(const char *filename, BitStream *stream, Quadtree *quadtree);

/**
 * @brief Reads a BitStream from a QTC file.
 * @param filename Path to the QTC file.
 * @return Pointer to the reconstructed BitStream.
 */
BitStream * read_qtc(const char *filename);

/**
 * @brief Writes a PGM image in P5 format.
 * @param filename Path to the output PGM file.
 * @param image Pointer to the Image structure to write.
 */
void write_pgm(const char * filename, Image * image);

#endif // UTILS_H