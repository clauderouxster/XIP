# How to compile XIP

First, if you want to compile the Windows version, for which we already have (see [Binaries](https://github.com/clauderouxster/XIP/tree/main/Windows/Binaries)), you can use the Visual Studio [SLN project](https://github.com/clauderouxster/XIP/blob/main/Windows/XIP7/xipdll2.sln).
For other platforms, you need to install a JDK (1.8 for instance) and ANT.

*Note that this version of build needs to be updated for later version of Mac OS.*

## Environment Variables for Python

XIP is linked with Python, but you need to export the path to the libraries and the includes:

```sh
# For instance, your PYTHONHOME is installed here
export PYTHONHOME=~/miniconda3
# Automatically compute the version number
export THEPYTHONVERSION=`python --version | gawk -F'[ .]' '/Python/ {print $2 "."  $3}'`
```

## XML

XIP also needs to link with the `libxml2.so`. We provide the includes in the `parSem/xml/dev`.
By default, the system expect the library: `libxml2.so` to be in: `parSem/xml/dev/libs`.

Usually, you don't need to do anything since `libxml2.so` is available on all platforms. However, in some cases, it might be missing along the compiling paths. 
The easiest to solve this issue is to create a _symbolic link_ in `parSem/xml/dev/libs`.

```sh
cd parSem/xml/dev
# create a symbolic link
ln -s /usr/lib64/libxml2.so.2 libxml2.so
```

## Compiling

Compiling `xip` consists into positionning yourself in: `parSem` and then executing: `ant compile`.

The compiler will then compiles three libraries in a row:

```
fst: for fast lexicon access
ntm: for fast tokenization
xip: the final parser
```

The binaries will be produced in: `parSem/xip/dev/xipFst/bin`
