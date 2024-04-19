# CSAPP - Chapter 7 Homework

#### 7.6

| Symbol | swap.o .symtab entry? | Symbol type | Module where defined | Section |
| :----- | :-------------------- | :---------- | :------------------- | ------- |
| buf    | Yes                   | external    | m                    | .data   |
| bufp0  | Yes                   | global      | swap                 | .data   |
| bufp1  | Yes                   | local       | swap                 | .bss    |
| swap   | Yes                   | global      | swap                 | .text   |
| temp   | No                    | ——          | —–                   | ——      |
| incr   | Yes                   | local       | swap                 | .text   |
| count  | Yes                   | local       | swap                 | .bss    |

test:

```bash
$ gcc -c swap.c
$ objdump -t swap.o

swap.o:     file format elf64-x86-64

SYMBOL TABLE:
0000000000000000 l    df *ABS*  0000000000000000 swap.c
0000000000000000 l    d  .text  0000000000000000 .text
0000000000000000 l    d  .data  0000000000000000 .data
0000000000000000 l    d  .bss   0000000000000000 .bss
0000000000000000 l    d  .data.rel      0000000000000000 .data.rel
0000000000000000 l     O .bss   0000000000000008 bufp1
0000000000000000 l     F .text  000000000000001a incr
0000000000000008 l     O .bss   0000000000000004 count.1915
0000000000000000 l    d  .note.GNU-stack        0000000000000000 .note.GNU-stack
0000000000000000 l    d  .note.gnu.property     0000000000000000 .note.gnu.property
0000000000000000 l    d  .eh_frame      0000000000000000 .eh_frame
0000000000000000 l    d  .comment       0000000000000000 .comment
0000000000000000 g     O .data.rel      0000000000000008 bufp0
0000000000000000         *UND*  0000000000000000 buf
000000000000001a g     F .text  0000000000000051 swap


```



#### 7.7

Turn x in bar5.c into a local symbol.

```c
/* bar5.c */
static double x;

void f() {
	x = -0.0;
}
```



#### 7.8

A.

(a) REF(main.1) -> DEF(main.1)

(b) REF(main.2) -> DEF(main.2)

B.

(a) REF(x.1) -> DEF(unkown)

(b) REF(x.2) -> DEF(unkown)

C.

(a) REF(x.1) -> DEF(ERROR)

(b) REF(x.2) -> DEF(ERROR)



#### 7.9

```c
/* foo6.c */
void p2(void);

int main() {
	p2();
	return 0;
}
```

```c
/* bar6.c */
#include <stdio.h>

char main;

void p2() {
	printf("0x%x\n", main);
}
```

function main print `0xfffffff3` on my machine.

```bash
$ gcc -Wall -o foobar6 foo6.c bar6.c
bar6.c:4:6: warning: ‘main’ is usually a function [-Wmain]
    4 | char main;
      |      ^~~~
$ ./foobar6
0xfffffff3
```

using objdump inspect : `objdump -d foobar6`

find function main

```assembly
0000000000001149 <main>:
    1149:       f3 0f 1e fa             endbr64
    114d:       55                      push   %rbp
    114e:       48 89 e5                mov    %rsp,%rbp
    1151:       e8 07 00 00 00          callq  115d <p2>
    1156:       b8 00 00 00 00          mov    $0x0,%eax
    115b:       5d                      pop    %rbp
    115c:       c3                      retq
```

`0xf3` is the first byte content of function `main`.

works same like:

```c
/* another-bar6.c */
#include <stdio.h>

int main();

void p2() {
  printf("0x%x\n", *(char *)main);
}
```

```bash
$ gcc -Wall -o another-foobar6 foo6.c another-bar6.c
$ ./another-foobar6
0xfffffff3
```

**explanation** :  `(char *)main` points to a signed value `f3` (`char` is same as `signed char` on my machine, which is determined by complier.), and symbol expansion occurs to match `%x` in `printf`.



#### 7.10

A. 

gcc p.o libx.a

B.

gcc p.o libx.a liby.a libx.a

C.

gcc p.o libx.a liby.a libx.a libz.a



#### 7.11

> The remaining 8 bytes in the segment correspond to .bss data that will be initialized to zero at run time.



#### 7.12

A.

ADDR(s) = ADDR(.text) = 0x4004e0

ADDR(r.symbol) = ADDR(swap) = 0x4004f8

refaddr = ADDR(s) + r.offset = 0x4004ea

*refptr = (unsigned) (ADDR(r.symbol) + r.addend - refaddr) = (unsigned) 0xa

B.

ADDR(s) = ADDR(.text) = 0x4004d0

ADDR(r.symbol) = ADDR(swap) = 0x400500

