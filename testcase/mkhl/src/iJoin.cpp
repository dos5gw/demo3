/**
 ** Copyright (c) Infor-trans Technologies, Inc. Unpublished
 **
 ** Infor-Trans Technologies, Inc.
 ** Proprietary & Confidential
 **
 ** This source code and the algorithms implemented therein constitute
 ** confidential information and may comprise trade secrets of Infor-trans
 ** or its associates, and any use thereof is subject to the terms and
 ** conditions of the Non-Disclosure Agreement pursuant to which this
 ** source code was originally received.
 **/

/******************************************************************************
DESCRIPTION:
   ITI TOOLS.

SEE ALSO:
******************************************************************************/

/* 
modification history 
-------------------------------------------------------------------------------
01a,21Dec2006,water     written
*/
#include "stdafx.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <iOsalString.h>
#include <errno.h>
#include <sys/stat.h>

//#define DEFAULT_CHAR 0x00
#define DEFAULT_CHAR 0xFF

int iOsalIsFileExist(char *path);

int iOsalIsFileExist(char *path)
    {
//#if (OSAL_CC_TYPE == OSAL_CC_MSVC || OSAL_CC_TYPE == OSAL_CC_VXWORKS)
    struct stat s;
    int         ret;

    ret = stat(path, &s);
    return !ret;
//#elif (OSAL_CC_TYPE == OSAL_CC_GnuC)
//    return !access(path, F_OK);
//#endif
    }

unsigned int iOsalAtoi(const char *a)
    {
    if (a[0] == '0')
        {
        if (a[1] == 'x' || a[1] == 'X')
            return strtoul(a + 2, NULL, 16);
        else
            return strtoul(a + 1, NULL, 8);
        }
    else
        {
        return strtoul(a, NULL, 10);
        }
    }

int fill_char;
int fill_size;

char tmp_file[100] = "tmp_file_abcdefg1234567";


void setdefault(void)
{
    fill_char = DEFAULT_CHAR;
    fill_size = 0;
}

void usage(char *prog)
{
    printf(
        "usage: %s -b/-s NUMBER file1 -b/-s NUMBER file2 ... out-file\n"
        "-b begin from NUMBER\n"
        "-s give size to file\n"
        , prog);
    
}

void my_rename(char *src, char *dst)
    {
    int rc;
    rc = rename(src, dst);
    if (rc != 0)
        {
        if (errno == EXDEV)
            {
            FILE *fp1, *fp2;
            char c;
            int i, len;
            
            fp1 = fopen(src, "rb");
            fp2 = fopen(dst, "wb");
            
            assert(fp1);
            assert(fp2);
            
            //fseek(fp1, 0, SEEK_SET);
            fseek(fp1, 0, SEEK_END);
            len = ftell(fp1);
            fseek(fp1, 0, SEEK_SET);
            
            //printf("sss %d %s %s\n", len, src, dst);
            
            for (i = 0; i < len; i++)
                {
                fread(&c, 1, 1, fp1);
                fwrite(&c, 1, 1, fp2);
                }
            fclose (fp1);
            fclose (fp2);
            
            unlink(src);
            }
        }
    }

int main(int argc, char *argv[])
{
    int i, totalLen = 0, rc;
    FILE *out, *in;
    char *filename;
        
    if (iOsalIsFileExist(tmp_file))
    {
        rc = unlink(tmp_file);
        assert(rc == 0);
    }
    out = fopen(tmp_file, "wb");
    if (out == NULL)
    {
        printf("@err: can't open `%s'\n", tmp_file);
        exit(-1);
    }
    
    setdefault();
    
    for (i = 1; ; )
    {
        char *param;
        
        char  ch;
        int   thisLen = 0, xxx;
        
        if ((argc - i) < 3)
        {
            //printf("argc=%d i=%d\n", argc, i);
            usage(argv[0]);
            exit(-1);
        }
        
        param = argv[i];
        xxx = iOsalAtoi(argv[i+1]);
        filename = argv[i+2];

        in = fopen(filename, "rb");
        if (in == NULL)
        {
            printf("@err: can't open `%s'\n", filename);
            exit(1);
        }
        
        if (strcmp(param, "-s") == 0) {
            while (fread(&ch, 1, 1, in) > 0)
            {
                fwrite(&ch, 1, 1, out);
                totalLen++;
                thisLen++;
            }
            
            if (xxx < thisLen)
            {
                printf("@err: `%s' exceed %d\n", filename, xxx);
                exit(1);
            }
            
            ch = fill_char;
            while (thisLen < xxx)
            {
                fwrite(&ch, 1, 1, out);
                totalLen++;
                thisLen++;
            }
            
        } else if (strcmp(param, "-b") == 0) {
            if (totalLen > xxx)
            {
                printf("@err: `%s' current size exceed %d (%#x)\n", filename, xxx, xxx);
                exit(1);
            }
            
            ch = fill_char;
            while (totalLen < xxx)
            {
                fwrite(&ch, 1, 1, out);
                totalLen++;
            }
            
            while (fread(&ch, 1, 1, in) > 0)
            {
                fwrite(&ch, 1, 1, out);
                totalLen++;
            }
            
        } else {
            printf("@err: bad param '%s'\n", param);
            exit(-1);
        }
        
        i += 3;
        fclose(in);
        if ((argc - i) == 1)
            break;
    }
    
    fclose(out);
    filename = argv[argc-1];
    if (iOsalIsFileExist(filename))
    {
        rc = unlink(filename);
        assert(rc == 0);
     }
    
    my_rename(tmp_file, filename);
    //assert(rc == 0);
    
    return 0;
}

