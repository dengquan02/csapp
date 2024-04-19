# Bomb Lab: Defusing a Binary Bomb

[TOC]





## phase_1

### 调试过程

设置断点在 explode_bomb， phase_1。

In : main

​	调用 phase_1(input)。

In : phase_1

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230811214333219.png" alt="image-20230811214333219" style="zoom: 67%;" />

- %rdi 存放 input 字符串地址，%rsi 是另外一个字符串地址，

- 调用 strings_not_equal，传入上述两个字符串，

- 如果不相同返回1，调用 explode_bomb，BOOM！

  相同返回0，回到 main 中调用 phase_defused()，炸弹解除！

### 解题方法

让 input 等于 phase_1中赋给 %rsi 的字符串："Border relations with Canada have never been better."

------



## phase_2

### 调试过程

设置断点在 explode_bomb， phase_2。

In : main

​	调用 phase_2(input)。

In : phase_2

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230811165831084.png" alt="image-20230811165831084" style="zoom: 67%;" />

- <phase_2> - <phase_2+6> : 

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>   
      <tr height="40">
          <td colspan="2">old %rbp</td>
      </tr>
      <tr height="40">
          <td colspan="2">old %rbx</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"><span style="float:right">(%rsp)、(%rsi)</span></td>
      </tr>
  </table>

- <phase_2+9> : 调用 read_six_numbers，读入6个数字，假设输入的是“n1 n2 n3 n4 n5 n6”。

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>   
      <tr height="40">
          <td colspan="2">old %rbp</td>
      </tr>
      <tr height="40">
          <td colspan="2">old %rbx</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td>n6</td>
          <td>n5</td>
      </tr>
      <tr height="40">
          <td>n4</td>
          <td>n3</td>
      </tr>
      <tr height="40">
          <td width="100">n2</td>
          <td width="100">n1<span style="float:right">(%rsp)</span></td>
      </tr>
  </table>

- <phase_2+14> : `cmpl $0x1,(%rsp)` 表示**第一个数字必须是1**（否则爆炸），然后跳转到<phase_2+52>

- <phase_2+52> - <phase_2+62> : 给 %rbx、%rbp 赋值，然后跳转到 <phase_2+27>

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>   
      <tr height="40">
          <td colspan="2">old %rbp</td>
      </tr>
      <tr height="40">
          <td colspan="2">old %rbx</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"><span style="float:right">(%rbp)</span></td>
      </tr>
      <tr height="40">
          <td>n6</td>
          <td>n5</td>
      </tr>
      <tr height="40">
          <td>n4</td>
          <td>n3</td>
      </tr>
      <tr height="40">
   		<td>n2<span style="float:right">(%rbx)</span></td>
          <td>n1<span style="float:right">(%rsp)</span></td>
      </tr>
  </table><phase_2+27>

- <phase_2+27> - <phase_2+50> : 
  
  - 初始时，%rbx 指向第二个数字，%rbp 指向第六个数字的身后。
  - **为 %eax 赋值为 %rbx 指向的前一个数字，然后与自己相加（相当于乘以2），将结果与 %rbx指向的数字比较，两者必须相等**（否则爆炸）；
  - 然后 %rbx 向上加4，指向下一个数字；
  - 然后判断 %rbx 是否等于 %rbp，若相等表示已经判断完毕，跳出循环然后返回到 main；若不相等，则跳转回 <phase_2+27>，开启下一次循环。

### 解题方法

首项为1、公比为2的等比数列：“1 2 4 8 16 32”

------



## phase_3

### 调试过程

设置断点在 explode_bomb， phase_3。

In : main

​	调用 phase_3(input)。

In : phase_3

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230811220815436.png" alt="image-20230811220815436" style="zoom: 67%;" />

- <phase_3> - <phase_3+24> : %rdi 指向 input 字符串，%rsi 指向字符串 "%d %d"，表示从 input 中读入2个数字。如果 input 中包含的数字小于两个，则会爆炸。假设输入的是“n1 n2”，跳转到 <phase_3+39>。

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td>n2</td>
          <td>n1</td>
      </tr>
      <tr height="40">
          <td colspan="2"><span style="float:right">(%rsp)</span></td>
      </tr>
  </table>

