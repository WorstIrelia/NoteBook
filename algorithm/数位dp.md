模板
```c++
int digit[];//保存每位数字的个数
long dfs(int pos, int num, bool lead, bool limit){
    if(!limit && !lead && dp[pos][num] != -1) return dp[pos][num];
    if(!pos) return 1;
    int up = limit? digit[pos - 1] : 9;
    long ret = 0;
    for(int i = 0; i <= up; i++){
        ret += dfs(pos - 1, i, lead && !i, limit && (i == up));
    }
    if(!limit && !lead) dp[pos][num] = ret;
    return ret;
}
void solve(){
    dfs(pos, 0, 1, 1);//首位0开始，lead和limit都是1
}
```

dp[i][j] 表示第i位是j时，不管(1到i-1位的数字取什么)，满足某个条件的个数。
如654321


$f(i, j)$表示第i位是j时候，j之前没有前导0,不管1到i-1位的数字取什么，满足某条件的个数。
注意j可以是0,因为之前有大于0的数字出现过。
$g(i, 0) = \sum_{k=1}^{k=up-1}f(i, k) + ans(i-1,up) + g(i-1, 0)$
j没有到限制时
$f(i, j) = \sum_{k=0}^{k=9} f(i-1, k)$ 
j到限制的时候
$ans(i, j) = \sum_{k=0}^{k=up-1}f(i-1,k) + ans(i-1,up)$

1,2,3,4,5直接记数，注意0的时候是前导0，需要特殊处理，到达6的时候是限制
如果前导0对计数没有影响，那么就直接记录,不需要lead
前导零是为了区别(000123)与(123)的区别(前者前面一定放了非0数，后者前面全是0,计数不一样)