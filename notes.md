# Notes on dpu_trees

## Sorting

Sklearn uses qsort from stdlib to sort the samples at every iteration.
NOTE: we'll not use sorting for now. Instead we're going to implement the random splitter algorithm.

## Parallelisation

The simplest approach is gonna be to sort on the host and simply use the DPUs to evaluate the chosen criterion (gini entropy by default).

For that purpose we are going to do is the following.
Gini index is defined as

<!-- $$
\begin{aligned}
G &= \sum_{k=0}^{K-1} count_k (1 - count_k) \\
&= 1 - \sum_{k=0}^{K-1} {count_k}^2
\end{aligned}
$$ -->

<div align="center"><img style="background: white;" src="https://render.githubusercontent.com/render/math?math=%5Cbegin%7Baligned%7D%0AG%20%26%3D%20%5Csum_%7Bk%3D0%7D%5E%7BK-1%7D%20count_k%20(1%20-%20count_k)%20%5C%5C%0A%26%3D%201%20-%20%5Csum_%7Bk%3D0%7D%5E%7BK-1%7D%20%7Bcount_k%7D%5E2%0A%5Cend%7Baligned%7D"></div>

where

<!-- $$
count_k = \frac{1}{N_m} \sum_{x_i \in R_m} \delta_{x_i,k}
$$ -->

<div align="center"><img style="background: white;" src="https://render.githubusercontent.com/render/math?math=count_k%20%3D%20%5Cfrac%7B1%7D%7BN_m%7D%20%5Csum_%7Bx_i%20%5Cin%20R_m%7D%20%5Cdelta_%7Bx_i%2Ck%7D"></div>

is the proportion of class k observations in node <!-- $m$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=m"> and <!-- $N_m$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=N_m"> is the number of observations in region <!-- $R_m$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=R_m">.

If <!-- $N_{m,D}$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=N_%7Bm%2CD%7D"> is the number of observations in DPU <!-- $D$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=D"> and <!-- $R_{m,D}$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=R_%7Bm%2CD%7D"> is the subset of region <!-- $R_m$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=R_m"> held in DPU <!-- $D$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=D">, then all we have to send back to the host is <!-- $N_{m,D}$ --> <img style="transform: translateY(0.1em); background: white;" src="https://render.githubusercontent.com/render/math?math=N_%7Bm%2CD%7D"> and

<!-- $$
pcount_{k,D} = \sum_{x_i \in R_{m,D}} \delta_{x_i,k}
$$ -->

<div align="center"><img style="background: white;" src="https://render.githubusercontent.com/render/math?math=pcount_%7Bk%2CD%7D%20%3D%20%5Csum_%7Bx_i%20%5Cin%20R_%7Bm%2CD%7D%7D%20%5Cdelta_%7Bx_i%2Ck%7D"></div>

Then we can get the count as

<!-- $$
count_k = \frac{\sum_{D} pcount_{k,D}}{\sum_{D} N_{m,D}}
$$ -->

<div align="center"><img style="background: white;" src="https://render.githubusercontent.com/render/math?math=count_k%20%3D%20%5Cfrac%7B%5Csum_%7BD%7D%20pcount_%7Bk%2CD%7D%7D%7B%5Csum_%7BD%7D%20N_%7Bm%2CD%7D%7D"></div>

Thus we'll avoid any multiplication on DPUs.

## Types

Since all we're gonna do on DPUs are comparisons and occurence counting, we don't need to quantize the data beforehand.
