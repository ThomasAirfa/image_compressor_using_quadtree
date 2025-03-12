/**
 * @file quadtree.h
 * @brief Header file for a Quadtree.
 */

#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char moyenne; // Moyenne on 8 bits
    unsigned char epsilon; // Epsilon on 2 bits 
    unsigned char u;       // Uniformity on 1 bit
    double v;              // Node variance
}Node;

typedef struct {
    Node *nodes;      // Nodes array
    int total_nodes;  // Total_nodes number
    int levels;       // Quadtree levels
    double medvar;    // Average variance of the Quadtree
    double maxvar;    // Maximum variance 
}Quadtree;

/**
 * @brief Checks if the given node is a leaf or not.
 * @param quadtree Current quadtree.
 * @param index Index of the node we want to check. 
 */
int is_leaf(Quadtree * quadtree, int index);

/**
 * @brief Creates and initializes an empty Quadtree.
 * @param levels Quadtree levels.
 */
Quadtree* create_empty_quadtree(int levels);

/**
 * @brief Frees allocated memory for a Quadtree.
 * @param quadtree Pointer to the Quadtree to be freed.
 */
void free_quadtree(Quadtree *quadtree);

#endif // QUADTREE_H