#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <string>
#include "test.hpp"
#include "demo3_typedef.h"
#include "demo3_sort.h"
#include "demo3_thread_pool.h"

using namespace std;

static const char *optString = "Il:o:vh?";
static struct globalArgs_t globalArgs;

// extern declare c-function list:

#ifdef __cplusplus
extern "C" {
#endif
void demo3_big_or_small_endian()
{
    unsigned int a = 1;
    BYTE_T *ptr = (BYTE_T*)&a; // cast int* to unsigned char* 

    dbgprt("system is %s endian\n",(*ptr==1?"small":"big"));
}
#ifdef __cplusplus
}
#endif

int main(int argc, char* argv[]) 
#if 1
{
    _DEBUG_ENTER(main);
    demo3_big_or_small_endian();

    demo3_simple_sort_test();
    demo3_distribution_sort_test();
    
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
