# 关于文件输入输出的一点思考
#### 综述

压缩中零阶编码的数据要以二进制方式写入文件，解压中要从文件中读取零阶编码的数据，钳制C++程序执行速度的瓶颈就是IO（Input and Output）操作，一般对RAM之外的所有数据流的操作都认为是IO

c++提供了完整的流式IO函数库，可以使用它进行所需的一切操作

此处需要对文件进行二进制方式的IO，而在MS Windows和Unix-like的系统上，IO行为略有不同（即同样的函数，产生的输出是不同的）

#### 文件IO

一般认为c++中有两套文件IO的API，一套是C++标准的`fstream`，即文件流；另一套是兼容c语言的`cstdio`，也被称为“C风格的IO函数”（C-style IO）

这两者在底层上的操作是相同的，都是先写入缓存，而后由缓存刷新进入文件，不同之处在于文件流的抽象程度是更高的，而C-style IO较为底层；一般情况下，二者是兼容的

如果使用了`std::ios_base::sync_with_stdio(false)`函数，二者的同步将会终止，此时不应该再使用C-style IO，这将无法保证数据的准确性

STL中提供的`std::fstream::read`和`std::fstream::write`函数在指定了`std::ios::binary`参数的文件对象中将以二进制的方式进行操作，但是对于C-style IO而言，不同的实现有不同的行为定义（纯粹是历史包袱）

+ on MS Windows
	Windows 默认以`CRLF`作为换行符，即`\r\n`或者说`0D0Ah`，因此使用`std::fopen`函数打开文件时，必须附加`b`标志符，写成如下形式
	```c++
	std::FILE* fp = std::fopen(fn, "rb");
	```
	否则写入值为“0Ah”的时候，将自动扩展为`0D0Ah`，读入该值的时候则只返回`0Ah`，但是这事实上造成了文件的不兼容（即Windows和Unix-Like上产生的文件是不同的，故软件也无法跨平台）
+ on Unix-Like
	由于历史原因，C语言诞生于Unix系统，故很多东西都延续了Unix的风格，在Unix及Unix-Like操作系统中，换行符为`LF`，即`0Ah`，故读写文件时不存在文本方式和二进制方式的区别，此时C语言将会忽略`b`标志符

在实际程序实现中，NXZIP使用了一个定义在匿名空间中的`_io_file`的辅助类，其中构造函数如下：

```c++
/**
 * @brief	Construction
 */
_io_file(const char* fn, const char* rw, bool fflag = false)
{
	std::string _rw = rw;
#if WINNT || WIN32 || WIN64
	_rw += "b";
#endif
	_p_fp = std::fopen(fn, _rw.c_str());
	_p_rw = _rw;
	_p_fflag = fflag;
}
```

即采用条件编译，使得在windows环境下自动添加`b`标志符，而在Unix-Like环境忽略它

#### 关于条件编译

NXZIP使用条件编译确定宿主机的系统环境

一般GCC的预定义宏中会有平台相关定义，这些定义是不允许用户私自更改的，查看预定义宏的命令如下：
+ MS Windows
	```powershell
	> g++ -dM -E xx.h | sls WIN
	```
	在输出中应该可以找到形如`WINNT`、`WIN32`、`WIN64`之类的宏，应该还有上述宏之前加了下划线的版本，不过只需要使用这些即可

	其中`WIN64`是64位windows特有的宏，`WINNT`是使用了NT内核的windows特有的宏，本项目并不打算支持诸如Win95/98/2000/me等使用旧内核或者架构不足32位的系统

	`sls`是powershell命令，作用同linux的`grep`命令，都是匹配字符串

+ Linux 2.xx+
	```bash
	username@path# g++ -dM -E -< /dev/null | grep linux
	```

#### \<filesystem\>

c++17提供了脱胎于`boost::filesystem`的文件系统库，作为各系统的文件系统的兼容层，本项目使用该库进行文件路径的管理

---
`created by vscode, wrote by markdown(MPE), 2019-05-02, uncompleted`