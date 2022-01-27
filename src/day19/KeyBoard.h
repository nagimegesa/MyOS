#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "buf.h"

struct Sheet;

#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

void dealKeyBoardInterrupt(int data, struct Sheet* sheet);
#endif
