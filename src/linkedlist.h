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
linklist_t* newLinkedList(unsigned int itemSize);
void deleteLinkedList(linklist_t* list);

// Just fills out the struct. No allocations
linklist_t initLinkedList(unsigned int itemSize);
void clearLinkedList(linklist_t* list);


// Returns the associated data
void* accessLinkedItem(linkitem_t* item);

// Push mem to back of list
linkitem_t* pushLinkedItem(linklist_t* list, void* mem); 
// Pop item off back of list
void popLinkedItem(linklist_t* list);

// Inserts mem after the item in the list
//linkitem_t* insertLinkedItemAfter(linklist_t* list, linkitem_t* item, void* mem);
// Inserts mem before the item in the list
//linkitem_t* insertLinkedItemBefore(linklist_t* list, linkitem_t* item, void* mem);

// Copies the contents of a linked list into an array. Size is the size of the destination. Returns last written to
void* copyLinkedListInto(void* dest, linklist_t* src, size_t size);