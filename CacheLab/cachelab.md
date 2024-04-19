# Cache Lab: Understanding Cache Memories

[TOC]

## Part A:  Writing a Cache Simulator

### 1. 处理命令行参数：getopt()

使用 `man 3 getopt` 查看。提取以下有效信息：

**int getopt(int argc, char * const argv[], const char *optstring);**

- getopt() 函数解析命令行参数。argv 中以 '-' 开头的元素是一个 option element，该元素 '-' 之后的字符是 option characters。如果 getopt() 被重复调用，它依次返回每一个 option element 的 option characters。
- optstring 是一个包含合法的 option characters 的字符串。如果这样一个 character 后跟一个冒号，表示这个选项需要后接一个参数，该参数放在 optarg 中。
- 默认情况下，getopt() 在扫描时排列 argv 的内容，因此最终所有 nonoptions 都在末尾。
- 如果成功找到某个选项，getopt() 将返回该 option character 。如果所有命令行选项都已解析，getopt() 将返回 -1。如果 getopt() 遇到一个 option character 不在 optstring 中,则返回 '?' 。如果 getopt() 遇到一个选项缺少参数，则返回值取决于 optstring 中的第一个字符：如果是 ':' ，则返回 ':' ；否则返回 '?' 。

### 2. 进行cache存储

模拟器需要能够正确处理任意的 s, E, b，因此必须使用 malloc（calloc） 来为数据结构分配空间。需要注意 malloc 的内容一定要 free 掉，不然就会造成内存泄漏。

### 3. 从文件中读入内容并处理

