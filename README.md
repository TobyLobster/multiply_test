# 6502 Integer Multiplication - which is best?


## Contents

* [Introduction](#Introduction)
* [The Algorithms](#TheAlgorithms)
	* [Binary Multiplication (Shift and Add)](#BinaryMultiplicationShiftandAdd)
	* [Tables of Squares](#TablesofSquares)
	* [Logarithms](#Logarithms)
	* [Four bit multiply](#Fourbitmultiply)
	* [Booth's Algorithm](#BoothsAlgorithm)
    * [Hardware support](#Hardwaresupport)
    * [Repeated addition](#Repeatedaddition)
* [The Implementations](#TheImplementations)
* [The Results](#TheResults)
* [Customising](#Customising)
* [How to run the tests](#Howtorunthetests)


## Introduction

This document compares the runtime performance and memory use of many different multiplication routines for the 6502 CPU. Each implementation is executed exhaustively over every possible input, correctness is verified, and cycle counts are tallied. The results are shown in graphs below.

## The Algorithms

### 1. Binary Multiplication (Shift and Add)

This is the classic algorithm found in all good textbooks. A friendly introduction is found [here](https://www.llx.com/Neil/a2/mult.html). In short, one number is shifted left (doubled) each time around a loop, and the binary bits of the other number are used to determine whether to add this shifted number to a running total.

This is the method used by most programs that need multiplication. It has the advantage that the code is small and it performs reasonably well. Also known as [Ancient Egyptian multiplication](https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication).

### 2. Tables of Squares

By storing tables of square numbers, we can speed up multiplication. This uses:

    a*b = f(a+b) - f(a-b),   where f(x) = x^2/4

So using two table lookups, an addition and two subtractions, we can multiply. This is fast! The downside is how much memory needed to store the data. For 8 bit multiplication, the amount of data varies depending on the exact implementation, but is either 2k of data (fastest), or 1k (only marginally slower), or 512 bytes (slightly slower again).

An added feature of the 1k and 2k routines particularly is that if many multiplications are being done with one of the inputs unchanging then some setup code can be skipped, for even better performance. For example if a number of points are being rotated by some known angle.

The data tables can be either loaded from storage, or initialised in code.

### 3. Logarithms

This is an approximation for multiplication. This uses:

    log(a*b) = log(a) + log(b)

By using a log and exponentiation tables, we can multiply using just three table lookups and one addition. This is fast too. For the 8 bit multiplication implementations I have seen, the memory required is about 768 bytes.

However, since we are working with integers, this is only gives an approximation. For 8 bit multiplication, the error is at most 5. This may be close enough for some purposes.

This method is described further [here](https://codebase64.org/doku.php?id=base:mathematics_in_assembly_part_6). *Elite* for the BBC Micro uses this method for some multiplications (see [here](https://www.bbcelite.com/deep_dives/multiplication_and_division_using_logarithms.html)). 

### 4. Four bit multiply
Instead of 'binary multiplication' using base 2 (as described above), we use base 16 (hexadecimal). We use a 256 byte table that stores the result of multiplying two 4 bit numbers together.

To get an 8 bit x 8 bit multiply, we think of our two 8 bit numbers as being two pairs of hex digits AB and CD. We multiply each pair of hex digits together using the lookup table, and add them together as shown below. This is the same method as regular pen and paper multiplication:

```
        AB
       *CD
      ----
        xx      (B*D)+
       xx0      (A*D*16)+
       xx0      (B*C*16)+
      xx00      (A*C*256)
```

This algorithm is not the fastest, it's nearly 2 times slower than a regular shift and add.

'Aviator' for the BBC Micro uses this method (see [here](https://aviator.bbcelite.com/deep_dives/times_tables_and_nibble_arithmetic.html)).

### 5. Booth's Algorithm

The classic shift and add algorithm can sometimes end up doing a lot of addition. For instance multiplying by 15 involves four additions since 15 = 1+2+4+8, corresponding to a run of set bits in the multiplier. It would be quicker to multiply by 16 and subtract the original number.

[Booth's Algorithm](https://en.wikipedia.org/wiki/Booth's_multiplication_algorithm) tracks when the successive bits of one number *change* and either adds or subtracts the other number from the total as needed.

Unusually, this method is designed for signed numbers, not unsigned.

This method turns out to be ~2.7 times slower on the 6502 than an equivalent 'shift-and-add' routine, so doesn't seem to be used much in practice.

### 6. Hardware support
Some hardware has support for multiplication in silicon. These are likely to be the fastest and smallest code of all, where available. For instance, the SNES has hardware for 'unsigned 8 bit x 8 bit = 16 bit' and 'signed 16 bit x 8 bit = 24 bit' routines.

### 7. Repeated addition ###
The *supidly slow* method. To multiply m x n, just add m, n times. Avoid!

## The Implementations

I test the following routines:

| Source                     | Bits     | Method                    | From |
| -------------------------- | :------: | :-----------------------: | :---- |
| [mult1.a](tests/mult1.a)   | 16x16=32 | shift and add             | [codebase64](https://www.codebase64.org/doku.php?id=base:16bit_multiplication_32-bit_product) and [*6502 Software Design* by Leo J Scanlon](https://archive.org/details/6502softwaredesi0000scan/page/124/mode/1up) |
| [mult2.a](tests/mult2.a)   | 16x16=32 | shift and add             | [The Fridge](http://www.ffd2.com/fridge/math/mult-div.s)
| [mult3.a](tests/mult3.a)   | 16x16=32 | shift and add             | [Neil Parker](https://llx.com/Neil/a2/mult.html)
| [mult4.a](tests/mult4.a)   | 16x16=32 | shift and add             | TobyLobster and mult39 |
| [mult5.a](tests/mult5.a)   | 8x8=16   | tables of squares         | [yerricde at Everything2](https://everything2.com/user/yerricde/writeups/Fast+6502+multiplication) |
| [mult6.a](tests/mult6.a)   | 8x8=16   | tables of squares         | [eurorusty at Everything2](https://everything2.com/user/eurorusty/writeups/Fast+6502+multiplication) |
| [mult7.a](tests/mult7.a)   | 8x8=16   | shift and add             | [Apple Assembly Line, January 1986](http://www.txbobsc.com/aal/1986/aal8601.html#a5) |
| [mult8.a](tests/mult8.a)   | 8x8=16   | shift and add             | [Apple Assembly Line, January 1986](http://www.txbobsc.com/aal/1986/aal8601.html#a5) |
| [mult9.a](tests/mult9.a)   | 8x8=16   | shift and add             | [The Fridge](http://www.ffd2.com/fridge/math/mult-div8.s) |
| [mult10.a](tests/mult10.a) | 8x8=16   | shift and add             | [codebase64](https://www.codebase64.org/doku.php?id=base:8bit_multiplication_16bit_product) |
| [mult11.a](tests/mult11.a) | 8x8=16   | shift and add             | [codebase64](https://www.codebase64.org/doku.php?id=base:short_8bit_multiplication_16bit_product) |
| [mult12.a](tests/mult12.a) | 8x8=16   | shift and add             | [codebase64](https://www.codebase64.org/doku.php?id=base:8bit_multiplication_16bit_product_fast_no_tables) |
| [mult13.a](tests/mult13.a) | 8x8=16   | tables of squares         | [Apple Assembly Line, March 1986](http://www.txbobsc.com/aal/1986/aal8603.html#a5) |
| [mult14.a](tests/mult14.a) | 8x8=16   | tables of squares         | [codebase64](https://codebase64.org/doku.php?id=base:seriously_fast_multiplication) |
| [mult15.a](tests/mult15.a) | 16x16=32 | tables of squares         | [codebase64](https://www.codebase64.org/doku.php?id=base:fastest_multiplication) |
| [mult16.a](tests/mult16.a) | 8x8=16   | tables of squares         | [codebase64](https://www.codebase64.org/doku.php?id=base:another_fast_8bit_multiplication) |
| [mult17.a](tests/mult17.a) | 8x8=16   | shift and add             |[*Elite*](https://www.bbcelite.com/cassette/main/subroutine/mu11.html) |
| [mult18.a](tests/mult18.a) | 8x8=16   | shift and add             | [*Elite*](https://www.bbcelite.com/master/main/subroutine/mu11.html) |
| [mult19.a](tests/mult19.a) | 8x8=16   | shift and add             | [Neil Parker](https://www.llx.com/Neil/a2/mult.html)
| [mult20.a](tests/mult20.a) | 8x8=16   | shift and add             | [Becoming Julie](https://becomingjulie.blogspot.com/2020/07/multiplying-on-6502-but-faster.html) |
| [mult21.a](tests/mult21.a) | 8x8=16   | shift and add             | [Niels Möller](https://www.lysator.liu.se/~nisse/misc/6502-mul.html) |
| [mult22.a](tests/mult22.a) | 8x8=16   | tables of squares         | [Niels Möller](https://www.lysator.liu.se/~nisse/misc/6502-mul.html) |
| [mult23.a](tests/mult23.a) | 8x8=16   | shift and add             | [tepples at NesDev](https://www.nesdev.org/wiki/8-bit_Multiply) |
| [mult24.a](tests/mult24.a) | 8x8=16   | shift and add             | [tepples unrolled at NesDev](https://www.nesdev.org/wiki/8-bit_Multiply)
| [mult25.a](tests/mult25.a) | 8x8=16   | shift and add             | [Bregalad at NesDev](https://www.nesdev.org/wiki/8-bit_Multiply) |
| [mult26.a](tests/mult26.a) | 8x8=16   | shift and add             | [frantik at NesDev](https://www.nesdev.org/wiki/8-bit_Multiply) |
| [mult27.a](tests/mult27.a) | 8x8=16   | tables of squares         | [H2Obsession](https://sites.google.com/site/h2obsession/programming/6502) |
| [mult28.a](tests/mult28.a) | 8x8=16   | shift and add             | [Apple Assembly Line, January 1986](http://www.txbobsc.com/aal/1986/aal8601.html#a5) |
| [mult29.a](tests/mult29.a) | 8x8=16   | shift and add             | [Apple Assembly Line, January 1986](http://www.txbobsc.com/aal/1986/aal8601.html#a5) (loop unrolled) |
| [mult30.a](tests/mult30.a) | 8x8=16   | shift and add             | [tepples unrolled at NesDev](https://www.nesdev.org/wiki/8-bit_Multiply) (adjusted) |
| [mult31.a](tests/mult31.a) | 16x16=32 | tables of squares         | [codebase64](https://codebase64.org/doku.php?id=base:seriously_fast_multiplication) |
| [mult32.a](tests/mult32.a) | 8x8=16   | 4 bit multiply            | [keldon at Everything2](https://everything2.com/user/keldon/writeups/Fast+6502+multiplication)
| [mult33.a](tests/mult33.a) | 16x16=32 | tables of squares         | [Retro64](https://retro64.altervista.org/ProgrammingExamples/AssemblyLanguage/fast_mult_16bit_v05_pres.txt)
| [mult34.a](tests/mult34.a) | 8x8=16   | shift and add             | [AtariArchives](https://www.atariarchives.org/roots/chapter_10.php) |
| [mult35.a](tests/mult35.a) | 8x8=16   | shift and add             | [AtariArchives](https://www.atariarchives.org/roots/chapter_10.php) |
| [mult36.a](tests/mult36.a) | 16x16=32 | shift and add             | [*Best of Personal Computer World, ASSEMBLER ROUTINES FOR THE 6502* by David Barrow](https://archive.org/details/assembler-routines-for-the-6502/page/162/mode/2up) |
| [mult37.a](tests/mult37.a) | 8x8=16   | shift and add             | [Andrew Blance at codeburst](https://codeburst.io/lets-write-some-harder-assembly-language-code-c7860dcceba) |
| [mult38.a](tests/mult38.a) | 8x8=16   | 4 bit multiply            | [*Aviator*](https://aviator.bbcelite.com/source/main/subroutine/multiply8x8.html) |
| [mult39.a](tests/mult39.a) | 8x8=16   | shift and add             | [*Revs*](https://revs.bbcelite.com/source/main/subroutine/multiply8x8.html) |
| [mult40.a](tests/mult40.a) | 8x8=16   | shift and add             | [*Meteors*](https://gitlab.riscosopen.org/RiscOS/Sources/Apps/Diversions/Meteors/-/blob/master/Srce6502/MetSrc) |
| [mult41.a](tests/mult41.a) | 16x16=32 | tables of squares         | TobyLobster and mult13 |
| [mult42.a](tests/mult42.a) | 16x16=32 | tables of squares         | TobyLobster and mult16 |
| [mult43.a](tests/mult43.a) | 8x8=16   | shift and add             | [*6502 assembly language programming* by Lance A. Leventhal](https://archive.org/details/6502-assembly-language-programming/page/n251/mode/2up)
| [smult1.a](tests/smult1.a) | 8x8=16 (signed)           | tables of squares         | [codebase64](https://codebase64.org/doku.php?id=base:seriously_fast_multiplication) |
| [smult2.a](tests/smult2.a) | 8x8=16 (signed)           | Booth's algorithm         | [Marcus Thill](https://markusthill.github.io/programming/an-efficient-implementation-of-the-booth-algorithm-in-6502-assembler/) |
| [smult3.a](tests/smult3.a) | 16x16=32 (signed)         | tables of squares         | [codebase64](https://codebase64.org/doku.php?id=base:seriously_fast_multiplication) |
| [omult1.a](tests/omult1.a) | 16x16=16 (partial result) | shift and add             | [Apprize](https://apprize.best/programming/65816/17.html) |
| [omult2.a](tests/omult2.a) | 8x8=8   (partial result)  | shift and add             | [*The BBC Micro Compendium* by Jeremy Ruston](https://archive.org/details/BBCMicroCompendium/page/38/mode/2up) |


## The Results

In the diagrams below, greyed out dots are beaten for cycles and memory by the orange dots.
All cycle counts/byte counts include the final RTS (1 byte, 6 cycles), but do not include any initial JSR mult (3 bytes, 6 cycles).

### 8 bit x 8 bit unsigned multiply with 16 bit result

![Results of 8 x 8 bit unsigned multiply](results/6502 unsigned multiply (16x16=32 bit), Memory vs speed.svg)

| Source                     | Average Cycles | Memory (bytes) | Changes                                      |
| -------------------------- | -------------: | -------------: | :------------------------------------------- |
| [mult5.a](tests/mult5.a)   | 93.55          | 834            |                                              |
| [mult6.a](tests/mult6.a)   | 137.92         | 620            |                                              |
| [mult7.a](tests/mult7.a)   | 133.53         | 36             | with slight change to swap output parameters |
| [mult8.a](tests/mult8.a)   | 153.45         | 29             |                                              |
| [mult9.a](tests/mult9.a)   | 162.00         | 17             |                                              |
| [mult10.a](tests/mult10.a) | 221.08         | 27             |                                              |
| [mult11.a](tests/mult11.a) | 162.00         | 17             |                                              |
| [mult12.a](tests/mult12.a) | 108.64         | 71             | slightly tweaked                             |
| [mult13.a](tests/mult13.a) | 54.00          | 1075           |                                              |
| [mult14.a](tests/mult14.a) | 46.99          | 2078           |                                              |
| [mult16.a](tests/mult16.a) | 67.48          | 574            |                                              |
| [mult17.a](tests/mult17.a) | 150.47         | 28             | tweaked to handle X=0 on input               |
| [mult18.a](tests/mult18.a) | 111.62         | 73             | tweaked to handle X=0 on input               |
| [mult19.a](tests/mult19.a) | 185.00         | 18             |                                              |
| [mult20.a](tests/mult20.a) | 244.00         | 27             | *bug fixed*                                  |
| [mult21.a](tests/mult21.a) | 150.00         | 18             |                                              |
| [mult22.a](tests/mult22.a) | 77.49          | 563            |                                              |
| [mult23.a](tests/mult23.a) | 153.00         | 21             |                                              |
| [mult24.a](tests/mult24.a) | 110.63         | 70             | slightly tweaked                             |
| [mult25.a](tests/mult25.a) | 243.00         | 28             | *bug fixed*, tweaked parameter passing       |
| [mult26.a](tests/mult26.a) | 278.14         | 47             | *bug fixed*                                  |
| [mult27.a](tests/mult27.a) | 51.49          | 1316           | slightly tweaked                             |
| [mult28.a](tests/mult28.a) | 130.00         | 27             | tweaked                                      |
| [mult29.a](tests/mult29.a) | 120.00         | 43             | tweaked                                      |
| [mult30.a](tests/mult30.a) | 114.00         | 74             | tweaked                                      |
| [mult32.a](tests/mult32.a) | 117.14         | 592            |                                              |
| [mult34.a](tests/mult34.a) | 280.00         | 36             |                                              |
| [mult35.a](tests/mult35.a) | 191.00         | 22             |                                              |
| [mult37.a](tests/mult37.a) | 278.00         | 35             |                                              |
| [mult38.a](tests/mult38.a) | 97.00          | 1345           |                                              |
| [mult39.a](tests/mult39.a) | 107.00         | 69             | tweaked slightly                             |
| [mult40.a](tests/mult40.a) | 278.00         | 35             |                                              |
| [mult43.a](tests/mult43.a) | 208.90         | 26             |                                              |


### 16 bit x 16 bit unsigned multiply with 32 bit result

![Results of 16 x 16 bit unsigned multiply](results/6502 unsigned multiply (16x16=32 bit), Memory vs speed.svg)

| Source                     | Average Cycles | Memory (bytes) | Changes                                                                      |
| -------------------------- | -------------: | -------------: | :--------------------------------------------------------------------------- |
| [mult1.a](tests/mult1.a)   | 751            | 38             |                                                                              |
| [mult2.a](tests/mult2.a)   | 578            | 33             | optimised slightly                                                           |
| [mult3.a](tests/mult3.a)   | 711            | 36             |                                                                              |
| [mult4.a](tests/mult4.a)   | 567            | 137            | I use the 8x8 multiply 'mult39' (from *Revs*) and combine to make 16x16      |
| [mult15.a](tests/mult15.a) | 206.60         | 2181           |                                                                              |
| [mult31.a](tests/mult31.a) | 238.07         | 2219           |                                                                              |
| [mult33.a](tests/mult33.a) | 609.86         | 1277           | with test code removed, and tables page aligned. Stores numbers in MSB order |
| [mult36.a](tests/mult36.a) | 973.01         | 62             |                                                                              |
| [mult41.a](tests/mult41.a) | 350            | 1150           | I use the 8x8 multiply 'mult13' and combine to make 16x16                    |
| [mult42.a](tests/mult42.a) | 404.59         | 648            | I use the 8x8 multiply 'mult16' and combine to make 16x16                    |

### Miscellaneous Examples

Some signed multiply routines, and multiply with partial results.

| Source                     | Average cycles | Memory (bytes) | Notes               |
| -------------------------- | -------------: | -------------: | ------------------- |
| [smult1.a](tests/smult1.a) | 62.99          | 2095           | 8 x 8 bit *signed* multiply (16 bit result), tweaked for size and speed (based on the unsigned mult14.a) |
| [smult2.a](tests/smult2.a) | 329.67         | 49             | 8 x 8 bit *signed* multiply (16 bit result), Booth's Algorithm, *bug fixed* and optimised                |
| [smult3.a](tests/smult3.a) | 277.57         | 2253           | 16 x 16 bit *signed* multiply (32 bit result), tweaked slightly (based on the unsigned mult31.a)         |
| [omult1.a](tests/omult1.a) | 649.00         | 33             | 16 x 16 bit unsigned multiply, *ONLY lower 16 bit* result                                                |
| [omult2.a](tests/omult2.a) | 145.00         | 16             | 8 x 8 bit unsigned multiply, *ONLY lower 8 bit* result                                                   |


## Customising

### Number of bits
The most common routines I've found either multiply two 8 bit values to get a 16 bit result, or multiply two 16 bit values to get a 32 bit result.

These are useful, but in practice what you may need is something different, something custom made. For example you may need to multiply a 24 bit number by an 8 bit number, scaling the result down by 256 to get a new 24 bit number. It helps to realise that you can make these routines by building on your favourite standard 8 bit x 8 bit = 16 bit routine.

Just as binary multiplication works in base 2, this works in base 256. Each byte is one digit. For example, to make a 16 x 16 bit multiply:

```
        AB
       *CD
      ----
        xx      (B*D)+
       xx0      (A*D*256)+
       xx0      (B*C*256)+
      xx00      (A*C*256*256)
```
Adding the four partial results as shown.

### In and Out Parameters
Routines can gather input values either from registers or from memory. It can also return results in registers and/or memory.

#### 8 bit x 8 bit = 16 bit
The routines presented here will use whichever parameter method is fastest for the routine. Often the most useful from the point of view of the calling code is to use registers for both input and output, so you may want to adjust this depending on your usage. Also, if the low byte of the result is in A it can be used as the starting point for a subsequent add or subtract.

#### 16bit x 16 bit = 32 bit
These routines mostly use memory locations for most or all of the in/out parameters, as there are too many values to hold in the registers.

### Partial results only
For speed, some routines only provide a partial answer. e.g. it may return only the high byte of the result (as an approximation, often used with fixed point calculations) or the low byte (for multiplying small numbers that don't overflow).

For example, if a routine wants to multiply a 16 bit number by the sine of an angle this is a problem for an integer routine since the sine of an angle is a floating point number not an integer. By scaling up the fractional value to an integer e.g. `N=256*sin(angle)`, then the integer multiplication can happen and the result scaled down by 256. Note that negative numbers will need special treatment.

### Self modifying code
Some of the implementations use self modifying code for speed, so won't work without change from ROM code. If you can use self-modifying code, putting the code itself in zero page will make it run a little faster, if you have the space!

### Signed multiply
I will assume here the standard two's compliment representation is used. There are two methods of dealing with signed numbers; one fairly obvious, the other less obvious but better.

The more obvious method is to remove the signs from the inputs, do an unsigned multiply, then apply the appropriate sign to the result.

1. remember if the sign of the two input numbers are different
2. remove the sign bit from each input value
3. do a regular unsigned multiply
4. recall if the signs differ and negate the result if needed

The craftier method is:

1. do a regular unsigned multiply
2. If the first input is negative, subtract the second input from the result.
3. If the second input is negative, subtract the first input from the result.

That's it. This takes less memory and fewer cycles than the more obvious method so should be preferred.

### Multiply using Binary Coded Decimal (BCD)

This can be done but not very efficiently. [Here](https://llx.com/Neil/a2/decimal.html) is an implementation that uses the '[Russian peasant multiplication](https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication#Russian_peasant_multiplication)'.


## How to run the tests
* Check the dependencies below.
* The 'go' script specifies which tests to execute.
* Run the 'go' script to execute the tests.
* The 'tests' folder contains a number of assembly language files (.a files) to test.
* The testing is configured by a small associated .c file.
* The test results are written to json files.

### How testing works
* The assembly language code is assembled into a binary file using the acme assembler.
* The tester C code is compiled (using clang) along with the test parameters .c file.
* The binary is loaded and executed (simulated) multiple times, usually over all possible inputs (specified by the test's .c file).
* Any unexpected results (e.g. due to errors in the algorithm) are reported. The test case that failed is re-run with full disassembly output to aid debugging.
* The average cycle count is reported and results are output to a json file.
* Tests can be executed on multiple threads for speed.

### Dependencies
* I use the [MOS Technology 6502 CPU Emulator](https://github.com/redcode/6502) to emulate the 6502 (not included).
* I use the [Z](https://github.com/redcode/Z) header only library as it is required by the emulator (not included).
* I use the [acme](https://github.com/meonwax/acme) assembler (not included).
* I use clang to compile the C code (not included).

## See Also
See also my [test_sqrt](https://github.com/TobyLobster/sqrt_test) repository for comparing implementations of square root.
