/**
 * @file common.h
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Common data structures and info for host and DPU
 *
 */

#ifndef _H_TREES_COMMON
#define _H_TREES_COMMON /**< header guard */

/** @name Constraints
 * @brief Data size constraints
 */
/**@{*/
#define MAX_FEATURE_DPU                                                        \
    5000000 /**< How many features we fit into one DPU's MRAM. Can be          \
               increased further. */
#define MAX_SAMPLES_DPU                                                        \
    5000000 /**< How many samples we fit into one DPU's MRAM. Can be increased \
               further. */
#define MAX_STACK_DPU                                                          \
    500 /**< How many split records we fit into one DPU's MRAM. Can be         \
           increased further. */
#define MAX_CLASSES 32 /**< How many different target classes we allow */
/**@}*/

// Define the size of features (choose one):

typedef float feature_t;
////////// OR
// typedef int8_t int_feature;
////////// OR
// typedef int16_t int_feature;
////////// OR
// typedef int32_t int_feature;
// #define FEATURETYPE_32

#endif