- <phase_3+39> - <phase_3+118> : 

  - 若第一个数 n1大于7则跳转到<phase_3+106>，爆炸；
  - 否则跳转到 M(0x402470 + 8 * n1)，执行一条 mov 指令，然后跳转到<phase_3+123>。

  查看各地址映射到内存中的内容（地址）：

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230811233055588.png" alt="image-20230811233055588" style="zoom: 67%;" />

  得出下表：

  |  n1  |  M(0x402470 + 8 * n1)  | mov指令后的 %eax |      |
  | :--: | :--------------------: | :--------------: | :--: |
  |  0   |      <phase_3+57>      |       0xcf       | 207  |
  |  1   |     <phase_3+118>      |      0x137       | 311  |
  |  2   |      <phase_3+64>      |      0x2c3       | 707  |
  |  3   |      <phase_3+71>      |      0x100       | 256  |
  |  4   |      <phase_3+78>      |      0x185       | 389  |
  |  5   |      <phase_3+85>      |       0xce       | 206  |
  |  6   |      <phase_3+92>      |      0x2aa       | 682  |
  |  7   |      <phase_3+99>      |      0x147       | 327  |
  | > 7  | <phase_3+106> --> BOOM |                  |      |

- <phase_3+123> : 比较 %eax 和第二个数 n2，如果相等则正常返回 main，否则就会爆炸。

### 解题方法

要让 n1 不大于 7，且其对应的 %eax 等于 n2，因此有解集：{"0 207", "1 311", "2 707", "3 256", "4 389", "5 206", "6 682", "7 327"}。

------



## phase_4

### 调试过程

设置断点在 explode_bomb， phase_4。

In : main

​	调用 phase_4(input)。

In : phase_4

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230812005754957.png" alt="image-20230812005754957" style="zoom: 67%;" />

- <phase_4> - <phase_4+41> : %rdi 指向 input 字符串，%rsi 指向字符串 "%d %d"，表示从 input 中读入2个数字。如果 input 中包含的数字小于两个，则会爆炸。

  假设输入的是“n1 n2”，比较 n1 和 0xe 的大小：（`jbe`描述的是无符号<=，读入负数会转成unsigned）

  - **0 <= n1 <= 0xe**，跳转到 <phase_4+46>；
  - 否则，爆炸！

- <phase_4+46> - <phase_4+85> : 调用 func4(%rdi, %rsi, %rdx) = func4(n1, 0, 14)。**func4 结果为0**，且 **n2 等于 0**，正常返回 main；否则爆炸。

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td>n2</td>
          <td>n1<span style="float:right">(%rdi)</span></td>
      </tr>
      <tr height="40">
          <td colspan="2"><span style="float:right">(%rsp)</span></td>
      </tr>
  </table>

In : func4

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230812111128599.png" alt="image-20230812111128599" style="zoom: 67%;" />

- \<func4> - <func4+17> :  对于 func4(a, b, c)，%ecx 赋值为 {[(c - b) + (c - b) >><sub>L</sub> 31] >><sub>A</sub> 1} + b，记作 d。
  - c >= b 时，d = (c - b) / 2 + b = (c + b) / 2
  - c < b 时，d = (c - b + 1) / 2 + b =  (c + b + 1) / 2

- <func4+20> - <func4+61> : 比较 %ecx（d） 和 %edi（n1），
  - case1 : d > n1，返回 2 * func4(n1, b, d - 1)。
  - case2 : d == n1，返回 0。
  - case3 : d < n1，返回 1 + 2 * func4(n1, d + 1, c)。

### 解题方法

要想炸弹不爆炸，必须满足：

- 0 <= n1 <= 14

- n2 == 0

