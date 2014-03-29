#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <malloc.h>
#include <semaphore.h>
#include "../include/demo3_typedef.h"
#include "../include/demo3_c_mem_fs.h"

/******************************************************************************/
/*                                                                            */
/*                  simple RAM File System                                    */
/*                                                                            */
/******************************************************************************/


static FS_NODE* fs_head = NULL;
static sem_t mem_fs_enques;
static sem_t mem_fs_deques;
VOID create_mem_fs_semaphore()
{
    sem_init( &mem_fs_enques,0,0);
    sem_init( &mem_fs_deques,0,0);
}

VOID destroy_mem_fs_semaphore()
{
    sem_destroy(&mem_fs_enques);  
    sem_destroy(&mem_fs_deques); 
}


/* runtime base time, and unit is millisecond (1/1000 second) */
UINT32_T mem_fs_porting_time_ms(VOID)
{	
#ifdef ST_PLATFORM
    clock_t now_clock = time_now(); // clock_t is 64bit
#else
    long now_clock = clock();
#endif
	UINT32_T time_ms = 0;
#ifdef ST_PLATFORM
	time_ms = (UINT32_T)(now_clock/(ST_GetClocksPerSecond()/1000));
#else
    time_ms = (UINT32_T)(now_clock/CLOCKS_PER_SEC/1000);
#endif
	return time_ms;
}


/******************************************************************************
**  @brief          打开文件                                                    
**	@param[in]      filename－文件名  
**	@param[in]      mode－打开方式，其值为"wb"                                                       
**	@return			DEMO_OK:成功; DEMO_ERR:失败。                             
/******************************************************************************/
UINT32_T mem_fs_porting_file_open(CONST CHAR_T *filename, CONST CHAR_T *mode)
{
	FS_NODE*  fs_list;

	if(filename == NULL) return NULL;
	
	
	if(*filename=='/')
	{
		filename++;
	}
	//dbgprt(" -->mem_fs_porting_file_open: mode = %s  file_name = %s \n",mode,filename);

	////Semapore_Wait(file_access);
	if(fs_head==NULL)
	{
		fs_head = (FS_NODE*)malloc(sizeof(FS_NODE));
		memset(fs_head,0,sizeof(FS_NODE));
	}
	fs_list = fs_head;
	while(fs_list->next_file!=NULL)
	{
		if(strcmp(filename,fs_list->file_name)==0)
		{
			if(fs_list->is_open)
			{
				////Semapore_Signal(file_access);
				dbgprt(" -->mem_fs open file %s ERR! already opened!\n",filename);
				return NULL;
			}
			else
			{
				fs_list->is_open=1;
				////Semapore_Signal(file_access);
				//dbgprt(" -->mem_fs open file %s\n",filename);
				return (UINT32_T)fs_list;
			}
		}
		fs_list = fs_list->next_file;
	}

	// new file:
	if(strcmp(mode,"rb")==0)
	{
		//printf("mem_fs_porting_file_open   READ ONLY MODE\n");
		////Semapore_Signal(file_access);
		return NULL;
	}
	fs_list->is_open=1;
	fs_list->file_name=(char*)malloc(strlen(filename)+1);
	strcpy(fs_list->file_name,filename);
	fs_list->next_file=(FS_NODE*)malloc(sizeof(FS_NODE));
	memset(fs_list->next_file,0,sizeof(FS_NODE));

	////Semapore_Signal(file_access);
	//dbgprt(" -->mem_fs open new file %s\n",filename);
	return (UINT32_T)fs_list;
}


/******************************************************************************
**  @brief          向文件写入一个数据块                                         
**	@param[in]      fd－ 打开文件返回的句柄        
**	@param[in]      buffer－要输出数据的地址
**	@param[in]      nbytes－要进行写入sizeof(char)字节的数据项的个数                                                
**	@return			DEMO_OK:成功; DEMO_ERR:失败。                            
/******************************************************************************/
INT32_T mem_fs_porting_file_write(UINT32_T fd, CONST BYTE_T *buffer, INT32_T nbytes)
{
	FS_NODE*  fs_file = (FS_NODE*)fd;
	//dbgprt(" -->mem_fs_porting_file_write: fd = 0x%x, len = %d \n",fd,nbytes);

	//Semapore_Wait(file_access);
	if(0==fs_file->is_open)
	{
		//Semapore_Signal(file_access);
		return DEMO_ERR;
	}	
	if(fs_file->file_data)
	{
		free(fs_file->file_data);
	}
	fs_file->file_data=(unsigned char*)malloc(nbytes);
	if(fs_file->file_data==NULL)
	{
		//Semapore_Signal(file_access);
		return 0;
	}
	memcpy(fs_file->file_data,buffer,nbytes);
	fs_file->file_size = nbytes;
	fs_file->file_pose = 0;
	//Semapore_Signal(file_access);
	return nbytes;
}

/******************************************************************************
**  @brief          关闭文件                                                    
**	@param[in]      fd－打开文件返回的句柄                                                         
**	@return			DEMO_OK:成功; DEMO_ERR:失败。                             
/******************************************************************************/
INT32_T mem_fs_porting_file_close(UINT32_T fd)
{
	FS_NODE*  fs_file = (FS_NODE*)fd;
	//dbgprt(" -->mem_fs_porting_file_close: fd = %d \n",fd);
	
	//Semapore_Wait(file_access);
	if(fs_file->is_open)
	{
		fs_file->is_open = 0;
		fs_file->file_pose = 0;
		//Semapore_Signal(file_access);
		return DEMO_OK;
	}	
	//Semapore_Signal(file_access);
	return DEMO_ERR;
}

/* log output buf */
int HAVE_MEM_FS_DEBUG = 0;
static char printbuf[512] = {0};

INT32_T mem_fs_porting_dprintf(CONST CHAR_T *fmt, ...)
#if 1
{
	if(HAVE_MEM_FS_DEBUG == 1)
	{
		if(fmt == NULL) 
		{
			return -1;
		}
		va_list args;
		va_start(args, fmt);
		vsnprintf(printbuf,500, fmt, args);
		va_end(args);
		dbgprt(printbuf);
	}
	return 0;
}
#else
{
	
	if(HAVE_MEM_FS_DEBUG == 1)
	{
    	if(fmt == NULL) 
    	{
        	return -1;
    	}
    	va_list ap;
    	va_start(ap, fmt);
    	Selfvsprintf(printbuf,(char *)fmt, ap);
    	va_end(ap);	
    	dbgprt("%s",printbuf);	
	}
	return 0;
}
#endif
	
