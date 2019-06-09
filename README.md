# Preface
---
这是我本科阶段最后一个成熟项目，也是我的毕业设计：
> 设计一款基于BWT的数据压缩程序

#### 思考
目前可供参考的项目是[bzip2](http://www.bzip.org/)，但是bzip2并无算法相关文档，仅提供API手册，可供参考之处并不多  

bzip2的压缩比大概在80%-85%，但是由于使用了BWT算法，处理速度要比gzip(LZ77/LZ78)等慢一些，此外，压缩时占用的内存大概是解压时的三到七倍  

BWT算法唯一的难点在于内存，随着原数组的增长，原理算法占用的内存以指数形式增长，而且遍历原理算法的缓存矩阵的时间复杂度在最好的情况下应该是O($n^3$)，因此主要针对BWT算法的时间复杂度和空间复杂度进行优化

#### 文件
+ 后缀名暂定为`zipx`

#### 工程
| language | Compiler | Project Management | Version Control | Project Name | 
| :-: | :-: | :-: | :-: | :-: |
| c++17(gnu++17) | GCC v8.2.0 | cmake 3.13.2 | github | [nxzip](https://github.com/stark-zhang/nxzip) |

任务列表
- [x] 实现基本的BWT算法
- [x] 实现基本的IBWT算法
- [x] 使用quick sort替换bubble sort(注：实际使用merge sort)
- [x] 实现基本的MTF和IMTF算法
- [x] 实现`Huffman Coding`算法
- [x] 实现范式`Huffman Coding`算法
- [x] 实现仅利用`STL`的文件读写和其他`file IO`
- [x] 定义压缩文件的结构
- [x] 实现软件的CRC-32C校验算法
- [x] 实现基于SSE4.2指令集的CRC-32C算法

#### 其他
2019-06-09 Freeze This Project
毕业设计业已完成，虽然程序仍有些许错误，但是眼下亦无时间修复订正，留待日后解决
算法的世界是很奇妙的，希望日后仍有机会遨游其中

已知问题：
1. BWT算法的正变换和逆变换无法对应，导致解压时CRC32校验失败（Fatal Error）
2. 文件结构中一些冗余成分未能删除

---
`created by vscode, wrote with markdown(MPE), 2019.02.04`