- func4(n1, 0, 14) == 0

  - 初始时 b == 0, c == 14，所以在递归调用过程中 d = (c + b) / 2，每深入一层参数 b 和 c 的差距会减 1，d 也因此越来越接近 n1，直到 d == n1。
  - func4 内部有递归调用，最深层一定是case 2 返回 0，在回溯过程中，只有不遇见 case 3，才能使得最终结果为 0。因此设计 n1 的值的关键在于避开 case 3，过程调用链一定是这种形式：**case 1 --> case 1 --> ... --> case 1 --> case 2**。因此 b 总是为 0，则 d 总是等于 c / 2，假设尽可能多地遇见 case 1，即让 n1 尽量小，我们来看看 d 在该条件下有哪些取值。
  
  |  b   |    c     |  d   |
  | :--: | :------: | :--: |
  |  0   |    14    |  7   |
  |  0   | 7-1 = 6  |  3   |
  |  0   | 3-1 = 2  |  1   |
  |  0   | 1-1 = 0  |  0   |
  |  0   | 0-1 = -1 |  0   |
  |  0   | 0-1 = -1 |  0   |
  | ...  |   ...    | ...  |
  |  0   |    -1    |  0   |

​		可以看到 d 有四种取值 {7,3,1,0}，而 func4 返回的条件一定是 case 2，即让 d = n1，因此 {7,3,1,0} 也是 n1 的取值范围。                                                                                        

综上所述，有解集：{"0 0", "1 0", "3 0", "7 0"}。

------



## phase_5

### 调试过程

设置断点在 explode_bomb， phase_5。

In : main

​	调用 phase_5(input)。

In : phase_5

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230812145451402.png" alt="image-20230812145451402" style="zoom: 67%;" />

- <phase_5> - <phase_5+34> : %rdi 指向 input 字符串，要求**输入的字符串长度等于6**（否则爆炸！），跳转到 <phase_5+112>。

- <phase_5+112> - <phase_5+117>、<phase_5+41> - <phase_5+76> : 

  input 指向输入的字符串，以 %rax 为偏移量，寻址到的字符 ch 存入栈顶位置；

  0x4024b0 指向另一个字符串常量 s，以 ch 的低4位作为偏移量，寻址到的字符存入 0x10(%rsp)。

  ![image-20230812162032943](https://gitee.com/dq-agj/imags/raw/master/img/image-20230812162032943.png)

  逆向工程如下：

  ```c
  char *s = "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?";
  
  // input in %rbx, i in %rax, ch in (%rsp), res in 0x10(%rsp)
  char ch;
  char *res;
  for (int i = 0; i < 6; i ++) {
      ch = input[i];
      res[i] = s[ch & 0xf + i];
  }
  ```

- <phase_5+81> - <phase_5+100> : %rdi 指向上面循环中得到的字符串（6个字符）res，%rsi 是另外一个字符串 "flyers"，如果**两个字符串不相同就会爆炸**，如果相同则跳转到 <phase_5+119>。

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230812171646489.png" alt="image-20230812171646489" style="zoom:80%;" />

- <phase_5+119> - <phase_5+145> : 检查缓冲区是否溢出，如果不溢出则正常返回到 main，否则调用 __stack_chk_fail。

### 解题方法

要想在循环中得到 "flyers"，每个字符在字符串 s 中对应的合法下标如下表所示，input[0] ~ input[5] 的低四位表示分别应该是 9, f, e, 5, 6, 7。

| 字符 | 字符串 s 中对应的下标 | 合法下标∈ [0, 0xf] | 以该下标作为低四位表示的字符 |
| :--: | :-------------------: | :----------------: | :--------------------------: |
|  f   |           9           |         9          |         ) 9 I Y i y          |
|  l   |        15, 59         |         15         |        / ? O _ o DEL         |
|  y   |    14, 19, 29, 67     |         14         |         . > N ^ n ~          |
|  e   |         5, 44         |         5          |         % 5 E U e u          |
|  r   |         6, 58         |         6          |         & 6 F V f v          |
|  s   |         7, 37         |         7          |         ' 7 G W g w          |

输入长度为6字符串，每个字符从上表中挑选。

例如：")/.%&7"

------



## phase_6

### 调试过程

设置断点在 explode_bomb， phase_6。

In : main

​	调用 phase_6(input)。

In : phase_6

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230812225348282.png" alt="image-20230812225348282" style="zoom: 67%;" />

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230812225555636.png" alt="image-20230812225555636" style="zoom: 67%;" />

