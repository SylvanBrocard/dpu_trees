/**
 * @file splitter.c
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Functions for splitters
 * Defines the methods that are used by our custom splitter.
 */

#include "../trees.h"
#include "../trees_common.h"

int node_split(double impurity, SplitRecord* split, size_t* n_constant_features) {
    __init_split();

    while (f_i > n_total_constants and /* Stop early if remaining features are
                                          constant */
           (n_visited_features < max_features or
            /* At least one drawn features must be non constant */
            n_visited_features <= n_found_constants + n_drawn_constants)) {
    }
}
