#include <iostream>
using namespace std;

void test01()
{
    //static_cast:可以实现C++中内置基本数据类型之间的相互转换。
    int a = 10;
    int b = 20;
    double res = static_cast<double>(a)/static_cast<double>(b);
    cout <<res;
}

void test02()
{
    //const_cast:强制去掉这种不能被修改的常数特性
    const int a = 10;
    const int *p = &a;

    int *q = const_cast<int *>(p);
    *q = 30;

    cout <<"a="<<a<<endl;
    cout <<"*q="<<*q<<endl;
}

void test03()
{
    //reinterpret_cast:改变指针或引用的类型、将指针或引用转换为一个足够长度的整形、将整型转换为指针或引用类型。
    int *a = new int;
    double *d = reinterpret_cast<double *>(a);  
}

class base
{
public :
    void m(){cout<<"m"<<endl;}
};
 
class derived : public base
{
public:
    void f(){cout<<"f"<<endl;}
};

void test04()
{
    //dynamic_cast
    //1 在运行时处理
    //2 基类中一定要有虚函数（动态多态）
    derived * p;
    p = static_cast<derived *>(new base);
    p->m();
    p->f();
}

int main()
{
    //test01();
    //test02();
    test04();
    return 0;
}