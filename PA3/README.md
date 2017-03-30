# PA3: Bucket Sort
## Building
To build this project, type `make`.

```
$ make
```

This will build the program in the root directory.

You can also build the generator by adding the generator qualifier.

```
$ make generator
```

This will build the generator in the `test` directory.

## Running
First, generate test files by issuing the following command:

```
$ ./test/generator > ./test/[filename]
```

You'll then get a blank input prompt.  Type in the size of the
test file you'd like in number of integers.

Now that you've generated 1 or more test files, the `seq_test.sh`
script will run the program on all of your `.txt` files in your
`test` folder.

```
$ ./seq_test.sh
```

Output will be in the form of `results.csv`.
