# Data Lab: Manipulating Bits

[TOC]

## 1.1 bitXor

应用德摩根定律：

x ^ y = (x & ~y) | (~x & y)				*x为1且y为0的情况和x为0且y为1的情况进行位或*

​		 =  **~(~(x & ~y) & ~(~x & y))**		

​		 = ~((~x | y) & (x | ~y))

​		 = ~((~x | y & x) | (~x | y & ~y))

​		 = ~((x & y) | (~x & ~y))

​		 = **~(x & y) & ~(~x & ~y)**		*不是同时为1和不是同时为0的情况进行位与*

```c
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  // return ~(~(x & ~y) & ~(~x & y));
    
  return ~(x & y) & ~(~x & ~y);
}
```



## 1.2 tmin

使用位运算获取对整数的最小补码 TMin32。

```c
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
```



## 1.3 isTmax

**思路一：**

考虑 x = TMax32 = 01......11，x + 1 = 10......00，**~(x + 1) = 01......11 = x**。

最终要返回0或1，要想判断给定值 x 是否是 TMax，需要将 x 像全0值转换判断，因为0的布尔值是0，非0的布尔值就是1。

这里用到位运算的一个小技巧，两个相等的数异或会得到0，因此考虑 !(x ^ ~(x + 1)) 来判断x是否为TMax。

但这里有一个例外，当 x = 0xFFFFFFFF时，~(x + 1) = x同样成立，因此对其进行特判。

------

**思路二：**

考虑 x = TMax，x + 1 = TMin，x + 1 + x = 11......11，**~(x + 1 + x) = 0**。

同样需要特判例外 0xFFFFFFFF。

```c
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  //return !!(x + 1) & !(x ^ ~(x + 1));
    
  return !!(x + 1) & !~(x + 1 + x);
}
```



## 1.4 allOddBits

> Integer constants 0 through 255 (0xFF)

使用移位运算构造出奇数位全为1、偶数位全为0的掩码mask = 0xAAAAAAAA，然后获取 x 的奇数位，偶数位清零(mask & x)，然后通过异或操作判断结果是否等于 mask。

```c
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int mask = 0xAA << 8 | 0xAA;
  mask = mask << 16 | mask;
  return !((mask & x) ^ mask);
}
```



## 1.5  negate

x + ~x = 1

-x = ~x + 1

```c
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
```



## 1.6 isAsciiDigit

如果 x ∈ [0x30, 0x39]，则 x - lowerBound 大于等于0（符号位0）且 x - upperBound - 1 小于0（符号位1）。

不能使用符号 - ，利用 ~x + 1 实现 -x。

```c
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int a = (x + (~0x30 + 1)) >> 31 & 1;
  int b = (x + (~0x3A + 1)) >> 31 & 1;
  return !a & b;
}
```



## 1.7 conditional

核心思想是利用 x 得出全为1或全为0的位模式。

x为真，表达式 ~(!x) + 1 结果为0。

```c
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3 
 */
int conditional(int x, int y, int z) {
  x = ~(!x) + 1;
  return (~x & y) | (x & z);
}
```



## 1.8 isLessOrEqual

直接用 y - x 可能会溢出（eg : x = TMin，y = TMax，y - x = -1），不能使用这样简单的判断。

返回1的情况有以下两种：

1. x负数 y非负数；
2. x，y的符号位相同，y - x 的结果是非负数。

------

为什么使用 y - x 而不是 x - y ?

题目要判断 x <= y，如果使用 x - y，其结果的符号位为0代表 x >= y，为1代表 x < y，无法直接对应判断 x <= y。

```c
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int a = x >> 31 & 1;
  int b = y >> 31 & 1;
  int c = y + (~x + 1); // y - x
  c = c >> 31 & 1;
  return (a & !b) | (!(a ^ b) & !c);
}
```



## 1.9 logicalNeg

!x 等于 1 当且仅当 x为0。关键在于如何将0构造成1。

比较 x 和 -x(~x + 1) 的符号位：

1. x 为 0：0 0
2. x 为正数：0 1
3. x 为负数（除TMin）：1 0
4. x 为TMin：1 1

思路一：提取出两个符号位 a b，分别判断两个符号位是否都是0

