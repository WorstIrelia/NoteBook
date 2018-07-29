## 最短路

spfa 写的比较简单，可以处理负边权，稀疏图比较快

dijkstra + 堆优化 稠密图 不能计算负边权

次短路 spfa的时候转移多加一维度

```c++
void spfa(int s){//严格第k短路，cost[i][j]表示从源点到i点的第k短路，anscnt[i][j]表示从源点到i点第j短路的个数
    memset(vis,0,sizeof(vis));
    memset(cost,0x3f,sizeof(cost));
    queue<int>q;
    q.push(s);
    vis[s]=1;
    cost[s][0]=0;
    anscnt[s][0]=1;
    while(!q.empty()){
        int u = q.front();
        vis[u]=0;
        q.pop();
        for(auto &elem:vec[u]){//这么做是不对的，应该让元素后移动
            int v = elem.first, w=elem.second;
            int j=0;
            bool flag=0;
            for(int i=0;i<k;i++){
                int tmp = cost[u][i]+w;
                while(j<k&&cost[v][j]<tmp) j++;
                if(j==k) break;
                if(cost[v][j]==tmp){
                    anscnt[v][j]+=anscnt[u][i];
                }
                else{
                    cost[v][j]=tmp;
                    anscnt[v][j]=anscnt[u][i];
                    flag=1;
                }
            }
            if(flag&&!vis[v]){
                q.push(v);
                vis[v]=1;
            }
        }
    }
}
```


```c++
void spfa(int s){//严格第k短路，cost[i][j]表示从源点到i点的第k短路，anscnt[i][j]表示从源点到i点第j短路的个数
    memset(vis,0,sizeof(vis));
    memset(cost,0x3f,sizeof(cost));
    queue<pair<int,int> >q;
    q.push({s,0});
    vis[s][0]=1;
    cost[s][0]=0;
    anscnt[s][0]=1;
    while(!q.empty()){
        pair<int,int> elem = q.front();
        vis[elem.first][elem.second]=0;
        q.pop();
        for(int aa=0 ;aa<vec[elem.first].size();aa++){
            pair<int,int> &tmpelem = vec[elem.first][aa];
            int v = tmpelem.first, w=tmpelem.second;
            int j=0;
            bool flag=0;
            int tmp = cost[elem.first][elem.second]+w;
            while(j<k&&cost[v][j]<tmp) j++;
            if(j==k) continue;
            if(j==0) cost[v][1]=cost[v][0];//仅仅是次短路 记住要后边移动
            cost[v][j]=tmp;
            if(!vis[v][j]){
                vis[v][j]=1;
                q.push({v,j});
            }

        }
    }
}
```