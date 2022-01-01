#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>


linklist_t ll_create(unsigned int itemSize)
{
    return (linklist_t){.first = 0, .last = 0, .count = 0, .itemSize = itemSize};
}
void ll_clear(linklist_t* list)
{
    linkitem_t* next = 0;
    for(linkitem_t* item = list->first; item; item = next)
    {
        next = item->next;
        free(item);
    }

    list->first = 0;
    list->last = 0;
    list->count = 0;
}

linklist_t* ll_createNew(unsigned int itemSize)
{
    linklist_t* list = (linklist_t*)malloc(sizeof(linklist_t));
    *list = ll_create(itemSize);
}

void ll_delete(linklist_t* list)
{
    ll_clear(list);

    free(list);
}

// The data is stored directly after the linkitem struct
void* ll_access(linkitem_t* item) { return (void*)((char*)item + sizeof(linkitem_t)); }
linkitem_t* ll_item(void* data)   { return (linkitem_t*)((char*)data - sizeof(linkitem_t));}


linkitem_t* ll_push(linklist_t* list, void* mem)
{
    linkitem_t* item = (linkitem_t*)malloc(sizeof(linkitem_t) + list->itemSize);
    memcpy(ll_access(item), mem, list->itemSize);

    ll_attach(list, item);

    return item;
}
void ll_pop(linklist_t* list)
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

void ll_popfrompushto(linklist_t* to, linklist_t* from, linkitem_t* item)
{
    ll_detatch(from, item);
    ll_attach(to, item);
}

void ll_attachlist(linklist_t* to, linklist_t* from)
{
    
    linkitem_t* tail = 0;
    linkitem_t* tip = 0;

    // Get Tip
    if(from->first)
        tip = from->first;
    else
        return; // No Tip? No attach!

    // Get tail
    if(to->last)
        tail = to->last;
    else
    {
        // No Tail? Whole list move
        *to = *from;

        // Zero it 
        from->count = 0;
        from->first = 0;
        from->last = 0;
        return;
    }

    // Attach tip to tail
    tail->next = tip;
    tip->last = tail;
    to->last = from->last;
    to->count += from->count;

    // Zero it. We sucked its soul already
    from->count = 0;
    from->first = 0;
    from->last = 0;
}

//linkitem_t* insertLinkedItemAfter(linklist_t* list, linkitem_t* item, void* mem);
//linkitem_t* insertLinkedItemBefore(linklist_t* list, linkitem_t* item, void* mem);

void* ll_copyInto(void* dest, linklist_t* src, size_t size)
{
    char* out = dest;
    char* end = out + size;
    unsigned int itemSize = src->itemSize;
    for(linkitem_t* item = src->first; item; item = item->next)
    {
        if(out + itemSize > end) break;
        
        memcpy(out, ll_access(item), itemSize);
        
        out += itemSize;
    }

    return out;
}

void ll_attach(linklist_t* list, linkitem_t* item)
{
    item->last = list->last;
    item->next = 0;
    if(list->last)
        list->last->next = item;
    if(!list->first)
        list->first = item;
    list->last = item;
    list->count++;
}

void ll_detatch(linklist_t* list, linkitem_t* item)
{
    // Zip ourself out of the chain
    if(item->last)
        item->last->next = item->next;
    if(item->next)
        item->next->last = item->last;
        
    // Remove ourself from the head or tail of the list
    if(list->last == item)
        list->last = item->last;
    if(list->first == item)
        list->first = item->next;
    list->count--;
}


void ll_remove(linklist_t* list, linkitem_t* item)
{
    ll_detatch(list, item);

    // Free
    free(item);
}