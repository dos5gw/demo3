#include "stdio.h"
#include "stdlib.h"
#include <string.h>

/* 下面的代码在哪一行core down？ */
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
    char arr[5] = {0x0,0x1,0x2,0x3,0x4};
    printf("arr=0x%x, &aa=0x%x, &arr[0]=0x%x\n",arr,&arr,&arr[0]);

    /*  下面的代码在哪一行core?  */
    struct foo f = {0};
    //if(f.a->len)          //(0)
    //if (&(f.a->len))      // ?
    //if ((int)&(f.a->len)) // ?
    if(f.a->s){             //(1)
        //printf(f.a->s);   // (2)
        printf("0x%x",f.a->s);  // (3)
    }
}
/* -- test 01 end -- */



// ----- test 02 ------
struct str2{
    int len;
    char *s;
};
struct foo2{
    struct str *a;
};
void test02()
{

    struct foo2 f2 = {0};
    if(f2.a->s){             //(1)
        //printf(f2.a->s);   // (2)
        printf("0x%x",f2.a->s);  // (3)
    }
}

// http://coolshell.cn/articles/11377.html
// ------ test 03 -------
struct test03{
    char i;
    short c;
    char *p;
    char s[10];
};

struct test04{
    int i;
    char s[0];
};
struct line {
   int length;
   char contents[0]; // C99的玩法是：char contents[]; 没有指定数组长度
};


void test03(){
    struct test03 *pt=NULL;
    printf("&test.i = %x\n", &pt->i);  
    printf("&test.c = %x\n", &pt->c); 
    printf("&test.p= %x\n", &pt->p);
    printf("&test.s %x\n", pt->s);

    struct test04 *pt2=NULL;
    printf("&t4.i = %x\n", &pt2->i); 
    printf("&t4.s = %x\n",&pt2->s);

    int this_length=10;
    struct line *thisline = (struct line *)malloc (sizeof (struct line) + this_length);
    thisline->length = this_length;
    memset(thisline->contents, 'a', this_length);
    
    printf("&line=%x\n",thisline);
    printf("&line.content=%x\n",&thisline->contents);


    char arr[5] = {0};
    char (*ptr)[5] = &arr;
    printf("arr=0x%x, &arr=0x%x, &arr[0]=0x%x, ptr=0x%x\n",arr,&arr,&arr[0],ptr);
    return ;
}


 
