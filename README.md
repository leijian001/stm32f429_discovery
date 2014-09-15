stm32f429_discovery
===================

这个仓库的移植了RAW-OS到stm32f429_discovery评估板上。<br>

另外还实现了一些功能:<br>
	1)移植了Lua解释器。<br>
	2)实现了Ymodem协议。<br>
	3)移植了fatfs文件系统。由于板子上没有FLASH，所以disk是用RAM模拟。<br>
	4)移植了u-boot的命令行。支持自动补全和历史命令。<br>
