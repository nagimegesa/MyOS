#ifndef __FILE_H__
#define __FILE_H__
#include "dsctbl.h"
#include "BaseFunction.h"
#define ADR_BOOTINFO 0x00000ff0
#define ADR_DISKIMG 0x00100000
#ifndef DEFINE_NULL
#define DEFINE_NULL
#define NULL ((void*)0)
#endif
typedef struct FileInfo {
    unsigned char name[8], ext[3], type;
    char reserve[10];
    unsigned short time, date, clustno;
    unsigned int size;
} FileInfo;
int** getFATInfo();
void initFATInfo();
int getFileInfo(char* filename, FileInfo* info);
void getFileList(FileInfo* buf, unsigned* size);
void getFileContent(char* filename, char* buf, unsigned* size);
#endif
