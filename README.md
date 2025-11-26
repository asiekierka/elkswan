# What is elkswan?

elkswan is a WonderSwan-targeted experimental fork of ELKS, which is an early fork of
the Linux OS for systems based on the Intel IA16 architecture.

elkswan stands for `Embedded Linux Kernel Subset With Aswan Nuances`, with `Aswan`
being the name of the original WonderSwan system-on-chip.

## Motivation

As of writing, there were two actively maintained forks of ELKS, all on friendly relations with each other:

* ghaerr's [ELKS](https://github.com/ghaerr/ELKS), which primarily aims to support PCs (IBM, PC-98) and SBCs;
* Mellvik's [TLVC](https://github.com/Mellvik/TLVC/), which gives up portability and BIOS compatibility for greater performance;

elkswan came into being as upstream ELKS [does not wish to increase its code complexity](https://github.com/ghaerr/elks/issues/2477#issuecomment-3581928650)
to support even more niche platforms like 16-bit x86 palmtops, PDAs or game consoles; in effect, it is a fork for
exploring compatibility and providing releases with these devices in mind, potentially leading to compatibility
and functionality improvements upstream where doing so does not create a significant maintenance burden.

elkswan is also not a hard fork; I will try my best to periodically rebase it on top of upstream, at least whenever
I'm actively hacking on the project.

## Targets

### WonderSwan

Requirements:

* WonderSwan Color or SwanCrystal console. The original ("mono") WonderSwan is not supported!
* One of the following devices:

|   | WonderWitch | nileswan | Generic flashcart |
| - | ----------- | -------- | ----------------- |
| Process RAM | 3 x 64 KiB | 8 x 64 KiB (WIP) | Depends |
| Maximum ROMFS size | ~320 KiB | 732 KiB (WIP) | Up to 704 KiB (WIP) |
| Real-Time Clock | WIP | WIP | :x: |
| FAT support | :x: | WIP | :x: |
| USB serial port | :x: | WIP | :x: |

Notes:

* Fields marked "WIP" refer to current functionality that may be improved or expanded in the future.
* An emulator without special configuration counts as a "generic flashcart".

### Potential future targets

This serves merely as a list of devices that could be neat to port elkswan to; it does not signal active development.

* Casio Pocket Viewer - still inexpensive V30MZ-based PDA family,
* Psion Series 3/3a/3c/3mx - somewhat expensive V30-based PDA family,
* Atari Portfolio - rather expensive 80C88 PDA,
* HP 95LX/100LX/200LX - nearly unobtainable 80186-compatible PDA.

## Downloads

TODO.

## How to build

Full build instructions are [here](https://github.com/asiekierka/elkswan/blob/master/BUILD.md).

## Wiki

Help on how to use ELKS, as well as technical tutorials, are available on our [Wiki](https://github.com/ghaerr/elks/wiki).

## Documentation

More information is in the Documentation folder: [Index of ELKS Documentation](https://htmlpreview.github.io/?https://github.com/asiekierka/elkswan/blob/master/Documentation/index.html).

## Resources

Other projects and resources interesting to ELKS and our programming community:

- [8086 toolchain](https://github.com/ghaerr/8086-toolchain) A full C toolchain running
on the host desktop and ELKS itself, featuring C compiler, the as86 assembler,
ld86 linker, make and a complete C library..
- [blink16](https://github.com/ghaerr/blink16) A visual 8086 emulator and debugger capable of booting the ELKS kernel for symbolic debugging, as well as an emulator for ELKS executables.
- [Size Optimization Tricks](https://justine.lol/sizetricks/) A great article from Justine Tunney's blog showing how big things can be done without bloat.
- [gcc-ia16](https://github.com/tkchia/gcc-ia16) TK Chia's gcc compiler targeted for 8086, maintained and used for the ELKS kernel and all its applications.

## More information

Questions? Problems? Patches? Open an issue on the elkswan GitHub project!
