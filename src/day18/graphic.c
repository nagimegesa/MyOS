#include "graphic.h"

#include "BaseFunction.h"
#include "MyFont.h"
#include "memory.h"
#include "mouse.h"

typedef SheetManger Manger;

#define REFRESH_MAP 0
#define REFRESH_SUB 1

static void sheetRefreshMap(int x, int y, unsigned wide, unsigned high,
                            int height);

static void sheetRefreshImpl(int x, int y, unsigned wide, unsigned high,
                             int height, char tag);
// 初始化调色板
void initPalette(void) {
    static unsigned char table_rgb[16 * 3] = {
        0x00, 0x00, 0x00, /*  0:黑 */
        0xff, 0x00, 0x00, /*  1:亮红 */
        0x00, 0xff, 0x00, /*  2:亮绿 */
        0xff, 0xff, 0x00, /*  3:亮黄 */
        0x00, 0x00, 0xff, /*  4:亮蓝 */
        0xff, 0x00, 0xff, /*  5:亮紫 */
        0x00, 0xff, 0xff, /*  6:浅亮蓝 */
        0xff, 0xff, 0xff, /*  7:白 */
        0xc6, 0xc6, 0xc6, /*  8:亮灰 */
        0x84, 0x00, 0x00, /*  9:暗红 */
        0x00, 0x84, 0x00, /* 10:暗绿 */
        0x84, 0x84, 0x00, /* 11:暗黄 */
        0x00, 0x00, 0x84, /* 12:暗青 */
        0x84, 0x00, 0x84, /* 13:暗紫 */
        0x00, 0x84, 0x84, /* 14:浅暗蓝 */
        0x84, 0x84, 0x84  /* 15:暗灰 */
    };
    setPalette(0, 15, table_rgb);
    return;
}

