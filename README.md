# Preface
---
这是我本科阶段最后一个成熟项目，也是我的毕业设计：
> 设计一款基于BWT的数据压缩程序

#### 思考
目前可供参考的项目是[bzip2](http://www.bzip.org/)，但是bzip2并无算法相关文档，仅提供API手册，可供参考之处并不多
bzip2的压缩比大概在80%-85%，但是由于使用了BWT算法，处理速度要比gzip(LZ77/LZ78)等慢一些，此外，压缩时占用的内存大概是解压时的三到七倍
BWT算法唯一的难点在于内存，随着原数组的增长，原理算法占用的内存以指数形式增长，而且遍历原理算法的缓存矩阵的时间复杂度在最好的情况下应该是O($n^3$)，因此主要针对BWT算法的时间复杂度和空间复杂度进行优化
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
假设在`32-bit`系统上，每个结构体占用的内存是$4+4+4$共$12$字节，共取出$n+1$个后缀数组，则占用的总内存应该是$12(n+1)$字节，相对原理算法的$(n+1)^2$字节来说，极大的降低了内存的占用

对后缀数组的字典序排序，目前使用的是冒泡排序（bubble sort），该算法的时间复杂度在最好情况下O($n^2$)（原数组已经按正序排序好），最坏情况下为O($n^3$)（原数组已经按倒序排序好），这样整个BWT算法的时间复杂度在O($n^2$)到O($n^3$)之间，且实际上取不到极限情况
若将这部分更换为快排序（quick sort），算法的时间复杂度最好情况下应该是O($nlog_2n$)，最坏情况下应该是O($n^2log_2n$)，最好与最坏情况同上，也取不到极限情况
综上，BWT的算法的时间复杂度和空间复杂度的优化可以告一段落，应该考虑具体的工程实现了

#### 工程
| language | Compiler | Project Management | Version Control | Project Name | 
| :-: | :-: | :-: | :-: | :-: |
| c++17(gnu++17) | GCC v8.2.0 | cmake 3.13.2 | [github] | [nxzip](https://github.com/stark-zhang/nxzip) |
任务列表
- [x] 实现基本的BWT算法
- [ ] 实现基本的IBWT算法
- [ ] 使用quick sort替换bubble sort
- [x] 实现基本的MTF和IMTF算法
- [ ] 实现`Huffman Coding`算法
- [ ] 实现仅利用`STL`的文件读写和其他`file IO`
- [ ] 定义压缩文件的结构

#### 其他

---
`created by vscode, wrote with markdown(MPE), 2019.02.04`