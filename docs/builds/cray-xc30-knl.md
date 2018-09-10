How to Build QUO on a KNL Cray XC30
===================================

## Setup Build Environment

* Auto-Generate Build Scripts **NOTE: Required Only if NOT Building from a
  Release.** See [this](typical.md) for more information.

* Log into a front-end node (NOT a login node).

* Load appropriate programming environment (Intel, PGI, etc.).

* Swap to appropriate Cray PE
```
module swap craype-haswell craype-mic-knl
```

* Set QUO_PREFIX
```
export QUO_PREFIX=[prefix]
```

## Build Static libnuma and Install to QUO's Install Prefix.

```
make prefix=$QUO_PREFIX libdir=$QUO_PREFIX/lib CC=gcc all install
```

* Delete $QUO_PREFIX/bin directory (we don't need those utilities -- only need
  libs and includes).

## Build QUO and Link Against Installed Version of libnuma.

```
./configure CC=cc FC=ftn --host=x86_64-unknown-linux-gnu \
LDFLAGS="-dynamic -L${QUO_PREFIX}/lib" \
--prefix=$QUO_PREFIX --enable-static
```

## Install

```
make && make install
```
