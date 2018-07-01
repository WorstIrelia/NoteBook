#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<pthread.h>
#include<sys/wait.h>
#define MAXN 110
#define debug(...) fprintf(stderr,"file in %s , line = %d , %s\n",__FILE__,__LINE__,__VA_ARGS__)
#define assert(x) if(x){}else{\
    debug("assert failed");\
    fprintf(stderr,#x"\n"); \
    exit(-1);\
}

int tot[MAXN];
int id[MAXN];
pid_t pid;
pthread_t tid[MAXN];
pthread_mutex_t mutex[MAXN];
int n,m;
struct Accept{
    int id;
    int value;
};
struct Accept accept[110];
void swap(int *a,int *b){
    int tmp=*b;
    *b=*a;
    *a=tmp;
}
static inline int max(int x,int y){
    return x>y?x:y;
}
void prepropose(int id,int proposer_id,int *maxn,int *cnt){
    pthread_mutex_lock(&mutex[id]);
    if(accept[id].id<proposer_id){
        (*cnt)++;
        accept[id].id=proposer_id;
        *maxn=max(*maxn,accept[id].value);
    }
    pthread_mutex_unlock(&mutex[id]);    

}
void set_accept(int id,int proposer_id,int value){
    //printf("%d %d %d\n",id,proposer_id,value);
    pthread_mutex_lock(&mutex[id]);
    //printf("%d %d %d ttt\n",id,accept[id].id,proposer_id);
    if(accept[id].id==proposer_id){
        printf("%d %d %d %d\n",id,proposer_id,accept[id].value,value);
        assert(accept[id].value==-1||accept[id].value==value);
        accept[id].id=proposer_id;
        accept[id].value=value;
    }
    pthread_mutex_unlock(&mutex[id]);
}
void *proposer(void *arg){
    int proposer_id=*(int*)arg;
    free(arg);
    int a[MAXN];
    for(int i=0;i<m;i++){
        a[i]=i;
    }
    int tmp=m;
    int maxn=-1;
    int i;
    int cnt=0;
    for(i=0;i<m;i++){
        int index=rand()%tmp;
        //printf("%d %d %d %daaa\n",proposer_id,index,i,a[index]);
        prepropose(a[index],proposer_id,&maxn,&cnt);
        swap(&a[index],&a[tmp-1]);
        tmp--;
        if(cnt>m/2) break;
    }
    if(maxn==-1) maxn=proposer_id;
    //printf("%d %d!!!\n",proposer_id,maxn);
    for(int j=m-cnt;j<m&&cnt>m/2;j++){
        //printf("%d %d!!!\n",proposer_id,maxn);
        set_accept(a[j],proposer_id,maxn);
    }
    

}
void do_it(int* flag){
    int tmpn=n;
    int *q;
    for(int i=0;i<n;i++){
        int index=rand()%tmpn;
        //printf("%d heiheihei\n",id[index]);
        q=malloc(sizeof(int));
        *q=id[index];
        pthread_create(&tid[id[index]],NULL,proposer,(void*)q);
        swap(&id[index],&id[tmpn-1]);
        tmpn--;
    }
    //sleep(3);
    for(int i=0;i<n;i++){
        //printf("%d\n",i);
        pthread_join(tid[i],NULL);
    }
    for(int i=0;i<m;i++){
        tot[accept[i].value]++;
        printf("%d %d\n",accept[i].id,accept[i].value);
    }
    printf("....\n");
    for(int i=0;i<n;i++){
        if(tot[i]>m/2) *flag=0;
        if(tot[i]) printf("%d %d\n",i,tot[i]);
    }
    

}
int main(){
    srand(time(0));
    scanf("%d%d",&n,&m);
    
    assert(n>0&&m>0);
    //assert(n==0||m==0);
    pid=getpid();
    for(int i=0;i<m;i++){
        accept[i].value=-1;
    }
    for(int i=0;i<m;i++){
        pthread_mutex_init(&mutex[i],NULL);
    }
    for(int i=0;i<n;i++){
        id[i]=i;
    }
    int flag=1;
    while(flag){
        for(int i=0;i<m;i++) tot[i]=0;
        do_it(&flag);
    }
    return 0;
}