#ifndef _LIB_KERNEL_LIST_H_
#define _LIB_KERNEL_LIST_H_
#include "global.h"

#define offset(struct_type, member) (int)(&((struct_type *)0)->member)
#define elem2entry(struct_type, struct_member_name, elem_ptr) \
    (struct_type *)((int)elem_ptr - offset(struct_type, struct_member_name))

typedef struct ListElem
{
    struct ListElem *next;
    struct ListElem *prev;
} ListElem;

typedef struct List
{
    ListElem head;
    ListElem tail;
} List;

typedef bool(function)(ListElem *, int arg);

void list_init(List *list);
void list_insert_before(ListElem *before, ListElem *elem);
void list_push(List *plist, ListElem *elem);
void list_iterate(List *plist);
void list_append(List *plist, ListElem *elem);
void list_remove(ListElem *elem);
ListElem *list_pop(List *plist);
bool list_empty(List *plist);
uint32_t list_len(List *plist);
ListElem *list_traveral(List *plist, function func, int arg);
bool elem_find(List *plist, ListElem *obj_elem);
#endif
