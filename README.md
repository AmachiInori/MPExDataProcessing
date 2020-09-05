# MPExDataProcessing

BIT材料物理实验数据处理项目

本项目为自动化处理BIT材料物理实验的数据而作。

本项目宿主语言是C++，使用Visual Studio 2019搭建，使用C++17标准，编译器MSVC，依赖于图形库EasyX.

如无C++17及以上的编译/开发环境和工程组建环境，请使用release版本中的可执行文件

解放双手去打代码！

# 如何使用

解压，执行/run.cmd，按照提示操作。

注意：`start [expr number]`命令中的参数`expr number`不是实验讲义上的序号，而是调用`list`指令之后产生的序号。

注意：没怎么做稳定性优化，别瞎玩儿，瞎玩儿就崩溃给你看。

有额外需求或者测试发现功能性问题请Issue或直接PR(前提是你码力比我高)

# 更新日志

2020.09.05 0.10beta2 修复了在unicode字符集上的显示错误，修复了start处理异常输入时产生的问题
2020.09.05 0.10beta 第一版