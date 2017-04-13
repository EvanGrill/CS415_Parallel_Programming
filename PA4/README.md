# PA4: Matrix Multiplication
## Building
To build this project, type `make`.

```
$ make
```

This will build the program in the build directory.

## Running
You can run the program in the following manner.

```
$ srun ./build/main [data size]
```

Where `[data size]` is a single dimension of the two
square matrices to be multiplied.  Data is printed
in a format for adding to a csv like this:

```
size,execution_time
```

Where execution time is in seconds.