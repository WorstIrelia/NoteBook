#epoll ET and LT
[learning-link](https://blog.csdn.net/liu0808/article/details/52980413)

##socket block and nonblock

```c
//set block
socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0)
//or
fcntl()
ioctl()


```

> 如果socket的设置为阻塞，当read的时，无数据可读，程序会被阻塞，直到内核通知socket有数据可以读取，~~write的时候，可能其他进程在使用，阻塞？~~
> 如果socket的设置为非阻塞，当read的时候，若无数据可读，返回EAGIN,~~write时候，其他进程在使用，不阻塞？~~

##epoll_wait

###LT

>epoll常见的工作模式，水平触发。可以理解为检测到事件发生(可读取的缓冲中有数据，内核会通知用户)，如果数据没有读完，就会一直通知。

* 循环读取
> 如果是阻塞socket，读取完所有的数据socket就会阻塞(需要break)，否则gg。
> 如果是非阻塞socket，读取完所有的数据socket就返回，无需关心。

* 读取一次

> 如果是阻塞socket，因为有事件，肯定可以读取一次。这时候如果没有读取完，**内核下次还会通知这个句柄上有事件**,无需关心。
> 如果是非阻塞socket,同上。

###ET

>epoll垂直触发的工作模式，可以想象成为一个上升沿触发，只会通知一次，和LT比较一下。

* 循环读取
>如果是阻塞socket，读取完所有数据socket就会阻塞(需要break)，否则GG。
>如果是非阻塞socket,循环读取完所有数据，ok。

* 读取一次
>如果是阻塞socket，因为有事件，肯定可以读取一次，**但是，若是本次数据没有读完，缓冲区内还有数据，下次内核并不会通知用户，除非再次又有事件发生。故读取一次若再无事件发生可能句柄永远不会被epoll_wait调用得到。**
>非阻塞socket，同上。

###conclusion

>**LT**模式的情况下，读取socket推荐 **阻塞单次读取**或者 **非阻塞循环读取(阻塞循环读取要自己判断结束break，否则gg)。**

>**ET**模式的情况下，读取socket推荐 **非阻塞循环读取(因为阻塞单次读取可能永久忽略后来的数据，循环读取也要手动break)。**

