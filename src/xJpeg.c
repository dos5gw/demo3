#include <stdio.h>
#include <string.h>

static int UsbGetPictureFormat( char* pic);
static void UsbParseJpegHead( char* buffer, int length, int* pWidth, int* pHight);
static void UsbParseBmpHead(char* buffer, int length, int* pWidth, int* pHight);
static void UsbParseGifHead(char* buffer, int length, int* pWidth, int* pHight);
static void UsbParsePngHead(char* buffer, int length, int* pWidth, int* pHight);

enum PicFormat{
        PIC_FORMAT_UNKNOWN = 0,
        PIC_FORMAT_JPG,
        PIC_FORMAT_BMP,
        PIC_FORMAT_GIF,
        PIC_FORMAT_PNG
};

void test_parse_jpeg()
{

    int read_length = 0;
    int width=0, hight=0;
    char buffer[1024] = {0};
    FILE *file = NULL;
    enum PicFormat format = PIC_FORMAT_UNKNOWN;


    //char* pic = "~/TEMP/255147-1680x1050.jpg";
    //char* pic =  "~/TEMP/bitmap.bmp";
    //char* pic =  "~/TEMP/205106oe81.gif";
    char* pic = "~/TEMP/30052010060.jpg";

    
    format = (enum PicFormat)UsbGetPictureFormat(pic);
    printf("[%s] %s format %d ,sizeof(char)=%d\n",__FUNCTION__,pic,format,sizeof(char));

    file = fopen(pic,"rb");
    if( NULL == file)
    {
        printf("open pic failed\n");
        return ;
    }
    read_length = fread(buffer,sizeof(char),sizeof(buffer),file);
    printf(" read %d bytes\n",read_length);

    switch(format){
    case PIC_FORMAT_JPG:
        UsbParseJpegHead(buffer,read_length,&width,&hight);
        break;
    case PIC_FORMAT_BMP:
        UsbParseBmpHead(buffer,read_length,&width,&hight);
        break;
    case PIC_FORMAT_GIF:
        UsbParseGifHead(buffer,read_length,&width,&hight);
        break;
    case PIC_FORMAT_PNG:
        UsbParsePngHead(buffer,read_length,&width,&hight);
        break;
    default:
        printf("[%s] unknown format %d\n",__FUNCTION__,format);
    }
    
    fclose(file);    
    return;
}



/* function: parse jpeg file head
 * input:    
 * output: 
 * return:
 * */
