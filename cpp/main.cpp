#include <cstdio>
#include <cstdlib>
#include "../include/demo3_typedef.h"

// extern declare c-function list:
extern int file_io_test();

#ifdef __cplusplus
extern "C" {
#endif
void test_big_small_endian()
{
    unsigned int a = 1;
    BYTE_T *ptr = (BYTE_T*)&a; // cast int* to unsigned char* 

    dbgprt("system is %s endian\n",(*ptr==1?"small":"big"));
}
#ifdef __cplusplus
}
#endif

int main(void) {

    _ENTER(main);
    test_big_small_endian();


    file_io_test();

    return 0;
}
