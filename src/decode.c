/**
 * @file decode.c
 * @brief Implementation of the decoding operations.
 */

#include "decode.h"

/**
 * @brief Reads a non-leaf node from a BitStream.
 * 
 * Reads the `moyenne`, `epsilon` and optionally `u` values  corresponding to 
 * a non-leaf node from the BitStream and stores them in the given node.
 * 
 * @param stream BitStream to read from.
 * @param node Node to store the read data.
 */
static void read_node(BitStream * stream, Node * node) {
    read_n_bits(stream, &node->moyenne, 8);
    read_n_bits(stream, &node->epsilon, 2);
    (!node->epsilon) ? read_n_bits(stream, &node->u, 1) : (node->u = 0);
}

/**
 * @brief Reads a leaf node from a BitStream.
 * 
 * Reads only `moyenne` value because this is the only value in the BitStream for a leaf node,
 * their `epsilon` and `u` are constant : `epsilon` = 1, `u` = 0.
 * 
 * @param stream BitStream to read from.
 * @param node Node to store the read data.
 */
static void read_leaf(BitStream * stream, Node * node) {
    read_n_bits(stream, &node->moyenne, 8);
    node->epsilon = 0;
    node->u = 1;
}

/**
 * @brief Interpolates the average of the 4th child of a node.
 * 
 * Calculates the `moyenne` value of the 4th child node based on the parent node and the three other childs.
 * Reads additionnal values from the BitStream if the node is not a leaf.
 * 
 * @param stream BitStream to read from.
 * @param quadtree Quadtree containing the nodes.
 * @param node Node to store the interpolated data.
 * @param parent_node Parent node of the current node.
 * @param index Index of the current node.
 */
static void interpolation_4th_child(BitStream * stream, Quadtree * quadtree, Node * node, Node * parent_node, int index) {
    node->moyenne = 4 * parent_node->moyenne + parent_node->epsilon - quadtree->nodes[index - 1].moyenne - quadtree->nodes[index - 2].moyenne - quadtree->nodes[index - 3].moyenne;
    if (is_leaf(quadtree, index)) {
        node->epsilon = 0;
        node->u = 1;
        return;
    } 
    read_n_bits(stream, &node->epsilon, 2);
    !node->epsilon ? read_n_bits(stream, &node->u, 1) : (node->u = 0);
}

/**
 * @brief Constructs a quadtree from a BitStream.
 * 
 * Decodes a Quadtree by reading its nodes from the given BitStream.
 * 
 * @param stream BitStream to read from.
 * @return Pointer to the constructed Quadtree.
 */
Quadtree * decode(BitStream * stream) {
    // Read the first byte to determine the levels of the quadtree
    unsigned char levels;
    read_n_bits(stream, &levels, 8);
    Quadtree * quadtree = create_empty_quadtree(levels);

    for (int i = 0; i < quadtree->total_nodes; i++) {
        Node * current_node = &quadtree->nodes[i];

        // Special case for the root to avoid iterating on its parent (itself) while it is not initialized
        if (!i) {
            read_node(stream, current_node);
            continue;
        }   

        Node * current_node_parent = &quadtree->nodes[(i - 1) / 4];

        // if the parent is uniform, then the children are also uniform and their averages are equal to that of the parent node
        if (current_node_parent->u) {
            current_node->moyenne = current_node_parent->moyenne;
            current_node->epsilon = 0;
            current_node->u = 1;
            continue;
        }

        // Interpolation of the 4th child
        if (!(i % 4)) {
            interpolation_4th_child(stream, quadtree, current_node, current_node_parent, i);
            continue;
        }
        
        // if the node is a leaf we read the node as a leaf otherwise we read it as a node
        is_leaf(quadtree, i) ? read_leaf(stream, current_node) : read_node(stream, current_node);
    }
    return quadtree;
}

/**
 * @brief Recursively builds an image from a quadtree.
 * 
 * Helper function to build an Image recursively based on a Quadtree.
 * 
 * @param quadtree Quadtree to build the image from.
 * @param image Image to fill.
 * @param index Index of the current node.
 * @param x X coordinate of the current node.
 * @param y Y coordinate of the current node.
 * @param size Size of the current pixel bloc.
 */
static void build_image_from_quadtree_rec(Quadtree * quadtree, Image * image, int index, int x, int y, int size) {
    if (is_leaf(quadtree, index)) {
        image->image[y * image->width + x] = quadtree->nodes[index].moyenne;
        return;
    }
    // size of the child bloc
    int child_size = size / 2;

    // Builds the 4 children recursively clockwise 
    build_image_from_quadtree_rec(quadtree, image, 4 * index + 1, x, y, child_size);                           // Top left 
    build_image_from_quadtree_rec(quadtree, image, 4 * index + 2, x + child_size, y, child_size);              // Top right 
    build_image_from_quadtree_rec(quadtree, image, 4 * index + 3, x + child_size, y + child_size, child_size); // Bottom right 
    build_image_from_quadtree_rec(quadtree, image, 4 * index + 4, x, y + child_size, child_size);              // Bottom left 
}

/**
 * @brief Builds an image from a quadtree.
 * 
 * Builds an Image by recursively traversing its nodes and assigning pixel values to the corresponding blocks in the Image.
 * 
 * @param quadtree Quadtree representation of the Image. 
 * @return Pointer to the constructed Image.
 */
Image * build_image_from_quadtree(Quadtree * quadtree) {
    int width = 1 << quadtree->levels; // 2^quadtree->levels
    int image_size = width * width;
    Image * image = allocate_image(width, image_size, 255);
    build_image_from_quadtree_rec(quadtree, image, 0, 0, 0, width);
    return image;
}