用 fscanf 读入 trace 文件内容。具体用法参考 man fscanf 或 [The GNU C Programming Tutorial (crasseux.com)](http://crasseux.com/books/ctutorial/fscanf.html)。



### 测试结果

![image-20230906151340745](https://gitee.com/dq-agj/imags/raw/master/img/image-20230906151340745.png)

------



## Part B: Optimizing Matrix Transpose

**要求**

实现函数 `void trans(int M, int N, int A[N][M], int B[M][N])` 转置矩阵 A 并存储到矩阵 B 中，限制：

- 缓存参数为：s = 5, E = 1, b = 5。
- 最多能够定义 12 个 int 类型的局部变量。
- 不允许修改矩阵 A，但能任意修改矩阵 B。

（评分规则）每个矩阵大小的性能得分随未命中次数 m 线性缩放，直至达到某个阈值：

- 32 × 32: 8 points if m < 300, 0 points if m > 600
- 64 × 64: 8 points if m < 1, 300, 0 points if m > 2, 000
- 61 × 67: 10 points if m < 2, 000, 0 points if m > 3, 000

> Blocking is a useful technique for reducing cache misses. See http://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf for more information.

### 32 × 32

#### v1

一个 cache line 为 32 字节，可以存放 8 个 int，共 32 个 set。对于 32 x 32 矩阵，有下面的图：

![32 x 32](https://gitee.com/dq-agj/imags/raw/master/img/14557681819928.jpg)

这里的数字表示对应的值会存在 cache 的哪个 set 中，可以看到第 9 行和第 1 行会冲突，但是如果我们分成 8 x 8 的小块，就可以保证尽可能利用到 cache 的特性（读取第 1 个的时候后面 7 个也载入 cache 中）。

理想情况下：一个 block 需要 16 个 cache line（读和写各 8 行），会 miss 16 次（这是无法避免的）；而一共有 4 × 4 = 16 个 block，所以理论上最少的 miss 次数 = 16 × 16 = 256。

写出如下代码并测试：

```c
void trans_32_32_1(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, j;
    for (ii = 0; ii < N; ii += 8) {
        for (jj = 0; jj < M; jj += 8) {
            for (i = ii; i < ii + 8; i ++) {
                for (j = jj; j < jj + 8; j ++) {
                    B[j][i] = A[i][j];
                }
            }
        }
    }
}
```

![image-20230909215934936](https://gitee.com/dq-agj/imags/raw/master/img/image-20230909215934936.png)

**分析：**

miss 超过理想次数的主要原因在于：A 和 B 中相同位置的元素是映射在同一 cache line 上的，但转置并不会把 A 中某位置的元素放到 B 中相同的地方，除非在对角线上，因为下标是一样的，此时就会发生 conflict miss。

eg：在加载 A 的第二行之后，在将 A22 传送给 B22 时，cache line 中 A 的第二行被替换成 B 的第二行，待到将 A23 传送给 B32 时，又去重新加载 A 的第二行。

对于不在对角线上的一个块，其miss情况如下：

| not in diagonal |  w   |  r   |
| --------------- | :--: | :--: |
| line 1          |  8   |  1   |
| line 2 - 8      |  0   |  1   |

对于对角线上的一个块，其miss情况如下：

| in diagonal(v1) |  w   |  r   |
| --------------- | :--: | :--: |
| line 1          |  8   |  2   |
| line 2 - 7      |  2   |  2   |
| line 8          |  2   |  1   |

从上述两表可看到，对角线上的块比其他块多出 21 次 miss，有 4 个这样的块，加之函数开销造成的 3 次 miss，因此 misses = 256 + 4 × 21 + 3 = 343。

#### v2

可以使用一个简单的办法，以空间换时间——把一行一次性全部读到临时变量中，减少冲突不命中。代码如下：

```c
void trans_32_32_2(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    for (ii = 0; ii < N; ii += 8) {
        for (jj = 0; jj < M; jj += 8) {
            for (i = ii; i < ii + 8; i ++) {
                tmp1 = A[i][jj];
                tmp2 = A[i][jj + 1];
                tmp3 = A[i][jj + 2];
                tmp4 = A[i][jj + 3];
                tmp5 = A[i][jj + 4];
                tmp6 = A[i][jj + 5];
                tmp7 = A[i][jj + 6];
                tmp8 = A[i][jj + 7];
                B[jj][i] = tmp1;
                B[jj + 1][i] = tmp2;
                B[jj + 2][i] = tmp3;
                B[jj + 3][i] = tmp4;
                B[jj + 4][i] = tmp5;
                B[jj + 5][i] = tmp6;
                B[jj + 6][i] = tmp7;
                B[jj + 7][i] = tmp8;
            }
        }
    }
}
```

![image-20230909220113771](https://gitee.com/dq-agj/imags/raw/master/img/image-20230909220113771.png)

**分析：**

这里 miss 次数 287 仍然比 256 大是因为：对于对角线上的一个块，读 A 的每一行都会 miss 一次，把 A 的 line 1 写入 B 时有 8 次 miss，这都是不可避免的。问题出在，把 A 的 line 2 - 8 写入 B 时都有一次 miss。

即：在加载 A 的第 i 行（i > 1）时，cache line 中 B 的第 i 行被替换成 A 的第 i 行，而在将 Aii 写入 Bii 时，cache line 中 A 的第 i 行仍需被替换成 B 的第 i 行，这就导致了一次 conflict miss。

对角线上的 miss情况如下：

| in diagonal(v2) |  w   |  r   |
| --------------- | :--: | :--: |
| line 1          |  8   |  1   |
| line 2 - 8      |  1   |  1   |

对角线上的块比其他块多出 7 次 miss，因此 misses = 256 + 4 × 7 + 3 = 287。

#### v3

要避免 v2 中的问题，可以在 A 访问第 i 行之前，不让 B 访问第 i 行。原本的方式中，读完一行就写完该行，现在换一个思路：

```c
// 不引用临时变量的swap实现
void swap_noTemp(int *a, int *b) {
    *a = *a ^ *b;
    *b = *a ^ *b;
    *a = *a ^ *b;
}

void trans_32_32_3(int M, int N, int A[N][M], int B[M][N]) 
{   
    int ii, jj, i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;

    for (ii = 0; ii < N; ii += 8) {
        for (jj = 0; jj < M; jj += 8) {
            if (ii == jj) {
                // loop(i)完成 (ii, ii) ~ (i, i) 正方形范围内的转置任务
                for (i = ii; i < ii + 8; i ++) {
                    /*
                     * 一次性读完A的第 i 行：
                     * 导致一次 miss，不可避免。
                     */
                    tmp1 = A[i][jj];
                    tmp2 = A[i][jj + 1];
                    tmp3 = A[i][jj + 2];
                    tmp4 = A[i][jj + 3];
                    tmp5 = A[i][jj + 4];
                    tmp6 = A[i][jj + 5];
                    tmp7 = A[i][jj + 6];
                    tmp8 = A[i][jj + 7];
                    /* 
                     * 存储 A 的第 i 行到 B 的第 i 行：
                     * 导致一次 miss，不可避免。
                     * 1：B[i][ii ~ i - 1]在下面会被覆写（if i > ii）
                     * 2：把 A[i][i] 写入 B[i][i]
                     * 3：B[i][i + 1 ~ ii + 7] 保存 A[i][i + 1 ~ ii + 7]，供后面的loop使用
                     */
                    B[i][jj] = tmp1;
                    B[i][jj + 1] = tmp2;
                    B[i][jj + 2] = tmp3;
                    B[i][jj + 3] = tmp4;
                    B[i][jj + 4] = tmp5;
                    B[i][jj + 5] = tmp6;
                    B[i][jj + 6] = tmp7;
                    B[i][jj + 7] = tmp8;
                    /*
                     * 写 B[i][ii ~ i - 1] 和 B[ii ~ i - 1][i]：
                     * cache line 中已经有 B 的第 ii ~ i 行，不会导致 miss。
                     */
                    for (j = ii; j < i; j ++) {
                        // 交换前：
                        // B[j][i]的值来源之前的loop(i)，等于A[j][i]
                        // B[i][j]的值来源当前的loop(i)，等于A[i][j]
                        // 必须调用不引用临时变量的swap函数，这样不会带来第13个变量，符合规则！
                        swap_noTemp(&B[i][j], &B[j][i]);
                    }
                }
            }
            else {
                for (i = ii; i < ii + 8; i ++) {
                    for (j = jj; j < jj + 8; j ++) {
                        B[j][i] = A[i][j];
                    }
                }
            }
        }
    }
}
```

上述代码可能有点晦涩（尤其是不加注释的情况下），实际上它就等效于将 A 复制到 B，然后 B 再通过调用不引用临时变量的swap函数，完成 B 自身的转置，整个过程没有额外的 miss：

```c
//trans_32_32_3 的等效版本
void trans_32_32_4(int M, int N, int A[N][M], int B[M][N]) 
{   
    int ii, jj, i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;

    for (ii = 0; ii < N; ii += 8) {
        for (jj = 0; jj < M; jj += 8) {
            if (ii == jj) {
                // 复制 A 的当前分块到 B
                for (i = ii; i < ii + 8; i ++) {
                    /*
                     * 一次性读完 A 的第 i 行：
                     * 导致一次 miss，不可避免。
                     */
                    tmp1 = A[i][jj];
                    tmp2 = A[i][jj + 1];
                    tmp3 = A[i][jj + 2];
                    tmp4 = A[i][jj + 3];
                    tmp5 = A[i][jj + 4];
                    tmp6 = A[i][jj + 5];
                    tmp7 = A[i][jj + 6];
                    tmp8 = A[i][jj + 7];
                    /* 
                     * 存储 A 的第 i 行到 B 的第 i 行：
                     * 导致一次 miss，不可避免。
                     */
                    B[i][jj] = tmp1;
                    B[i][jj + 1] = tmp2;
                    B[i][jj + 2] = tmp3;
                    B[i][jj + 3] = tmp4;
                    B[i][jj + 4] = tmp5;
                    B[i][jj + 5] = tmp6;
                    B[i][jj + 6] = tmp7;
                    B[i][jj + 7] = tmp8;
                }
                // B 的对应分块自身转置
                for (i = ii; i < ii + 8; i ++) {
                    for (j = ii; j < i; j ++) {
                        swap_noTemp(&B[i][j], &B[j][i]);
                    }
                }
            }
            else {
                for (i = ii; i < ii + 8; i ++) {
                    for (j = jj; j < jj + 8; j ++) {
                        B[j][i] = A[i][j];
                    }
                }
            }
        }
    }
}
```

![image-20230910171545331](https://gitee.com/dq-agj/imags/raw/master/img/image-20230910171545331.png)

最终达到了 259 次 miss，多出的 3 次是函数的开销所造成的。

------



### 64 × 64

#### v1

对于 64 x 64 矩阵，有下面的图：

![64 x 64](https://gitee.com/dq-agj/imags/raw/master/img/14557683448384.jpg)

由于宽度的变化，现在第 5 行和第 1 行就会冲突，如果还是用原来的 8 × 8 分块（ 4 × 8 分块实际效果相当于 8 × 8 分块），肯定是不行的。尝试 4 × 4 分块：

```c
void trans_64_64_1(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, tmp1, tmp2, tmp3, tmp4;

    for (ii = 0; ii < N; ii += 4) {
        for (jj = 0; jj < M; jj += 4) {
            for (i = ii; i < ii + 4; i ++) {
                tmp1 = A[i][jj];
                tmp2 = A[i][jj + 1];
                tmp3 = A[i][jj + 2];
                tmp4 = A[i][jj + 3];
                B[jj][i] = tmp1;
                B[jj + 1][i] = tmp2;
                B[jj + 2][i] = tmp3;
                B[jj + 3][i] = tmp4;
            }
        }
    }
}
```

![image-20230910201311450](https://gitee.com/dq-agj/imags/raw/master/img/image-20230910201311450.png)

**分析：**

 64 x 64 矩阵的 set index 排列具有“4 × 8”的风格，尽管不能直接使用原来的 8 × 8 分块，但我们可以分别考虑对角线上 8 × 8 分块中的 4 × 4 分块。其 miss 情况如下：

| the index of 4 × 4 block in the 8 × 8 block in diagonal: (0,0) (0,1) (1,0) (1,1) |  w   |  r   |
| ------------------------------------------------------------ | :--: | :--: |
| line 1                                                       |  4   |  1   |
| line 2 - 4                                                   |  1   |  1   |

非对角线上 8 × 8 分块中的 4 × 4 分块：

| the index of 4 × 4 block in the 8 × 8 block not in diagonal = (0,0) (1,0) |  w   |  r   |
| ------------------------------------------------------------ | :--: | :--: |
| line 1                                                       |  4   |  1   |
| line 2 - 4                                                   |  0   |  1   |

| the index of 4 × 4 block in the 8 × 8 block not in diagonal = (0,1) (1,1) |  w   |  r   |
| ------------------------------------------------------------ | :--: | :--: |
| line 1                                                       |  4   |  0   |
| line 2 - 4                                                   |  0   |  0   |

(0,1) 所需读入的内容已经在 (0,0) 处理过程中被载入 cache，因此没有 read miss。同理，(1,1) 也没有read miss。

- 对角线上的 8 × 8 分块：

  - 分块数量：8

  - miss 数量：4 × (5 + 3 × 2) = 44

  - 额外 miss：对于其中任一 4 × 4 分块内部的读写过程，
    - problem 1：读取 A 的每一行都会 miss 一次，这对于 (0,0) 和 (1,0) 不可避免，但在该两块中都只使用了载入的 8 个数字中的 4 个，而在 (0,1) 和 (1,1) 中又重新载入这 8 个数字，导致了 8 次 conflict miss；
    - problem 2：把 A 的 line 1 写入 B 时，载入的 8 个数字，但只写入 4 个，因此造成了 8 次 conflict miss。
    - problem 3：把 A 的 line 2 - 4 写入 B 时都有一次 conflict miss，一共 4 × 3 = 12 次；（可利用 32 × 32矩阵 v3 的思路解决）

- 非对角线上的 8 × 8 分块：

  - 数量：56

  - miss 数量：2 × [(5 + 3 × 1) + 4] = 24

  - 额外 miss：
    - problem 4：写入时，每次载入的 8 个数字只使用 4 个，因此造成了 8 次 conflict miss。

加之函数开销造成的 3 次 miss，因此 misses = 8 × 44 + 56 × 24 + 3 = 1699。

#### v2

受上述分析的启发，先进行 8 × 8 分块，再对每一个 8 × 8 分块进行 4 × 4 分块。

```c
void trans_64_64_2(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;

    for (ii = 0; ii < N; ii += 8) {
        for (jj = 0; jj < M; jj += 8) {
        // 以下描述中：A 代表 8 × 8 分块位置，B 代表 A 转置后的 8 × 8 分块位置
            if (ii == jj) {
                // 复制 A 的上半部分到 B：4(read) + 4(write) miss
                for (i = ii; i < ii + 4; i ++) {
                    tmp1 = A[i][jj];
                    tmp2 = A[i][jj + 1];
                    tmp3 = A[i][jj + 2];
                    tmp4 = A[i][jj + 3];
                    tmp5 = A[i][jj + 4];
                    tmp6 = A[i][jj + 5];
                    tmp7 = A[i][jj + 6];
                    tmp8 = A[i][jj + 7];
                    B[i][jj] = tmp1;
                    B[i][jj + 1] = tmp2;
                    B[i][jj + 2] = tmp3;
                    B[i][jj + 3] = tmp4;
                    B[i][jj + 4] = tmp5;
                    B[i][jj + 5] = tmp6;
                    B[i][jj + 6] = tmp7;
                    B[i][jj + 7] = tmp8;
                }
                // B(0,0) (0,1) 自身转置
                for (i = 0; i < 4; i ++) {
                    for (j = 0; j < i; j ++) {
                        swap_noTemp(&B[ii + i][jj + j], &B[ii + j][jj + i]);
                        swap_noTemp(&B[ii + i][jj + 4 + j], &B[ii + j][jj + 4 + i]);
                    }
                }
                
                // 复制 A 的下半部分到 B：4(read) + 4(write) miss
                for (i = ii + 4; i < ii + 8; i ++) {
                    tmp1 = A[i][jj];
                    tmp2 = A[i][jj + 1];
                    tmp3 = A[i][jj + 2];
                    tmp4 = A[i][jj + 3];
                    tmp5 = A[i][jj + 4];
                    tmp6 = A[i][jj + 5];
                    tmp7 = A[i][jj + 6];
                    tmp8 = A[i][jj + 7];
                    B[i][jj] = tmp1;
                    B[i][jj + 1] = tmp2;
                    B[i][jj + 2] = tmp3;
                    B[i][jj + 3] = tmp4;
                    B[i][jj + 4] = tmp5;
                    B[i][jj + 5] = tmp6;
                    B[i][jj + 6] = tmp7;
                    B[i][jj + 7] = tmp8;
                }
                // B(1,0) (1,1) 自身转置
                for (i = 0; i < 4; i ++) {
                    for (j = 0; j < i; j ++) {
                        swap_noTemp(&B[ii + 4 + i][jj + j], &B[ii + 4 + j][jj + i]);
                        swap_noTemp(&B[ii + 4 + i][jj + 4 + j], &B[ii + 4 + j][jj + 4 + i]);
                    }
                }

                /* 交换 B(0,1) 和 B(1,0) */
                for (i = ii; i < ii + 4; i ++) {
                    // 保存 B(1,0) 的一行
                    tmp1 = B[i + 4][jj];
                    tmp2 = B[i + 4][jj + 1];
                    tmp3 = B[i + 4][jj + 2];
                    tmp4 = B[i + 4][jj + 3];
                    // 保存 B(0,1) 的一行：1 × 4 miss
                    tmp5 = B[i][jj + 4];
                    tmp6 = B[i][jj + 5];
                    tmp7 = B[i][jj + 6];
                    tmp8 = B[i][jj + 7];
                    // 写 B(0,1) 的一行
                    B[i][jj + 4] = tmp1;
                    B[i][jj + 5] = tmp2;
                    B[i][jj + 6] = tmp3;
                    B[i][jj + 7] = tmp4;
                    // 写 B(1,0) 的一行：1 × 4 miss
                    B[i + 4][jj] = tmp5;
                    B[i + 4][jj + 1] = tmp6;
                    B[i + 4][jj + 2] = tmp7;
                    B[i + 4][jj + 3] = tmp8;
                }
            }
            else {
                // 读 A(0,0)并转置写入 B(0,0)：4(read) + 4(write) miss
                // 读 A(0,1)并以转置后的角度暂存 B(0,1) ：0 miss
                for (i = ii; i < ii + 4; i ++) {
                    for (j = jj; j < jj + 4; j ++) {
                        B[j][i] = A[i][j];
                        B[j][i + 4] = A[i][j + 4];
                    }
                }
                
                for (j = jj; j < jj + 4; j ++) {
                    // 在写入 B(0,1) 的对应行前，保存该行，因为其上还存储着 A(0,1)的一列：0 miss
                    tmp1 = B[j][ii + 4];
                    tmp2 = B[j][ii + 5];
                    tmp3 = B[j][ii + 6];
                    tmp4 = B[j][ii + 7];
                    // 读 A(1,0) 一列并转置写入 B(0,1) 对应行：4 miss
                    for (i = ii + 4; i < ii + 8; i ++) {
                        B[j][i] = A[i][j];
                    }
                    // 把暂存的 A(0,1) 的列内容写入到 B(1,0) 的对应行：4 × 1 miss
                    B[j + 4][ii] = tmp1;
                    B[j + 4][ii + 1] = tmp2;
                    B[j + 4][ii + 2] = tmp3;
                    B[j + 4][ii + 3] = tmp4;
                }
                // 读 A(1,1)并转置写入 B(1,1)：0 miss
                for (i = ii + 4; i < ii + 8; i ++) {
                    for (j = jj + 4; j < jj + 8; j ++) {
                        B[j][i] = A[i][j];
                    }
                }
            }
        }
    }
}
```

![image-20230911213120807](https://gitee.com/dq-agj/imags/raw/master/img/image-20230911213120807.png)

**分析：**

- 对角线上的 8 × 8 分块：额外 miss 来源于交换 B(0,1) 和 B(1,0)，共 4 行，每行交换有 2 次 miss。

- 非对角线上的 8 × 8 分块：没有额外 miss 。

因此 misses = 8 × (16 + 8) + 56 × 16 + 3 = 1091。应该没法继续优化了吧:smile:

------



### 61 × 67

| 0~7                     | 8~15                    | 16~23 | 24~31 | 32~39 | 40~47 | 48~55 | 56~63                   | 64~66    |
| ----------------------- | ----------------------- | ----- | ----- | ----- | ----- | ----- | ----------------------- | -------- |
| 0                       | 1                       | 2     | 3     | 4     | 5     | 6     | 7                       | 8 8 8    |
| 8 8 8 8 8 9 9 9         | 9 9 9 9 9 10 10 10      |       |       |       |       |       | 15 15 15 15 15 16 16 16 | 16 16 16 |
| 16 16 17 17 17 17 17 17 | 17 17 18 18 18 18 18 18 |       |       |       |       |       | 23 23 24 24 24 24 24 24 | 24 24 25 |
| 25 25 25 25 25 25 25 26 | 26 26 26 26 26 26 26 27 |       |       |       |       |       | 0 0 0 0 0 0 0 1         | 1 1 1    |
| 1 1 1 1 2 2 2 2         | 2 2 2 2 3 3 3  3        |       |       |       |       |       |                         |          |
|                         |                         |       |       |       |       |       |                         |          |
|                         |                         |       |       |       |       |       |                         |          |

#### v1

由于不规整性，很难找到显然的规律看出合适的分块大小，先用以下代码（直接进行转置操作）尝试不同的分块大小：

```c
void trans_61_67_1(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, j, block_size = 16;

     for(ii = 0; ii < N; ii += block_size) {
        for(jj = 0; jj < M; jj += block_size) {
            for (i = ii; i < ii + block_size && i < N; i ++) {
                for (j = jj; j < jj + block_size && j < M; j ++) {
                    B[j][i] = A[i][j];
                }
            }
        }
    }
}
```

结果归纳如下：

| 分块 | misses |
| :--: | :----: |
|  8   |  2118  |
|  16  |  1992  |
|  24  |  2015  |
|  32  |  2590  |

#### v2 ~ v4

根据之前的经验，尝试一些优化方法。

- v2 优化一：临时变量存储一整行。

```c
// 以分块 8 × 8 为例
void trans_61_67_2(int M, int N, int A[N][M], int B[M][N]) 
{    
    int ii, jj, i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;

    for (ii = 0; ii + 8 < N; ii += 8) {
        for (jj = 0; jj + 8 < M; jj += 8) {
            for (i = ii; i < ii + block_size; i ++) {
                tmp1 = A[i][jj];
                tmp2 = A[i][jj + 1];
                tmp3 = A[i][jj + 2];
                tmp4 = A[i][jj + 3];
                tmp5 = A[i][jj + 4];
                tmp6 = A[i][jj + 5];
                tmp7 = A[i][jj + 6];
                tmp8 = A[i][jj + 7];
                B[jj][i] = tmp1;
                B[jj + 1][i] = tmp2;
                B[jj + 2][i] = tmp3;
                B[jj + 3][i] = tmp4;
                B[jj + 4][i] = tmp5;
                B[jj + 5][i] = tmp6;
                B[jj + 6][i] = tmp7;
                B[jj + 7][i] = tmp8;
            }
        }
    }
    for (i = N / 8 * 8; i < N; i ++) {
        for (j = M / 8 * 8; j < M; j ++) {
            B[j][i] = A[i][j];
        }
    }
    for (i = 0; i < N; i ++) {
        for (j = M / 8 * 8; j < M; j ++) {
            B[j][i] = A[i][j];
        }
    }
    for (i = N / 8 * 8; i < N; i ++) {
        for (j = 0; j < M; j ++) {
            B[j][i] = A[i][j];
        }
    }
}
```

| 分块 | misses |
| :--: | :----: |
|  8   |  2109  |
|  16  |  2008  |
|  24  |  2881  |
|  32  |  2684  |

- v3 优化二：原本所有分块之间的处理顺序是行序（优先从左向右），现在尝试列序（优先从上到下）。

```c
void trans_61_67_3(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, j, block_size = 8;

    for(jj = 0; jj < M; jj += block_size) {
        for(ii = 0; ii < N; ii += block_size) {
            for (i = ii; i < ii + block_size && i < N; i ++) {
                for (j = jj; j < jj + block_size && j < M; j ++) {
                    B[j][i] = A[i][j];
                }
            }
        }
    }
}
```

| 分块 | misses |
| :--: | :----: |
|  8   |  1913  |
|  16  |  1816  |
|  24  |  1928  |
|  32  |  2520  |

- v4 优化一 + 优化二：

```c
// 以分块 16 × 16 为例
void trans_61_67_4(int M, int N, int A[N][M], int B[M][N]) 
{    
    int i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, block_size = 16;
    
    for (j = 0; j < M / block_size * block_size; j += block_size) {
        for (i = 0; i < N / block_size * block_size; i ++) {
            tmp1 = A[i][j];
            tmp2 = A[i][j + 1];
            tmp3 = A[i][j + 2];
            tmp4 = A[i][j + 3];
            tmp5 = A[i][j + 4];
            tmp6 = A[i][j + 5];
            tmp7 = A[i][j + 6];
            tmp8 = A[i][j + 7];
            B[j][i] = tmp1;
            B[j + 1][i] = tmp2;
            B[j + 2][i] = tmp3;
            B[j + 3][i] = tmp4;
            B[j + 4][i] = tmp5;
            B[j + 5][i] = tmp6;
            B[j + 6][i] = tmp7;
            B[j + 7][i] = tmp8;

            tmp1 = A[i][j + 8];
            tmp2 = A[i][j + 9];
            tmp3 = A[i][j + 10];
            tmp4 = A[i][j + 11];
            tmp5 = A[i][j + 12];
            tmp6 = A[i][j + 13];
            tmp7 = A[i][j + 14];
            tmp8 = A[i][j + 15];
            B[j + 8][i] = tmp1;
            B[j + 9][i] = tmp2;
            B[j + 10][i] = tmp3;
            B[j + 11][i] = tmp4;
            B[j + 12][i] = tmp5;
            B[j + 13][i] = tmp6;
            B[j + 14][i] = tmp7;
            B[j + 15][i] = tmp8;
        }
    }
    for (i = N / block_size * block_size; i < N; i ++) {
        for (j = M / block_size * block_size; j < M; j ++) {
            B[j][i] = A[i][j];
        }
    }
    for (i = 0; i < N; i ++) {
        for (j = M / block_size * block_size; j < M; j ++) {
            B[j][i] = A[i][j];
        }
    }
    for (i = N / block_size * block_size; i < N; i ++) {
        for (j = 0; j < M; j ++) {
            B[j][i] = A[i][j];
        }
    }
}
```

| 分块 | misses |
| :--: | :----: |
|  8   |  1905  |
|  16  |  1897  |
|  24  |  2847  |
|  32  |  2684  |

#### 总结

| 分块 | v1（直接转置） | v2（临时变量存储一整行） | v3（列序处理分块） | v4（优化一 + 优化二） |
| :--: | :------------: | :----------------------: | :----------------: | :-------------------: |
|  8   |      2118      |           2109           |        1913        |         1905          |
|  16  |      1992      |           2008           |      **1816**      |         1897          |
|  24  |      2015      |           2881           |        1928        |         2847          |
|  32  |      2590      |           2684           |        2520        |         2684          |

可以看到，在上述所有 case 中，当**在分块之间列序处理，在分块内部直接转置，且分块大小为 16 × 16** 时，得到了最小的 misses：1816。

![image-20230911235917120](https://gitee.com/dq-agj/imags/raw/master/img/image-20230911235917120.png)



### 测试结果

![image-20230912000045915](https://gitee.com/dq-agj/imags/raw/master/img/image-20230912000045915.png)

------



## 参考链接

[CSAPP - cachelab · Mcginn's Blog (mcginn7.github.io)](https://mcginn7.github.io/2020/02/23/CSAPP-cachelab/)

[【读厚 CSAPP】IV Cache Lab | 小土刀 2.0 (wdxtub.com)](https://wdxtub.com/csapp/thick-csapp-lab-4/2016/04/16/)

[CSAPP实验之cache lab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/79058089)

[《深入理解计算机系统》配套实验：Cache Lab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/33846811)

[CS:APP配套实验4：Cache Lab笔记 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/28585726)

[CSAPP-Labs/notes/cachelab.md at master · Exely/CSAPP-Labs (github.com)](https://github.com/Exely/CSAPP-Labs/blob/master/notes/cachelab.md)