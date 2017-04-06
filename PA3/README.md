# PA3: Bucket Sort
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

Where `[data size]` is the number of integers to randomly
generate for testing the program.  Additionally, a variety
of scripts for SBATCHing have been provided in the `test`
directory.  They can be run individually or all of them
can be queue at once (1 per minute) by issuing the following
command.

```
$ ./test.sh
```

Output will be in the form of `results.csv`.
