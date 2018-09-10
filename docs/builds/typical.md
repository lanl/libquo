How to Configure and Build QUO on a Typical Linux System
========================================================

## Auto-Generate Build Scripts
**NOTE: Required Only if NOT Building from a Release**

### Requisite Software
- GNU m4 (https://www.gnu.org/software/m4/m4.html)
- GNU autoconf v2.6+ (https://www.gnu.org/software/autoconf/autoconf.html)
- GNU automake v1.11+ (https://www.gnu.org/software/automake)
- GNU libtool v2.4.2+ (https://www.gnu.org/software/libtool)

```
./autogen
```

## Configure libquo

```
./configure CC=[MPICC] [FC=[MPIFC]] [--prefix=INSTALL-PATH]
```

Where MPICC is your system's MPI CC wrapper compiler and MPIFC is your system's
MPI Fortran wrapper compiler (Optional).

For a list of available configure options,
```
./configure --help
```

## Build libquo

```
make && make install
```

## Example Configure and Build

```
./configure CC=mpicc FC=mpif90 && make && make install
```

***NOTE: a Fortran wrapper compiler is needed only if you wish to build with
Fortran support.***
