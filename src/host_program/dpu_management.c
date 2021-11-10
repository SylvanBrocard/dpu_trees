/**
 * @file dpu_management.c
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Utility functions for DPU management
 *
 */

#include <dpu.h>
#include <dpu_log.h>

#include "../trees.h"

 /**
 * @brief Allocates all DPUs
 *
 * @param p Algorithm parameters.
 */
void allocate(Params *p)
{
    DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, NULL, &p->allset));
    DPU_ASSERT(dpu_get_nr_dpus(p->allset, &p->ndpu));
}

/**
 * @brief Frees all DPUs
 *
 * @param p Algorithm parameters.
 */
void free_dpus(Params *p)
{
    DPU_ASSERT(dpu_free(p->allset));
}

/**
 * @brief Loads a binary in the DPUs.
 *
 * @param p Algorithm parameters.
 * @param DPU_BINARY path to the binary
 */
void load_kernel(Params *p, const char *DPU_BINARY)
{
    DPU_ASSERT(dpu_load(p->allset, DPU_BINARY, NULL));
}
