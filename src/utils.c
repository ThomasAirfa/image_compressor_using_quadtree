/**
 * @file utils.c
 * @brief Implementation of utility functions for handling PGM images and QTC files.
 * 
 * This file contains utility functions for reading and writing PGM images and QTC files.
 */

#include "utils.h"

/*
Why did i use fgets and sscanf, because in the documentation of fscanf,
it is recommended not to use fscanf and instead use fgets and sscanf.

Possible improvements : 

- Change the retrun value of readline to an int and retrun the number of elements in the line.
This would help avoid the case where the dimensions are each on a separate line, as currently, 
the dimensions are read on he same line as he grayscale value.

- Add a function specifically for reading the image dimension to reduce code duplication in the two functions that handle reading PGM files.
*/

/**
 * @brief Global variable used to store compression information.
 * 
 * Stocks metadata about the compression process such as the date and compression rate.
 */
char compression_info[128]; 

/**
 * @brief Reads a line from a file and handles errors.
 * 
 * Reads a line of text from the specified file. If an error occurs during the read,
 * an error message is printed, and the program exits.
 * 
 * @param file Pointer to the file to read from.
 * @param line Buffer to store the line.
 * @param size Size of the buffer.
 * @param error_message Error message to display in case of failure.
 */
static void readline(FILE * file, char * line, int size, char * error_message) {
    if (!fgets(line, size, file)) {
        fprintf(stderr, "%s\n", error_message);
        fclose(file);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Skips comments in a PGM file.
 * 
 * Reads and skips linnes starting with `#` (comments) in a PGM file.
 * Stops when a non-comment line occurs.
 * 
 * @param file File pointer to read from.
 * @param line Buffer to store the line.
 * @param size Size of the buffer.
 */
static void ignore_comments(FILE * file, char * line, int size) {
    unsigned char c = fgetc(file); // Read the line first caracter
    while(c == '#') {
        readline(file, line, size, "Error while reading comments.");
        c = fgetc(file); 
    }
    ungetc(c, file); // Putback the readed caracter if its not a #
    return;
}

/**
 * @brief Reads an int value from a line.
 * 
 * Parses an int value from a line of text. If parsing fails, an error message is displayed,
 * and the program exits.
 * 
 * @param file Pointer to file for error handling.
 * @param line Line to parse.
 * @param value Pointer to store the parsed int value.
 * @param error_message Error message to display in case of failure.
 */
static void scan_int(FILE * file, char * line, int * value, char * error_message) {
    if (sscanf(line, "%d", value) != 1) {
        fprintf(stderr, "%s\n", error_message);
        fclose(file);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Validates a pixel value.
 * 
 * Checks if a pixel value is within the valid range (0 to max_val).
 * If the value is invalid, an error message is displayed, and the program exits.
 * 
 * @param file Pointer to file for error handling.
 * @param image Pointer to the Image for cleanup in case of error.
 * @param pxeil Pixel value to check.
 * @param max_val Maximum valid pixel value.
 */
static void check_pixel_value(FILE * file, Image * image, int pixel, int max_val) {
    if (pixel < 0 || pixel > max_val) {
        fprintf(stderr, "EError invalid pixel value.\n");
        fclose(file);
        free(image);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Reads a PGM image in P2 format.
 * 
 * Reads an ASCII PGM image (P2 format) from the specified file and stores it in an Image struture.
 * 
 * @param file Pointer to file to read from.
 * @preturn Pointer to the allocated Image structure.
 */
static Image* read_pgm_P2(FILE * file) {
    char line[128];
    int width, height, max_val;

    // Reads file format
    readline(file, line, sizeof(line), "Error while reading file format.");

    // Ignoring comments
    ignore_comments(file, line, sizeof(line));

    // Reads width and height
    readline(file, line, sizeof(line), "Error while reading width/height.");
    scan_int(file, line, &width, "Error while reading image width.");
    scan_int(file, line, &height, "Error while reading image height.");

    // Reads grayscale
    readline(file, line, sizeof(line), "Error while reading max grayscale value.");
    scan_int(file, line, &max_val, "Error while reading max grayscale value.");

    // Allocation memory for Image structure
    int image_size = width * height;
    Image *image = allocate_image(width, image_size, max_val);

    // Reads Image pixels
    char data[image_size * 4]; // a line = 4 * image_size (max val = 255 so 3 char + space 4 char) * image_size number of pixel on a line
    int i = 0;
    while(i < image_size) {
        readline(file, data, sizeof(data), "Error while reading pixels values.");
        // Use strtok to split pixels values on a same line
        char * token = strtok(line, " ");
        while (token != NULL && i < image_size) {
            int pixel = strtol(token, NULL, 10); // Convertion of a string to a long base 10
            check_pixel_value(file, image, pixel, max_val);
            image->image[i] = (unsigned char) pixel;
            i++;
            token = strtok(NULL, " "); // Move the pointer on token
        }
    }

    fclose(file);
    return image;
}

/**
 * @brief Reads a PGM image in P5 format.
 * 
 * Reads a binary PGM image (P5 format) form the specified file and stores int in an Image structure.
 * 
 * @param file Pointer to file to read from.
 * @return Pointer to the allocated Image structure.
 */
static Image* read_pgm_P5(FILE * file) {
    char line[128];
    int width, height, max_val;

    // Reads file format
    readline(file, line, sizeof(line), "Error while reading file format.");

    // Ignores comments
    ignore_comments(file, line, sizeof(line));

    // Reads width and height
    readline(file, line, sizeof(line), "Error while reading width/height.");
    scan_int(file, line, &width, "Error while reading image width.");
    scan_int(file, line, &height, "Error while reading image height.");

    // reads grayscale
    readline(file, line, sizeof(line), "Error while reading max grayscale value.");
    scan_int(file, line, &max_val, "Error while reading max grayscale value.");

    // Memory allocation for Image structure
    int image_size = width * height;
    Image *image = allocate_image(width, image_size, max_val);

    // Reads image pixels values
    int i = 0;
    while(i < image_size) {
        int pixel = fgetc(file);

        check_pixel_value(file, image, pixel, max_val);
        image->image[i] = pixel;
        i++;
    }

    fclose(file);
    return image;

}

/**
 * @brief Reads a PGM image from a file.
 * 
 * Determines the format of a PGM file (P2 or P5) and calls the appropriate function to read the file.
 * 
 * @param filename Path to the PGM file.
 * @return Pointer to the allocated Image structure.
 */
Image* read_pgm(const char *filename) {
    FILE * file = fopen(filename, "rb"); // Open file in read binary
    if (!file) {
        fprintf(stderr, "Error while opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Reads file format
    char format[3];
    readline(file, format, sizeof(format), "Error while reading file format.\n");

    // Calls the appropriate function
    if (strncmp(format, "P2", 2) == 0) {
        return read_pgm_P2(file);
    } else if (strncmp(format, "P5", 2) == 0) {
        return read_pgm_P5(file);
    } else {
        fprintf(stderr, "Unsupported file format.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Writes a BitStream to a file at Q1 format.
 * 
 * Writes a QTC file using a BitStream representation of the data.
 * 
 * @param file File pointer to write to.
 * @param stream BitStream containing the data.
 * @param quadtree Quadtree to retrieves info such as his levels and total_nodes.
 */
static void write_bitstream_to_file_Q1(FILE * file, BitStream *stream, Quadtree *quadtree) {
    // Wrtting format
    fprintf(file, "Q1\n");

    // Adds comments with date of creation and compression rate
    time_t now = time(NULL);
    if (now == (time_t)(-1)) {
        fprintf(stderr, "Error while retrieving date.\n");
        exit(EXIT_FAILURE);
    }
    snprintf(compression_info, sizeof(compression_info), "# Compression date : %s", ctime(&now));
    fprintf(file, "%s", compression_info);

    size_t original_image_size = (1 << quadtree->levels) * (1 << quadtree->levels) * 8; // Size of an Image in bits
    size_t compressed_image_size = (stream->ptr - stream->start) * 8 - 8; // Compressed size in bits
    double compression_rate = 100.0 * compressed_image_size / original_image_size; // Compression rate
    fprintf(file, "# Compression rate %.2f%%\n", compression_rate);
    
    // Wrtting encoded data
    size_t bytes_to_write = stream->ptr - stream->start;
    if (fwrite(stream->start, 1, bytes_to_write, file) != bytes_to_write) {
        fprintf(stderr, "Error while reading encoded data.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

/**
 * @brief Writes a BitStream to a QTC file.
 * 
 * Writes a BitStream and metadata (including compression information) to a QTC file.
 * 
 * @param filename Path to the output QTC file.
 * @param stream Pointer to the BitStream o write.
 * @param quadtree Pointer to the associaed Quadtree.
 */
void write_qtc(const char * filename, BitStream *stream, Quadtree *quadtree) {
    FILE * file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error while opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    write_bitstream_to_file_Q1(file, stream, quadtree);
}

/**
 * @brief Reads a QTC file and retrieves his data into a BitStream.
 * 
 * Reads a QTC file and retireves his data into a BitStream.
 * 
 * @param file Pointer to file.
 * @return A BitStream filled with the encoded data.
 */
static BitStream * read_bitstream_from_file_Q1(FILE *file) {
    char line[128];
    readline(file, line, sizeof(line), "Error while reading file format.");
    ignore_comments(file, line, sizeof(line));

    // Save current position
    long pos = ftell(file);

    // Go to the end of file to compute buffer size
    fseek(file, 0, SEEK_END);
    int size = ftell(file) - pos;
    // Replace cursor at the saved position
    fseek(file, pos, SEEK_SET);
    
    // Initialize BitStream
    BitStream * stream = initBitStream(size);

    // Reads encoded data
    unsigned char buffer[size];
    size_t bytes_readed = fread(buffer, 1, size, file);
    if ((int) bytes_readed != size) {
        fprintf(stderr, "Error while reading encoded data.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    // Writes data into BitStream
    for (int i = 0; i < size; i++) {
        push_n_bits(stream, buffer[i], 8);
    }
    finishBitStream(stream);
    fclose(file);
    return stream;
}

/**
 * @brief Reads a BitStream from a QTC file.
 * 
 * Reads encoded data from a QTC file and reconstucts the corresponding BitStream.
 * 
 * @param filename Path to the QTC file.
 * @return Pointer to the reconstructed BitStream.
 */
BitStream * read_qtc(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error while opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    return read_bitstream_from_file_Q1(file);
}

/**
 * @brief Writes a Image into a file at P5 format.
 * 
 * @param file Pointer to file to write to.
 * @param image Pointer to Image to retrieves data from.
 */
static void write_pgm_P5(FILE * file, Image * image) {
    // Writes format
    fprintf(file, "P5\n");
    // Adds comments with the date of compression
    time_t now = time(NULL);
    if (now == (time_t)(-1)) {
        fprintf(stderr, "Error while retrieving date.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s# Decompression date : %s", compression_info, ctime(&now));

    // Writes image dimensions
    fprintf(file, "%d %d\n%d\n", image->width, image->width, image->max_val);

    // Writes pixels values
    for (int i = 0; i < image->image_size; i++) {
        fputc(image->image[i], file);
    }
    fclose(file);
}

/**
 * @brief Writes a PGM image in P5 format.
 * 
 * Writes a binary PGM image (P5 format) to the specified file.
 * Includes metadata such as compression and decompression dates in the header.
 * 
 * @param filename Path to the output PGM file.
 * @param image Pointer to the Image structure to write.
 */
void write_pgm(const char * filename, Image * image) {
    FILE * file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error while opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    // Possiblity to add P2 format writting
    write_pgm_P5(file, image);
}