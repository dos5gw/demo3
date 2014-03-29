#include <cstdio>
#include <cstdlib>
#include "../include/demo3_typedef.h"

// extern declare c-function list:
extern void test_parse_jpeg();
extern void test_ccode();  

void test_big_small_endian()
{
    unsigned int a = 1;
    BYTE *ptr = (BYTE*)&a; // cast int* to unsigned char* 

    printf("system is %s endian\n",(*ptr==1?"small":"big"));
}

int main(void) {

    test_big_small_endian();
    return 0;
}
