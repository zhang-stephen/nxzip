# BWT及其逆变换的算法分析

---

[BWT算法](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform)于1994年由Michael Burrows和David Wheeler提出，它的作用主要是改善序列的局部相关性，以便使用MTF算法降低其信息熵，从而获得更高的压缩比

#### 原理
可以在wiki上看到，BWT的原理实现中，对一个长度为n的序列，需要一个大小为(n+1)^2^的矩阵进行变换，于是在序列增长的情况下，占用的内存急剧增加，在n=10^6^B时，矩阵占用的内存大于1TB，这是无法实现的；同时，对于这么大的矩阵进行遍历和排序的时间复杂度也是不可接受的，因此对于该算法的设计，应当**着眼于时间复杂度和空间复杂度的优化**

#### 方法
> 一种基于后缀排序快速实现Burrows-Wheeler变换的方法

基于后缀排序的BWT实现，基本上解决了空间复杂度的问题
对于一个长度为$n$的数组，实现基于后缀排序的BWT的过程应该如下：
+ 插入标志元素，这个标志元素的字典序应该大于数组中的所有元素（即值最小） 
+ 取后缀数组，并对取到的后缀数组进行字典序排序
+ 排序依次取每个后缀数组前的一个元素（对于长度为$n+1$的后缀数组，就取标志元素）
+ 将上一步中取到的元素依次存储，即是所求的`Burrows-Wheeler`转换的结果，标志元素的位置（从零开始数）就是所求的索引值  

在取后缀数组的时候，不必将后缀数组的元素存储到别处，只需取原数组的指针偏移，使用结构体记录下后缀数组相对原数组的位置和长度即可
```c++
/* Logic Suffix Array Definition */
typedef struct 
{
	int16_t* nnstr;			/* !< the cache pointer */
	uint32_t position;		/* !< the position in origin Array */
	uint32_t length;		/* !< the length of nnstr */
}SuffixArray_TypeDef;
```
假设在`32-bit`系统上，每个结构体占用的内存是12字节，共取出n+1个后缀数组，则占用的总内存应该是12(n+1)字节，相对原理算法的(n+1)^2^字节来说，极大的降低了内存的占用

对后缀数组的字典序排序，目前使用的是冒泡排序（bubble sort），该算法的时间复杂度在最好情况下O(n^2^)（原数组已经按正序排序好），最坏情况下为O(n^3^)（原数组已经按倒序排序好），这样整个BWT算法的时间复杂度在O(n^2^)到O(n^3^)之间，且实际上取不到极限情况

若将这部分更换为快排序（quick sort），算法的时间复杂度最好情况下应该是O(nlog~2~n)，最坏情况下应该是O(n^2^log~2~)，最好与最坏情况同上，也取不到极限情况

综上，BWT的算法的时间复杂度和空间复杂度的优化可以告一段落，应该考虑具体的工程实现了

#### 程序中的特殊实现
```c++
/* Non-public Namespace */
/* source file: bwt.cpp */
namespace 
{
	/* Logic Suffix Array Definition */
	typedef struct 
	{
		int16_t* nnstr;			/* !< the cache pointer */
		uint32_t position;		/* !< the position in origin Array */
		uint32_t length;		/* !< the length of nnstr */
	}SuffixArray_TypeDef;

	/* the cache for origin Array */
	class arrayCache
	{
	public:
		/* Class Data Member */
		int16_t* cache1;		/* !< cache1 to store origin array */
		int16_t* cache2;		/* !< cache2 to store result after suffix sort */
		uint32_t length;		/* !< length of cache(sizeof(origin)+1) */

		/* Constructor and Destructor */
		arrayCache(uint32_t length);
		~arrayCache(void);

		/* Public Method */
		void copy_to_cache1(uint8_t* srcArray);
	};
}
```
这里使用了c++中一种特殊的语法：匿名空间（anonymous namespace），它的用法相当于C语言中的`static`关键字，但是更强大

`static`关键字的作用：
- 修饰变量，使得变量作为静态变量，且对其他编译单元不可见
- 修饰函数，使得函数对其他编译单元不可见，只能在本单元内访问

而匿名空间的作用除了以上所说，它甚至还支持对类型的隐藏，使之对其他的编译单元不可见，如上代码中，便隐藏了`SuffixArray_TypeDef`和`arrayCache`这两个类型，而访问这两个类型时，有两种写法（以定义某个数据变量为例）

```c++
// SuffixArray
::SuffixArray_TypeDef suf{nullptr, 0, 0};

// arrayCache
arrayCache* cache = new arrayCache(xxx);
```

匿名空间的使用需要注意：
+ 不宜在匿名空间中建立过于复杂的函数，只适宜于简单的helper函数
+ 某些命令行式调试工具可能无法命中匿名空间中的断点
+ 不要和`static`混用，否则可能会有无法预料的问题，甚至不建议在c++中使用该关键字

---
`created by vscode, wrote by markdown(MPE), 2019-02-05`
