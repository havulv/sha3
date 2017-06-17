
# SHA3

A rough SHA-3 256 and 512 bit implementation for both practice and understanding of the Keccack-p family of functions. Note: not for use in any security setting.

[See here for test vectors](http://csrc.nist.gov/groups/STM/cavp/index.html)
[And here for the specification](http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)

## Compilation

### Windows

On Windows platforms, you can compile with the MSVC toolchain. I usually use `cl` for things, as I have a dislike for how cumbersome Visual Studio is. Make sure the `vcvarsall.bat` has been run to set up the proper environment variables for `nmake`, otherwise nothing will work. Navigate to the root (`'.'` not `/`) directory and run:

```
nmake Makefile.win
```

#### Debugging

Currently, the Windows Makefile (`Makefile.win`) is set up for debugging and memory analysis with [drmemory](http://drmemory.org/) and the flags: `/Zi /MT /EHsc /Oy- /Ob0`. You can switch these to the normal flags `/W2 /O1`.

### Linux

On Linux (or anything with `gcc` really), use the following command in the top-level directory:

```
make
```

#### Debugging

Currently, the flags for `gcc` are `-Wall -O3`, but feel free to supplement that with whatever debugger you have on your current system.

