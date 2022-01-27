#ifndef __BUF_H__
#define __BUF_H__

#include "BaseFunction.h"
#include "memory.h"
#define MSG_BUF_SIZE 128

#define MSG_MOUSE_MIN 512
#define MSG_MOUSE_MAX 767
#define MSG_KEYBOARD_MIN 256
#define MSG_KEYBOARD_MAX 511
struct Task;
typedef struct FIFOBuf {
    int* buf;
    unsigned char head, tail, len, maxLen;
    struct Task* task;
} FIFOBuf;

void FIFOInit(FIFOBuf* FIFObuf, int* buf, int size, struct Task* task);
const char isFIFOEmpty(FIFOBuf* buf);
const char isFIFOMax(FIFOBuf* buf);
char FIFOPush(FIFOBuf* buf, const int key);
const int FIFOPop(FIFOBuf* buf);
const int getBufLen(FIFOBuf* buf);
FIFOBuf* getMsgBuf();
void MsgBufInit(struct Task* task);
FIFOBuf* getFIFOBuf(unsigned size, struct Task* task);
inline int isMouseMsg(int msg);
inline int isKeyBoardMsg(int msg);

#ifndef DEFINE_NULL
#define DEFINE_NULL
#define NULL ((void*)0)
#endif

// #ifndef DEFINE_SIZE_T
// #define DEFINE_SIZE_T
// typedef unsigned size_t
// #endif

