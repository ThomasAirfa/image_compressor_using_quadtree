/**
 * @file bit.h
 * @brief Headers file for the BitStream structure and operations.
 */

#ifndef BIT_H
#define BIT_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // for CHAR_BIT

/**
 * @struct BitStream
 * @brief Represents a stream of bits for encoding/decoding operations.
 */
typedef struct {
    unsigned char * start; ///< Pointer to the start of the buffer.
    unsigned char * ptr;   ///< Pointer to the current byte in the buffer.
    size_t capa;           ///< Capacity of the current byte.
    void * address;        ///< Pointer to the start of the buffer (easier for freeing allocated memory because of start and ptr manipulation).
} BitStream;   

/**
 * @brief Initializes a BitStream.
 * @param size Size of the stream in bytes.
 * @return Pointer to the initialized BitStream.
 */
BitStream * initBitStream(int size);

/**
 * @brief Reads n bits from a BitStream.
 * @param stream BitStream to read from.
 * @param dest Pointer to the destination byte to store the bits.
 * @param n Number of bits to read.
 */
void read_n_bits(BitStream * stream, unsigned char * dest, int n);

/**
 * @brief Writes n bits to a BitStream.
 * @param stream BitStream to write to.
 * @param src Source byte containing the bits to write.
 * @param n Number of bits to write.
 */
void push_n_bits(BitStream * stream, unsigned char src, int n);

/**
 * @brief Checks if a byte is partially filled, if so, fills it with padding bits and advances the pointer.
 * @param stream BitStream to check and fill.
 */
void finishBitStream(BitStream * stream);

/**
 * @brief Returns the size in bits of a BitStream.
 * @param stream BitStream to measure.
 * @return Size in bits of the BitStream.
 */
size_t BitStream_size(BitStream * stream);

/**
 * @brief Frees the memory allocated for a BitStream.
 * @param stream BitStream to free.
 */
void freeBitStream(BitStream * stream);

#endif // BIT_H