# PA4: Matrix Multiplication
## About
This project implements Cannon's algorithm for
parallel matrix multiplication.  It times the
actual matrix multiplication not the transfer to
and from the parallel processors.

## Building
To build this project, type `make`.

```
$ make
```

This will build the program in the build directory.

## Running
You can run the program in one of the following
manners.

```
$ srun -n [# tasks] --mpi=pmi2 ./build/main [data size]
```

Where `[data size]` is a single dimension of the two
square matrices to be multiplied.

or

```
$ srun -n [# tasks] --mpi=pmi2 ./build/main [input 1] [input 2] [output]
```

Where `[input 1]`, `[input 2]`, and `[output]` are files
of the format: 

```
4
1 2 3 4
2 3 4 5
3 4 5 6
4 5 6 7
```

For either version, timing data is printed to stdout in 
a format for adding to a csv like this:

```
num_tasks,size,execution_time
```

Where execution time is in seconds.

If using the input/output file version, actual matrix
results will be printed to the output file in the same
format as the input files.

*Note:* The number of tasks must be a perfect square
and Size must be a multiple of the square root of the
number of tasks. 