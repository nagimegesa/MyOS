#ifndef __MOUSE_H__
#define __MOUSE_H__

struct Screen;
struct Mouse;
struct MouseBuf;

struct Mouse* getMouse();

// 初始化鼠标
void initMouse(struct Mouse* mouse, struct Screen* screen);

// 绘制鼠标
void drawMouse(const struct Mouse* mouse);

void initMouseDevice();

struct MouseBuf* getMouseBuf();
void MouseBufInit(struct MouseBuf* MouseBuf);
const char isMouseBufEmpty(struct MouseBuf* buf);
const char isMouseBufMax(struct MouseBuf* buf);
void MouseBufPush(struct MouseBuf* buf, const unsigned char key);
const unsigned char MouseBufPop(struct MouseBuf* buf);

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4
#define PORT_KEYDAT 0x0060
#define PORT_KEYCMD 0x0064

#endif