- <phase_6> - <phase_6+18> : 

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230813234916558.png" alt="image-20230813234916558" style="zoom:80%;" />

  调用 read_six_numbers，从input字符串读入6个数字，假设输入的是“n1 n2 n3 n4 n5 n6”。

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r14</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r13</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r12</td>
      </tr>
      <tr height="40">
          <td colspan="2">%rbp</td>
      </tr>
      <tr height="40">
          <td colspan="2">%rbx</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td>n6</td>
          <td>n5</td>
      </tr>
      <tr height="40">
          <td>n4</td>
          <td>n3</td>
      </tr>
      <tr height="40">
          <td width="100">n2</td>
          <td width="100">n1<span style="float:right">(%rsp)</span></td>
      </tr>
  </table>

- <phase_6+23> - <phase_6+93> : 

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230813234957781.png" alt="image-20230813234957781" style="zoom:80%;" />

  逆向工程如下：

  ```c
  // int[] a in %rsp, i in %r12d
  for (int i = 0; i <= 5; i ++) {
      if (a[i] - 1 > 5u) {
          explode_bomb();
      }
      // j in %ebx
      for (int j = i + 1; j <= 5; j ++) {
          if (a[j] == a[i]) {
              explode_bomb();
          }
      }
  }
  ```

  **输入的每个数必须小于等于 6**（同时要**大于等于1**，因为`jbe`描述的是无符号<=，读入负数会转成unsigned），且**各不相同**，否则爆炸。因此**输入只能是 1 ~ 6 的排列**。

- <phase_6+95> - <phase_6+121> : 

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230813235427021.png" alt="image-20230813235427021" style="zoom:80%;" />

  逆向工程如下：

  ```c
  for (int i = 0; i < 6; i ++) {
      a[i] = 7 - a[i];
  }
  ```

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r14</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r13</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r12</td>
      </tr>
      <tr height="40">
          <td colspan="2">%rbp</td>
      </tr>
      <tr height="40">
          <td colspan="2">%rbx</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td>7 - n6</td>
          <td>7 - n5</td>
      </tr>
      <tr height="40">
          <td>7 - n4</td>
          <td>7 - n3</td>
      </tr>
      <tr height="40">
          <td width="100">7 - n2</td>
          <td width="100">7 - n1<span style="float:right">(%rsp)</span></td>
      </tr>
  </table>

- <phase_6+123> - <phase_6+181> : 

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230813235638379.png" alt="image-20230813235638379" style="zoom:80%;" />

  可以看到，0x6032d0 指向了一串链表 node_list，每个节点node包含8字节内容和指向下一个node的next指针，最后一个节点node6的next指针指向NULL。

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230814160359011.png" alt="image-20230814160359011" style="zoom:80%;" />

  进一步分析，node的前8字节内容应该是两个4字节数字组成，第二个数字记录了该node的序号。

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230813193634335.png" alt="image-20230813193634335" style="zoom:80%;" />

  逆向工程如下：

  ```c
  // i in %rsi, num in %ecx, node_array in 0x20(%rsp)
  
  struct Node {
      int val;
      int ser;
      Node *next;
  };
  
  Node *node_array[6];
  for (int i = 0; i < 6; i ++)
      int idx = a[i];// 7 - n(i+1)
      if (idx <= 1) {
  		Node *node = node_head();
          node_array[i] = node;
      }
      else {   
          Node *node = node_head();//mov    $0x6032d0,%edx
          for (j = 1; j < idx; j ++) {
              node = node -> next;//mov    0x8(%rdx),%rdx
          }
          node_array[i] = node;//mov    %rdx,0x20(%rsp,%rsi,2)
      }
  }
  ```

  将 node_list 链表中的6个结点放置到 node_array 数组中，**node_array[i] 指向 node_list 的第 (7 - n<sub>i+1</sub>) 个结点**。

  <table border="1" style="width:30%; text-align:center; margin:auto">
      <tr height="40">
          <td colspan="2">main返回地址</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r14</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r13</td>
      </tr>   
      <tr height="40">
          <td colspan="2">%r12</td>
      </tr>
      <tr height="40">
          <td colspan="2">%rbp</td>
      </tr>
      <tr height="40">
          <td colspan="2">%rbx</td>
      </tr>
      <tr height="40">
          <td colspan="2">node_array[5]</td>
      </tr>
      <tr height="40">
          <td colspan="2">node_array[4]</td>
      </tr>
      <tr height="40">
          <td colspan="2">node_array[3]</td>
      </tr>
      <tr height="40">
          <td colspan="2">node_array[2]</td>
      </tr>
      <tr height="40">
          <td colspan="2">node_array[1]</td>
      </tr>
      <tr height="40">
          <td colspan="2">node_array[0]</td>
      </tr>
      <tr height="40">
          <td colspan="2"></td>
      </tr>
      <tr height="40">
          <td>7 - n6</td>
          <td>7 - n5</td>
      </tr>
      <tr height="40">
          <td>7 - n4</td>
          <td>7 - n3</td>
      </tr>
      <tr height="40">
          <td width="100">7 - n2</td>
          <td width="100">7 - n1<span style="float:right">(%rsp)</span></td>
      </tr>
  </table>

