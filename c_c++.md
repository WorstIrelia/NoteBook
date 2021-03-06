## 继承多态
### 运行时多态

c++ 通过虚函数指针和虚函数表
父类指针可以调用子类方法
本质调用是
```c++

*(*(v_ptr *)(*this)+offset)(param)

```
值得注意的是，一个类共享一个虚函数table,每个table中的一项是一个func

值得注意的是，代码段是不能修改的，如果在c++中用指针强制转化一个函数指针赋值给虚函数指针的值，会core dump

值得一提的是，virtual 关键字只需要申明一次即可(在基类中)，继承下来的类默认都是虚函数

一个类共用一个虚函数表

个人感觉c++运行时多态的底层实现机制是将基类的按照public,private,protect三个关键字的类型和基类声明的顺序做一个内存上的放置，同时修改覆写的虚函数类型和新增的虚函数。所以这种规则在菱形继承的时候会出现问题(多个同名变量和同名函数),必须加上作用域来限定。

#### 普通的继承
覆盖掉同名的虚函数的函数指针
若没有覆盖，增加到虚函数表中
 
#### 多重继承

继承类中会有多个虚函数指针
每个虚函数指针指向一个虚函数表
继承类中会覆盖掉**所有**同名虚函数(覆写)
值得一提的是，新增加的**虚函数只会放到第一个虚函数指针指向的虚函数表里**

#### 菱形继承
由于类的内存放置原因会出现多个同名变量或者函数。
可以通过作用域或者虚继承来解决

##### 虚继承

使用虚基类
虚基类指针+偏移量的方式实现

### 函数重载(编译时多态)
底层随即加字符串，保证函数名称不一样。值得注意的是，函数返回值不是判断的标准。

### 三种继承方式

私有属性**继承后->不可访问**注意！
私有的意思应该是本人持有，只允许本人访问，外界访问不行，但是内部可以通过函数访问。

私有继承都私有
公有继承保持
保护继承都保护

protected 在本类中与private相同，但是protected被继承后还可以被访问，private不可见。(外部访问为private，内部操作为public)

友元只能访问声明友元的这个类的中所有成员。
如果在继承类中声明基类的友元函数，只能访问公有成员，保护和私有成员都不能访问。



## 四大类型转换

c 语言的强制类型转换有时候会改变 bit value
```c++
int a = 1
float b = a
```

c++不提倡c类型的转换
继承多态之后不在是pod(plain old data)类型，强制类型转换会出问题
提高代码可读性，提醒转化的类型

###const_cast<>()

修改指针引用

#### 常量折叠现象
预编译的时候，c++会将使用到的常量替换成声明的值，但是还是会为该常量分配空间！(与宏替换的不同之处)，所以可以用const_cast<>()去掉const属性，进行修改(仅限于在栈上分配的，全局的常量放在数据段，只读，修改会segment fault)，所以直接输出常量的值没有改变，但是输出修改后的指针的值改变了，但是他们是一个变量。
```c++
int main(){
    const int aa = 123;
    int& p = const_cast<int &>(aa);
    //int *P = const_cast<int *>(&aa);
    p = 234;
    cout<<aa<<' '<<p<<endl;
    cout<<&aa<<' '<<&p<<endl;
}
```

### reinterpret_cast<>()

按位解释转换，指针引用等，**整数到指针，枚举等等也都行**
不能从类型到类型

### statc_cast<>()
只在**编译时检查**，可以从类型到类型
```c++
    float a = 123.456;
    int b = static_cast<int>(a);
    // it's ok
```
面向对象编程的过程中，下行到上行总是安全的。(子类对象总是包含了父类对象的成员)。
上行到下行不一定是安全的。(转化的过程中会加一个offset,有多个父类的情况)

static_cast<>() 只支持有类型的指针到void的指针的转换，或者void的类型的指针到有类型的指针的转换

### dynamic_cast<>()

和dynamic_cast<>()密切相关的是 **RTTI run-time type identification**

运行时检查

**使用dynamic_cast一定得要有虚函数**
因为他通过RTTI的机制在虚表上附带一个information的指针，指向的内存区域带有类的信息，c++通过这个方法进行运行时动态类型判断。

