# Floatsum

*Originally completed on September - December 2019*

## Task
Given a file containing a list of floating point numbers and the number of worker threads, memory map the file, and uses multiple worker threads to calculate the total sum. 
`gendata.c` is a helper file provided by instructor Ding that will generate a file with N values and print their sum.
`floatsum.c` handles the actual sum/threading/memory mapping, and was written by myself.