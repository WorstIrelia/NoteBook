#include<stdlib.h>
#include<stdio.h>

#define debug(...) printf(__VA_ARGS__)
#define debug2(x...) printf(x)
#define fun(n) printf("test"#n"=%d",token##n)
#define token9 9

static inline int func(int x,int y){
    return x+y;

    //printf("%s\n",__func__);
}
int main(){
    func(1,2);
    debug("%s\n",__FILE__);
    debug("%d\n",__LINE__);
    debug("hello world %d\n",1);
    debug2("hello %d %s\n",123,"abcd");
    fun(9);
    int *p;
    printf("%d\n",sizeof(*p));
    //name(abcde);
}