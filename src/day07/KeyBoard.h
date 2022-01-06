#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

struct KeyBoardBuf;
struct KeyBoardBuf* getKeyBuf();
void keyBoardInit();
void keyBufInit(struct KeyBoardBuf* buf);
const char isKeyBufEmpty(struct KeyBoardBuf* buf);
const char isKeyBufMax(struct KeyBoardBuf* buf);
const char keyBufPush(struct KeyBoardBuf* buf, const unsigned char key);
const unsigned char keyBufPop(struct KeyBoardBuf* buf);

#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

#endif
