/**
 * @file trees_dpu_kernel.c
 * @author Sylvan Brocard (sbrocard@upmem.com)
 * @brief DPU side of the tree algorithm
 *
 */

#ifndef _TREES_DPU_KERNEL_H_
#define _TREES_DPU_KERNEL_H_ /**< guard to prevent linking with CPU binaries   \
                              */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mram.h>

#include "../../trees_common.h"

/*------------------ TYPES ------------------------------*/
/**
 * @brief Data to track sample split
 */
typedef struct StackRecord {
    size_t start;
    size_t end;
    size_t depth;
    size_t parent;
    bool is_left;
} StackRecord;

/**
 * @brief LIFO data structure
 */
typedef struct Stack {
    int top;
    unsigned capacity;
    StackRecord stack[MAX_STACK_DPU];
} Stack;

/**
 * @brief Criterion data
 *
 * The criterion computes the impurity of a node and the reduction of
 * impurity of a split on that node. It also computes the output statistics
 * such as the mean in regression and class probabilities in classification.
 */
typedef struct Criterion {
    size_t *y; /**< Values of y */

    size_t *samples; /**< Sample indices in X, y */
    size_t start;    /**< samples[start:pos] are the samples in the left node */
    size_t pos;      /**< samples[pos:end] are the samples in the right node */
    size_t end;

    size_t n_samples;      /**< Number of samples */
    size_t n_node_samples; /**< Number of samples in the node (end-start) */
    size_t n_left;         /**< number of samples in the left node */
    size_t n_right;        /**< number of samples in the right node */

    size_t *sum_total; /**< For classification criteria, the sum of the weighted
                            count of each label. For regression, the sum of w*y.
                            sum_total[k] is equal to sum_{i=start}^{end-1}
                            w[samples[i]]*y[samples[i], k], where k is output
                            index. */
    size_t *sum_left;  /**< Same as above, but for the left side
                            of the split */
    size_t *sum_right; /**< same as above, but for the right side
                            of the split */

    /* Parameters specific to classification criteria */
    size_t n_classes;
} Criterion;

/*================== VARIABLES ==========================*/
/*------------------ LOCAL ------------------------------*/
/** @name Globals
 * Global variables shared between tasklets
 */
/**@{*/
Criterion criterion; /**< data about the current leaf */
/**@}*/

/*------------------ INPUT ------------------------------*/
/** @name Host
 * Variables for host application communication
 */
/**@{*/
__host size_t nfeatures;
__host size_t n_classes;
/**@}*/

/*================== TABLES =============================*/
/*------------------ LOCAL ------------------------------*/
/** @name Globals
 * Global tables shared between tasklets
 */
/**@{*/
Stack stack;                   /**< Stack of split records. */
size_t sum_total[MAX_CLASSES]; /**< sum of class observations for the current
                                  node */
size_t sum_left[MAX_CLASSES];  /**< sum of class observations for the current
                                  left child */
size_t sum_right[MAX_CLASSES]; /**< sum of class observations for the current
                                  right child */
__mram_noinit feature_t
    feature_values[MAX_SAMPLES_DPU]; /** Vector of one type of features. */
/**@}*/

/*------------------ INPUT ------------------------------*/
/** @name Input
 * Input arrays
 */
/**@{*/
__mram_noinit feature_t t_features[MAX_FEATURE_DPU]; /**< Vector of features. */
__mram_noinit size_t t_targets[MAX_SAMPLES_DPU];     /**< Vector of targets. */
/**@}*/

/**
 * @brief Initialize the Stack structure
 *
 * @param self [in, out] pointer to the stack structure
 */
void init_stack(Stack *self) {
    self->capacity = MAX_STACK_DPU;
    self->top = 0;
}

/**
 * @brief Returns whether a Stack object is empty
 *
 * @param self [in] pointer to the Stack object
 * @return true Object is empty
 * @return false Object is not empty
 */
bool is_empty(Stack *self) { return self->top <= 0; }

/**
 * @brief Push a new element onto the stack.
 *
 * @param self [in,out] Pointer to the Stack object.
 * @param start [in]
 * @param end [in]
 * @param depth [in]
 * @param parent [in]
 * @param is_left [in]
 * @return int -1 in case of insufficient memory or 0 otherwise.
 */
int push(Stack *self, size_t start, size_t end, size_t depth, size_t parent,
         bool is_left) {
    size_t top = self->top;
    StackRecord *stack = NULL;

    /* Throw error if capacity is not sufficient */
    if (top >= self->capacity) {
        printf("Stack exceeded capacity of %d", self->capacity);
        exit(EXIT_FAILURE);
    }

    stack = self->stack;
    stack[top].start = start;
    stack[top].end = end;
    stack[top].depth = depth;
    stack[top].parent = parent;
    stack[top].is_left = is_left;

    /* increment stack pointer */
    self->top = top + 1;
    return 0;
}

/**
 * @brief Remove the top element from the stack and copy to ``res``.
 *
 * @param self [in] Pointer to the stack
 * @param res [out] Pointer to the popped element
 * @return int 0 if pop was successful (and ``res`` is set); -1 otherwise.
 */
