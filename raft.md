# 一些胡乱的思绪
* 两个一致性算法都是为了达到 **在多数派**同意的情况下，（写入的log或者）达到最终一致性
* > raft通过commit来保证(包括一系列的restriction) , paxos 通过Maxvote来保证


-----


##前提
* 信道可靠





# RAFT




## 三个角色

* leader
* candicate
* follower


leader -> follower  (RPC收到的回应中的term>curren_term)

follower -> candicate (timeout) term++,vote self

candicate -> leader (获得超过一半的选票，上取整)

candicate -> follower 发现leader或者收到的RPC_result的term>current_term，同时更新自己的term。





----


### leader election


#### 一般情况
1. leader获得超过一半的选票
2. 每个follower只能投一个term中的leader最多一次（避免了重复投票) 
注:要是有term 4,5同时发起投票，4会被5回绝，同时更新自己的term为5，并转化为follower

保证只选出一个leader

#### 竞选的同时遇到leader RPC请求

1.如果candicate的current_term>term 拒绝请求，同时leader收到返回信息，转化为follower,否则自身转化为follower

该情况可能发生的情况如下

因为某些原因，导致少部分（<n/2）的**包含leader**的server与剩余server之间不可通信

从而剩余的server中选出了新的leader,term往前推进。当某一时刻，剩余的server之间正在进行选举时，

少部分的server又可以和剩余的server继续通信，会导致第二种情况发生。

#### 选票被瓜分

1. 同时有多个cadicate进行选举，每个candicate都不足以得到全部的选票。

raft运用的随即超时时间的设置方式，减少多个follower同时转变成candicate的概率。

### leader election 小结

* 无论何种情况，保证了至多只有一个leader

----

### Log repolication

1. leader 接受 client的请求，并将log加到entries尾。
2. leader 复制log 到 server中。（通过AppendEntries RPC）,超过半数follower复制则应用此Log到自身状态机中，这个过程称作commit。raft保证commit的entries肯定会最终在所有的机器上执行。

两点保障
* 索引相同和term相同的logs[]一定含有相同命令。
* 索引相同和term相同的logs[]之前的命令一定相同。

how?
* 在一个term的index上，leader只创建至多一条日志条目。
* leader 发送AppendEntries RPC的时候，带上该日志的前一个日志的index和term，follower收到后做一个检查，满足才做添加。（最初肯定是一个一致的状态）

以上条件在leader无宕机的时候很容易维护。

在leader出现宕机的时候，不同server间的log还是会出现不一致的情况。
leader对这种场景的处理是强制使follower复制他的日志。

方法:leader维护了next_index给所有server（初始化为leader的last_index+1）。
如果Appendentries被拒绝，那么leader将next_index的值-1,重复AppendEntreis的过程。
最后肯定有一个匹配的（空），然后leader开始复制自己的日志到server的日志。

这样就保证了每个follower强制复制他的日志。

可以用一个二分查找做一个优化。

#### 安全问题

一个可能的情况
五台机器 分别为 a b c d e
a得到log A
同步到b c 并自身进行了commit，然而commit后马上崩溃。
若没有限制条件，此时d或者e选举成为了leader，则破坏了一致性条件。（b,c的log应commit）

解决方法
只有包含所有commited entries 的follower才有机会成为leader。
在RequestVote RPC中有体现，candicate必须是比follower更新才能获得投票。


仅仅如此，还是会出现超过半数server同意了某日志，并且该日志被提交，然而日志被覆盖(如下)

继续考虑a b c d e
a 得到log A，复制给b，然后崩溃
接着e选举成leader，得到log E,崩溃
此时a又选举成为leader，将日志复制给c，此时满足多数派条件

若此时，commit A ，然后a崩溃，接着e选举成为leader，根据前文的条件，log E是可以复制commit的a日志。出错。

所以每一个term不应该commit先前term的日志，哪怕其满足多数条件。

raft对此做出的限制是必须提交 **满足多数派条件** **term为当前leader term**的日志
并且在提交该日志的时候对之前的日志也会提交(根据commitedID)。这样避免了上述的问题出现。


### 安全性认证


server 

维护的信息 
```c++
struct{
    current_term;
    log[];
    voted;//??
}

```


# PAXOS

在读这篇论文的同时，我参考了关于 Paxos-Made-Simple 的前身 The-Part-Time-Parliament 数学证明部分，之后对应于这篇论文，感觉获得了很多理解和收获。

在 The-Part-Time-Parliament中，定义了如下的一些参数

###基础定义

一轮投票由**四个要素**构成

$B_{bol}$  **唯一**标记一次投票过程
$B_{qrm}$  参加投票的人的集合，可以理解为多数派(人数$> \dfrac{n}{2}$) 注:参加投票不一定投出选票
$B_{vot}$  在参加投票的集合中投出选票的人的集合 
$B_{dec}$  要投票通过的提案

一个提案通过，当且仅当 $B_{qrm} \subseteq B_{vot}$

因为一次投票过程往往无法决定一个提案的值(考虑如下情况)

