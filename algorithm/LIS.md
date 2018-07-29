```c++
int LIS(vector<type>& vec)){
    type dp[vector.size()];
    dp[0] = vec[0] ; 
    int len=0 ; 
    for(int i = 1;i < vec.size(); i++){
        if(dp[len]<vec[i]) dp[++len]=vec[i];
        else *lower_bound(dp,dp+len+1,vec[i])=vec[i];
    }
    return len+1;
}
```