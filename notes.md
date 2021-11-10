# Notes on dpu_trees

## Sorting

Sklearn uses qsort from stdlib to sort the samples at every iteration.

## Parallelisation

The simplest approach is gonna be to sort on the host and simply use the DPUs to evaluate the chosen criterion (gini entropy by default).
