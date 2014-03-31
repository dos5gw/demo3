#ifndef _DEMO_TYPEDEF_H_
#define _DEMO_TYPEDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define _DEMO_DBG_PRINT

/******************************************************************************/
/* generic type redefinitions */
typedef int								INT32_T;
typedef unsigned int					UINT32_T;
typedef short							INT16_T;
typedef unsigned short					UINT16_T;
typedef char							CHAR_T;
typedef unsigned char					BYTE_T;
typedef unsigned long                   DWORD_T;
	
#define CONST				const
#define VOID				void

/* general return values */
#define DEMO_OK	0
#define DEMO_ERR	(-1)
#define DEMO_NULL 0

/* unset warning "define but not used" */
#define UNSET(X) \
do{\
X=X;\
}\
while(0)

#define _DEBUG_ENTER(FUNC) \
do{\
printf("Function: "#FUNC" Enter!\n");\
}\
while(0)

#ifdef _DEMO_DBG_PRINT
#define dbgprt(fmt,...) do{printf("DEMO-DBG: ");printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgprt(fmt,...)
#endif

struct globalArgs_t {
    int noIndex;                /* -I option */
    char *langCode;             /* -l option */
    const char *outFileName;    /* -o option */
    FILE *outFile;
    int verbosity;              /* -v option */
    char **inputFiles;          /* input files */
    int numInputFiles;          /* # of input files */
};


#ifdef __cplusplus
}
#endif

#endif