refaddr = ADDR(s) + r.offset = 0x4004da

*refptr = (unsigned) (ADDR(r.symbol) + r.addend - refaddr) = (unsigned) 0x22



#### 7.13

A. How many object files are contained in the versions of libc.a and libm.a on your system?

**libc.a**

```bash
$ whereis libc.a
libc: /usr/lib/x86_64-linux-gnu/libc.so /usr/lib/x86_64-linux-gnu/libc.a /usr/share/man/man7/libc.7.gz
$ ar -t /usr/lib/x86_64-linux-gnu/libc.a
| wc -l
1742
```

**libm.a**

```bash
$ whereis libm.a
libm: /usr/lib/x86_64-linux-gnu/libm.a /usr/lib/x86_64-linux-gnu/libm.so
$ ar -t /usr/lib/x86_64-linux-gnu/libm.a | wc -l
ar: /usr/lib/x86_64-linux-gnu/libm.a: file format not recognized
0
```

Check the format of `libm.a`

```bash
$ file /usr/lib/x86_64-linux-gnu/libm.a
/usr/lib/x86_64-linux-gnu/libm.a: ASCII text
```

By contrast, check the format of `libc.a`

```bash
$ file /usr/lib/x86_64-linux-gnu/libc.a
/usr/lib/x86_64-linux-gnu/libc.a: current ar archive
```

See the content of `libm.a`

```bash
$ cat /usr/lib/x86_64-linux-gnu/libm.a
/* GNU ld script
*/
OUTPUT_FORMAT(elf64-x86-64)
GROUP ( /usr/lib/x86_64-linux-gnu/libm-2.31.a /usr/lib/x86_64-linux-gnu/libmvec.a )
```

This file tells us the real library file is `/usr/lib/x86_64-linux-gnu/libm-2.31.a`  and `/usr/lib/x86_64-linux-gnu/libmvec.a` .

```bash
$ ar -t /usr/lib/x86_64-linux-gnu/libm-2.31.a | wc -l
802
$ ar -t /usr/lib/x86_64-linux-gnu/libmvec.a | wc -l
128
```

B. Does `gcc -Og` produce different executable code than `gcc -Og -g`?

```c
/* test.c */
int main() {
        int a = 1;
        int b = a + 1;
        return 0;
}
```

```bash
$ gcc -Og -o og test.c
$ gcc -Og -g -o ogg test.c
```

```bash
$ sha256sum og ogg
08a039901a606d8b466fab1ff62e346ff1cda40e42debe556001dbfbefb82b97  og
8939ad0acbf253f00378fc193f1cda7b449c52bafc9f70769668f826bbb4cc8e  ogg
```

og differs from ogg.

Displays the information contained in these two ELF format object files' section headers respectively and show their differences.

```bash
$ readelf -S og > og.txt
$ readelf -S ogg > ogg.txt
$ diff og.txt ogg.txt
1c1
< There are 29 section headers, starting at offset 0x3910:
---
> There are 34 section headers, starting at offset 0x3bb8:
58,60c58,70
<   [26] .symtab           SYMTAB           0000000000000000  00003040
<        00000000000005d0  0000000000000018          27    44     8
<   [27] .strtab           STRTAB           0000000000000000  00003610
---
>   [26] .debug_aranges    PROGBITS         0000000000000000  0000303b
>        0000000000000030  0000000000000000           0     0     1
>   [27] .debug_info       PROGBITS         0000000000000000  0000306b
>        000000000000006e  0000000000000000           0     0     1
>   [28] .debug_abbrev     PROGBITS         0000000000000000  000030d9
>        000000000000004c  0000000000000000           0     0     1
>   [29] .debug_line       PROGBITS         0000000000000000  00003125
>        0000000000000043  0000000000000000           0     0     1
>   [30] .debug_str        PROGBITS         0000000000000000  00003168
>        00000000000000c3  0000000000000001  MS       0     0     1
>   [31] .symtab           SYMTAB           0000000000000000  00003230
>        0000000000000648  0000000000000018          32    49     8
>   [32] .strtab           STRTAB           0000000000000000  00003878
62,63c72,73
<   [28] .shstrtab         STRTAB           0000000000000000  00003800
<        000000000000010c  0000000000000000           0     0     1
---
>   [33] .shstrtab         STRTAB           0000000000000000  00003a68
>        000000000000014c  0000000000000000           0     0     1
```

**conclusion**: The difference between them is the production of `gcc -Og -g` contains debugging information.

C. What shared libraries does the gcc driver on your system use?

```bash
$ which gcc
/usr/bin/gcc
$ ldd /usr/bin/gcc
        linux-vdso.so.1 (0x00007ffd237d4000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f1709077000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f1709276000)
```

