
#ifndef pq_class
#define pq_class

#include "PCB.h"

typedef struct Node
{
    struct PCB pcb;
    int priority;
    struct Node *next;
} Node;

typedef struct PriorityQueue
{
    Node *head;
    int count;
} PriorityQueue;
// intiallizing the priority queue
void setpqueue(PriorityQueue *queue)
{
    queue->head = NULL;
    queue->count = 0;
}
// add to the priority queue by a specific priority
void enqueue(PriorityQueue *queue, struct PCB item, int priority)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->pcb = item;
    new_node->priority = priority;
    new_node->next = NULL;

    if (queue->head == NULL)
    {
        queue->head = new_node;
    }
    else if (priority < queue->head->priority)
    {
        new_node->next = queue->head;
        queue->head = new_node;
    }
    else
    {
        Node *current = queue->head;
        while (current->next != NULL && priority >= current->next->priority)
        {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
    queue->count++;
}
// remove the first node
struct PCB dequeue(PriorityQueue *queue)
{

    if (queue->head == NULL)
    {
        printf("Queue is empty\n");
        struct PCB item;
        item.id = -1;
        return item;
    }
    struct PCB item = queue->head->pcb;
    Node *temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
    queue->count--;
    return item;
}
// get the first node of the queue
struct PCB peek(PriorityQueue *queue)
{
    if (queue->count == 0)
    {
        printf("Queue is empty\n");
        struct PCB item;
        item.id = -1;
        return item;
    }
    struct PCB item = queue->head->pcb;
    return item;
}
// check if the priority queue is empty or not
bool isEmpty(PriorityQueue *queue)
{
    return queue->count == 0;
}
#endif