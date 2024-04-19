homework参考：https://dreamanddead.github.io/CSAPP-3e-Solutions/



相关阅读：

[linux中的ld命令及其搜索路径顺序_linux ld_Cc1924的博客-CSDN博客](https://blog.csdn.net/qq_42731705/article/details/123934842)https://blog.csdn.net/u010429831/article/details/103150893)

默认的系统库路径在不同的系统上可能会有所不同。以下是一些常见的系统库路径：

**在Linux上：**

1. **/lib：** 包含系统最基本的动态链接库，是系统启动时所需的。
2. **/usr/lib：** 包含系统安装的其他动态链接库。
3. **/usr/local/lib：** 用户安装的软件通常将它们的库文件放在这个目录。

- 静态库链接时搜索路径顺序
        1. ld会去找GCC命令中的参数-L
        2. 再找gcc的环境变量LIBRARY_PATH
        3. 再找内定目录 /lib /usr/lib /usr/local/lib


- 动态链接时、执行时搜索路径顺序:
  1. 编译目标代码时指定的动态库搜索路径
  2. 环境变量LD_LIBRARY_PATH指定的动态库搜索路径
  3. 配置文件 /etc/ld.so.conf 中指定的动态库搜索路径
  4. 默认的动态库搜索路径/lib 
  5. 默认的动态库搜索路径/usr/lib
  6. **/usr/local/lib**       (☆) 
- 有关环境变量
  1. LIBRARY_PATH环境变量：指定程序静态链接库文件搜索路径
  2. LD_LIBRARY_PATH环境变量：指定程序动态链接库文件搜索路径

------

[#include＜＞和#include“”的区别_include <>-CSDN博客](https://blog.csdn.net/rammuschow/article/details/107947302)

- **查找的目录不同**

  1. #include<>：编译器直接从**系统类库目录**（在Linux GCC编译环境下，一般为 /usr/include 和 /usr/local/include）里查找头文件。

  2. #include""：默认从**项目当前目录**查找头文件；

     如果在项目当前目录下查找失败，再从**项目配置的头文件引用目录**（在Linux GCC编译环境下，则一般通过在Makefile文件中使用-L参数指定引用目录）查找头文件；

     如果项目配置的头文件引用目录中仍然查找失败，再从**系统类库目录**里查找头文件。

     > 注意：虽然#include""的查找范围更广，但是这并不意味着，不论是系统头文件，还是自定义头文件，一律用#include""包含。
     >
     > 因为#include""的查找顺序存在先后关系，如果项目当前目录或者引用目录下存在和系统目录下重名的头文件，那么编译器在当前目录或者引用目录查找成功后，将不会继续查找，所以存在头文件覆盖的问题。
     >
     > 另外，对于系统头文件，用#include<>包含，查找时一步到位，程序编译时的效率也会相对更高。

- **使用场景不同**
  1. #include<>一般用于包含系统头文件，诸如stdlib.h、stdio.h、iostream等；
  2. #include""一般用于包含自定义头文件，比如我们自定义的test.h、declare.h等。

------

[csapp.h头文件的使用 ---- 3种方法运行《深入理解计算机系统》中的代码-CSDN博客](https://blog.csdn.net/ustc_sse_shenzhang/article/details/105744435)

------

[gcc编译选项-lpthread和-pthread的区别 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/89678862)

[-pthread 和 -lpthread 的区别_墨一鉴的博客-CSDN博客](https://blog.csdn.net/c2682736/article/details/119789156)

------