int pop(Stack *self, StackRecord *res) {
    size_t top = self->top;
    StackRecord *stack = self->stack;

    if (top <= 0)
        return -1;

    res[0] = stack[top - 1];
    self->top = top - 1;

    return 0;
}

/**
 * @brief Finds the min and max of a feature, and fills `current_feature` with
 * the values of that feature.
 *
 * @param start [in] index of the first element
 * @param end [in] index of the last element
 * @param samples [in] array of sample indices
 * @param Xf [in] array of sample feature
 * @param min [out] minimum found
 * @param max [out] maximum found
 * @param current_feature [in] index of the feature under consideration
 */
void find_min_max(size_t start, size_t end, size_t *samples, feature_t *Xf,
                  feature_t *min, feature_t *max, size_t current_feature) {
    feature_t min_feature_value =
        t_features[samples[start] * nfeatures + current_feature];
    feature_t max_feature_value = min_feature_value;
    Xf[start] = min_feature_value;

    for (size_t p = start + 1; p < end; p++) {
        feature_t current_feature_value =
            t_features[samples[p] * nfeatures + current_feature];
        Xf[p] = current_feature_value;

        if (current_feature_value < min_feature_value)
            min_feature_value = current_feature_value;
        else if (current_feature_value > max_feature_value)
            max_feature_value = current_feature_value;
    }
}

/**
 * @brief Threshold-based partitions of a list of samples based on one feature
 *
 * @param start [in] index of the first element
 * @param end [in] index of the last element
 * @param threshold [in] threshold for partitioning
 * @param samples [in, out] array of sample indices
 * @param Xf [in, out] array of sample feature
 * @return size_t index of the first element greater than `threshold`
 */
size_t partition(size_t start, size_t end, feature_t threshold, size_t *samples,
                 feature_t *Xf) {
    size_t p = start, partition_end = end;
    size_t swap_index;
    feature_t swap_feature;

    while (p < partition_end) {
        if (Xf[p] <= threshold) {
            p++;
        } else {
            partition_end--;

            swap_feature = Xf[p];
            Xf[p] = Xf[partition_end];
            Xf[partition_end] = swap_feature;

            swap_index = samples[p];
            samples[p] = samples[partition_end];
            samples[partition_end] = swap_index;
        }
    }

    return partition_end;
}

/**
 * @brief Resets the criterion at pos=start.
 *
 * @param self [in, out] pointer to the criterion
 */
void criterion_reset(Criterion *self) {
    self->pos = self->start;

    self->n_left = 0;
    self->n_right = self->n_node_samples;

    size_t *sum_total = self->sum_total;
    size_t *sum_left = self->sum_left;
    size_t *sum_right = self->sum_right;

    size_t n_classes = self->n_classes;

    memset(sum_left, 0, n_classes * sizeof(*sum_left));
    memcpy(sum_right, sum_total, n_classes * sizeof(*sum_right));
}

/**
 * @brief Resets the criterion at pos=end.
 *
 * @param self [in, out] pointer to the criterion
 */
void criterion_reverse_reset(Criterion *self) {
    self->pos = self->end;

    self->n_left = self->n_node_samples;
    self->n_right = 0;

    size_t *sum_total = self->sum_total;
    size_t *sum_left = self->sum_left;
    size_t *sum_right = self->sum_right;

    size_t n_classes = self->n_classes;

    memset(sum_right, 0, n_classes * sizeof(*sum_right));
    memcpy(sum_left, sum_total, n_classes * sizeof(*sum_left));
}

/**
 * @brief Updated statistics by moving samples[pos:new_pos] to the left child.
 *
 * Given that
 *  sum_left[x] +  sum_right[x] = sum_total[x]
 * and that sum_total is known, we are going to update sum_left from the
 * direction that require the least amount of computations, i.e. from pos to
 * new_pos or from end to new_pos.
 *
 * @param self [in, out] pointer to the criterion
 * @param new_pos [in] The new ending position for which to move samples from
 * the right child to the left child.
 */
void criterion_update(Criterion *self, size_t new_pos) {
    size_t pos = self->pos;
    size_t end = self->end;

    size_t *sum_left = self->sum_left;
    size_t *sum_right = self->sum_right;
    size_t *sum_total = self->sum_total;

    size_t n_classes = self->n_classes;
    size_t *samples = self->samples;

    if ((new_pos - pos) <= (end - new_pos)) {
        for (size_t p = pos; p < new_pos; p++) {
            size_t i = samples[p];

            size_t label_index = self->y[i];
            sum_left[label_index]++;

            self->n_left++;
        }
    }
    else {
        criterion_reverse_reset(self);

        for(size_t p = end-1; p > new_pos - 1; p--) {
            size_t i = samples[p];

            size_t label_index = self->y[i];
            sum_left[label_index]--;

            self->n_left--;
        }
    }

    /* Update right part statistics */
    self->n_right = self->n_node_samples - self->n_left;
    for(size_t c = 0; c < n_classes; c++) {
        sum_right[c] = sum_total[c] - sum_left[c];
    }

    self->pos = new_pos;
}

/*================== MAIN FUNCTION ======================*/
/**
 * @brief Main function DPU side.
 *
 * @return int 0 on success.
 */
int main() {
    printf("%u", sizeof(size_t));

    return 0;
}

#endif