- <phase_6+183> - <phase_6+222> : 

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230814023319084.png" alt="image-20230814023319084" style="zoom:80%;" />

  逆向工程如下：

  ```c
  // n in %rbx
  Node *node = node_array[0];// 新链表的表头
  
  for (int i = 1, Node *cur = node; i < 6; i ++, cur = cur -> next) {
      cur -> next = node_array[i];
  }
  node_array[5] -> next = NULL;
  ```

  将链表改为：**node_array[0] -> node_array[1] -> ... -> node_array[5]**。

- <phase_6+230> - <phase_6+271> : 

  <img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230814023448632.png" alt="image-20230814023448632" style="zoom:80%;" />

  循环部分逆向工程如下：

  ```c
  for (int i = 5; i; i --) {
      if ((node -> val) < (node -> next -> val)) {
          explode_bomb();
      }
      node = node -> next;
  }
  ```

  链表修改后，要求**前一个节点的 val 必须大于等于后一个节点的 val**，否则爆炸。

### 解题方法

- 输入的6个数是 1 ~ 6 的排列。

- 修改后的链表要求按 val 值降序排序，应该是 node3 -> node4 -> node5 -> node6 -> node1 -> node2，即 node_array 中的顺序，而 node_array[i] 指向 node_list 的第 (7 - n<sub>i+1</sub>) 个结点，因此有下列对应关系：

  | i（node_array ） | 7 - n<sub>i+1</sub>（node_list） | n<sub>i+1</sub> |
  | :--------------: | :------------------------------: | :-------------: |
  |        0         |                3                 |        4        |
  |        1         |                4                 |        3        |
  |        2         |                5                 |        2        |
  |        3         |                6                 |        1        |
  |        4         |                1                 |        6        |
  |        5         |                2                 |        5        |

  因此 (n1, n2, n3, n4, n5, n6) = "4 3 2 1 6 5"。

------



## secret_phase（彩蛋）

### 进入方式

查看反汇编代码，发现在 phase_defused 中调用了 secret_phase，首先阅读和调试 phase_defused 代码段，找到进入隐藏关卡 secret_phase 的方法：

- 在 phase_4 中，原本只需输入满足条件的两个数字"%d %d"，想进入隐藏关，则需要多输入一个字符串"DrEvil"。
- 在 phase_6 破解后调用的 phase_defused 中，将会进入到 secret_phase。

