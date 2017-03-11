Bug fix (Note: This solution did not correct the problem)
-----------------------------------

### code compiled with g++ > 4.2.1 crashes with "pointer being freed not allocated

On OSX SDK10.9+, launching ldglite from the command line crashes with one of the 3 follwoig errors. Launching from the GUI is OK.
This problem is reproducable on x86 and x86_64 builds. 

1. `ldglite(13111,0x7fffabf8a3c0) malloc: *** error for object 0x7ff7b2515478: incorrect checksum for freed object - object was probably modified after being freed.`
   `*** set a breakpoint in malloc_error_break to debug`
2. `line 44: 13122 Segmentation fault: 11  05/ldglite -l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -w1 -mFTest_gcc_10.8_Foo2.png Foo2.ldr`
3. `Ldglite Version 1.3.2`      
   `FOV = 0.01`
   `FROM = (23, -45, 3.03133e+06)`
   `Znear, Zfar = 3.02833e+06, 3.03433e+06`
   `CAM = (-1.97308e+06, 1.18443e+06, 1.97308e+06)`
   `Rendering OffScreen`
   `CGLChoosePixelFormat failed, PixelFormatObj is NULL!`   
   `setupCGL failed!`

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
