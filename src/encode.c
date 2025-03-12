/**
 * @file encode.c
 * @brief Implementation of the encoding operations.
 */

#include "encode.h"

// Fonction qui écrit un noeud qui n'est pas une feuille dans le BitStream
/**
 * @brief Writes a non-leaf node.
 * 
 * Writes the `moyenne`, `epsilon` and if necessary, the `u` bit of a node into a BitStream.
 * If a node is the 4th we only writes `epsilon` and if necessary te `u` bit, decoding will interpolates the value of `moyenne`.
 * 
 * @param stream BitStream where we write the data.
 * @param node Current node that we write data from.
 * @param index Index of the current node.
 */
static void write_node(BitStream * stream, Node * node, int index) {
    if (index % 4 || !index){                   // !index to write the root, index % 4 to write the first 3 childs
        push_n_bits(stream, node->moyenne, 8);
    }
    push_n_bits(stream, node->epsilon, 2);
    if (!node->epsilon) {                       // If epsilon == 0, writes the u bit
        push_n_bits(stream, node->u, 1);
    } 
}

// Fonction qui érit un noeud qui est une feuille dans le BitStream
/**
 * @brief Writes a leaf node.
 * 
 * Writes only the `moyenne` of a leaf node because leaf have per definition epsilon = 0 and u = 1.
 * 
 * @param BitStream BitStream where we write the data.
 * @param node Current node that we write data from.
 * @param index Index of the current node.
 */
static void write_leaf(BitStream * stream, Node * node, int index) {
    if (index % 4) { // We write only the first 3 childs, decoding will interpolates th 4th
        push_n_bits(stream, node->moyenne, 8);
    }
}

/**
 * @brief Encodes a Quadtree into a BitStream.
 * 
 * Encodes Quadtree data into a BitStream, following the current logic :
 * - Writes `moyenne` on 8 bits.
 * - Writes `epsilon` on  2 bits.
 * - If `epsilon` = 0, writes `u` on 1 bit.
 * - `moyenne` is not written for the 4th child of a node, decoding will interpolates its value.
 * For a leaf, only writes `moyenne` because `epsilon` and `u` are constant.
 * 
 * @param quadtree Quadtree to encode.
 * @return BitStream representation of the Quadtree.
 */
BitStream * encode(Quadtree * quadtree) {
    BitStream * stream = initBitStream(quadtree->total_nodes * 2); // We write at max 11 bits per node, so allacoting 2 bytes per node for the BitStream buffer
    push_n_bits(stream, quadtree->levels, 8);                      // Writes quadtree's levels
    for (int i = 0 ; i < quadtree->total_nodes ; i++) {
        Node * current_node = &quadtree->nodes[i];

        // To avoid case where the root is uniform or to iterate on itself while it's not initialized
        if (!i) {
            write_node(stream, current_node, i);
            continue;
        }

        Node * current_node_parent = &quadtree->nodes[(i - 1) / 4];

        // If parent node is uniform, ignores his childs
        if (current_node_parent->u) {
            continue;
        }

        // If i is a leaf index, writes it has a leaf, elsewise writes it a non-leaf node (leaf index starts at quadtree->total_nodes - (1 << (2 * quadtree->levels)))
        (is_leaf(quadtree, i)) ? write_leaf(stream, current_node, i) : write_node(stream, current_node, i);
    }
    finishBitStream(stream);
    return stream;
}

