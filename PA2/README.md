# PA2: Mandelbrot
## Building
To build this project, type `make`.

```
$ make
```

This will build the program in the build directory.

## Running
To run this project, either run the corresponding script using
`sbatch`:

```
$ sbatch ./test/size_v_core/par_2_1000_50.sh 
```

or run the program using command line arguments directly.

```
$ srun -n [Tasks] ./build/main [rows] [cols] [iters] [filename]
```

*You may wish to add arguments to `srun` to lock execution to
specific amounts of nodes.*

The output is formatted to `num_tasks,image_size,exec_time` for
easy import into Microsoft Excel.  
