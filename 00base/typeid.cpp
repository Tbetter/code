#include <iostream>
using namespace std;
#include <typeinfo>

int main(int argc, char const *argv[])
{
    const type_info &info = typeid(int);
    cout <<info.name()<<info.hash_code();
    return 0;
}
