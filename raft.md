

## need to do 

raft 快照压缩

两种方式，无主式或者基于领导者



# 前言

在阅读这两篇论文之前，我并没有分布式系统相关的经验。初次阅读这两篇论文，我感到有很多概念需要恶补，如状态机理论，一致性等等，才能在此基础上更好的理解这两篇**著名的且在工业界有具体应用**的论文。通过不断的细读论文和查阅资料，感觉对分布式一致性算法方面有了一个较为初步的认识和感受，也对此产生了较为浓厚的兴趣，故我粗略读了**Made Paxos Simple**的前身论文**The Part-time parliament**的数学证明部分以及最新的**Raft**算法的论文(在原来的论文的**日志压缩**和**配置更新**做了一些改进)。收获还是很大的。


-----


#两篇论文解决的问题

在以下两个条件下

* 不考虑消息篡改
* 在容忍进程慢，崩溃，重启，网络的延迟，丢失，重复的情况下。


为了在 **在多数派**同意的情况下，达到一致性。

个人认为一致性是所有机器**最终会处于一种共同的状态**（最终一致性），但在某个时刻(如raft算法commit,paxos算法达成多数派)也是一种达成一致的状态，这种状态很快会收敛到最终一致的状态。同时，这种一致性应该是一种强一致性，如对用户来说，操作成功后立即查询有效(因为paxos和raft都有leader机制，如果写入成功，查询必定有效，除非leader挂到，那么剩下来的机器也很快能达到一致性)。




# RAFT

##简要心得

这篇论文标题提倡的就是**容易理解**的一致性算法，然而对我而言，初次接触相关的概念，也感觉不是很容易。不过这篇论文图文并茂，例子翔实，甚至给出了具体RPC的方式，不断的推敲下，还是能进行初步的理解。以下是我梳理的具体的流程。

##RAFT算法流程


### 三个角色

* leader （每个集群至多一个)
* candicate (可能同时存在多个)
* follower (剩下的都是)

角色的状态转换

leader -> follower  (RPC收到的回应中的term>curren_term)

follower -> candicate (timeout) term++,vote self

candicate -> leader (获得超过一半的选票，上取整)

candicate -> follower 发现leader或者收到的RPC_result的term>current_term，同时更新自己的term。

candicate -> candicate 多个candicate,选票被瓜分，每个candicate都没有获得多数投票

**值得注意的是，每个角色的 term 异常重要**
**这里有两个基本原则**
* 不管任何角色收到的信息中，只有其他角色的term大于自己的term，便把自己的term设定为其他角色的term，并转化到follower态
* commitIndex > lastApplied 应用一个 log[lastApplied]的日志到状态机。

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

#### 心跳机制
选举出来的leader用心跳机制防止follower进入candicate阶段。
一般来说,candicate的超时时间设置为150ms-300ms。

----

### Log repolication

1. leader 接受 client的请求，并将log加到entries尾。
2. leader 复制log 到 server中。（通过AppendEntries RPC）,超过半数follower复制则应用此Log到自身状态机中，这个过程称作commit。raft保证commit的entries肯定会最终在所有的机器上执行。

两点保障
* 索引相同和term相同的logs[]一定含有相同命令。
* 索引相同和term相同的logs[]之前的命令一定相同。

如何保证?
* 在一个term的index上，leader只创建至多一条日志条目。
* leader 发送AppendEntries RPC的时候，带上该日志的前一个日志的index和term，follower收到后做一个检查，满足才做添加。（最初肯定是一个一致的状态）

以上条件在leader无宕机的时候很容易维护。

在leader出现宕机的时候，不同server间的log还是会出现不一致的情况。
leader对这种场景的处理是强制使follower复制他的日志。

方法:leader维护了next_index给所有server（初始化为leader的last_index+1）。
如果Appendentries被拒绝，那么leader将next_index的值-1,重复AppendEntreis的过程。
最后肯定有一个匹配的（空），然后leader开始复制自己的日志到server的日志。

这样就保证了每个follower强制复制他的日志。

**可以用一个二分查找做一个优化**

### 安全问题


#### 根据日志最新程度进行选举


一个可能的情况
五台机器 分别为 a b c d e
a得到log A
同步到b c 并自身进行了commit，然而commit后马上崩溃。
若没有限制条件，此时d或者e选举成为了leader，则破坏了一致性条件。（b,c的log应commit）

解决方法
只有包含所有commited entries 的follower才有机会成为leader。
在RequestVote RPC中有体现，candicate必须是比follower更新才能获得投票。

**所以成为candicate一定要保证自己的日志是最新的，换句话说就是比较投票阶段返回的日志index和term值，这样就避免了d,e成为candicate（term越大越新，term相同log index越大越新）**


#### 不要commit先前term的日志，哪怕达到了大多数条件。
仅仅如此，还是会出现超过半数server同意了某日志，并且该日志被提交，然而日志被覆盖(如下)

继续考虑a b c d e
a 得到log A，复制给b，然后崩溃
接着e选举成leader，得到log E,崩溃
此时a又选举成为leader，将日志复制给c，此时满足多数派条件

若此时，commit A ，然后a崩溃，接着e选举成为leader，根据前文的条件，log E是可以覆盖commit的a日志。出错。

所以每一个term不应该commit先前term的日志，哪怕其满足多数条件。

raft对此做出的限制是必须提交 **满足多数派条件**而且**term为当前leader term**的日志
并且在提交该日志的时候对之前的日志也会提交(根据commitedID)。这样避免了上述的问题出现。

