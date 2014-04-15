#ifndef _DEMO_TEST_H
#define _DEMO_TEST_H

#include <iostream>

namespace demo3{

using std::cout;
using std::endl;

void test();

template <typename T>
class A{

public:
    T* GetValue(int i){
        return NULL;
    };
    A():size(0),pValue(0){
        cout << "A()" << endl;
    };
    A& operator=(A&){
        return *this;
    };
    virtual ~A(){
        cout << "~A()" << endl; 
        delete pValue;
    };
protected:
    int size;
private:
    T* pValue;
};

class B1: public virtual A<int>
{ //virtual void b1func(); 
};

class B2: public virtual A<int>
{ //virtual void b2func(); 
};

class D: public B1,public B2
{};        

// auto ptr;
// virtual inheritance
// template inheritance
// mutiple inheritance

} // end namespace


#endif
