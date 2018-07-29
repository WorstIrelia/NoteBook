## Overview

n个core 运行n个threads

每个运行一个single transaction 

用HTM 进行同步

one-sided RDMA（基于效率的角度考虑)

transaction 太大，会chopping 还有一些限制 only the first piece may contain a user-initated abort

preserve strict serializability , 设计了一种类似 2PL 的协议 

coordinate accesses to the same database records from **local** and **remote** worker threads

使用 one-sided RDMA 实现了 exclusive and shared lock -> 与local accesses cahce-coherent 同时与HTM保持了很强的一致性(consistency)

HTM/RDMA firendly hash table

3个限制。

requires advanced knowledge 

hash table(unorderd store) is ok, but it still need two-side RDMA to remote accesses of the ordered stores .

preserves durability rather than avialability in case of machine failures .


## HTM

remote 

point : lock and lease

上锁可以避免不同分布式事务的冲突，但是不能避免本地事务的冲突，甚至会abort本地事务

一个是abort  剩下两幅图是造成不一样的结果，检查到confict ,abort 

所以必须检查锁的状态

值得注意的是 RDMA的会把remote machine 的 memroy fetch 到本地的 memory (cache) 然后做本地的read 和 write, write 需要写回 加锁解锁

### exclusive and shared lock implementation

CAS指令 原子

这篇论文中的RDMA CAS指令只能与CAS instruction 兼容在IBV_ATOMIC_GLOB下， 然而infiniBand NIC所在环境IBV_ATOMIC_HCA 

然而幸运的是，这个锁只会被RDMA CAS锁住。。

exclusive lock 用 CAS很容易实现。 0->1 1->0

RDMA三种操作的语义不足以支撑实现shared lock(读，写,CAS)

how to implement the shared lock? -> lease !

local write and remote write check state and abort itself if lease is not expired

读锁用过期来保证，写锁要显示的释放

### read write data structure

REMOTE_READ(key,end_time)

状态设置，成功，读取，返回end_time。
上写锁,abort.
过期，重新进行。
没过期，读取，返回读锁的租赁期。

REMOTE_WRITE(key)

状态设置，成功，读取。
上写锁，abort.
过期，重新进行。
没过期，abort.（有读锁）

写回。
显示释放。

LOCAL_READ(key)

上锁，abort
没上锁，读

LOCAL_WRITE(key,value)

上锁，abort
过期，清零租赁期
没过期,abort

值得注意的是，local write没有加锁，但是会清零租赁期，从而避免remote read and write

副作用？

talbe 2 看到不是很明白

这种例子非常少，但是对性能影响微乎其微

### 表述性的证明

### Read-only Transcation

不能用HTM 

采用另一种方式，没有理解 end_time = now + duration

### durability

可靠性与可用性

本论文保证的是可靠性，不是可用性。

可靠性，**持续**的时间内都可以稳定使用。

可用性，可以使用的百分比

不宕机，但是要关闭，可靠性高，可用性低

超短时间宕机，可靠性低，可用性高




durability rather than availability in case of machine failure

不间断电源

NVARM 断电后还可以看到内存分布 

通过zookeeper来协助重启

日志处理的难点 
即使在机器crash的情况下，也要完成日志的上传
机器不仅仅只上传自己的日志，还要上传远程remote的日志

值得注意的是只需要log CAS 和 log REMOTE WRITE

begin, log chopping info & remote-write set 释放锁
commit, log local and remote updates 释放+写回

crash 非为两大种

执行remote的机器挂了 -> 通过log的信息来释放锁或者写回加释放锁
被执行remote的机器挂了 -> wait



crash 分为两种，commit 前和 commit 后 

用 owner_id 来判断谁获取了锁
write_ahead log

surviving machine just wait 



## memory store layer


highly optimized hash table based on RDMA and HTM.

如果使用RDMA，因为不经过CPU，所以存储器的缓存优化就失去了作用。三级缓存。

间接读取需要两次访存，性能打对折。

1. 用HTM来避免冲突
2. 使用remote write,read来避免host主机的无谓的操作
3. two-level looksup???所以只用一次 RDMA_READ 可以做一层cache(hash表在本地)


基于offset的缓存比基于内容的缓存key-value强

1. 只要没有删除，那么这条缓存信息不需要判断合法性，同时也不需要同步。如果删除，只需要increase incarnation,这很容易判断得出。
2. 啊
3. 占用空间极少


不明白他这么设计如何达到他们说的优点。

只是给出了slot 和 entry 的数据存储 其他也没怎么说




## 两个图表

RTM uses the first-level cache to track the read set, and relies on the **cache coherence protocol** to detect conflicts.

RTM使用L1缓存来存储读集，同时依靠缓存一致性协议来发现矛盾。(conflict) (MESI协议) 

### MESI
定义了缓存的一些状态
modified
exclusive
share
invalid

每次对内存的操作会修改缓存中的状态


所以local read 先读 
然后remote read 再读的时候会修改内存的state， 会导致local read 缓存一致性不满足， 所以会abort!!!




## Implementation Issues

### 同步时间

softtime 

普通的now++ 可能会导致频繁的矛盾

1. 加长时间间隔，然而会导致DETLA增长，从而导致租赁期确认的失败
2. 解决办法，只获取两次时间。第一此获取，直接使用，以后都复用第一此的时间，第二次获取，用来对租赁期的判定。

### fallback handler 

如果aborts的数量超过了一定的预制，那么fallback handler就会运行。
值得注意的是fallback handler的运行可能与外部机器RDMA冲突，于是就用remote operator 来避免冲突。

### 原子性问题

remote CAS 与 local CAS冲突(在论文提到的这个level下)

随意之前fallback handler里可以直接使用remote CAS 来锁本地事务。然而这样会导致很严重的性能下降。(两个数量级)


### Remote Range Query
远程请求排序好的数据等价与本地请求非排序好的数据，所以传送一下请求即可。

### 说说测试

6台机器
每台机器 2个10核心的带RTM的核心
64G 内存
56Gpbs infinband NIC
unbuntu 14.04
OFED stack.

之前一个机器一个节点跑8个线程
DrTM(s)之后有虚拟了一个节点在跑8个线程(两个cpu)

用一个机器不断的开虚拟结点来模拟多机器的情况

## Q1 什么是RDMA 

传统的网络编程中，我们的cpu需要进行TCP/IP协议栈的解析。现在网卡的发送接受速率越来越快，cpu跟不上网卡的接受速度。DMA技术解放了CPU，RDMA是Remote Direct Memory Access技术，可以由机器直接操作远程机器的内存，而不需要经过cpu处理，大大降低了延迟和提高了吞吐量。
