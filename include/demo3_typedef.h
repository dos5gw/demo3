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
	
#define CONST				const
#define VOID				void

/* general return values */
#define DEMO_OK	0
#define DEMO_ERR	(-1)
#define DEMO_NULL 0

#define _ENTER(FUNC) \
do{\ 
printf("Function: "#FUNC" Enter!\n");\
}\
while(0)

#ifdef _DEMO_DBG_PRINT
#define dbgprt(fmt,...) do{printf("DEMO-DBG: ");printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgprt(fmt,...)
#endif


#ifdef __cplusplus
}
#endif

#endif
