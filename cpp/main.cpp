#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "../include/demo3_typedef.h"

static const char *optString = "Il:o:vh?";
static struct globalArgs_t globalArgs;
// extern declare c-function list:
extern int file_io_test();
extern void demo3_test_parse_jpeg();
extern void demo3_thread_pool_test();

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

/* -- test 01 start -- */
struct str{
    int len;
    char s[0];
};
struct foo{
    struct str *a;
};
void test01()
{
    struct foo f = {0};
    if(f.a->s)
        printf(f.a->s);  // core down this line
}


int main(int argc, char* argv[]) 
#if 1
{
    _DEBUG_ENTER(main);
    test_big_small_endian();
    test01();



    //demo3_thread_pool_test();

    return 0;
}
#else
{

    int opt = 0;

    _DEBUG_ENTER(main);
    test_big_small_endian();

    //demo3_test_parse_jpeg();
    //file_io_test();
    opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
        switch( opt ) {
            case 'I':
                globalArgs.noIndex = 1; /* true */
                break;

            case 'l':
                globalArgs.langCode = optarg;
                break;

            case 'o':
                globalArgs.outFileName = optarg;
                break;

            case 'v':
                globalArgs.verbosity++;
                break;

            case 'h':   /* fall-through is intentional */
            case '?':
                //display_usage();
                break;

            default:
                /* You won't actually get here. */
                break;
        }
        
        opt = getopt( argc, argv, optString );
    }
    return 0;
}
#endif
