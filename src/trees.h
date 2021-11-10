/**
 * @file trees.h
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Header file for the trees project
 *
 */

#ifndef _H_TREES
#define _H_TREES /**< header guard */

#ifndef _TREES_DPU_KERNEL_H_
#include <dpu.h>
typedef struct dpu_set_t dpu_set;
#endif

// Define the size of discretized features (choose one):

// typedef int8_t int_feature;
////////// OR
typedef int16_t int_feature;
////////// OR
// typedef int32_t int_feature;
// #define FEATURETYPE_32

#ifndef _KMEANS_DPU_KERNEL_H_
// Parameters holding struct
typedef struct Params {
    uint64_t npoints;
    uint64_t npadded;
    uint64_t npointperdpu;
    int nfeatures;
    float scale_factor;
    float threshold;
    float *mean;
    int isOutput;
    int nloops;
    int max_iter;
    uint32_t ndpu;
    dpu_set allset;
    int from_file;
} Params;

// Function declarations
/** @name dpu_management.c */
/**@{*/
void allocate(Params *p);
void free_dpus(Params *p);
void load_kernel(Params *p, const char *DPU_BINARY);
/**@}*/

/** @name input.c */
/**@{*/
void read_bin_input_with_int_targets(Params *p, const char *filename,
                                     float ***features_out, int **targets_out);
void read_txt_input_with_int_target(Params *p, const char *filename,
                                    float ***features_out, int **targets_out);
void save_dat_file_with_int_target(Params *p, const char *filename_in,
                                   float **features, int *targets);
void format_array_input(Params *p, float *data, float ***features_out);
/**@}*/
#endif

#endif
