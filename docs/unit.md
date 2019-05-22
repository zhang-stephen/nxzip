# 单元测试

为了排查压缩和解压算法中的潜在错误，故而增加单元测试的内容

单元测试统一在[Test](../test/)文件夹中提供`main()`函数，但是算法部分调用该文件夹同级的其他文件夹的程序。一共提供四个相关的单元测试文件

这些文件相互独立，与各自依赖算法的编译和管理使用[emake](https://github.com/skywind3000/emake)

单元测试使用的输入文件统一为[zlibh.c](../huffman/zlibh.c)，测试通过则输出`Okay`，反之输出`ERROR`

如果是在windows上进行测试，则可以在设置好Powershell执行策略的情况下直接执行[测试脚本](../test/unit_test.ps1)，该脚本提供编译、测试、清理二进制文件的所有功能，全自动化，无需干预

Linux的bash脚本暂时不纳入计划

----
`created by vscode, wrote by Markdown(MPE), 21019-05-23`