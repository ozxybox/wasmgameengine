#pragma once
#include <stddef.h> // for size_t

// These are allocated with a size of your struct + the two pointers
// The attached data is right after the two pointers
typedef struct linkitem_t
{
    struct linkitem_t* last;  
    struct linkitem_t* next;  

} linkitem_t;


typedef struct
{
    linkitem_t* first;
    linkitem_t* last;
    unsigned int count;

    unsigned int itemSize;
} linklist_t;

// Allocates a new linked list for items of a certain size
linklist_t* ll_createNew(unsigned int itemSize);
void ll_delete(linklist_t* list);

// Just fills out the struct. No allocations
linklist_t ll_create(unsigned int itemSize);
void ll_clear(linklist_t* list);


// Returns the associated data
void* ll_access(linkitem_t* item);

// Converts data from ll_access back into an item
linkitem_t* ll_item(void* data);

// Push mem to back of list
linkitem_t* ll_push(linklist_t* list, void* mem); 
// Pop item off back of list
void ll_pop(linklist_t* list);

// Pops an item off one list and pushes it onto another
void ll_popfrompushto(linklist_t* to, linklist_t* from, linkitem_t* item); 

// Attaches an item to the back of a list
void ll_attach(linklist_t* list, linkitem_t* item);
// Detatches an item from a list
void ll_detatch(linklist_t* list, linkitem_t* item);

// Attaches a list to the back of another list
void ll_attachlist(linklist_t* to, linklist_t* from); 


// Inserts mem after the item in the list
//linkitem_t* insertLinkedItemAfter(linklist_t* list, linkitem_t* item, void* mem);
// Inserts mem before the item in the list
//linkitem_t* insertLinkedItemBefore(linklist_t* list, linkitem_t* item, void* mem);

void ll_remove(linklist_t* list, linkitem_t* item);

// Copies the contents of a linked list into an array. Size is the size of the destination. Returns last written to
void* ll_copyInto(void* dest, linklist_t* src, size_t size);