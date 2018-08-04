# 统计学习方法
监督学习
从给定的，**有限**的，用于学习的 **训练数据(training data))** 集合出发，假设数据是独立同分布产生的，并且假设要学习的模型属于某个函数的集合，称为**假设空间(hypothesis space)**。应用某个**评价准则(evaluation criterion)**,从假设空间选取一个最优的模型，使他对已知训练数据及未知**测试数据(test data)**在给定的评价准则下有最优的预测。最优模型的选取由算法实现。
key word: training data, hypothesis space, evaluation criterion, model, algorithm, test data 


## 三要素
* **模型(model)** 假设空间，学习模型的集合
* **策略(strategy)** 学习的策略
* **算法(algorithm)** 选取最优的模型

## 问题
* 分类问题
* 标注问题
* 回归问题



## 感知机模型

### 解决的问题
解决二类分类问题
输入 特征向量
输出 类别，为+1, -1值
等价在特征空间中划分出一个将正负两类分离的超平面
通过导入基于误分类的损失函数，利用梯度下降法极小化损失函数
有原始形式和对偶形式
### 解决的方法
$f(\boldsymbol{x}) = sign(\boldsymbol{w}{\cdot}\boldsymbol{x}+b)$
我们需要找到$\boldsymbol{w},b$
假设我们输入的点(向量)集合是**线性可分**的，那么存在$\boldsymbol{w},b$满足使得所有预测成立
定义损失函数点到超平面的距离定义损失函数
$L(\boldsymbol{w},b) = -\sum_{\boldsymbol{x}_{i}\in M}{y_{i}(\boldsymbol{w}{\cdot}\boldsymbol{x_{i}}+b)}$
Ｍ是误分类点的集合，关于$\boldsymbol{w},b$是连续可导的
对$\boldsymbol{w},b$求梯度
得到算法的迭代过程
$\boldsymbol{w^{'}} = \boldsymbol{w} + y_{i}\cdot x_{i}$
$b^{'} = b + y_{i}$
学习率是 1
### 算法过程
任取一个$\boldsymbol{w},b$
遍历一遍，找到首个不满足的点
调整$\boldsymbol{w},b$
重复，知道找不到不满足的点

### 推导过程和对偶形式

设 max 和 min 
构造$\boldsymbol{w_{opt}}\cdot \boldsymbol{w_{k}}>= kn\eta$
构造${\parallel{\boldsymbol{w_{k}}}\parallel}^{2} <= k \eta ^{2}R^2$
$R = max({\parallel{\boldsymbol{x_{i}}}\parallel})$
$\eta = min(y_{i}(\boldsymbol{w_{opt}\cdot \boldsymbol{x_{i}}}))$

$\boldsymbol{w}$可以表示成$a_{i}x_{i}y_{i}$的线性组合,$a_{i}$是系数

### 不可分的条件

+1 和 -1 的凸包不相交

## k 近邻法