```assembly
00000000004015c4 <phase_defused>:
  4015c4:	48 83 ec 78          	sub    $0x78,%rsp
  4015c8:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  4015cf:	00 00 
  4015d1:	48 89 44 24 68       	mov    %rax,0x68(%rsp)
  4015d6:	31 c0                	xor    %eax,%eax
  4015d8:	83 3d 81 21 20 00 06 	cmpl   $0x6,0x202181(%rip)        # 603760 <num_input_strings> // 最新解决的phase序号
  4015df:	75 5e                	jne    40163f <phase_defused+0x7b>
  4015e1:	4c 8d 44 24 10       	lea    0x10(%rsp),%r8 // %s abc
  4015e6:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx // %d 0
  4015eb:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx // %d 7
  4015f0:	be 19 26 40 00       	mov    $0x402619,%esi // "%d %d %s"
  4015f5:	bf 70 38 60 00       	mov    $0x603870,%edi // "7 0 abc" --> phase_4的输入
  4015fa:	e8 f1 f5 ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  4015ff:	83 f8 03             	cmp    $0x3,%eax
  401602:	75 31                	jne    401635 <phase_defused+0x71>
  401604:	be 22 26 40 00       	mov    $0x402622,%esi // "DrEvil"
  401609:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  40160e:	e8 25 fd ff ff       	callq  401338 <strings_not_equal>
  401613:	85 c0                	test   %eax,%eax
  401615:	75 1e                	jne    401635 <phase_defused+0x71>
  401617:	bf f8 24 40 00       	mov    $0x4024f8,%edi // "Curses, you've found the secret phase!"
  40161c:	e8 ef f4 ff ff       	callq  400b10 <puts@plt>
  401621:	bf 20 25 40 00       	mov    $0x402520,%edi // "But finding it and solving it are quite different..."
  401626:	e8 e5 f4 ff ff       	callq  400b10 <puts@plt>
  40162b:	b8 00 00 00 00       	mov    $0x0,%eax
  401630:	e8 0d fc ff ff       	callq  401242 <secret_phase>
  401635:	bf 58 25 40 00       	mov    $0x402558,%edi // "Congratulations! You've defused the bomb!"
  40163a:	e8 d1 f4 ff ff       	callq  400b10 <puts@plt>
  40163f:	48 8b 44 24 68       	mov    0x68(%rsp),%rax
  401644:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  40164b:	00 00 
  40164d:	74 05                	je     401654 <phase_defused+0x90>
  40164f:	e8 dc f4 ff ff       	callq  400b30 <__stack_chk_fail@plt>
  401654:	48 83 c4 78          	add    $0x78,%rsp
  401658:	c3                   	retq   
  401659:	90                   	nop
  40165a:	90                   	nop
  40165b:	90                   	nop
  40165c:	90                   	nop
  40165d:	90                   	nop
  40165e:	90                   	nop
  40165f:	90                   	nop
```

### 调试过程

设置断点在 explode_bomb， secret_phase。

In : main

​	调用 phase_6(input) 语句后的 phase_defused()。

In : phase_6

​	调用 secret_phase()。

In : secret_phase

```assembly
0000000000401242 <secret_phase>:
  401242:	53                   	push   %rbx
  401243:	e8 56 02 00 00       	callq  40149e <read_line>
  401248:	ba 0a 00 00 00       	mov    $0xa,%edx
  40124d:	be 00 00 00 00       	mov    $0x0,%esi
  401252:	48 89 c7             	mov    %rax,%rdi // 保存输入的字符串s
  401255:	e8 76 f9 ff ff       	callq  400bd0 <strtol@plt> // %rax:str2int(s), %rdx:-%rax, %rsi:length(s), %rdi:""，
  40125a:	48 89 c3             	mov    %rax,%rbx
  40125d:	8d 40 ff             	lea    -0x1(%rax),%eax
  401260:	3d e8 03 00 00       	cmp    $0x3e8,%eax // 必须 str2int(s)-1 <= 1000u
  401265:	76 05                	jbe    40126c <secret_phase+0x2a>
  401267:	e8 ce 01 00 00       	callq  40143a <explode_bomb>
  40126c:	89 de                	mov    %ebx,%esi
  40126e:	bf f0 30 60 00       	mov    $0x6030f0,%edi // 指向结点n1的指针
  401273:	e8 8c ff ff ff       	callq  401204 <fun7> // fun7(root, str2int(s))
  401278:	83 f8 02             	cmp    $0x2,%eax // fun7结果必须等于2
  40127b:	74 05                	je     401282 <secret_phase+0x40>
  40127d:	e8 b8 01 00 00       	callq  40143a <explode_bomb>
  401282:	bf 38 24 40 00       	mov    $0x402438,%edi // "Wow! You've defused the secret stage!"
  401287:	e8 84 f8 ff ff       	callq  400b10 <puts@plt>
  40128c:	e8 33 03 00 00       	callq  4015c4 <phase_defused>
  401291:	5b                   	pop    %rbx
  401292:	c3                   	retq   
  401293:	90                   	nop
  401294:	90                   	nop
  401295:	90                   	nop
  401296:	90                   	nop
  401297:	90                   	nop
  401298:	90                   	nop
  401299:	90                   	nop
  40129a:	90                   	nop
  40129b:	90                   	nop
  40129c:	90                   	nop
  40129d:	90                   	nop
  40129e:	90                   	nop
  40129f:	90                   	nop
```

