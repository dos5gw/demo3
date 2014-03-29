#ifndef _DEMO_MEM_FS_H
#define _DEMO_MEM_FS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FS_NODE
{
	char *file_name;
	unsigned char * file_data;
	char is_open;
	int  file_size;
	int  file_pose;
	struct FS_NODE* next_file;
}FS_NODE;

#ifdef __cplusplus
}
#endif

#endif
