/**
 * @file quadtree.c
 * @brief Quadtree implementation.
 */
#include "quadtree.h"

/**
 * @brief Memory allocation for Quadtree nodes.
 * 
 * Allocates memory for Quadtree nodes and handles errors.
 * 
 * @param total_nodes Quadtree nodes number.
 * 
 * @note Exits program with an error message if memory allocation fails.
 */
static Node* allocate_nodes(int total_nodes) {
    Node *nodes = (Node*) malloc(total_nodes * sizeof(Node));
    if (!nodes) {
        fprintf(stderr, "Error while allocating memory for Quadtree nodes.\n");
        exit(EXIT_FAILURE);
    }
    return nodes;
}

/**
 * @brief Memory allocation for Quadtree structure.
 * 
 * Allocates memory for Quadtree structure and handles errors.
 * 
 * @note Exits program with an error message if memory allocation fails.
 */
static Quadtree* allocate_quadtree() {
    Quadtree *quadtree = (Quadtree*) malloc(sizeof(Quadtree));
    if (!quadtree) {
        fprintf(stderr, "Error while allocating memory for Quadtree structure.\n");
        exit(EXIT_FAILURE);
    }
    return quadtree;
}

/**
 * @brief Checks if the given node is a leaf or not.
 * 
 * Checks if the given node is a leaf or not.
 * 
 * @param quadtree Current quadtree.
 * @param index Index of the node we want to check. 
 */
int is_leaf(Quadtree * quadtree, int index) {
    return index >= quadtree->total_nodes - (1 << (2 * quadtree->levels));
}

/**
 * @brief Creates and initializes an empty Quadtree.
 * 
 * Creates an empty Quadtree and initializes its values.
 * 
 * @param levels Quadtree levels.
 */
Quadtree* create_empty_quadtree(int levels) {
    int total_nodes = 0;
    for (int i = 0; i <= levels; i++) {
        total_nodes += (1 << (2 * i)); // 4^i
    }
    // Memory allocation 
    Quadtree *quadtree = allocate_quadtree();
    quadtree->nodes = allocate_nodes(total_nodes);
    quadtree->total_nodes = total_nodes;
    quadtree->levels = levels;
    quadtree->medvar = 0.;
    quadtree->maxvar = 0.;
    return quadtree;
}

/**
 * @brief Frees allocated memory for a Quadtree.
 * 
 * Frees allocated memory for the Quadtree nodes and structure.
 * 
 * @param quadtree Pointer to the Quadtree to be freed.
 */
void free_quadtree(Quadtree *quadtree) {
    free(quadtree->nodes);
    free(quadtree);
}