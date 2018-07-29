#include<bits/stdc++.h>
#include<new>
#include<unistd.h>
using namespace std;
typedef void (*ptr)();

#define GET_TABLE(p, num) (*(((long **)&(*p))+num))
#define FUNC(table, num) (*(ptr)(*(table + num)))()

class BaseA{
public:
    virtual void A(){
        printf("BaseA A\n");
    }
    virtual void B(){
        printf("BaseA B\n");
    }
    void C(){
        printf("BaseA C\n");
    }
    void test(){
        printf("testA\n");
    }

};
class BaseB{
public:
    virtual void A(){
        printf("BaseB A\n");
    }
    virtual void B(){
        printf("BaseB B\n");
    }
    virtual void D(){
        printf("BaseD D\n");
    }
    void C(){
        printf("BaseB C\n");
    }
    void test(){
        printf("testB\n");
    }

};
class Drive:public BaseA,public BaseB{
public:
    void A(){
        printf("Drive A\n");
    }
    void C(){
        printf("Drive C\n");
    }
    void D(){
        printf("Drive D\n");
    }

};
class _Drive:public Drive{
    void A(){
        printf("_Drive A\n");
    }
    void C(){
        printf("_Drive C\n");
    }
    void D(){
        printf("_Drive D\n");
    }

};



class BaseV{
public:
    long x;
    BaseV():x(0x12345678){}
    virtual void A(){
        printf("BaseV\n");
    }
    
};
class DriveA :public virtual BaseV{
public:
    long y;
    DriveA():BaseV(),y(0x87654321){}
    virtual void A(){
        printf("DriveA\n");
    }
};
class DriveB :public virtual BaseV{
public:
    long z;
    DriveB():BaseV(),z(0x22334455){}
    virtual void A(){
        printf("DriveA\n");
    }
};
class DriveV :public DriveA, public DriveB{
public:
    long fin;
    DriveV():DriveA(),DriveB(),fin(0x66778899){}
    void A(){
        printf("DriveV\n");
    }
};

void foo(){

    printf("foo\n");
}
class A{
private:
    int x;
public:
    int y;
    A():x(1),y(2),z(3){}
    void pri(){
        printf("%d %d %d\n",x,y,z);
    }
protected:
    int z;
};
class B{
private:
    int x;
public:
    int y;
    B():x(1),y(2),z(3){}
    void pri(){
        printf("%d %d %d\n",x,y,z);
    }
protected:
    int z;
};

class C:public A,public B{
public:
    int aaaa;
    void test(){
        A::pri();
    }

};

class test{
private:
    ~test(){}
public:
    test(){}
    void destroy(test *p){
        delete p;
    }
    void destroy_all(test *p){
        delete[] p;
    }
};


void abctest(){
    printf("malloc failed, try again.\n");
    // sleep(1);
}

char *my_strpy(char *dest, char *src){
    assert(dest != NULL && src != NULL);
    char *ret = dest;
    while(*src != 0){
        *dest ++ = *src ++;
    }
    return ret;

}



class test_const{
public:
    int a;
    void pri(){
        cout << a << endl;
    }

};

template<typename T>
T foo(T = int x){
    return x + 1;
}








int main(){
    // set_new_handler(abctest);
    double x = foo<double>(5.6);
    cout << x << endl;
}

// int main(int argc,char *argv[]){
    
//     DriveV *p = new DriveV();
//     printf("hello world\n");
//     printf("%lx\n",p->x);
//     //long * test = (long *)&p;
//     // cout<<sizeof(DriveV)<<endl;
//     // p->A();
//     // printf("%lx %lx %lx %lx\n",p->x,p->y,p->z,p->fin);
//     // cout<<sizeof(DriveV)<<endl;
//     long * test = (long *)&(*p);
//     for(int i = -4 ; i < 7; i ++){
//         printf("%lx\n",*(test + i));
//     }
//     //long * v_ptra = GET_TABLE(p,0);
//     // FUNC(v_ptra,0);
    
//     // long * v_ptrb = GET_TABLE(p,2);
//     // FUNC(v_ptrb,0);
//     //long * t_ptr = GET_TABLE(p,5);


//     // printf("%lx\n",*(v_ptra+0));
//     // printf("%lx\n",*(v_ptra+1));
//     // printf("%lx\n",*(v_ptra+2));
//     // printf("%lx\n",*(v_ptra+3));

//     // printf("----------------------\n");
//     // printf("%lx\n",*(v_ptrb+0));
//     // printf("%lx\n",*(v_ptrb+1));
//     // printf("%lx\n",*(v_ptrb+2));
//     // printf("%lx\n",*(v_ptrb+3));


//     // printf("%lx\n",**(long **)&*(v_bptr));
//     // printf("%lx\n",*(t_ptr+0));
//     // printf("%lx\n",*(t_ptr+1));
//     // printf("%lx\n",*(t_ptr+2));
//     // printf("%lx\n",*(t_ptr+3));
//     // printf("%lx\n",*(t_ptr+4));
//     // printf("%lx\n",*(t_ptr+5));
//     // printf("%lx\n",*(t_ptr+6));
//     // printf("%lx\n",*(t_ptr+7));
//     // printf("%lx\n",*(t_ptr+8));
//     // printf("%lx\n",*(t_ptr+9));
//     // printf("%lx\n",*(t_ptr+10));
//     // printf("%lx\n",*(t_ptr+11));
//     // printf("%lx\n",*(t_ptr+12));
//     // printf("%lx\n",*(t_ptr+13));
//     // printf("%lx\n",*(t_ptr+14));
//     // printf("%lx\n",*(t_ptr+15));
//     // printf("%lx\n",*(t_ptr+16));
// }