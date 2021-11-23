/**
 * @file tree_structure.c
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Functions for tree handling.
 * C re-implementation of the scikit-learn Tree object from
 * sklearn.tree._tree.pyx
 */

#include "../trees.h"

/**
 * @brief Function to realloc some memory to a growing Tree.
 *
 * @param p
 * @param nelems
 * @return size_t*
 */
static size_t *safe_realloc(size_t *p, size_t nelems) {
    // TODO
    return p;
}

/**
 * @brief Constructor for Tree
 *
 * @param tree [in,out] The tree to initialize.
 * @param nfeatures [in] Number of features in X.
 * @param n_classes [in] Number of classes in y[:, k].
 * @param n_outputs [in] Number of outputs in y.
 */
void cinit(Tree *tree, int nfeatures, size_t *n_classes, int n_outputs) {
    tree->nfeatures = nfeatures;
    tree->n_outputs = n_outputs;
    tree->n_classes = NULL;
    safe_realloc(tree->n_classes, n_outputs);

    tree->max_n_classes = 0;
    for (size_t k; k < n_outputs; k++) {
        if (tree->max_n_classes < n_classes[k])
            tree->max_n_classes = n_classes[k];
    }
    tree->value_stride = n_outputs * tree->max_n_classes;

    memcpy(tree->n_classes, n_classes, n_outputs * sizeof(*n_classes));

    tree->max_depth = 0;
    tree->node_count = 0;
    tree->capacity = 0;
    tree->value = NULL;
    tree->nodes = NULL;
}

/**
 * @brief Destructor for Tree.
 *
 * @param tree The tree to destroy.
 */
void dealloc(Tree *tree) {
    /* Free all inner structures */
    free(tree->n_classes);
    free(tree->value);
    free(tree->nodes);
}
