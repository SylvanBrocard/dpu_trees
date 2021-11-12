/**
 * @file preprocessing.c
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief Function for data preprocessing
 *
 */

#include <float.h>

#include "../trees.h"

void preprocessing(Params *p, float **features_float)
{

/* ALL OF THE FOLLOWING SHOULD REALLY BE HANDLED BY NUMPY */
//     int ipoint, ifeature;

//     float *features_mins = (float *) malloc(p->nfeatures* sizeof(*p->features_mins));
//     float *features_maxes = (float *) malloc(p->nfeatures* sizeof(*p->features_maxes));

//     /* finding feature-wise minima */
// #pragma omp parallel for
//     for(ifeature = 0; ifeature < p->nfeatures; ifeature++)
//         features_mins[ifeature] = FLT_MAX;

// #pragma omp parallel for collapse(2) \
//     reduction(min : features_mins[:p->nfeatures])
//     for (ifeature = 0; ifeature < p->nfeatures; ifeature++)
//         for(ipoint = 0; ipoint < p->npoints; ipoint++)
//             if(features_float[ipoint][ifeature] < p->features_mins[ifeature])
//                 features_mins[ifeature] = features_float[ipoint][ifeature];


//     /* finding featre-wise maxima */
// #pragma omp parallel for
//     for(ifeature = 0; ifeature < p->nfeatures; ifeature++)
//         features_maxes[ifeature] = FLT_MIN;

// #pragma omp parallel for collapse(2) \
//     reduction(min : features_maxes[:p->nfeatures])
//     for (ifeature = 0; ifeature < p->nfeatures; ifeature++)
//         for(ipoint = 0; ipoint < p->npoints; ipoint++)
//             if(features_float[ipoint][ifeature] < p->features_mins[ifeature])
//                 features_mins[ifeature] = features_float[ipoint][ifeature];
}
