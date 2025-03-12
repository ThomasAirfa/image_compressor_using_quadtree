/**
 * @file bit.c
 * @brief Implementation of a BitStream.
 */

#include "bit.h" 

/**
 * @brief Initializes a BitStream.
 *
 * Allocates memory for the BitStream and initializes it.
 * 
 * @param size Size of the stream in bytes.
 * @return A pointer to the initialized BitStream.
 */
BitStream * initBitStream(int size) {
    BitStream * stream = (BitStream *) malloc(sizeof(BitStream));
    if(!stream) {
        fprintf(stderr, "Memory allocation for BitStream structure failed.\n");
        exit(EXIT_FAILURE);
    }
    stream->start = (unsigned char *) malloc(sizeof(unsigned char) * size);
    if (!stream->start) {
        fprintf(stderr, "Memory allocation for BitStream stream failed.\n");
        exit(EXIT_FAILURE);
    }
    stream->ptr = stream->start;
    stream->capa = CHAR_BIT;         // Capacity of the current byte.
    stream->address = stream->start;
    return stream;
}

/**
 * @brief Retrieves a bit at a specific position from a byte.
 * 
 * @param byte Byte to extract bit from.
 * @param b Position of the bit to extract.
 * @return Value of the extracted bit (0 or 1).
 */
static unsigned char getbit(unsigned char byte, size_t b) {
    return (byte >> b) & 1;
}

/**
 * @brief Sets a specific bit in a byte.
 * 
 * @param byte Pointer to the byte to modify.
 * @param b Position of the bit to set.
 * @param valb Value to set (0 or 1).
 */
static void setbit(unsigned char * byte, size_t b, unsigned char valb) {
    valb ? (*byte |= (1 << b)) : (*byte &= ~(1 << b));
}

/**
 * @brief Pushes bits from a source byte into a BitStream.
 * 
 * Writes bits from the most significant to the least significant position.
 * Automatically manages byte boundaries and advances the stream pointer.
 * 
 * @param curr BitStream to write to.
 * @param src Source byte containing the bits to push.
 * @param nbit Number of bits to push.
 * @return Number of bits successfully written.
 */
static size_t pushbits(BitStream * curr, unsigned char src, size_t nbit) {
    size_t bits_written = 0;
    for(size_t i = 0 ; i < nbit ; i++) {
        if(!curr->capa) { // Checks if the current byte is full, if true, advances the pointer to the next one
            curr->ptr++;
            curr->capa = CHAR_BIT;
        }
        unsigned char bit_to_write = getbit(src, nbit - 1 - i); // Retrieves bits from left to right (most to least significant).
        setbit(curr->ptr, curr->capa - 1, bit_to_write);
        curr->capa--;
        bits_written++;
    }
    return bits_written;
}

/**
 * @brief Pulls bits from a BitStream into a destination byte.
 * 
 * Reads bits from a BitStream from most to least significant position.
 * Automatically manages byte boundaries and advances the stream pointer.
 * 
 * @param curr Bitstream to read (pull) from.
 * @param dest Pointer to the destination byte to store the bits.
 * @param nbit Number of bits to read.
 * @return Number of bits successfully read.
 */
static size_t pullbits(BitStream * curr, unsigned char * dest, size_t nbit) {
    size_t bits_readed = 0;
    *dest = 0;
    for(size_t i = 0 ; i < nbit ; i++) {
        if(!curr->capa) {
            curr->start++;
            curr->capa = CHAR_BIT;
        }
        unsigned char bit_to_read = getbit(*curr->start, curr->capa - 1);
        setbit(dest, nbit - 1 - i, bit_to_read);
        curr->capa--;
        bits_readed++;
    }
    return bits_readed;
}

/**
 * @brief Reads a fixed number of bits from a BitStream.
 * 
 * Handles errors in case the requested number of bits cannot be read.
 * 
 * @param stream BitStream to read from.
 * @param dest Pointer to the destination byte to store the bits.
 * @param n Number of bits to read.
 */
void read_n_bits(BitStream * stream, unsigned char * dest, int n) {
    if (pullbits(stream, dest, n) != (size_t) n) {
        fprintf(stderr, "Erreur lors de la lecture du flux binaire.\n");
        freeBitStream(stream);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Pushes a fixed number of bits into a BitStream.
 * 
 * Handles errors in case the requested number of bits cannot be written.
 * 
 * @param stream BitStream to write to.
 * @param src Source byte containing the bits to push.
 * @param n Number of bits to push.
 */
void push_n_bits(BitStream * stream, unsigned char src, int n) {
    if (pushbits(stream, src, n) != (size_t) n) {
        fprintf(stderr, "Erreur lors de l'écriture des bits\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Finalizes a BitStream by padding the last partially filled byte (if any).
 * 
 * Pads the current byte with zeros if it is partially filled and moves the pointer to the next byte.
 * 
 * @param stream BitStream to finalize.
 */
void finishBitStream(BitStream * stream) {
    if(stream->capa != CHAR_BIT) {
        // adds stram->capa padding bites to the partially filled byte.
        push_n_bits(stream, 0, stream->capa);
        stream->ptr++;
        stream->capa = CHAR_BIT;
    }
}

/**
 * @brief Returns the size of a BitStream in bites.
 * 
 * Calcules the total number of bits written to the BitStream.
 * 
 * @param stream BitStream to measure.
 * @return Total size in bits.
 */
size_t BitStream_size(BitStream * stream) {
    return (stream->ptr - stream->start) * CHAR_BIT + (CHAR_BIT - stream->capa);
}

/**
 * @brief Frees allocated memory of a BitStream.
 * 
 * Safely frees the memory used by a BitStream structure and its buffer.
 * 
 * @param stream BitStream to free.
 */
void freeBitStream(BitStream * stream) {
    if (stream) {
        free(stream->address);
        stream->address = NULL;
    }
    free(stream);
}