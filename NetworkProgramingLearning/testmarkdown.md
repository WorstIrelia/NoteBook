# this is markdown test
```c++
#include<iostream>
using namespace std;
int main(){
    cout<<"hello world"<<endl;
    return 0;
}
```



*a*
**a**
>text

* abcd
* abcde

1. abcde
2. abcde

[pic](https://www.baidu.com)

$c=a+b$

$math$

~~delete line~~

#### …or create a new repository on the command line
```
git init
git add README.md
git commit -m "first commit"
git remote add origin https://github.com/WorstIrelia/aaa.git
git push -u origin master
```
#### …or push an existing repository from the command line
```
git remote add origin https://github.com/WorstIrelia/aaa.git
git push -u origin master
```
#### …or import code from another repository
You can initialize this repository with code from a Subversion, Mercurial, or TFS project.



## 需求分析
* 数据模型 ER图
* 描述数据结构 层次方框图和Warnier图
* 数据结构规范化



----
* 数据流图
* 实体-联系图
* 状态转换图
* 数据字典

----
### 需求分析过程应该建立3种模型
* 数据模型 -> 实体-关系
* 功能模型 -> 数据流图
* 行为模型 -> 状态转换

**状态转换图** **ER图** 很重要

## 总体设计

过程设计->接口设计->体系结构设计->数据设计


### 模块化

抽象->逐步求精->信息隐藏和局部化->模块独立

耦合要低，模块之间关系简单

内聚要高，模块内部元素结合紧密程度高

### 耦合
* 完全独立
* 数据耦合
* 控制耦合 少用
* 特征耦合  ！！ 少用
* 公共环境耦合 限制
* 内容耦合 最高程度 不用

高内聚意味着低耦合

### 内聚

* 偶然内聚 松散
* 逻辑内聚 逻辑相似
* 时间内聚 同一段时间
* 过程内聚 特定次序
* 通信内聚 使用同一数据结构
* 顺序内聚 必须顺序执行
* 功能内聚 完成一个单一的功能


层次图

HIPO图

结构图 空心 实心

面向数据流的设计方法 ！！！！重要


## 详细设计

###结构程序设计

4个问题
* 系统响应时间
* 用户帮助设施
* 出错信息处理
* 命令交互

程序流程图->程序框图
盒图
PAD图
一张判定表
判断树
过程设计语言PDL


## 软件维护

完善性维护（改变和加强软件）

加强软件功能，性能占 百分之50

预防性维护

## 面向对象分析

三个子模型

* 对象模型
* 动态模型
* 功能模型

五个层次

* 主题层   找出类和对象
* 类与对象层    识别结构
* 结构层    识别主题
* 属性层    定义属性
* 服务层    定义服务



事件跟踪图
状态图





同步异步 阻塞非阻塞

同步 自己不断进行查询（非阻塞也是如此）
异步 事件完成的时候能对自己进行通知


