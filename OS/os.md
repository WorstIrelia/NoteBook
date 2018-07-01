# 协程

线程的切换还需要通过内核进行操作，协程是通过一定方法能通过在用户态自己实现寄存器堆栈的存储与切换，不需要操作系统的调度。

目前有一种实现方式 __LINE__ 编译器的一个宏，当前是第几行，通过这个进行上下文的保存，但是问题很大。（玩具实现） 

# 可变参数宏

```

__LINE__ //当前第几行
__FILE__ //一个字符串，表示当前的文件名
__VA_ARGS__ //与可变参数宏搭配

#define debug(...) printf(__VA_AGRS__)
#define debug(args...) printf(args) //这个args可以是任意的

//## 连接前后两个参数
//# 使后面的参数加上双引号，变成字符串
//可以直接定义两个相连的常量字符串

//坑，宏展开

#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))  //一个宏,限定词,只通过堆栈传递参数
#define FASTCALL(x) x __attribute__((regparm(3)))  //最多通过寄存器传递3个参数,暂时不知道这个x是什么意思
#define fastcall __attribute__((regparm(3))) //

static inline //inline + static 头文件里声明定义

```