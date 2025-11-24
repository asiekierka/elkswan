# What is elkswan?

elkswan is a WonderSwan-targeted experimental fork of ELKS, which is an early fork of
the Linux OS for systems based on the Intel IA16 architecture.

elkswan stands for `Embedded Linux Kernel Subset With Aswan Nuances`, with `Aswan`
being the name of the original WonderSwan system-on-chip.

## Requirements

TODO.

* WonderSwan Color or SwanCrystal console. The original ("mono") WonderSwan is not supported!

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
