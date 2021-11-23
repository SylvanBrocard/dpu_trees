/**
 * @file dpu_management.c
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Utility functions for DPU management
 *
 */

#include <dpu.h>
#include <dpu_log.h>

#include "../trees.h"
#include "../trees_common.h"

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

/**
 * @brief Fills the DPUs with their assigned points.
 */
void populateDpu(Params *p,       /**< Algorithm parameters */
                 feature_t **features) /**< [in] array: [npoints][nfeatures] */
{
    /* Iteration variables for the DPUs. */
    struct dpu_set_t dpu;
    uint32_t each_dpu;

    int *nreal_points;                     /* number of real data points on each dpu */
    int64_t remaining_points = p->npoints; /* number of yet unassigned points */

    DPU_FOREACH(p->allset, dpu, each_dpu)
    {
        int next;
        next = each_dpu * p->npointperdpu;
        DPU_ASSERT(dpu_prepare_xfer(dpu, features[next]));
    }
    DPU_ASSERT(dpu_push_xfer(p->allset, DPU_XFER_TO_DPU, "t_features", 0, p->npointperdpu * p->nfeatures * sizeof(feature_t), DPU_XFER_DEFAULT));

    /* telling each DPU how many real points it has to process */
    nreal_points = (int *)malloc(p->ndpu * sizeof(*nreal_points));
    for (int idpu = 0; idpu < p->ndpu; idpu++)
    {
        nreal_points[idpu] = (remaining_points <= 0)                ? 0
                             : (remaining_points > p->npointperdpu) ? p->npointperdpu
                                                                    : remaining_points;
        remaining_points -= p->npointperdpu;
    }

    /* DEBUG : print the number of non-padding points assigned to each DPU */
    // printf("nreal_points :\n");
    // for(int idpu = 0; idpu < ndpu; idpu++)
    // {
    //     printf("%d ", nreal_points[idpu]);
    // }
    // printf("\n");

    DPU_FOREACH(p->allset, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &nreal_points[each_dpu]));
    }
    DPU_ASSERT(dpu_push_xfer(p->allset, DPU_XFER_TO_DPU, "npoints", 0, sizeof(int), DPU_XFER_DEFAULT));

    free(nreal_points);
}