In : fun7

```assembly
0000000000401204 <fun7>:
  401204:	48 83 ec 08          	sub    $0x8,%rsp
  401208:	48 85 ff             	test   %rdi,%rdi
  40120b:	74 2b                	je     401238 <fun7+0x34>
  40120d:	8b 17                	mov    (%rdi),%edx
  40120f:	39 f2                	cmp    %esi,%edx // compare n->val : v
  401211:	7e 0d                	jle    401220 <fun7+0x1c>
  401213:	48 8b 7f 08          	mov    0x8(%rdi),%rdi // n = n->left (if n->val > v, goto here.)
  401217:	e8 e8 ff ff ff       	callq  401204 <fun7>
  40121c:	01 c0                	add    %eax,%eax
  40121e:	eb 1d                	jmp    40123d <fun7+0x39>
  401220:	b8 00 00 00 00       	mov    $0x0,%eax // (if n->val <= v, goto here.)
  401225:	39 f2                	cmp    %esi,%edx // compare n->val : v
  401227:	74 14                	je     40123d <fun7+0x39>
  401229:	48 8b 7f 10          	mov    0x10(%rdi),%rdi // n = n->right (if n->val < v, goto here.)
  40122d:	e8 d2 ff ff ff       	callq  401204 <fun7>
  401232:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax
  401236:	eb 05                	jmp    40123d <fun7+0x39>
  401238:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  40123d:	48 83 c4 08          	add    $0x8,%rsp
  401241:	c3                   	retq   
```

查看 0x6030f0 处起的内存情况，发现 0x6030f0 指向了一个树结构的根节点 root。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230814155005157.png" alt="image-20230814155005157" style="zoom:80%;" />

将该树结构绘制出来，不难看出这是一颗二叉查找树（binary search tree, BST）： 

<img src="https://gitee.com/dq-agj/imags/raw/master/img/bst.png" alt="bst" style="zoom:80%;" />

逆向工程如下：

```c
struct Node {
    int val;
    Node *left;
    Node *right;
};

int fun7(Node *n, int v) {
    if (n != NULL) { // case 0
        return -1;
    }
    if (n -> val > v) { // case 1
        return 2 * fun7(n -> left, v);
    }
    else if (n -> val < v) { // case 2
        return 1 + 2 * fun7(n -> right, v);
    }
    else { // case 3
        return 0;
    }
}
```

该段代码是一个在 BST 中寻找 v 值的过程。

### 解题方法

假设输入字符串 s，想要破解 secret_phase 需要满足以下条件：

- s 可以转换成无符号数，且 str2int(s) - 1 <= 1000u，则 **1 <= str2int(s) <= 1001**。
- fun7(root, str2int(s)) 必须返回2，则其调用链中应该形如：case 1 --> case 2 -> ... -> case 3，其中 case 2 有且仅有一次。这样的值有两个：0x16 = 22，0x14 = 20。

在 secret_phase 的读入中输入 22 或 20。

------



## 运行结果

![image-20230814173117946](https://gitee.com/dq-agj/imags/raw/master/img/image-20230814173117946.png)



## 参考链接

- [超精讲-逐例分析 CSAPP：Lab2-Bomb!(上) - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/339461318)
- [CSAPP 之 Bomb Lab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/60237228)
- [《深入理解计算机系统》配套实验：Bomblab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/31269514)
- [CSAPP - bomblab · Mcginn's Blog (mcginn7.github.io)](https://mcginn7.github.io/2020/02/16/CSAPP-bomblab/)
- [【读厚 CSAPP】II Bomb Lab | 小土刀 2.0 (wdxtub.com)](https://wdxtub.com/csapp/thick-csapp-lab-2/2016/04/16/)



## GDB 相关

- 解决调试时行号混乱的问题：[[Solved\] How to automatically refresh gdb in tui mode? | 9to5Answer](https://9to5answer.com/how-to-automatically-refresh-gdb-in-tui-mode)