思路二：x 和 -x 进行位或运算，只有x为0时结果的符号位是0，其余情况都是1。将位或结果算术右移31位，x为0对应的结果是0，其余情况是-1。

思路三：符号位为0表示x非负，而 x + TMax 不溢出表示x非正。

```c
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  // int a = x >> 31 & 1;
  // int b = (~x + 1) >> 31 & 1;
  // return (a ^ 1) & (b ^ 1);

  // return ((x | (~x + 1)) >> 31) + 1;

  int sign = x >> 31 & 1;
  int TMax = ~(1 << 31);
  return (sign ^ 1) & (((x + TMax) >> 31 & 1) ^ 1);
}
```



## 1.10 howManyBits

如果是一个非负数，需要找到最高的是1的位（假设是k-1)，再加上符号位，结果是需要 k + 1 位；如果是一个负数，对其进行取反，然后是和正数同样的操作。

使用**二分法**来寻找最高位：

- 首先判断高16位是否有1，有则表示最高位1在高16位中，低16位需要被使用，因此右移16位（表示这16位已经拿去用了）；没有则表示最高位1在低16位中。需要进一步考虑低16位。
- 迭代判断低16位、8位、4位、2位、1位。

```c
int howManyBits(int x) {
  int b16, b8, b4, b2, b1, b0;
  x = x ^ (x >> 31); // x 为负数则取反

  b16 = !!(x >> 16) << 4; // 判断高16位是否有1：有则为16，否则为0
  x >>= b16; // 如果有则将原数右移16位，表示需要这16位
  //下面再处理右移后的低16位
  b8 = !!(x >> 8) << 3;
  x >>= b8; 
  // 下面再处理右移后的低8位
  b4 = !!(x >> 4) << 2;
  x >>= b4; 
  // 下面再处理右移后的低4位
  b2 = !!(x >> 2) << 1;
  x >>= b2; 
  // 下面再处理右移后的低2位
  b1 = !!(x >> 1);
  x >>= b1; 
  // 下面再处理右移后的低1位
  b0 = x;
    
  return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
```



## 1.11 floatScale2

利用掩码提取出float各字段：s, exp, frac。

1. exp == 0：非规格化数

   - case 1：frac = 0a...b

     乘以2后，frac = a...b0

   - case 2：frac = 1...

     乘以2后，变成规格化数，exp = 1，frac = a...b0

   在代码实现中，利用 f <<= 1，case 1显然能够处理；即使是case 2，f  的有效位从23变为了24，看似不合理，但在返回值时，e 和 f 的位或操作使得 f 最高位上的 1 到了 exp 上，恰好满足要求。这里从一定程度上也反映了非规格化数到规格化数之间的平滑转变。

2. 0 < exp < 0xFF：规格化数

   形如 (-1)<sup>s</sup> * (1 + f) * 2<sup>(e - Bias)</sup> 

   乘以2，只能让 exp 加1。

   如果exp加1后为 0xFF，说明溢出了，将 frac 置0，表示 ∞。（btest默认测试用例中没有相关检测，但可以自己传入参数0x7F000001进行测试）

3. exp = 0xFF：特殊值

   - frac = 0：∞

     乘2后仍是∞，即原数。

   - frac ≠ 0：NaN

     按照题意，返回原数

   因此该情况下，直接返回原数。

```c
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  int s = (1 << 31) & uf; // s
  int f = ((1 << 23) - 1) & uf; // frac 
  int e = (uf - s - f) >> 23; // exp >> 23
  // int f1 = (1 << 22) & f; // frac第一位
  if (e == 0) { // 非规格化数
    // f = (f - f1) << 1;
    // if (f1) {
    //   e = 1;
    // }
    f <<= 1;
  }
  else if (e < 0xFF) { // 规格化数
    e = e + 1;
    if (e == 0xFF) {
      f = 0;
    }
  }
  else { // 特殊数
    return uf;
  }
  return s | (e << 23) | f;
}
```



## 1.12 floatFloat2Int

从 float 转换成 int，值将会舍入到零。

E = e - Bias

1. E < 0:

   非规格化数 及 E < 0的规格化数，因此值小于1，返回0。

