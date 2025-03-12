#include "qtc.h"
#include <getopt.h>
#include <string.h>
#define MAX_SIZE 256

// Displays help message (-h) on the standard output
static void print_help(char ** argv) {
    fprintf(stdout, " Usage: %s [-c|-u|-g] [-v] [-i input.{pgm|qtc}] [-o output.{qtc|pgm}] [-a alpha] [-h].\n"
                "-c : Encodes a PGM image into QTC format.\n"
                "-u : Decodes a QTC file into a PGM image.\n"
                "-g : Generates a segmentation grid from a PGM image.\n"
                "-v : Enables verbose mode (displays additional information during execution).\n"
                "-i : Specifies the input file.\n"
                "-o : Specifies the output file.\n"
                "-a : Specifies the alpha parameter for filtering (lossy compression). This option is applicable only with -c."
                "\n\tAlpha value range:"
                "\n\t\t- alpha <= 1.0 -> no filtering, no additional compression gain."
                "\n\t\t- alpha ~ 1.5 -> moderate filtering, reasonable compression gain."
                "\n\t\t- alpha >= 2.0 -> excessive filtering, significantly degraded image quality.\n"
                "-h : Displays this help message.\n", argv[0]);
}

// Function that checks if the given file extension matches the expected extension
static int check_file_extension(const char * filename, const char * expected_extension) {
    const char * extension = strrchr(filename, '.'); // Searches for the last '.' in the filename
    if (!extension || extension == filename) {
    }
    return strcmp(extension + 1, expected_extension) == 0; // Compares the extracted extension with the expected extension
}

// Function to generate a segmentation grid from a quadtree
static void handle_segmentation_grid(const char *grid_file, Quadtree *quadtree, int verbose) {
    if (verbose) fprintf(stdout, "Generating segmentation grid.\n");
    Image *segmentation_grid = generate_segmentation_grid(quadtree);
    write_pgm(grid_file, segmentation_grid);
    if (verbose) fprintf(stdout, "Segmentation grid generated. File written: %s\n", grid_file);
    free_image(segmentation_grid);
}

int main (int argc, char **argv) {
    int c = 0, u = 0, v = 0, g = 0, option;
    double alpha = 0.;
    char input_file[MAX_SIZE] = "";
    char output_file[MAX_SIZE] = "";
    char grid_file[MAX_SIZE] = "";

    while ((option = getopt(argc, argv, "cuvgi:o:a:h")) != -1) {
        switch (option) {
            case 'c':
                c = 1; // Encode
                break;
            case 'u':
                u = 1; // Decode
                break;
            case 'v':
                v = 1; // Verbose
                break;
            case 'g': 
                g = 1; // Segmentation grid
                break;
            case 'i':
                strncpy(input_file, optarg, sizeof(input_file) - 1);
                input_file[sizeof(input_file) - 1] = '\0';
                break;
            case 'o':
                strncpy(output_file, optarg, sizeof(output_file)-1);
                output_file[sizeof(output_file) - 1] ='\0';
                break;
            case 'a': 
                alpha = atof(optarg);
                if (alpha < 0) {
                    fprintf(stderr, "Alpha value must be greater than 0.\n");
                    print_help(argv);
                    return EXIT_FAILURE;
                }
                break;
            case 'h': // Help
                print_help(argv);
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Unknown option: %c\n", optopt);
                return EXIT_FAILURE;
        }
    }
    // Encoding and decoding cannot be done simultaneously
    if ((c + u) != 1) {
        fprintf(stderr,"You must choose either -c (encoding) or -u (decoding).\n");
        return EXIT_FAILURE;
    }
    // If no input file is provided
    if (strlen(input_file) == 0) {
        fprintf(stderr, "You did not provide an input file with -i.\n");
        return EXIT_FAILURE;
    }
    // If no output file is provided, use a default filename
    if (strlen(output_file) == 0) snprintf(output_file, MAX_SIZE, "%s", c ? "QTC/out.qtc" : "PGM/out.pgm");
    // If necessary, construct the filename for the segmentation grid
    if (g) {
        const char * basename = output_file; // By default, if no '/'
        const char * last_slash = strrchr(output_file, '/');
        if (last_slash) {
            basename = last_slash + 1; // After the last '/'
        }
        snprintf(grid_file, sizeof(grid_file), "PGM/%s", basename);
        // Add the "_g.pgm" suffix for the grid
        char * dot = strrchr(grid_file, '.'); // Locate the last extension
        if (dot) *dot = '\0'; // Replace the '.' with '\0' to remove the extension
        strncat(grid_file, "_g.pgm", sizeof(grid_file) - strlen(grid_file) - 1);
    }
    // Encoding case
    if (c) {
        // Validate the file extension
        if (!check_file_extension(input_file, "pgm")) {
            fprintf(stderr, "Input file must be in PGM format.\nCheck the file extension provided to the -i option.\n");
            return EXIT_FAILURE;
        }
        if (v) fprintf(stdout, "Encoding image %s started.\n", input_file);
        // Build the image and quadtree
        Image * image = read_pgm(input_file);
        Quadtree * quadtree = build_quadtree_from_image(image);
        // If alpha is provided, apply quadtree filtering
        if (alpha) {
            filtrage(quadtree, quadtree->nodes, 0, quadtree->medvar / quadtree->maxvar, alpha);
            if (v) fprintf(stdout, "Lossy compression applied with alpha = %.2f\n", alpha);
        }
        // Generate the segmentation grid
        if (g) handle_segmentation_grid(grid_file, quadtree, v);
        // Encode the quadtree
        BitStream * stream = encode(quadtree);
        write_qtc(output_file, stream, quadtree);
        if (v) fprintf(stdout, "Encoding completed. File written: %s\n", output_file);
        free_image(image);
        free_quadtree(quadtree);
        freeBitStream(stream);
    }
    // Decoding case
    if (u) {
        // Validate the file extension
        if (!check_file_extension(input_file, "qtc")) {
            fprintf(stderr, "Input file must be in QTC format.\nCheck the file extension provided to the -o option.\n");
            return EXIT_FAILURE;
        }
        if (v) fprintf(stdout, "Decoding file %s started.\n", input_file);
        // Decode the quadtree
        BitStream * stream = read_qtc(input_file);
        Quadtree * quadtree = decode(stream);
        Image * image = build_image_from_quadtree(quadtree);
        write_pgm(output_file, image);
        if (g) handle_segmentation_grid(grid_file, quadtree, v);
        if (v) fprintf(stdout, "Decoding completed. File written: %s\n", output_file);
        // Generate the segmentation grid
        freeBitStream(stream);
        free_quadtree(quadtree);
        free_image(image);
    } 
    return EXIT_SUCCESS;
}
