# DGD

This file was written for release 1.7 of DGD, Dworkin's Game Driver.

DGD is a rewrite from scratch of the LPMud server.  It runs on Windows, MacOS
and many versions of Unix.

[DGD source repository](https://github.com/dworkin/dgd)

[DGD's home page](https://www.dworkin.nl/dgd)

DGD offers JIT compilation support.  This requires the
[JIT extension module](https://github.com/dworkin/lpc-ext).

Alongside DGD, there is Hydra.  Hydra is fully compatible with DGD, but offers
acceleration for multi-core systems.  Hydra also supports JIT compilation,
using the same extension module as DGD.

[Hydra's home page](https://www.dworkin.nl/hydra-tech)

### PLEASE NOTE

DGD 1.7 is backward compatible with snapshots from DGD 1.5.9 and later, only.
To upgrade to DGD 1.7, first upgrade to the latest 1.5.x version, recompile
all objects, and create a non-incremental snapshot.


This distribution is organized as follows:

-   bin  
    Installation binaries will be created here (Unix).
-   container  
    Example files for running DGD in a container.
-   doc  
    Documentation on how to compile and run DGD.
-   lpc-doc  
    LPC documentation, still very incomplete.
-   src  
    Where the source code of DGD resides, and where you issue your `make`
    command on Unix.
-   src/host  
    Subdirectories for various platform-dependent files.

### Maldorne fork

This fork tracks upstream DGD closely and adds three small build flags.
Each is gated by `#ifdef`, so when the corresponding macro is *not*
defined the driver behaves exactly like upstream — keeping the source
tree trivially mergeable with `dworkin/dgd`.

All three flags are enabled by default in `src/Makefile` (so a plain
`make` produces the same binary as the published container image). To
build vanilla DGD, override with `make DEFINES=""`:

- **`-DSLASHSLASH`** — also documented upstream. Allows `//` line
  comments in LPC.
- **`-DPRESERVE_DEFAULTS_ON_RESTORE`** — when set, `restore_object()`
  leaves variables that are *not* mentioned in the `.o` save file at
  whatever value the object currently holds, instead of zeroing them.
  This matters when a savefile is older than the LPC source and a new
  variable was added: with this flag the new variable keeps the default
  assigned by the object's `create()` instead of becoming `0` / `0.0` /
  `nil`. Touches `src/kfun/file.cpp`.
- **`-DSUPPORT_PROXY_PROTOCOL`** — parse the [HAProxy PROXY protocol
  v1](https://www.haproxy.org/download/1.8/doc/proxy-protocol.txt)
  header on accepted TCP connections, so the driver sees the real client
  IP when running behind a TCP reverse proxy (Traefik, HAProxy, ...).
  Backwards compatible: connections without the header continue to work
  unchanged. Implemented in `src/host/proxy.{h,cpp}` and called from
  `src/host/unix/connect.cpp`.

#### Container image

The published image is `ghcr.io/maldorne/dgd:latest`. Builds and pushes
are automated by GitHub Actions on every push to `master`; manual
`docker push` is not part of the workflow.

To build locally for development or debugging:

```sh
docker build --no-cache . -t ghcr.io/maldorne/dgd:latest
docker run --rm -ti ghcr.io/maldorne/dgd:latest /bin/bash
```

Inside the container, `/opt/mud/bin/dgd` is the compiled driver. The
source tree is removed from the final image; comment out the cleanup
`RUN rm -Rf /opt/mud/driver` line in the `Dockerfile` if you need to
inspect the build environment.
