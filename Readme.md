### CDCL SAT Solver
#### Build
To compile the solver, a GCC version with C++14 support is required, that is, GCC version >= 4.9.

Run `make` to build the solver. The executable is in `build/satsolver`.

#### Execute
Run `build/satsolver path_to_input_file` to solve the CNF specified by the input file `path_to_input_file`.

The `--time timeLimitSeconds` option can be specified to adjust the maximum time the solver is allowed to run. By default the time is unlimited.

`Ctrl-C` can be used to cancel the solving.

The `--verbose` option gives more detailed output

#### Output
The program outputs the satisfiability of the given CNF; if it is satisfiable, one of the possible solutions, in the form of, `v1 -v2 ... vN 0`. That is, list of variables with their polarity indicating their assignments, followed by a `0` at the end.  
For example,
```
$ make
$ build/satsolver input.txt --verbose
sat
1 2 -3 0
```

This means that the following variable assignment makes the CNF satisfiable:  
```
1 = 1
2 = 1
3 = 0
```

#### Results
The results of CNFs in the [petite](https://github.com/dreal/sat-benchmarks/tree/master/petite) benchmarks, under `30` seconds time constraint (with `--time 30 --verbose`) is in [Results](Results.md).
