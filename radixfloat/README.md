# Radixfloat

*Originally completed on September - December 2019*

## Task
Given a file containing a list of floating point numbers, memory map the file, *treating it exclusively as binary data*, and sort it in place (ascending order) with radix sort of base 2. There are some intricacies based on the binary representation of floating point values in IEE standard; these are explained within `sort.c`.

`gendata.c` is a helper file provided by instructor Ding that will generate a file with N values and print their sum.
`checkdata.c` is a helper file provided by instructor Ding that will check a file to see that every value is in ascending order and print the sum of all of them.
`sort.c` handles the actual sorting/memory mapping, and was written by myself.