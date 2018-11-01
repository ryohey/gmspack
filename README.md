# gmspack

Extract the data.win file produced by Game Maker Studio

This program is mainly porting from [UndertaleTools](https://github.com/AlexWaveDiver/UndertaleTools).
I using data structures described in [Altar.NET](https://gitlab.com/PoroCYon/Altar.NET/) and Mirrawrs

## Motivation

- I want to modify the data.win file especially to apply translation to Undertale on Mac OS X.
- plain C, less library dependencies, short code for the code portability.

## Compile and Run

```bash
gcc -std=c99 main.c; ./a.out -e data.win
```

## Task

- Packing (especially replacement for texts and textures)
- Support GMX files (currently some data export as csv)

## Contribution

I want you to contribute to gmspack.