static void UsbParseJpegHead( char* buffer, int length, int* pWidth, int* pHight)
{
    char* last = NULL;
    char* ptr = NULL;
    unsigned char mark, type = 0xff;
    int m_ret=-1,m_pos=0,m_hpx=0,m_wpx=0,uits=0;
    int height=0, weidth=0, wpx=0, hpx=0;
    int i=0;
    if((NULL ==pWidth) || (NULL ==pHight) || (0 >= length))
    {
        return;
    }
    last = buffer + length;
    ptr = buffer;
    *pWidth = 0;
    *pHight= 0;

    if((unsigned char)*buffer++!=0xff || (unsigned char)*buffer++!=0xd8)
    //if( *buffer++!=0xff || *buffer++!=0xd8 )
    {
        printf("%s: not jpeg! \n",__FUNCTION__);
        return;
    }

    printf("  --- %s: enter ! --- \n",__FUNCTION__);
    do{
        //printf("    buf[%d]=0x%x\n",i,(unsigned char)buffer[i++]);
        printf("    buf[%d]=0x%x\n",i,buffer[i++]);
    }while(i<12);

    while( last>buffer && type!=0xDA )
    {
        do   
        {  
            mark=*buffer++;  
        } while ((unsigned char)mark!=0xff);  
        printf("   find mark=0x%x\n",mark);
        do   
        {  
            type=*buffer++;  
        } while (type==0xff);  
        printf("   find type 0x%x\n\n",type);
        switch(type)  
        {  
        case 0x00:  
        case 0x01:  
        case 0xD0:  
        case 0xD1:  
        case 0xD2:  
        case 0xD3:  
        case 0xD4:  
        case 0xD5:  
        case 0xD6:  
        case 0xD7:  
            break;  
        case 0xC0://SOF0
            printf("  -->find SOF0\n");
            ptr=buffer;  
            m_pos=((*buffer++)&0xff)<<8;  
            m_pos+=(*buffer++)&0xff;  
            buffer++; // skip precision 
            height=((*buffer++)&0xff)<<8;  
            height+=(*buffer++)&0xff;  
            weidth=((*buffer++)&0xff)<<8;  
            weidth+=(*buffer)&0xff;  
            m_ret++;  
            break;  
        case 0xE0: //APP0
            printf("  -->find e0\n");
            ptr=buffer;  
            m_pos=((*buffer++)&0xff)<<8;  
            m_pos+=(*buffer++)&0xff;  
            buffer=buffer+7;        //skip APP0(5bytes),version1(1bytes) and version2(1bytes)  
            uits=(*buffer++)&0xff;  //0: none unit, units=1:inch, units=2:cm 
            m_wpx=((*buffer++)&0xff)<<8;  
            m_wpx+=(*buffer++)&0xff;  
            m_hpx=((*buffer++)&0xff)<<8;  
            m_hpx+=(*buffer++)&0xff;  
            //hpx=m_hpx*JepgSize(uits);  
            //wpx=m_wpx*JepgSize(uits);  
            m_ret++;  
            break;  
        default:  
            ptr=buffer;  
            m_pos=((*buffer++)&0xff)<<8;  
            m_pos+=(*buffer++)&0xff;  
            break;  
        }  
        buffer=ptr+m_pos;         
    }
    *pWidth = weidth;
    *pHight = height;

    printf(" %s: w=%d, h=%d\n",__FUNCTION__,weidth,height);
    return;
}
static void UsbParseBmpHead(char* buffer, int length, int* pWidth, int* pHight)
{
    if((NULL ==pWidth) || (NULL ==pHight))
    {
        return;
    }
    printf("%s: length=%d \n",__FUNCTION__,length);
    *pWidth = 0;
    *pHight= 0;
    
    memcpy(pWidth,&buffer[18],4);
    memcpy(pHight,&buffer[22],4);

    printf("%s: w=%d, h=%d\n",__FUNCTION__,*pWidth,*pHight);
    return;

}
static void UsbParseGifHead(char* buffer, int length, int* pWidth, int* pHight)
{
    //short width=0, hight=0;
    if((NULL ==pWidth) || (NULL ==pHight))
    {
        return;
    }
    printf("%s: length=%d \n",__FUNCTION__,length);

    *pWidth = 0;
    *pHight= 0;

    if(('G' !=buffer[0]) || ('I' !=buffer[1]) || ('F' !=buffer[2]))
    {
        printf("not gif\n");
        return;
    }
    
    memcpy(pWidth,&buffer[6],2); // little endian
    memcpy(pHight,&buffer[8],2);


    printf("%s: w=%d, h=%d\n",__FUNCTION__,*pWidth,*pHight);
    return;

}
static void UsbParsePngHead(char* buffer, int length, int* pWidth, int* pHight)
{
    if((NULL ==pWidth) || (NULL ==pHight))
    {
        return;
    }
    printf("%s: length=%d \n",__FUNCTION__,length);
    *pWidth = 0;
    *pHight= 0;
}

static int UsbGetPictureFormat( char* filename)
{
    int pic_name_length= 0;
    int i = 0;
    enum PicFormat format = PIC_FORMAT_UNKNOWN;
    
/*
     enum PicFormat{
        PIC_FORMAT_UNKNOWN = 0,
        PIC_FORMAT_JPG,
        PIC_FORMAT_BMP,
        PIC_FORMAT_GIF,
        PIC_FORMAT_PNG
    }
 */
    if(NULL == filename)
    {
        printf("[%s] invalid param\n",__FUNCTION__);
        return (int)format;
    }

    printf("[%s] %s enter!\n",__FUNCTION__,filename);

    if((strstr(filename,".jpg") != NULL) || (strstr(filename,".JPG") != NULL) || 
        (strstr(filename,".jpeg") != NULL) || (strstr(filename,".JPEG") != NULL))
    {
        format = PIC_FORMAT_JPG;
    }
    else if((strstr(filename,".bmp") != NULL) || (strstr(filename,".BMP") != NULL))
    {
        format = PIC_FORMAT_BMP;
    }
    else if((strstr(filename,".gif") != NULL) || (strstr(filename,".GIF") != NULL))
    {
        format = PIC_FORMAT_GIF;
    }
    else if((strstr(filename,".png") != NULL) || (strstr(filename,".PNG") != NULL))
    {
        format = PIC_FORMAT_PNG;
    }
    return (int)format;
}