回到先前的例子，在a又选举成为leader的时候，因为a的term大于获得log A时候的term，所以即使a满足多数派条件，也不能提交A的日志，这样被E覆盖也不会造成问题。

#### 选举时间限制

broadcasttime << electionTimeout << MTBF

直观的讲，electionTimeout如果太短，会和broadcasttime形成竞争条件，降低可用性。
electionTimeout要是太长，竞选的时间也太长，会降低可用性。

论文作者选择了150-300ms，基于避免时间太多，与broadcasttime竞争，降低可用性。间隔大，又能保证选举的成功。

###补充

####快照

这部分主要考虑当机器运行的时间越来越久，从初始状态到最后状态的日志会越来越多。持久化的日志肯定要存储到具体的物理设备上，这样会导致物理设备的资源越來越少。



# PAXOS

在读made paxos simple 这篇论文的同时，我参考了关于 Paxos-Made-Simple 的前身 The-Part-Time-Parliament 数学证明部分，之后对应于这篇论文，感觉获得了很多理解和收获。

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


## Made paxos simple 

### 简要心得

总的来说,made paxos simple 是 The-Part-Time-Parliament 的具体实现。作者从进程集出发，通过proposer,acceptor,learner三者角色，通俗易懂的讲解了paxos的工作原理。

概括来讲，proposer,acceptor,learner三者交互的流程如下。


每个进程可以担任任意三个角色之一。

预提案与提案阶段。


流程一（prepare阶段):
Proposer
1. proposer向acceptor集合发起提议，每个提议带有标号n，不同的提议标号都不相同。**(标号唯一)**。只有收到**超过半数**的acceptor的回复，才算提议成功，进入流程二。

Acceptor
1. 如果acceptor收到的提议标号**小于**之前收到的提议标号，拒绝。
2. 如果acceptor收到的提议标号**大于**之前收到的提议标号，将自己的提议标号改为该标号。同时，如果该acceptor已经接收之前的提议，则返回之前接收提议的value，否则，返回空。

流程二 (propose):

Proposer
1. proposer从返回的答复中选择**提议标号最大**提议的value，向接受的acceptor发起提议value。如果所有答复都没有提议value，则选择自己的提议value。

Acceptor
1. acceptor 在只有**当前提议标号**与收到**提议标号**相同的情况下，更新自己的提议value为收到的value，否则，一律拒绝。

当proposer收到**超过一半**的提议通过，则**达成一致**

Learner

设learn的数量为$n$,acceptor的数量为$m$

作者提出了三种发送给learner的方式，
1. 每个acceptor将阶段二的结果发送给每个learner，这样复杂度为$n*m$，复杂度高，消耗大。
2. 每个acceptor将阶段二的结果发送给一个learner，将结果只发给**一个**learner，达成一致后learner将结果发送个所有learner。缺点是这一个learner宕机，服务将变得不可用。
3. 每个acceptor将阶段二的结果发送给一个learner集合，由learner集合中统计达成一致时的信息。第三个方案可以看成第一个方案和第二个方案的折衷。这种思想在操作系统原理上也很常见。

###活锁

值得一提的是，paxos这种竞选方式会出现活锁。考虑以下情况，有两个proposer进行提案。

proposer1 进行完阶段一。

proposer2 进行阶段一的时候，将proposer1的阶段一的标记全部覆盖。

proposer1 写入失败，重新进入阶段一，将proposer2的阶段一的标记全部覆盖。

proposer2 写入失败，重新进入阶段一，将proposer1的阶段一的标记全部覆盖。

...

如此循环。

论文中提到的是选择一个 distinguished proposer ，一个leader进行propose的过程。如果只有一个proposer，那么问题迎刃而解。(类似raft)。

同时论文还提到了一个结论，**在通信完备**的情况下，选举proposer一定得使用随机化方法和超时机制。(如同raft使用的机制)。

### 具体实现

文章最后一个篇章花了较大笔墨讲了关于状态机的paxos实现。就我个人感觉而言，有以下一些特点。

>采用了经典的客户端，服务端模型，选举出一个distingushed proposer去当做leader，只有leader有权利进行propose，避免了活锁。值得注意的是，选举的过程就用了paxos一致性的原理。选举出的leader要经过多数派的同意。

>paxos这种


###个人总结







-----

###程序模拟验证paxos

基于个人兴趣，我写了一个简单的小程序来模拟验证朴素的paxos方法的正确性。

主要基于c语言，（我熟练掌握c,c++，了解Python），用多个线程模拟多个proposer，用数组模拟acceptor，来模拟朴素paxos算法的过程。

主要设计的思路

1. 用线程模拟proposer，高并发的时候访问随机，模拟paxos中的proposer随机到来随机离去。
2. 用结构体来模拟acceptor，成员主要有接受的propose和value,用锁来保证单次访问的原子性质。
3. 程序用mod(n)同余的性质来保证任何两个proposer不会出现相同的提案号(proposer_id)。如，n=3,proposer_id=0+3n,1+3n,2+3n,当阶段一或者阶段二不成功的时候id会增加。
4. 验证两点。1.在不出现活锁的时候，会出现多数派。2.出现多数派后之后写入的都是多数派的value值。

####简要结果验证

**输入n<100,m<100,n为proposer数量，m为acceptor数量**

以下为 n=3 m=5时候的场景。

![图片](./abc.png)



