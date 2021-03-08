#include "list.h"
#include "interrupt.h"

void list_init(List *list)
{
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

void list_insert_before(ListElem *before, ListElem *elem)
{
    IntrStatus old_status = intr_disable();
    before->prev->next = elem;
    elem->next = before;
    elem->prev = before->prev;
    before->prev = elem;
    intr_set_status(old_status);
}

void list_push(List *plist, ListElem *elem)
{
    list_insert_before(plist->head.next, elem);
}

void list_append(List *plist, ListElem *elem)
{
    list_insert_before(&plist->tail, elem);
}

void list_remove(ListElem *elem)
{
    IntrStatus old_status = intr_disable();
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    intr_set_status(old_status);
}

ListElem *list_pop(List *plist)
{
    ListElem *elem = plist->head.next;
    list_remove(elem);
    return elem;
}

bool elem_find(List *plist, ListElem *obj_elem)
{
    ListElem *elem = plist->head.next;
    while (elem != &plist->tail)
    {
        if (elem == obj_elem)
        {
            return true;
        }
        elem = elem->next;
    }
    return false;
}

uint32_t list_len(List *plist)
{
    ListElem *elem = plist->head.next;
    uint32_t length = 0;
    while (elem != &plist->tail)
    {
        length++;
        elem = elem->next;
    }

    return length;
}

bool list_empty(List *plist)
{
    return (plist->head.next == &plist->tail ? true : false);
}

/* find the elem which meet the criteria of func */
ListElem *list_traveral(List *plist, function func, int arg)
{
    ListElem *elem = plist->head.next;

    if (list_empty(plist))
    {
        return NULL;
    }
    while (elem != &plist->tail)
    {
        if (func(elem, arg))
        {
            return elem;
        }
        elem = elem->next;
    }
    return NULL;
}