#define ALLOC memoryAlloc
#define FREE memoryFree
#define Declare_List(MyData)                                                   \
    typedef struct ListNode##MyData {                                          \
        MyData data;                                                           \
        struct ListNode##MyData* next;                                         \
    } ListNode##MyData;                                                        \
                                                                               \
    typedef struct List##MyData {                                              \
        ListNode##MyData *head, *end;                                          \
        unsigned size;                                                         \
        int (*cmp)(MyData data, MyData pNode);                                 \
    } List##MyData;                                                            \
    List##MyData* getEmptyList##MyData(int (*cmp)(MyData data, MyData pNode)); \
    ListNode##MyData* ListInsert##MyData(List##MyData* list, MyData data);     \
    void ListErase##MyData(List##MyData* list, MyData data);                   \
    void ListForEach##MyData(                                                  \
        List##MyData* list,                                                    \
        void (*op)(List##MyData * list, ListNode##MyData*, MyData * node));    \
    int ListNodeHasNext##MyData(List##MyData* list, ListNode##MyData* pNode);  \
    ListNode##MyData* ListFind##MyData(List##MyData* list, MyData data);       \
    int ListHasNext##MyData(List##MyData* list, ListNode##MyData* pNode);      \
    int ListEmpty##MyData(List##MyData* list);                                 \
    ListNode##MyData* ListGetFirst##MyData(List##MyData* list);                \
    ListNode##MyData* ListGetNext##MyData(List##MyData* list,                  \
                                          ListNode##MyData* pNode);

#define Define_List(MyData)                                                   \
    List##MyData* getEmptyList##MyData(                                       \
        int (*cmp)(MyData data, MyData pNode)) {                              \
        List##MyData* list = ALLOC(sizeof(List##MyData));                     \
        if (list != NULL) {                                                   \
            list->head = ALLOC(sizeof(ListNode##MyData));                     \
            list->end = ALLOC(sizeof(ListNode##MyData));                      \
            list->cmp = cmp;                                                  \
            if (list->head != NULL && list->end != NULL) {                    \
                list->head->next = list->end;                                 \
                list->end->next = NULL;                                       \
                list->size = 0;                                               \
            }                                                                 \
        }                                                                     \
        return list;                                                          \
    }                                                                         \
    ListNode##MyData* ListInsert##MyData(List##MyData* list, MyData data) {   \
        ListNode##MyData* pNode = list->head->next;                           \
        ListNode##MyData* node = ALLOC(sizeof(ListNode##MyData));             \
        if (node == NULL) return node;                                        \
        node->data = data;                                                    \
        node->next = NULL;                                                    \
        if (pNode == list->end || list->cmp(data, pNode->data)) {             \
            node->next = pNode;                                               \
            list->head->next = node;                                          \
            ++list->size;                                                     \
            return node;                                                      \
        }                                                                     \
        while (pNode != list->end) {                                          \
            if (!list->cmp(data, pNode->data)) {                              \
                if (!list->cmp(pNode->data, data)) {                          \
                    FREE(node, sizeof(node));                                 \
                    return pNode;                                             \
                };                                                            \
                node->next = pNode->next;                                     \
                pNode->next = node;                                           \
                ++list->size;                                                 \
                return node;                                                  \
            }                                                                 \
            pNode = pNode->next;                                              \
        }                                                                     \
        return NULL;                                                          \
    }                                                                         \
    void ListErase##MyData(List##MyData* list, MyData data) {                 \
        ListNode##MyData* pNode = list->head->next;                           \
        ListNode##MyData* lastNode = pNode;                                   \
        if (!list->cmp(pNode->data, data) && !list->cmp(data, pNode->data)) { \
            list->head->next = pNode->next;                                   \
            FREE(pNode, sizeof(pNode));                                       \
            return;                                                           \
        }                                                                     \
        while (pNode != list->end) {                                          \
            if (!list->cmp(pNode->data, data) &&                              \
                !list->cmp(data, pNode->data)) {                              \
                lastNode->next = pNode->next;                                 \
                FREE(pNode, sizeof(pNode));                                   \
                return;                                                       \
            }                                                                 \
            lastNode = pNode;                                                 \
            pNode = pNode->next;                                              \
        }                                                                     \
    }                                                                         \
    void ListForEach##MyData(                                                 \
        List##MyData* list,                                                   \
        void (*op)(List##MyData * list, ListNode##MyData*, MyData * node)) {  \
        ListNode##MyData* pNode = list->head->next;                           \
        while (pNode != list->end) {                                          \
            op(list, pNode, &pNode->data);                                    \
            pNode = pNode->next;                                              \
        }                                                                     \
    }                                                                         \
    int ListHasNext##MyData(List##MyData* list, ListNode##MyData* pNode) {    \
        return !(pNode == list->end || pNode->next == list->end);             \
    }                                                                         \
    int ListEmpty##MyData(List##MyData* list) {                               \
        return list->head->next == list->end;                                 \
    }                                                                         \
    ListNode##MyData* ListGetNext##MyData(List##MyData* list,                 \
                                          ListNode##MyData* pNode) {          \
        return pNode->next;                                                   \
    }                                                                         \
    ListNode##MyData* ListFind##MyData(List##MyData* list, MyData data) {     \
        ListNode##MyData* pNode = list->head->next;                           \
        while (pNode != list->end) {                                          \
            if (!list->cmp(pNode->data, data) &&                              \
                !list->cmp(data, pNode->data))                                \
                return pNode;                                                 \
        }                                                                     \
        return NULL;                                                          \
    }                                                                         \
    ListNode##MyData* ListGetFirst##MyData(List##MyData* list) {              \
        return list->head->next == list->end ? NULL : list->head->next;       \
    }

#define ListType(MyData) List##MyData
#define ListPointer(MyData) List##MyData*
#define ListNodeType(MyData) ListNode##MyData
#define ListNodePointer(MyData) ListNode##MyData*

#define ListInsert(MyData) ListInsert##MyData
#define GetEmptyList(MyData) getEmptyList##MyData
#define ListErase(MyData) ListErase##MyData
#define ListForEach(MyData) ListForEach##MyData
#define ListFind(MyData) ListFind##MyData
#define ListHasNext(MyData) ListNodeHasNext##MyData
#define ListGetFirst(MyData) ListGetFirst##MyData
#define ListEmpty(MyData) ListEmpty##MyData
#define ListGetNext(MyData) ListGetNext##MyData

#endif