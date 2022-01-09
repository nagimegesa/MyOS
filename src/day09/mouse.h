#ifndef __MOUSE_H__
#define __MOUSE_H__

struct Screen;
struct Mouse;
struct MouseBuf;
struct MouseStat;

struct Mouse* getMouse();

// 初始化鼠标
void initMouse(struct Mouse* mouse, struct Screen* screen);

// 绘制鼠标
void drawMouse(struct Mouse* mouse);

void initMouseDevice();

struct MouseStat* getMouseStat();

struct MouseBuf* getMouseBuf();
void mouseBufInit(struct MouseBuf* MouseBuf);
const char isMouseBufEmpty(struct MouseBuf* buf);
const char isMouseBufMax(struct MouseBuf* buf);
const char mouseBufPush(struct MouseBuf* buf, const unsigned char key);
const unsigned char mouseBufPop(struct MouseBuf* buf);
const int getMouseBufLen(struct MouseBuf* buf);
void changeMouseStat(struct MouseBuf* buf, struct Screen screen);
void dealMouseStatChange(struct Screen screen);

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
