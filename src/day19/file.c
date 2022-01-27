#include "file.h"

#include <string.h>

#include "memory.h"
int** getFATInfo() {
    static int* fat;
    return &fat;
}
void initFATInfo() {
    int** fat = getFATInfo();
    (*fat) = memoryAlloc4K(4 * 2880);
    unsigned char* img = (ADR_DISKIMG + 0x000200);
    for (int i = 0, j = 0; i < 2880; j += 3, i += 2) {
        (*fat)[i] = (img[j] | img[j + 1] << 8) & 0xfff;
        (*fat)[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
    }
}
int getFileInfo(char* filename, FileInfo* finfo) {
    FileInfo* info = (FileInfo*)(ADR_DISKIMG + 0x002600);
    int len = strlen(filename);
    int index = 0;
    char name[10];
    for (; index < len; ++index)
        if (filename[index] == '.') break;
    for (int i = 0; i < index; ++i) name[i] = filename[i] - 'a' + 'A';
    name[index] = '\0';
    for (int i = 0; i < 224; ++i) {
        if (info[i].name[0] == '\0') break;
        if (info[i].name[0] != 0xe5) {
            if ((info[i].type & 0x18) == 0) {
                if (strncmp(name, info[i].name, index) == 0) {
                    *finfo = info[i];
                    return 1;
                }
            }
        }
    }
    return 0;
}

void getFileList(FileInfo* buf, unsigned* size) {
    FileInfo* info = (FileInfo*)(ADR_DISKIMG + 0x002600);
    (*size) = 0;
    for (int i = 0; i < 224; ++i) {
        if (info[i].name[0] == '\0') break;
        if (info[i].name[0] != 0xe5) {
            if ((info[i].type & 0x18) == 0) {
                buf[*size] = info[i];
                ++(*size);
            }
        }
    }
}
void getFileContent(char* filename, char* buf, unsigned* size) {
    FileInfo info;
    if (!getFileInfo(filename, &info)) {
        buf = NULL;
        return;
    }
    *size = info.size;
    int tmpSize = info.size, i = 0, **fat = getFATInfo();
    int clustno = info.clustno;
    char* img = (char*)(ADR_DISKIMG + 0x003e00);
    for (;;) {
        if (tmpSize <= 512) {
            for (int i = 0; i < tmpSize; ++i) buf[i] = img[clustno * 512 + i];
            break;
        }
        for (int i = 0; i < 512; ++i) buf[i] = img[clustno * 512 + i];
        tmpSize -= 512;
        buf += 512;
        clustno = (*fat)[clustno];
    }
}