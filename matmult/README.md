# Matmult

*Originally completed on September - December 2019*

## Task
Given a file containing a matrix, the number of processes, and the file to save the result to, memory map the file, and uses multiple processes to calculate the product of multiplying the "source" matrix to an identity matrix (i.e the source) to the "destination" matrix. 
`genmatrix.c` is a helper file provided by instructor Ding that will generate a square NxN matrix and print their sum.
`matmult.c` handles the actual product/multiprocess/memory mapping, and was written by myself.