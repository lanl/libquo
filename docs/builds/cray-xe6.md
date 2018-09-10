How to Build QUO on a Cray XE6
==============================

## Auto-Generate Build Scripts
**NOTE: Required Only if NOT Building from a Release**

See [this](docs/builds/typical.md) for more information.

## Setup Build Environment

* Load appropriate programming environment (Intel, PGI, etc.).

* Set QUO_PREFIX
```
export QUO_PREFIX=[prefix]
```

## Build Static libnuma and Install to QUO's Install Prefix.

```
make CC=icc && make prefix=$QUO_PREFIX install
# Don't use the wrapper compiler cc.
```

* Rename install libdir to lib if lib64 (Why do you do this install script?)
* Delete bin dir (we don't need those utilities---only need libs and includes).

## Build QUO and Link Against Installed Version of libnuma.

```
./configure CC=cc FC=ftn \
LDFLAGS="-dynamic -Wl,-rpath=/usr/lib/alps -L${QUO_PREFIX}/lib" \
--prefix=$QUO_PREFIX --enable-static
```
