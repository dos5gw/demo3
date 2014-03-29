/**
 ** Copyright (c) Inspur Group Co., Ltd. Unpublished
 **
 ** Inspur Group Co., Ltd.
 ** Proprietary & Confidential
 **
 ** This source code and the algorithms implemented therein constitute
 ** confidential information and may comprise trade secrets of Inspur
 ** or its associates, and any use thereof is subject to the terms and
 ** conditions of the Non-Disclosure Agreement pursuant to which this
 ** source code was originally received.
 **/

/******************************************************************************
DESCRIPTION:
  do more jobs than iJoin

SEE ALSO:

NOTE:

TODO:
  REMOVE assert from this file !!!!!
  
******************************************************************************/

/* 
modification history 
-------------------------------------------------------------------------------
01a,31Jan2013,hehy@inspur.com           written
*/

#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

enum
{
    HLItem_type_loader = 1,
    HLItem_type_image,      // image
    HLItem_type_ubifs,
};

typedef struct
{
    unsigned long Base;
    unsigned long Real;         // real data size
    unsigned long Good;         // good block size
    unsigned long Zone;         // Zone(Partition) size
    
    unsigned char Type;
    unsigned char Reserved[15];
} HLItem_t;

typedef struct
{
    unsigned char magic[16];
    HLItem_t      array[8];
} HLCtrl_t;


static unsigned char HLCtrl_Magic[16] =
{
    "\x7FHLC"
    "\xDE\xAD\xBE\xAF"
    "\x7FRUPSNI\x7F"
};
static HLCtrl_t HLCtrl_g;


int g_param_clean = 0;
int g_param_type = 0;
int g_param_good = 0;
int g_param_zone = 0;
int g_param_base = 0;
int g_param_real = 0;

char g_param_input[128];
char g_param_output[128];

void usage(char *program)
{
    printf("usage ...\n");
    exit(-1);
}

unsigned long iOsalAtoi(const char *a)
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


int main(int argc, char *argv[])
{
    int    option_index = 0;
    char  *short_options = "hcg:z:t:i:o:b:r:";
    struct option long_options[] =
        {
        {"help",        no_argument,        NULL, 'h'},
        {"clean",       no_argument,        NULL, 'c'},
        {"type",        required_argument,  NULL, 't'},
        {"base",        required_argument,  NULL, 'b'},
        {"good",        required_argument,  NULL, 'g'},
        {"real",        required_argument,  NULL, 'r'},
        {"zone",        required_argument,  NULL, 'z'},
        {"input",       required_argument,  NULL, 'i'},
        {"output",      required_argument,  NULL, 'o'},
        };
    
    opterr = 0; /* Prevent some messages to stderr */
    while (1)
        {
        int c;
        c = getopt_long (argc, argv, short_options, long_options, NULL);

        if (c == -1)
            break;

        switch (c)
            {
            case 0:
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;
            case 'c':
                g_param_clean = 1;
                break;
            case 'g':
                g_param_good = iOsalAtoi(optarg);
                break;
            case 'b':
                g_param_base = iOsalAtoi(optarg);
                break;
            case 'r':
                g_param_real = iOsalAtoi(optarg);
                break;
            case 'z':
                g_param_zone = iOsalAtoi(optarg);
                break;
            case 'i':
                strcpy(g_param_input, optarg);
                break;
            case 'o':
                strcpy(g_param_output, optarg);
                break;
            case 't':
                if (strcmp(optarg, "LOADER") == 0)
                    g_param_type = HLItem_type_loader;
                else if (strcmp(optarg, "IMAGE") == 0)
                    g_param_type = HLItem_type_image;
                else if (strcmp(optarg, "UBIFS") == 0)
                    g_param_type = HLItem_type_ubifs;
                else
                    {
                    printf("unkown type: %s\n", optarg);
                    exit(-1);
                    }
                break;
                
            case '?':
            default:
                //printf ("?? getopt returned character code 0x%x, %d, %s ??\n", c, optind, optarg);
                usage(argv[0]);
                exit(1);
            }
        }

    if (optind != argc)
        {
        printf("@err: Non-option arguments on the command line (%d)!\n", optind, argc);
        usage(argv[0]);
        exit(1);
        }
        
    if (g_param_clean)
        {
        if (g_param_output[0] == '\0')
            usage(argv[0]);
        
        FILE *fp;
        fp = fopen(g_param_output, "wb");
        assert(fp);
        fclose(fp);
        exit(0);
        }

#define ALIGNMENT(x, a)  ( (x + (a - 1)) & ~ (a - 1) )

    FILE *ifp, *ofp;
    int   ilen, olen, rc, i;
    char *ibuf;
    
    ifp = fopen(g_param_input, "rb");
    assert(ifp);
    fseek(ifp, 0, SEEK_END);
    ilen = ftell(ifp);
    
    if (g_param_real == 0)
        g_param_real = ilen;
    if (g_param_real < ilen)
        g_param_real = ilen;
    if (g_param_real > g_param_good)
        g_param_real = g_param_good;
    
    #if 0
    printf("FOR debug, fill 0xFF to alignment\n");
    g_param_real = ALIGNMENT(ilen, 0x00100000);
    #endif
    
    rewind(ifp);
    ibuf = malloc(g_param_real);
    assert(ibuf);
    memset(ibuf, 0xFF, g_param_real);
    rc = fread(ibuf, 1, ilen, ifp);
    assert(rc == ilen);
    fclose(ifp);
    printf("[%s]: ilen=%08x, rlen=%08x\n", g_param_input, ilen, g_param_real);
    
    ofp = fopen(g_param_output, "rb");
    assert(ofp);
    fseek(ofp, 0, SEEK_END);
    olen = ftell(ofp);
    printf("olen=%08x\n", olen);
    
    if (olen >= sizeof(HLCtrl_g))
        {
        fseek(ofp, olen - sizeof(HLCtrl_g), SEEK_SET);
        fread(&HLCtrl_g, 1, sizeof(HLCtrl_g), ofp);
        printf("HLCtrl_g got\n");
        }
    fclose(ofp);
    
    ofp = fopen(g_param_output, "rb+");
    if (olen >= sizeof(HLCtrl_g))
        fseek(ofp, - sizeof(HLCtrl_g), SEEK_END);
    
    rc = fwrite(ibuf, 1, g_param_real, ofp);
    assert(rc == g_param_real);
    
    memcpy(HLCtrl_g.magic, HLCtrl_Magic, sizeof(HLCtrl_Magic));
    for (i = 0; i < sizeof(HLCtrl_g.array) / sizeof(HLCtrl_g.array[0]); i++)
        {
        if (HLCtrl_g.array[i].Real == 0)
            {
            HLCtrl_g.array[i].Base = g_param_base;
            HLCtrl_g.array[i].Good = g_param_good;
            HLCtrl_g.array[i].Zone = g_param_zone;
            HLCtrl_g.array[i].Real = g_param_real;
            
            HLCtrl_g.array[i].Type = g_param_type;
            memset(HLCtrl_g.array[i].Reserved, 0xEE, sizeof(HLCtrl_g.array[i].Reserved));
            break;
            }
        }
    
    rc = fwrite(&HLCtrl_g, 1, sizeof(HLCtrl_g), ofp);
    assert(rc == sizeof(HLCtrl_g));
    
    fclose(ofp);
    
    return 0;
}