// 设置调色板颜色
void setPalette(int start, int end, unsigned char *rgb) {
    int eflags = getLockTag(); /* 记录中断许可标志的值*/
    setLock();                 /* 将中断许可标志置为0，禁止中断 */
    writePort8(0x03c8, start);
    for (int i = start; i <= end; i++) {
        writePort8(0x03c9, rgb[0] / 4);
        writePort8(0x03c9, rgb[1] / 4);
        writePort8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    setLockTag(eflags); /* 复原中断许可标志 */
    return;
}

void drawRect(Sheet *sheet, const unsigned x, const unsigned y,
              const unsigned wide, const unsigned high,
              const unsigned char COLOR) {
    unsigned char *addr = sheet->buf;
    const unsigned xSize = sheet->info.wide;
    int x1 = wide + x, y1 = y + high;
    x1 = x1 <= sheet->info.wide ? x1 : sheet->info.wide;
    y1 = y1 <= sheet->info.high ? y1 : sheet->info.high;
    for (int i = y; i <= y1; ++i) {
        for (int j = x; j <= x1; ++j) {
            addr[i * xSize + j] = COLOR;
        }
    }
    return;
}

Sheet *initDesktop() {
    Manger *manger = getSheetManger();
    Sheet *desktop = getEmptySheet();
    SheetInfo info;
    info.height = 0;
    info.wide = manger->wide;
    info.high = manger->high;
    info.posx = 0;
    info.posy = 0;
    char *buf = memoryAlloc(manger->high * manger->wide);
    emptySheetInit(desktop, buf, info);
    drawRect(desktop, 0, 0, manger->wide, manger->high, DESKTOP_COLOR);
    return desktop;
}

// 在x, y位置放置一个字符 ch
void putChar(const char ch, Sheet *sheet, const int x, const int y,
             const char color) {
    unsigned char *addr = sheet->buf;
    const unsigned xsize = sheet->info.wide;
    const char *model = __FontModel + 16 * ch;
    for (int i = 0; i < 16; i++) {
        const char d = model[i];
        char *p = addr + (y + i) * xsize + x;
        if ((d & 0x80) != 0) p[0] = color;
        if ((d & 0x40) != 0) p[1] = color;
        if ((d & 0x20) != 0) p[2] = color;
        if ((d & 0x10) != 0) p[3] = color;
        if ((d & 0x08) != 0) p[4] = color;
        if ((d & 0x04) != 0) p[5] = color;
        if ((d & 0x02) != 0) p[6] = color;
        if ((d & 0x01) != 0) p[7] = color;
    }
    sheetRefreshSub(x, y, FONT_WIDE, FONT_HIGH, sheet->info.height);
    return;
}

void putString(const char *str, Sheet *sheet, int x, int y, const char color) {
    for (; *str != '\0'; ++str) {
        putChar(*str, sheet, x, y, color);
        x += 8;
    }
}

void sheetMangerInit(Manger *manger) {
    manger->wide = *(short *)0x0ff4;
    manger->high = *(short *)0x0ff6;
    manger->vram = (char *)(*(int *)0x0ff8);
    manger->sheetMap = memoryAlloc4K(manger->wide * manger->high);
    manger->cnt = 0;
    manger->area = manger->wide * manger->high;
    for (int i = 0; i < SHEET_MAX_CNT; ++i) manger->sheet[i].tag = SHEET_UNUSE;
}

SheetManger *getSheetManger() {
    static SheetManger manger;
    return &manger;
}

Sheet *getEmptySheet() {
    Manger *manger = getSheetManger();
    for (int i = 0; i < SHEET_MAX_CNT; ++i)
        if (manger->sheet[i].tag == SHEET_UNUSE) {
            manger->sheet[i].tag = SHEET_USING;
            manger->sheet[i].info.height = SHEET_HIDDEN;
            manger->pSheet[manger->cnt++] = &manger->sheet[i];
            return &manger->sheet[i];
        }
    return NULL;
}

void emptySheetInit(Sheet *sheet, unsigned char *buf, SheetInfo info) {
    sheet->buf = buf;
    sheet->info = info;
    sheet->isTop = SHEET_NOT_TOP;
}

int sheetChangeHeight(Sheet *sheet, int height) {
    Manger *manger = getSheetManger();
    if (height == SHEET_TOP) {
        sheet->isTop = SHEET_IS_TOP;
        manger->pSheet[manger->cnt - 1]->isTop = SHEET_NOT_TOP;
    }
    height = height < SHEET_HEIGHT_MIN ? SHEET_HIDDEN : height;
    height = height > manger->cnt - 1 ? manger->cnt - 1 : height;
    sheet->info.height = height;
    for (int i = 0; i < manger->cnt; ++i) {
        int height = SHEET_HEIGHT_MAX + 1;
        int index = SHEET_MAX_CNT - 1;
        for (int j = i; j < manger->cnt; ++j) {
            int tmp = manger->pSheet[j]->info.height;
            index = tmp < height ? j : index;
            height = tmp < height ? tmp : height;
        }
        Sheet *tmp = manger->pSheet[index];
        manger->pSheet[index] = manger->pSheet[i];
        manger->pSheet[i] = tmp;
    }
    for (int i = 0; i < manger->cnt - 1; ++i)
        if (manger->pSheet[i]->info.height > 0)
            manger->pSheet[i]->info.height = i;
    sheetRefresh(sheet, sheet->info.posx, sheet->info.posy, sheet->info.wide,
                 sheet->info.high);
    return height;
}

void sheetMove(Sheet *sheet, int x, int y) {
    int oldx = sheet->info.posx, oldy = sheet->info.posy;
    sheet->info.posx = x, sheet->info.posy = y;
    if (sheet->info.height > 0) {
        sheetRefreshMap(oldx, oldy, sheet->info.wide, sheet->info.high, 0);
        sheetRefreshMap(x, y, sheet->info.wide, sheet->info.high,
                        sheet->info.height);
        sheetRefreshSub(oldx, oldy, sheet->info.wide, sheet->info.high, 0);
        sheetRefreshSub(x, y, sheet->info.wide, sheet->info.high,
                        sheet->info.height);
    }
}

void sheetFree(Sheet *sheet) {
    Manger *manger = getSheetManger();
    int index = 0;
    for (; index < manger->cnt; ++index)
        if (manger->pSheet[index] == sheet) break;
    manger->pSheet[index]->tag = SHEET_UNUSE;
    manger->pSheet[index]->info.height = SHEET_HIDDEN;
    --manger->cnt;
    for (int i = index; i < manger->cnt; ++i)
        manger->pSheet[index] = manger->pSheet[index + 1];
}

void sheetRefresh(Sheet *sheet, int x, int y, int wide, int high) {
    if (sheet->info.height != SHEET_HIDDEN) {
        sheetRefreshMap(sheet->info.posx + x, sheet->info.posy + y, wide, high,
                        sheet->info.height);
        sheetRefreshSub(sheet->info.posx + x, sheet->info.posy + y, wide, high,
                        sheet->info.height);
    }
}

void sheetRefreshSub(int x, int y, unsigned wide, unsigned high, int height) {
    sheetRefreshImpl(x, y, wide, high, height, REFRESH_SUB);
}
Sheet *getTopSheet() {
    Manger *manger = getSheetManger();
    return manger->pSheet[manger->cnt - 2];
}
static void sheetRefreshMap(int x, int y, unsigned wide, unsigned high,
                            int height) {
    sheetRefreshImpl(x, y, wide, high, height, REFRESH_MAP);
}

static void sheetRefreshImpl(int x, int y, unsigned wide, unsigned high,
                             int height, char tag) {
    Manger *manger = getSheetManger();
    const unsigned screenWide = manger->wide;
    char *addr = manger->vram;
    int startIndex = 0;
    for (startIndex = 0; startIndex < manger->cnt; ++startIndex)
        if (manger->pSheet[startIndex]->info.height >= height) break;
    for (int k = startIndex; k < manger->cnt; ++k) {
        Sheet *sheet = manger->pSheet[k];
        if (sheet->info.height != SHEET_HIDDEN) {
            int x1 = sheet->info.posx;
            int y1 = sheet->info.posy;
            const unsigned wide1 = manger->pSheet[k]->info.wide;
            const unsigned high1 = manger->pSheet[k]->info.high;
            int x2 = x1 + wide1 < x + wide ? x1 + wide1 : x + wide;
            int y2 = y1 + high1 < y + high ? y1 + high1 : y + high;
            int x3 = x1 < x ? x : x1;
            int y3 = y1 < y ? y : y1;
            x2 = x2 < 0 ? 0 : x2;
            x2 = x2 < screenWide ? x2 : screenWide;
            y2 = y2 < 0 ? 0 : y2;
            y2 = y2 < manger->high ? y2 : manger->high;
            x3 = x3 < 0 ? 0 : x3;
            x3 = x3 < screenWide ? x3 : screenWide;
            y3 = y3 < 0 ? 0 : y3;
            y3 = y3 < manger->high ? y3 : manger->high;
            const unsigned char sheetIndex = sheet - manger->sheet;
            for (int i = y3; i < y2; ++i) {
                int x4 = i - y1;
                for (int j = x3; j < x2; ++j) {
                    int pos = screenWide * i + j;
                    int tmp = x4 * wide1 + j - x1;
                    if (tag == REFRESH_MAP && sheet->buf[tmp] != COLOR_NONE)
                        manger->sheetMap[pos] = sheetIndex;
                    else if (sheetIndex == manger->sheetMap[pos])
                        addr[screenWide * i + j] = sheet->buf[tmp];
                }
            }
        }
    }
}