[介绍c++虚函数，虚基类，dynamic_cast,typeid的性能分析](http://baiy.cn/doc/cpp/inside_rtti.htm)



## 右值引用和移动语义

## new delete 底层实现

malloc free 内存空间带链表的形式。

### new 
汇编调用 operator new
pod类型
调用malloc分配内存，分配内存空间失败的时候会调用一个函数判断，再抛出异常。
**值得注意的是这个函数可以由自己设置，回调函数**
```c++

set_new_handler(function);
//分配失败的时候回调函数(用户态，内核态?)

```

对象类型
调用malloc分配空间
然后在在分配的空间上依次调用构造函数。

### delete

调用operator delete

pod类型，简单的free掉
先调用析构函数，然后在free掉空间

### new[]
pod类型
分配一段空间
值得注意的是()会初始化

对象类型，会在前四个字节记录分配对象的数量
在调用构造函数，析构的时候使用


### delete[]
pod类型 new[]可以用 delete释放
非pod类型，一定得要用 delete[] 释放，因为还有 -4 offset的空间没有释放，并且还要调用析构函数。

#### 禁止在堆上分配空间

```c++
class test{
private:
    void* operator new(size_t size){
        return (test *)malloc(size);
        //do nothing, just forbiddit it
    }
public:
    
};

```
#### 禁止在栈上分配空间

将析构函数设置为私有
为什么不将构造函数设置为私有呢
构造函数设置为私有也不能分配空间
编译器分配空间的时候会通过上下文来检测构造函数和析构函数是否可以执行
通过以下的方式保证只能在堆上分配空间
不过释放空间只能通过成员函数的方式

```c++
class test{
private:
    ~test(){}
public:
    test(){}
    void destroy(test *p){
        delete p;
    }
    void destroy_all(test *p){
        delete[] p;
    }
};
```


## 网络

### 三次握手

防止网络中SYN-SEND包很久之后到达，服务器回应，占用服务器资源

### 四次挥手

两个wait

#### close_wait(被动关闭)
原因 由于自己代码中没有显示close socket,发送FIN,故一直处于close_wait

多个解决方法
* 检查read返回值
* 给socket设置时间戳，超时关闭
* 使用线程给socket发送嗅探报文， 收到RST关闭



#### time_wait(主动关闭)
主要是爬虫服务器或者web服务器(发起请求的一方)
编译内核的时候没有做参数优化(网络协议栈的优化)

2MSL原因
防止自己上次发的包丢失
防止握手时候阻塞在网络中的包

修改 /etc/sysctl.conf 中参数，或是编译内核的时候直接选

## c++中的const 
* const 修饰变量
* const 修饰对象
* const 对象中的成员函数

修饰变量的时候，特别注意指针，是指针常量，还是常量指针。
```c++
const int * ptr;
int * const ptr;
```

成员函数的const，内部不能出现 = 和调用非const函数
const 的对象只能调用const成员函数
(ps: 个人感觉const只是编译的时候的一个检查)
c++ mutable 修饰，都可以修改。
返回引用，有点破坏语义。

### const 重载的问题

成员函数+const 关键字修饰，可以重载
常量对象调用带 const的函数
非常量对象优先调用不带 const的函数
以上情况针对有两个同名函数

const 修饰参数
指针修饰指针或者引用可以重载
但是不能重载普通变量
从语义的条件来考虑，因为如果是普通变量或者对象，创建或者构造之后用户不关心这个变量的变化，那加不加const没有意义，所以禁止掉，默认出现这种情况是用户的错误。


## c++ 11 智能指针




## 迭代器与容器

回忆我们之前是如何构造数据结构的
for example
二叉数


数据放置于容器之中
```c++
struct tree_node{
    typename value;
    tree_node* lson;
    tree_node* rson;
    tree_node* father;
}

```
这里的tree_node就是一个容器，容易包裹了我们需要的数据value
如果我们想对树进行一些操作，那么可以用一个
```c++
tree_node* ptr;
```
但是这个指针的* 与 ->指向的是tree_node 这个节点。
对用户来讲，迭代器所指对象应该就是value本身，用户只关心数据，所以lson,rson,father这些信息不是必须的，所以我们得重新封装其的* ->操作
```c++
struct iterator{
    tree_node* ptr;
    reference operator*(){
        return ptr->value;
    }
    pointer operator->(){
        return &operator*();
    }
}

```
仅仅有容器和迭代器还是不够的，我们还需要算法作为灵魂，对迭代器指向的包裹数据的容器做一些操作。
为了泛化算法，统一接受的参数为迭代器。
所以迭代器要支持
```c++
    struct iterator{
    tree_node* ptr;
    reference operator*(){
        return ptr->value;
    }
    pointer operator->(){
        return &operator*();
    }
    self& operator++(){
        //do something
        return *this;
    }
    self operator++(int){
        self ret = *this;
        ++(*this);
        return *this;
    }
    //same as --
    bool operator==();
    bool operator!=();
}
```
这样 和原生指针的操作基本一样，用户看起来使用就像是在使用指向数据的指针一样。



## c++左值右值


引用只能和左值绑定
类型转换生成一个临时的右值，所以不能和引用绑定

## emplace_back

底层直接调用placement_new 在容器上构造对象，某些情况下可以减少一次临时对象的构造

## template 模板推导规则

### 1函数模板是一个指针或者引用，但不是通用引用
```c++
template<typename T>
void foo(T& x){

}
int x;
int &y = x;
const int &z = x;
foo(y) 去引用
foo(z) 保const

```

T -> int | const int
所以传递一个const int &给模板是安全的，因为类型推导保留关键字
引用特性忽略

```c++
void foo(const T&){

}
```
T -> int
忽略关键字特性和引用特性

指针同理

### 2 统一引用
```c++
template<typename T>
void foo(T&& x){

}

```

惟一的一个场景会把T推导成引用
T -> int& | int&&
左值 T -> int&
右值 T -> int&& 
我们利用这个特性可以配合remove_reference实现完美转发


### 普通
```c++

template<typename T>
void foo(T x){

}
```
规则 引用忽略，关键字忽略。
因为用户这么写往往意味着要拷贝一个新的对象(按值传递) 类型推导往往是
T -> int
去关键字往往以为这这个对象的关键字，而不是和该对象有关联的关键字
const char * const ptr;
推导后是 const char * ptr;
指针可变，指向的内容不可变。


### 应该记住
1. 在进行模板的类型推导时，传入参数如果是引用，会被当作非引用，即忽略掉引用部分。

2. 对统一引用参数进行类型推导时，左值参数会获得特殊处理。

3. 按值传递的参数进行类型推导时，const或者volatile参数会被处理成非const或非volatile。

4. 在模板类型推导时，数组和函数参数会退化成指针，除非它们被用做初始化引用。


## static 四种
### c中变量前+
静态存储区，链接TYPE 为local 对外文件不可见
值得注意的是局部静态变量存储区一样，但是有作用域是有限的
### 函数前+ 
链接TYPE为local 对外文件不可见
### c++ 类中静态变量
sizeof(class) 中不占用空间，所有类共享一个变量，通过class::name使用。
必须初始化
### 静态成员函数
只能操作静态成员，可以通过class::name直接使用，本质上没有传递this指针，所以不能够操作非静态成员。
无法是虚函数。


## 迭代器失效
某些容器的某些操作会导致迭代器失效，如vector，删除某个迭代器指向的元素，会导致后面的元素前移，后面的迭代器指向的某些东西会失效。
扩容也会失效。
vector还有一个坑就是clear()不会释放vector的空间。
swap到一个tmp变量上释放或者等其作用于结束就行。
但是某些容器不会，如list等。
具体失效要看具体的容器而言。

## map/set
map/set 底层采用的数据结构都是红黑树，插入删除查找操作的均摊复杂度是logn.
红黑树和平衡树的一些区别。
红黑树的插入调整最多3次，平衡树要调整到根。
平衡树高度平衡，红黑树高度最多相差一倍。
根黑叶黑，从根到叶节点的黑色节点树一样多。
map 在红黑树的节点上存的是一个pair<key,value>,set 就是 key

## type_traits编程技巧
本质 为了实现类型推导，STL能从迭代器的类型推导出所指对象的类型等等
为社么要加一层type_tratis 因为要解决原生指针推导的问题，偏特化一下
```c++
template<typename T>
struct iterator{
    typedef T value;
};
// so we can 
template<typename Iter>
Iter::value 
foo(Iter it){
    return *it;
}
// but if Iter is int *??? what to do?
// so we need to type_traits
template<typename Iter>
struct type_traits{
    typedef Iter::value value;
}
//partial specialization

template<typename Iter>
struct type_traits<Iter*>{
    typedef Iter value;
}

//so we can

template<typename Iter>
typename type_traits<Iter>::value
foo(Iter it){
    return *it;
}

```

## volatile
并发变成里用到
a++

编译器会优化成把变量放到寄存器，在寄存器++，这样加锁也没用

## restrict c99 c++未引入
促进编译器优化，保证指向该区域只有该一个指针可以修改

## extern 模板 
提醒编译器不要实例化该模板。加快编译速度，别的模块已经实例化过了

## 异常抛出底层实现机制
！！