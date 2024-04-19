/* 
 * name: dq
 * time: 2023/9/6
 */

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void trans_32_32_1(int M, int N, int A[N][M], int B[M][N]);
void trans_32_32_2(int M, int N, int A[N][M], int B[M][N]);
void trans_32_32_3(int M, int N, int A[N][M], int B[M][N]);
void trans_32_32_4(int M, int N, int A[N][M], int B[M][N]);

void trans_64_64_1(int M, int N, int A[N][M], int B[M][N]);
void trans_64_64_2(int M, int N, int A[N][M], int B[M][N]);

void trans_61_67_1(int M, int N, int A[N][M], int B[M][N]);
void trans_61_67_2(int M, int N, int A[N][M], int B[M][N]);
void trans_61_67_3(int M, int N, int A[N][M], int B[M][N]);
void trans_61_67_4(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32) {
        trans_32_32_4(M, N, A, B);
    }
    else if (M == 64 && N == 64) {
        trans_64_64_2(M, N, A, B);
    }
    else if (M == 61 && N == 67) {
        trans_61_67_3(M, N, A, B);
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */
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

//trans_32_32_3 的等效版本
void trans_32_32_4(int M, int N, int A[N][M], int B[M][N]) 
{   
    int ii, jj, i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;

    for (ii = 0; ii < N; ii += 8) {
        for (jj = 0; jj < M; jj += 8) {
            if (ii == jj) {
                // 复制 A 的当前分块 到 B
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

void trans_32_32_5(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j ++) {
            tmp1 = A[i][j];
            tmp2 = A[i + 1][j];
            tmp3 = A[i + 2][j];
            tmp4 = A[i + 3][j];
            tmp5 = A[i + 4][j];
            tmp6 = A[i + 5][j];
            tmp7 = A[i + 6][j];
            tmp8 = A[i + 7][j];
            B[j][i] = tmp1;
            B[j][i + 1] = tmp2;
            B[j][i + 2] = tmp3;
            B[j][i + 3] = tmp4;
            B[j][i + 4] = tmp5;
            B[j][i + 5] = tmp6;
            B[j][i + 6] = tmp7;
            B[j][i + 7] = tmp8;
        }
    }
}



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



void trans_61_67_1(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, j, block_size = 16; // 尝试不同的分块大小

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

void trans_61_67_2(int M, int N, int A[N][M], int B[M][N]) 
{    
    int ii, jj, i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    int block_size = 16;
    // int n = N / block_size * block_size;
    // int m = M / block_size * block_size;

    for (ii = 0; ii + block_size < N; ii += block_size) {
        for (jj = 0; jj + block_size < M; jj += block_size) {
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

                tmp1 = A[i][jj + 8];
                tmp2 = A[i][jj + 9];
                tmp3 = A[i][jj + 10];
                tmp4 = A[i][jj + 11];
                tmp5 = A[i][jj + 12];
                tmp6 = A[i][jj + 13];
                tmp7 = A[i][jj + 14];
                tmp8 = A[i][jj + 15];
                B[jj + 8][i] = tmp1;
                B[jj + 9][i] = tmp2;
                B[jj + 10][i] = tmp3;
                B[jj + 11][i] = tmp4;
                B[jj + 12][i] = tmp5;
                B[jj + 13][i] = tmp6;
                B[jj + 14][i] = tmp7;
                B[jj + 15][i] = tmp8;

                tmp1 = A[i][jj + 16];
                tmp2 = A[i][jj + 17];
                tmp3 = A[i][jj + 18];
                tmp4 = A[i][jj + 19];
                tmp5 = A[i][jj + 20];
                tmp6 = A[i][jj + 21];
                tmp7 = A[i][jj + 22];
                tmp8 = A[i][jj + 23];
                B[jj + 16][i] = tmp1;
                B[jj + 17][i] = tmp2;
                B[jj + 18][i] = tmp3;
                B[jj + 19][i] = tmp4;
                B[jj + 20][i] = tmp5;
                B[jj + 21][i] = tmp6;
                B[jj + 22][i] = tmp7;
                B[jj + 23][i] = tmp8;

                tmp1 = A[i][jj + 24];
                tmp2 = A[i][jj + 25];
                tmp3 = A[i][jj + 26];
                tmp4 = A[i][jj + 27];
                tmp5 = A[i][jj + 28];
                tmp6 = A[i][jj + 29];
                tmp7 = A[i][jj + 30];
                tmp8 = A[i][jj + 31];
                B[jj + 24][i] = tmp1;
                B[jj + 25][i] = tmp2;
                B[jj + 26][i] = tmp3;
                B[jj + 27][i] = tmp4;
                B[jj + 28][i] = tmp5;
                B[jj + 29][i] = tmp6;
                B[jj + 30][i] = tmp7;
                B[jj + 31][i] = tmp8;
            }
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

void trans_61_67_3(int M, int N, int A[N][M], int B[M][N]) 
{
    int ii, jj, i, j, block_size = 16; // 尝试不同的分块大小

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

void trans_61_67_4(int M, int N, int A[N][M], int B[M][N]) 
{    
    int i, j, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    int block_size = 16;
    // int n = N / block_size * block_size;
    // int m = M / block_size * block_size;
    
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

            // tmp1 = A[i][j + 16];
            // tmp2 = A[i][j + 17];
            // tmp3 = A[i][j + 18];
            // tmp4 = A[i][j + 19];
            // tmp5 = A[i][j + 20];
            // tmp6 = A[i][j + 21];
            // tmp7 = A[i][j + 22];
            // tmp8 = A[i][j + 23];
            // B[j + 16][i] = tmp1;
            // B[j + 17][i] = tmp2;
            // B[j + 18][i] = tmp3;
            // B[j + 19][i] = tmp4;
            // B[j + 20][i] = tmp5;
            // B[j + 21][i] = tmp6;
            // B[j + 22][i] = tmp7;
            // B[j + 23][i] = tmp8;

            // tmp1 = A[i][j + 24];
            // tmp2 = A[i][j + 25];
            // tmp3 = A[i][j + 26];
            // tmp4 = A[i][j + 27];
            // tmp5 = A[i][j + 28];
            // tmp6 = A[i][j + 29];
            // tmp7 = A[i][j + 30];
            // tmp8 = A[i][j + 31];
            // B[j + 24][i] = tmp1;
            // B[j + 25][i] = tmp2;
            // B[j + 26][i] = tmp3;
            // B[j + 27][i] = tmp4;
            // B[j + 28][i] = tmp5;
            // B[j + 29][i] = tmp6;
            // B[j + 30][i] = tmp7;
            // B[j + 31][i] = tmp8;
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



/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
   int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

    // registerTransFunction(trans_32_32, trans_32_32_desc);
    // registerTransFunction(trans_32_32_1, trans_32_32_1_desc); 

    // registerTransFunction(trans_64_64, trans_64_64_desc); 
    // registerTransFunction(trans_64_64_1, trans_64_64_1_desc);
    // registerTransFunction(trans_64_64_2, trans_64_64_2_desc); 

    // registerTransFunction(trans_61_67, trans_61_67_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

