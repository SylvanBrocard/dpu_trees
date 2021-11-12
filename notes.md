# Notes on dpu_trees

## Sorting

Sklearn uses qsort from stdlib to sort the samples at every iteration.
NOTE: we'll not use sorting for now. Instead we're going to implement the random splitter algorithm.

## Parallelisation

The simplest approach is gonna be to sort on the host and simply use the DPUs to evaluate the chosen criterion (gini entropy by default).

For that purpose we are going to do is the following.
Gini index is defined as

$$
\begin{aligned}
G &= \sum_{k=0}^{K-1} count_k (1 - count_k) \\
&= 1 - \sum_{k=0}^{K-1} {count_k}^2
\end{aligned}
$$

where

$$
count_k = \frac{1}{N_m} \sum_{x_i \in R_m} \delta_{x_i,k}
$$

is the proportion of class k observations in node $m$ and $N_m$ is the number of observations in region $R_m$.

If $N_{m,D}$ is the number of observations in DPU $D$ and $R_{m,D}$ is the subset of region $R_m$ held in DPU $D$, then all we have to send back to the host is $N_{m,D}$ and

$$
pcount_{k,D} = \sum_{x_i \in R_{m,D}} \delta_{x_i,k}
$$

Then we can get the count as

$$
count_k = \frac{\sum_{D} pcount_{k,D}}{\sum_{D} N_{m,D}}
$$

Thus we'll avoid any multiplication on DPUs.

## Types

Since all we're gonna do on DPUs are comparisons and occurence counting, we don't need to quantize the data beforehand.
