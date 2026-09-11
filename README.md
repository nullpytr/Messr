# Messr

Cross-platform port of `rdmsr`/`wrmsr` from [intel/msr-tools](https://github.com/intel/msr-tools). Windows support added via the [wMSR](https://github.com/nullpytr/wMSR) kernel driver. 

The upstream source files are unchanged, compatibility headers (`include/compat`) bridge the Linux kernel device (`/dev/cpu/N/msr`) interface to the wMSR driver interface at compile time.

## Requirements

**Windows**: the `msr.sys` kernel driver from [wMSR](https://github.com/nullpytr/wMSR) must be loaded before running either tool.

**Linux**: the `msr` kernel module must be loaded (`modprobe msr`).

## Usage

### rdmsr

```
Usage: rdmsr [options] regno
  --help         -h  Print this help
  --version      -V  Print current version
  --hexadecimal  -x  Hexadecimal output (lower case)
  --capital-hex  -X  Hexadecimal output (upper case)
  --decimal      -d  Signed decimal output
  --unsigned     -u  Unsigned decimal output
  --octal        -o  Octal output
  --c-language   -c  Format output as a C language constant
  --zero-pad     -0  Output leading zeroes
  --raw          -r  Raw binary output
  --all          -a  all processors
  --processor #  -p  Select processor number (default 0)
  --bitfield h:l -f  Output bits [h:l] only
```

### wrmsr

```
Usage: wrmsr [options] regno value...
  --help         -h  Print this help
  --version      -V  Print current version
  --all          -a  all processors
  --processor #  -p  Select processor number (default 0)
```

## Build

### for Windows 

Outputs `build-win32/rdmsr.exe` and `build-win32/wrmsr.exe`.

#### 1. Native

Requires MSVC. Run from a [Developer Command Prompt](https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell).

Use the provided build script:
```cmd
scripts\build
```

#### 2. Cross-compile from macOS or Linux

Requires [mingw-w64](https://www.mingw-w64.org).  
On macOS: 
```
brew install mingw-w64
```

Use the provided build script:

```sh
sh scripts/build.sh win32
```


### for Linux

```sh
sh scripts/build.sh native
``` 
