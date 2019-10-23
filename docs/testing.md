# Automated Testing
QUO has a rudimentary automated testing capability built atop `make check`. To
run the suite of tests currently available in this distribution, simply run
`make check` after successfully building the distribution's source.

The following environment variables are made available to alter the behavior of
QUO's automated testing infrastructure.

```
# Specifies the parallel launcher (e.g., mpiexec, srun).
QUO_TESTS_PRUN
# The argument specifying how many processes to launch (e.g., -n in many cases).
QUO_TESTS_PRUN_N
# Any other arguments passed to PRUN.
QUO_TESTS_PRUN_OTHER_ARGS
# Colon-delimited list of environment variables to export.
QUO_TESTS_EXPORT
```

## Examples

```
# Default
make check
```

```
# Use srun as the parallel launcher.
QUO_TESTS_PRUN=srun make check
```