// Fonction récursive pour construire un quadtree à partir d'une image
/**
 * @brief Builds recursively a Quadtree from an Image.
 * 
 * Helper function to build recursively a Quadtree from an Image.
 * 
 * @param quadtree Quadtree to fill.
 * @param image Image to build Quadtree from.
 * @param size Size of the current bloc.
 * @param index Index of the current node.
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 */
static void build_quadtree_rec(Quadtree *quadtree, Image *image, int size, int index, int x, int y) {
    // If a bloc is of size 1, it's a leaf
    if (size == 1) {
        quadtree->nodes[index].moyenne = image->image[y * image->width + x];
        quadtree->nodes[index].epsilon = 0;
        quadtree->nodes[index].u = 1;
        quadtree->nodes[index].v = 0.;
        return;
    }

    // Size of the child bloc
    int child_size = size / 2;

    // Builds recursively the 4 children clockwise
    build_quadtree_rec(quadtree, image, child_size, 4 * index + 1, x, y);                           // Top left 
    build_quadtree_rec(quadtree, image, child_size, 4 * index + 2, x + child_size, y);              // Top right 
    build_quadtree_rec(quadtree, image, child_size, 4 * index + 3, x + child_size, y + child_size); // Bottom right
    build_quadtree_rec(quadtree, image, child_size, 4 * index + 4, x, y + child_size);              // Bottom left

    // Retrieves moyenne, uniformity bit and variance of the 4 childs into arrays
    unsigned char child_moyennes[4];
    unsigned char child_u[4];
    double child_v[4];
    int somme_moyennes = 0;
    for (int i = 0; i < 4; i++) {
        child_moyennes[i] = quadtree->nodes[4 * index + i + 1].moyenne;
        child_u[i] = quadtree->nodes[4 * index + i + 1].u;
        child_v[i] = quadtree->nodes[4 * index + i + 1].v;
        somme_moyennes += child_moyennes[i];
    }

    // Calculate moyenne and epsilon of the current node
    quadtree->nodes[index].moyenne = somme_moyennes / 4;
    quadtree->nodes[index].epsilon = somme_moyennes % 4;

    // Calculate variance of the current node
    double somme_v = 0.;
    for (int i = 0; i < 4; i++) {
        double m_diff = quadtree->nodes[index].moyenne - child_moyennes[i];
        somme_v += (child_v[i] * child_v[i]) + (m_diff * m_diff);
    }
    quadtree->nodes[index].v = sqrt(somme_v) / 4;

    // Update medvar and maxvar of the Quadtree
    quadtree->medvar += quadtree->nodes[index].v;
    if (quadtree->nodes[index].v > quadtree->maxvar) {
        quadtree->maxvar = quadtree->nodes[index].v;
    }

    // Checks if the node is uniform
    quadtree->nodes[index].u = (child_moyennes[0] == child_moyennes[1] &&
                                child_moyennes[1] == child_moyennes[2] &&
                                child_moyennes[2] == child_moyennes[3] &&
                                child_u[0] == 1 &&
                                child_u[1] == 1 &&
                                child_u[2] == 1 &&
                                child_u[3] == 1) ? 1 : 0;
}

/**
 * @brief Build a Quadtree from an Image.
 * 
 * Builds a Quadtree from an Image.
 * 
 * @param image Image to build Quadtree from.
 * @return Quadtree representation of the given Image.
 */
Quadtree* build_quadtree_from_image(Image *image) {
    int n = log2(image->width); // Levels of the Quadtree
    Quadtree * quadtree = create_empty_quadtree(n);
    // Quadtree construction
    int child_size = image->width;
    build_quadtree_rec(quadtree, image, child_size, 0, 0, 0);
    quadtree->medvar /= quadtree->total_nodes - (1 << (2 * quadtree->levels));
    return quadtree;
}

/**
 * @brief Filtering Quadtree (lossy compression).
 * 
 * Filter recursively the Quadtree based on an given alpha and sigma (medvar/maxvar).
 * 
 * @param quadtree Quadtree to filter.
 * @param node Current node.
 * @param index Index of the current node.
 * @param sigma medvar/maxvar of the Quadtree.
 * @param alpha Given value to filter based on.
 */
int filtrage(Quadtree * quadtree, Node * node, int index, double sigma, double alpha) {
    // If a node is uniform returns 1 (as a leaf is always uniform no need to check if the node is a leaf)
    if (node->u) return 1;
    // Filtering sum of the 4 childs
    int s = 0;
    s += filtrage(quadtree, &quadtree->nodes[4 * index + 1], 4 * index + 1, sigma * alpha, alpha);
    s += filtrage(quadtree, &quadtree->nodes[4 * index + 2], 4 * index + 2, sigma * alpha, alpha);
    s += filtrage(quadtree, &quadtree->nodes[4 * index + 3], 4 * index + 3, sigma * alpha, alpha);
    s += filtrage(quadtree, &quadtree->nodes[4 * index + 4], 4 * index + 4, sigma * alpha, alpha);
    // If the 4 childs are not unifrom or if node's variance > sigma, we can't uniformize the node
    if (s < 4 || node->v > sigma) return 0;
    // Node is uniformized
    node->epsilon = 0;
    node->u = 1;
    return 1;
}