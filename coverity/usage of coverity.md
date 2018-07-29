# windows

## coverity 接入使用方法

首先填写项目注册信息，包括英文ID，项目中文信息，项目语言，项目所属部门等等。

选择相应的工具进行代码扫描（目前测试了coverity扫描c,c++,python)。

在windows进行c,c++,python的扫描要进行如下配置。

* c,c++编译器
* git
* python 2.7

确保能通过编译。

编译型语言需要配置一个编译文件的脚本，properties中要标注脚本的路径。

不同语言的选项在properties中配置。（编译型的配置与非编译型的配置不同）。

命令行中输入
python build.py {file_name} 即可。

还可以添加定时任务，自动编译上传检查结果。


