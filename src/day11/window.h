#ifndef __WINDOW_H__
#define __WINDOW_H__

#define WINDOW_MIN_WIDE 50
#define WINDOW_MIN_HIGH 50
#define WINDOW_MAX_WIDE 300
#define WINDOW_MAX_HIGH 300

struct Sheet;

typedef struct Window {
    struct Sheet* sheet;
} Window;

Window* makeBaseWindow(const char* title, int x, int y, unsigned wide,
                       unsigned high);

#endif