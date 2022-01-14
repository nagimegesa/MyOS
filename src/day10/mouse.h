#ifndef __MOUSE_H__
#define __MOUSE_H__
#include "buf.h"
struct FIFObuf;
struct Sheet;

typedef struct MouseBuf {
    struct FIFOBuf buf;
} MouseBuf;

typedef struct MouseStat {
    char btn;
    int offsetx, offsety;
} MouseStat;

// 初始化鼠标
struct Sheet* initMouse();

void initMouseDevice();

MouseStat* getMouseStat();

MouseBuf* getMouseBuf();
void mouseBufInit(MouseBuf* MouseBuf);
const char isMouseBufEmpty(MouseBuf* buf);
const char isMouseBufMax(MouseBuf* buf);
const char mouseBufPush(MouseBuf* buf, const unsigned char key);
const unsigned char mouseBufPop(MouseBuf* buf);
const int getMouseBufLen(MouseBuf* buf);
void changeMouseStat(MouseBuf* buf);
void dealMouseStatChange(struct Sheet* mouse);

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4
#define PORT_KEYDAT 0x0060
#define PORT_KEYCMD 0x0064

#define MOUSE_BTN_LEFT 0x01
#define MOUSE_BTN_RIGHT 0x02
#define MOUSE_BTN_MID 0x04

#define MOUSE_HIGH 16
#define MOUSE_WIDE 16
#endif
