#include <stdio.h>
#include <stdlib.h>

typedef struct LinkedListNode
{
    int start; // start of the slot
    int end;   // end of the slot
    int procNumber;
    struct LinkedListNode *next;
} LinkedListNode;

typedef struct LinkedList
{
    LinkedListNode *head;
} LinkedList;

void insert(LinkedList *list, int start, int end, int procNumber)
{
    LinkedListNode *newLinkedListNode = (LinkedListNode *)malloc(sizeof(LinkedListNode));
    newLinkedListNode->start = start;
    newLinkedListNode->end = end;
    newLinkedListNode->procNumber = procNumber;
    newLinkedListNode->next = NULL;

    if (list->head == NULL)
    {
        list->head = newLinkedListNode;
    }
    else if (start < list->head->start)
    {
        newLinkedListNode->next = list->head;
        list->head = newLinkedListNode;
    }
    else
    {
        LinkedListNode *current = list->head;
        while (current->next != NULL && start > current->next->start)
        {
            current = current->next;
        }
        newLinkedListNode->next = current->next;
        current->next = newLinkedListNode;
    }
}
// search by start value
LinkedListNode *searchStart(LinkedList *list, int start)
{
    LinkedListNode *current = list->head;
    while (current != NULL)
    {
        if (current->start == start)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
// search by end value
LinkedListNode *searchEnd(LinkedList *list, int end)
{
    LinkedListNode *current = list->head;
    while (current != NULL)
    {
        if (current->end == end)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
// search by process number value
LinkedListNode *searchProcessNumber(LinkedList *list, int pNum)
{
    LinkedListNode *current = list->head;
    while (current != NULL)
    {
        if (current->procNumber == pNum)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
/*
hole
search for the first fit
take the needed size(whethear equal or greater)
take start and create LinkedListNode to add to process
*/
void deleteLinkedListNode(LinkedList *list, LinkedListNode *LinkedListNodeToDelete)
{
    if (LinkedListNodeToDelete == NULL)
    {
        return;
    }

    // Case 1: LinkedListNode to delete is the head of the list
    if (LinkedListNodeToDelete == list->head)
    {
        list->head = LinkedListNodeToDelete->next;
        free(LinkedListNodeToDelete);
        return;
    }
    // Case 2: LinkedListNode to delete is in the middle of the list
    LinkedListNode *current = list->head;
    while (current != NULL && current->next != LinkedListNodeToDelete)
    {
        current = current->next;
    }
    if (current != NULL)
    {
        current->next = LinkedListNodeToDelete->next;
        free(LinkedListNodeToDelete);
        return;
    }
    // Case 3: LinkedListNode to delete is not in the list
    return;
}

LinkedListNode *searchBySize(LinkedList *list, int size)
{
    LinkedListNode *current = list->head;
    while (current != NULL)
    {
        if (current->end - current->start + 1 >= size)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

LinkedListNode temp2; // variable to hold the node as it's a pointer and any change affects it
LinkedListNode *holeAllocation(LinkedList *list, int size)
{
    LinkedListNode *temp = searchBySize(list, size);
    if (temp == NULL)
    {
        temp2.start = temp2.end = temp2.procNumber = -1;
        return &temp2;
    }
    int theStart = temp->start;
    int theEnd = temp->end;
    temp2.start = theStart;
    temp2.end = theStart - 1 + size;
    temp2.procNumber = -1;
    if (temp2.end == theEnd)
    {
        deleteLinkedListNode(list, temp);
    }
    else
    {
        temp->start = temp2.end + 1;
    }

    return &temp2;
}
// freeing the process from the process list as it's finished and add it to the holeList
// it merges the holes if on its left or its right or both are holes
void holeFree(LinkedList *list, LinkedListNode *proc)
{
    proc->procNumber = -1;
    LinkedListNode *tempStart = searchStart(list, proc->end + 1);
    LinkedListNode *tempEnd = searchEnd(list, proc->start - 1);
    if (tempStart == NULL && tempEnd == NULL)
        insert(list, proc->start, proc->end, proc->procNumber);
    else if (tempStart == NULL && tempEnd != NULL)
    {
        tempEnd->end = proc->end;
    }
    else if (tempStart != NULL && tempEnd == NULL)
    {
        tempStart->start = proc->start;
    }
    else
    {
        tempEnd->end = tempStart->end;
        deleteLinkedListNode(list, tempStart);
    }
}
// freeing the list after its mission ends
void freeList(LinkedList *list)
{
    LinkedListNode *current = list->head;
    while (current != NULL)
    {
        LinkedListNode *next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
}
