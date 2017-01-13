![logo](docs/img/quo-logo.png)

[![Build Status](https://travis-ci.org/losalamos/libquo.svg?branch=master)
](https://travis-ci.org/losalamos/libquo)

# The QUO Runtime Library

QUO (as in *"status quo"*) is a runtime library that aids in accommodating
thread-level heterogeneity in dynamic, phased MPI+X applications comprising
single- and multi-threaded libraries.

## Releases
Distribution tarballs can be found [here](http://losalamos.github.io/libquo/).

## Building
### Prerequisites
libquo requires an implementation of the message passing interface [(MPI)]
(http://mpi-forum.org/) and [hwloc](http://www.open-mpi.org/projects/hwloc).
hwloc is included with libquo, so the only external dependency is an MPI library
libquo has been extensively tested against both [Open MPI](
https://www.open-mpi.org/) and [MPICH](https://www.mpich.org/) based
implementations.

**Please see documentation located in docs/builds.**

## libquo API
### C Interface Users
See [src/quo.h](src/quo.h) for more information.

### Fortran Interface Users
See [src/fortran/quof.f90](src/fortran/quof.f90) and [src/quo.h](src/quo.h) for
more information.

### Interface Examples
In [tests](tests) and [demos](demos).

## Linking to libquo 
### Example 1
```
mpicc myquoapp.c -I[QUO-PREFIX]/include -L[QUO-PREFIX]/lib -o myquoapp -lquo
export LD_LIBRARY_PATH=[QUO-PREFIX]/lib:$LD_LIBRARY_PATH
```
### Example 2
```
mpicc myquoapp.c -o myquoapp -I[QUO-PREFIX]/include \
-L[QUO-PREFIX]/lib -lquo -Wl,-rpath=[QUO-PREFIX]/lib
```
### Example 3 (with [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/))
```
mpicc myquoapp.c -o myquoapp `pkg-config --cflags --libs libquo`
```

## Environment Variables
QUO_TMPDIR - specifies the base directory where temporary QUO files will be
             written.

## Los Alamos National Laboratory Code Release
QUO - LA-CC-13-076
