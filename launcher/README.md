Known Issue
-----------------------------------

### code compiled with g++ > 4.2.1 crashes with "pointer being freed not allocated

The problem comes from the fact that some system frameworks load the system libstdc++, which results in incompatible data structures. From SDK10.9+, Apple no longer uses libstdc, it uses libc.

A solution is to set the environment variable `DYLD_LIBRARY_PATH` to the path of a directory where you *only* put a symbolic link to the newest libstdc++ from GCC (libstdc++ is guaranteed to be backward-compatible, not other libraries).

The [`ldglite_launcher.c`](src/ldglite_launcher.c) source code is compiled to act as a "launcher" for the ldglite binary that correctly sets the `DYLD_LIBRARY_PATH` variable. See the full instructions at the top of the source file.

When building an application bundle, the ldglite executable is accompanied by by the small executable [`ldglite_launcher.c`](src/ldglite_launcher.c) that sets the environment variable `DYLD_LIBRARY_PATH`. It can be a script (see below), or a binary (better, because it handles spaces in arguments correctly).

An alternate solution, using a shell-script (doesn't handle spaces in arguments): http://devblog.rarebyte.com/?p=157

References:
- https://github.com/devernay/xcodelegacy
- http://stackoverflow.com/questions/11457932/incorrect-checksum-for-freed-object-on-malloc
- http://stackoverflow.com/questions/6365772/unable-to-run-an-application-compiled-on-os-x-snow-leopard-10-6-7-on-another-m
- http://stackoverflow.com/questions/4697859/mac-os-x-and-static-boost-libs-stdstring-fail
