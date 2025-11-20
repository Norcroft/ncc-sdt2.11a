# Norcroft C/C++ — SDT 2.11a Restoration

This repository contains a lightly modernised restoration of the historical 
Norcroft C/C++ compiler, based on ARM’s SDT 2.11a source release.

The aim is to preserve the original Norcroft compiler source as closely as
practical, with minimal changes, so it can be compiled with modern C compilers
and run on 64-bit computers (macOS, Linux). In addition, those builds can then
be used to compile itself to create C and C++ compilers that run on RISC OS.

Code is emitted in AOF files and supports several targets: ARM's generic 32-bit
development platform, RISC OS (32-bit and 26-bit), and Apple Newton (untested).

The historical RISC OS target produced 26-bit AOF. The default Makefile
overrides this to generate 32-bit RISC OS 5 output unless `TARGET=riscos26` is
specified.

## Contents

- **`ncc/`** — the historical compiler source with only minimal changes to compile
   on modern compilers and for 64-bit hosts. The intention is to keep the
   files in this directory unchanged, apart from fixing any remaining 64-bit bugs.

- **`ncc-support/`** — newly recreated support code and glue. These files will
  be replaced with original equivalents as they are located.

- **`external/clib/`** — standard C library header files. The original
   Acorn/Codemist C headers have not yet been released under an open source
   licence, so these are more modern versions from the ROOL fork of Norcroft,
   lightly 'de-updated'.

## Building
C:
```
make ncc [TARGET=<arm|riscos|riscos26|newton>] [HOST=riscos]
```
C++:
```
make n++ [TARGET=<arm|riscos|riscos26|newton>] [HOST=riscos]
```

Where:
- `TARGET` selects the code generation flags for the named target
 (Generic ARM, RISC OS (32-bit or 26-bit), or Apple Newton).

- `HOST=riscos` first compiles a compiler for the current host (ncc-riscos),
   and uses that to build a native RISC OS executable (`ncc,ff8`).

### Examples:
Cross-compiler for targeting 26-bit RISC OS 3 or 4. Builds `bin/ncc-riscos26`:
```
make ncc TARGET=riscos26
```

RISC OS-native compiler to run on RISC OS 5. Builds `bin/ncc,ff8`, where ff8
is the 'Absolute' filetype for a RISC OS executable:
```
make ncc TARGET=riscos HOST=riscos
```

### Other useful make targets:
```
make all        # ncc & n++
make clean
make distclean
```

## Notes
`TARGET=riscos` produces code for RISC OS 5 with unaligned loads disabled
for broad hardware compatibility. Use `-za0` to allow unaligned loads where
appropriate.

## Background

When Acorn developed the ARM, they showed it to some of their university
contemporaries who enthusiastically offered to develop an optimising C
compiler, creating Norcroft C. The compiler was developed jointly by Codemist
Ltd and Acorn, with Acorn retaining the rights for ARM-based targets and
Codemist retaining the rights for other CPU architectures. When Acorn spun ARM
out as a separate company, Norcroft C moved to ARM Ltd, with Acorn retaining a
licence. ARM and Codemist continued working together on the compiler —
including the creation of a new C++ front end — into the late 1990s.

These repositories are built on the surviving source code for the final
release of the jointly developed Norcroft C/C++. ARM continued developing
their compiler afterwards, but replaced the C++ front end with one licensed
from EDG.

Acorn continued to license Norcroft C from ARM until around 1994, when Acorn
didn't renew their contract. Since around 1999, Acorn's RISC OS successors
(Element 14 Ltd, Pace, Castle, ROOL) have updated their 1994 fork of the
compiler to support newer C standards. That version is available from [ROOL](https://www.riscosopen.org).

For more historical context, see [Codemist's archived website](https://web.archive.org/web/20250113002824/https://codemist.co.uk/ncc/index.html)
or watch Lee Smith (ARM) and Arthur Norman (Codemist) discussing ["the ARM... kind of needing compilers"](https://www.youtube.com/watch?v=hwUoVU_XCis).

## Acknowledgements

Thanks to Lee Smith (formerly ARM), Arthur Norman (Codemist) and
Alan Mycroft (Codemist) for access to the source and general encouragement.