>有 a , b , c ,d , e 五人
多数派条件满足人数大于等于3
现有提案 A ，B ，C ，D ，E
vote 表示参与并投票
o表示参与
x表示未参与
以后都用这个例子来说明，不再赘述

|$B_{bol}$ |$B_{dec}$| a | b| c | d | e
- | :-: |:-:| :-: | :-: | :-: | :-:
|1|A|vote|o|o|x|x|


>显然，我们需要对多个提案进行多次投票

**设$\beta$为多数投票过程的集合**


|$B_{bol}$ |$B_{dec}$| a | b| c | d | e
- | :-: |:-:| :-: | :-: | :-: | :-:
|1|A|vote|o|x|vote|x|
|3|B|o|vote|vote|x|x|
|4|C|vote|x|vote|o|o|
|2|D|o|x|o|vote|x|
|5|E|o|o|o|o|vote|

>显然，有五种提案等待通过，然而都没有达到通过的条件, 这时，我们必须考虑其他方法。

### $MaxVote(B_{bal},B_{qrm},\beta)$运算与三大法则

* 法则一: $B_{1}(\beta):= \forall B,B^{'} \in \beta : (B \neq B^{'} )\Longrightarrow (B_{bal} \neq B_{bal}^{'})$
* 法则二: $B_{1}(\beta):= \forall B,B^{'} \in \beta : (B_{qrm} \bigcap B^{'}_{qrm} ) \neq \varnothing$

>法则一和法则二较为简单。
法则一表示$B_{bal}$每次投票唯一
法则二表示法定集合必有交

* 法则三:$B_{1}(\beta):= \forall B,B^{'} \in \beta : MaxVote(B_{bal},B_{qrm},\beta)_{bal}\Longrightarrow (B_{dec}=MaxVote(B_{bal},B_{qrm},\beta)_{dec})$

>法则三比较抽象，但是是整个一致性算法的保证的基石，通俗来讲
如果在一次投票的过程中，投票者在先前**最近**投票的过程中有过投票，那么这次投票过程的提案为最近那次投票过程的提案。

>如果对之前那个例子运用$MaxVote$运算，结果如下

|$B_{bol}$ |$B_{dec}$| a | b| c | d | e
- | :-: |:-:| :-: | :-: | :-: | :-:
|1|A|**vote**|o|x|**vote**|x|
|3|B|o|vote|**vote**|x|x|
|4|**B**|**vote**|x|**vote**|o|o|
|2|**A**|o|x|o|**vote**|x|
|5|E|o|o|o|o|vote|

>解释说明：
1.$B_{bol}=1$时，先前没有已经投票过的人，提案由自己决定。
2.$B_{bol}=3$同1.
3.$B_{bol}=4$时，参与投票者a,c都在之前投出过票，$B_{bol}=3$是最近投票的过程，故$B_{dec}$为$B_{bol}=3$的$B_{dec}=B$
4.$B_{bol}=2$时，参与投票者d在$B_{bol}=1$投出过票，故$B_{dec}$为$B_{bol}=1$的$B_{dec}=A$
5.$B_{bol}=5$同1.


**如果这三条原则成立，可以推出**

$(B_{qrm} \subseteq B_{vot})\wedge(B_{bal}^{'}>B_{bal}) \Longrightarrow (B_{dec}^{'}=B_{dec})$

>通俗的讲
如果有一个提案得到通过，那么大于其标识的提案如果也通过，那么他们提案必定相同。

#### 证明与理解

从理解方面讲，如果标号小的通过了提案，标号大的多数派与其必定有交集，交集中的元素决定了$MaxVote$运算结果使得标号大的提案只能与标号小的提案取得相同的提案。

**严格证明的理解**
运用反证法

1. 设$B_{bal}$为通过提案。不妨设$C_{bal}$为$C_{bal}>B_{bal}$且$C_{dec}\neq B_{dec}$中**标号最小**的提案
2. $C_{bal} > B_{bal}$ 
前提条件之一
1. $B_{vot} \cap C_{vot} \neq \varnothing$ 
法则2，必定有交
1. $MaxVote(C_{bal},C_{qrm},\beta)_{bal} \ge B_{bal}$ 
根据$MaxVote$的运算规则，容易得出。
1. $MaxVote(C_{bal},C_{qrm},\beta)_{dec}=C_{dec} \neq B_{dec}$
由假设和结果及定理三可知。
1. $MaxVote(C_{bal},C_{qrm},\beta)_{bal} > B_{bal}$ 
由5可知必不可能相等，否则可能会取到$B_{bal}$
1. $MaxVote(C_{bal},C_{qrm},\beta)_{bal} < C_{bal}$
由$MaxVote$的定义可知，期间存在投票过程提案，他的标号在B与C之间，故与假设**标号最小**矛盾


**由于这个定理，很容易得出推论**

$(B_{qrm} \subseteq B_{vot}) \wedge(B_{qrm}^{'} \subseteq B_{vot}^{'})\Longrightarrow(B_{dec}^{'}=B_{dec})$

**如果在某个时刻通过提案（多数派），那么后来的通过的提案都与之前的相同**



Proposer

Acceptor

每个进程可以担任两个角色

预提阶段和提案阶段