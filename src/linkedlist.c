#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>


linklist_t initLinkedList(unsigned int itemSize)
{
    return (linklist_t){.first = 0, .last = 0, .count = 0, .itemSize = itemSize};
}
void clearLinkedList(linklist_t* list)
{
    for(linkitem_t* item = list->first; item;)
    {
        linkitem_t* next = item->next;
        free(item);
        item = next;
    }

    list->first = 0;
    list->last = 0;
    list->count = 0;
}

linklist_t* newLinkedList(unsigned int itemSize)
{
    linklist_t* list = (linkitem_t*)malloc(sizeof(linklist_t));
    *list = initLinkedList(itemSize);
}

void deleteLinkedList(linklist_t* list)
{
    clearLinkedList(list);

    free(list);
}

void* accessLinkedItem(linkitem_t* item)
{
    return (void*)((char*)item + sizeof(linkitem_t));
}

linkitem_t* pushLinkedItem(linklist_t* list, void* mem)
{
    linkitem_t* item = (linkitem_t*)malloc(sizeof(linkitem_t) + list->itemSize);
    item->last = list->last;
    item->next = 0;
    if(list->last)
        list->last->next = item;
    if(!list->first)
        list->first = item;
    list->last = item;
    list->count++;
    memcpy((char*)item + sizeof(linkitem_t), mem, list->itemSize);
}
void popLinkedItem(linklist_t* list)
{
    if(!list->last)
        return;
    
    linkitem_t* last = list->last->last;
    free(list->last);
    list->last = last;
    if(last)
        last->next = 0;
    list->count--;
}

//linkitem_t* insertLinkedItemAfter(linklist_t* list, linkitem_t* item, void* mem);
//linkitem_t* insertLinkedItemBefore(linklist_t* list, linkitem_t* item, void* mem);

void* copyLinkedListInto(void* dest, linklist_t* src, size_t size)
{
    char* out = dest;
    char* end = out + size;
    unsigned int itemSize = src->itemSize;
    for(linkitem_t* item = src->first; item; item = item->next)
    {
        if(out + itemSize > end) break;
        
        memcpy(out, accessLinkedItem(item), itemSize);
        
        out += itemSize;
    }

    return out;
}