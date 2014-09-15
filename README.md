stm32f429_discovery
===================

这个仓库的移植了RAW-OS到stm32f429_discovery评估板上。
另外还实现了一些功能
	1)移植了Lua解释器。
	2)实现了Ymodem协议。
	3)移植了fatfs文件系统。由于板子上没有FLASH，所以disk是用RAM模拟。
	4)移植了u-boot的命令行。支持自动补全和历史命令。
