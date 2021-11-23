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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <mram.h>

#include "../../common.h"

/*------------------ TYPES ------------------------------*/
/**
 * @brief Record of a split
 *
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
 *
 */
typedef struct Stack {
    int top;
    unsigned capacity;
    StackRecord stack[MAX_STACK_DPU];
} Stack;

/*------------------ INPUT ------------------------------*/
/** @name Input
 * Input arrays
 */
/**@{*/
__mram_noinit feature_t t_features[MAX_FEATURE_DPU];
/**@}*/

/*------------------ INTERNAL ------------------------------*/
/** @name Internal
 * Internal global variables
 */
/**@{*/
Stack stack;
/**@}*/

/**
 * @brief Initialize the Stack structure
 *
 * @param self pointer to the stack structure
 */
void init_stack(Stack *self) {
    self->capacity = MAX_STACK_DPU;
    self->top = 0;
}

/**
 * @brief Returns whether a Stack object is empty
 *
 * @param self Pointer to the Stack object
 * @return true Object is empty
 * @return false Object is not empty
 */
bool is_empty(Stack *self) { return self->top <= 0; }

/**
 * @brief Push a new element onto the stack.
 *
 * @param self Pointer to the Stack object.
 * @param start
 * @param end
 * @param depth
 * @param parent
 * @param is_left
 * @return int -1 in case of failure to allocate memory or 0 otherwise.
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
 * @param self Pointer to the stack
 * @param res Pointer to the residual stack
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

/*================== MAIN FUNCTION ======================*/
/**
 * @brief Main function DPU side.
 *
 * @return int 0 on success.
 */
int main()
{
    return 0;
}

#endif