2. E >= 31

   - E = 0x80  ==>  e = 0xFF：

     NaN or infinity，返回TMin。

   - 31 < E < 0x80：

     V = (1+f) * 2^E > 2^31

     超过int范围，返回TMin。

   - E = 31：
    数值绝对值大小 =  (1+f) * 2<sup>E</sup> >= 2^31 
   
     此时如果符号位 s=1 且小数字段 f=0 ，数值等于TMin；否则溢出，返回TMin。

   即：无论什么情况都返回 TMin。

3. 0 <= E <= 30

   1 <= (1+f) <= (1+f) * 2<sup>E</sup> <= (1+f) * 2^30 < 2^31

   该值及其相反数均在int范围内。根据 s 来判断数值的正反。

```c
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  int s = (uf >> 31) & 1; // s >> 31
  int f = ((1 << 23) - 1) & uf; // frac 
  int e = (uf - (s << 31) - f) >> 23; // exp >> 23
  int E = e - 127;
  int res;

  if (E < 0) {
    res = 0;
  }
  else if (E >= 31) { 
    res = 0x80000000u;
  }
  else {
    f = (1 << 23) + f;
    // 不对E判断直接 f << (E - 23) 会出现异常行为！！！见代码下面引用内容
    if (E > 23) { // 左移 
      res = f << (E - 23);
    }
    else { // 右移可能会舍弃一些为1的位，相当于实现了向零舍入
      res = f >> (23 - E);
    }

    if (s == 1) { // 负值
      res *= -1;
    }
  }
  return res;
}
```

注意：

> Has unpredictable behavior when shifting if the shift amount is less than 0 or greater than 31.

> 右侧的运算对象一定不能为负，而且值必须严格小于结果的位数，否则就会产生未定义的行为。 																				—— 《C++ Primer》p136 “移位运算符”



## 1.13 floatPower2

float:

|                             格式                             |               最小值（可表示为2<sup>n</sup>）                |               最大值（可表示为2<sup>n</sup>）                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
| 非规格化<br/> (-1)<sup>s</sup> * f * 2<sup>(1 - Bias)</sup>  | e = 0000 0000<br/>E = -126, M = 2<sup>-23</sup><br/>**2<sup>-149</sup>** | e = 0000 0000<br/>E = -126, M = 1 - 2<sup>-23</sup><br/>(1 - 2<sup>-23</sup>) * 2<sup>-126</sup> ==> **2<sup>-127</sup>** |
| 规格化<br/> (-1)<sup>s</sup> * (1 + f) * 2<sup>(e - Bias)</sup> |  e = 0000 0001<br/>E = -126, M = 0<br/>**2<sup>-126</sup>**  | e = 1111 1110<br/>E = 127, M = 2 - 2<sup>-23</sup><br/>(2 - 2<sup>-23</sup>) * 2<sup>127</sup> ==> **2<sup>127</sup>** |

1. x < -149

   " too small to be represented as a denorm, return 0 "

2. -149 <= x <= -127

   denorm（btest默认测试用例中没有相关检测）

3. -127 < x <= 127

   norm

4. x > 127

   return +INF

```c
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  unsigned res;
  if (x < -149) {
    res = 0;
  }
  else if (x < -126) { // e = 0
    res = 1 << 23 >> (-(x + 126));
  }
  else if (x <= 127) { // f = 0
    res = (x + 127) << 23;
  }
  else {
    res = 0x7F800000;
  }
  return res;
}
```





## 测试结果

![image-20230804215129133](https://gitee.com/dq-agj/imags/raw/master/img/image-20230804215129133.png)



## 参考链接

- [CSAPP:Lab1 -DataLab 超详解 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/339047608)

- [CSAPP 之 DataLab详解，没有比这更详细的了 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/59534845)

- [CSAPP DataLab 题解 | Claude's Blog (claude-ray.com)](https://claude-ray.com/2019/10/02/csapp-datalab/)

- [CSAPP - datalab · Mcginn's Blog (mcginn7.github.io)](https://mcginn7.github.io/2020/02/14/CSAPP-datalab/)

- [【读厚 CSAPP】I Data Lab | 小土刀 2.0 (wdxtub.com)](https://wdxtub.com/csapp/thick-csapp-lab-1/2016/04/16/)

  
