#ifndef buddy_cls
#define buddy_cls

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #include<headers.h>

// BuddyNode structure
struct BuddyNode
{
    int size;
    int isFree;
    int start;
    int ending;
    int processNumber;
    struct BuddyNode *left;
    struct BuddyNode *right;
    struct BuddyNode *parent;
};

// Create a new BuddyNode
struct BuddyNode *newBuddyNode(int size, int isFree, int start, int ending)
{
    struct BuddyNode *BuddyNode = (struct BuddyNode *)malloc(sizeof(struct BuddyNode));
    BuddyNode->processNumber = -1;
    BuddyNode->size = size;
    BuddyNode->isFree = isFree;
    BuddyNode->start = start;
    BuddyNode->ending = ending;
    BuddyNode->left = NULL;
    BuddyNode->right = NULL;
    BuddyNode->parent = NULL;
    return BuddyNode;
}

// Split a BuddyNode into two buddies with the half size of parent
void splitBuddyNode(struct BuddyNode *BuddyNode)
{
    int halfSize = BuddyNode->size / 2;
    int mid = BuddyNode->start + halfSize;
    BuddyNode->left = newBuddyNode(halfSize, 1, BuddyNode->start, mid - 1);
    BuddyNode->left->parent = BuddyNode;
    BuddyNode->right = newBuddyNode(halfSize, 1, mid, BuddyNode->ending);
    BuddyNode->right->parent = BuddyNode;
}

// Find the smallest BuddyNode in the binary tree that can fit a block of the given size
// The returned size must be of multiple 2
struct BuddyNode *findBuddyNode(struct BuddyNode *BuddyNode, int size)
{
    int powerOfTwo = (int)pow(2, ceil(log2(size)));
    size = powerOfTwo;

    if (BuddyNode == NULL || BuddyNode->size < size || !BuddyNode->isFree)
    {
        return NULL;
    }

    if (BuddyNode->size == size && BuddyNode->isFree == 1 && BuddyNode->left == NULL)
    {
        BuddyNode->isFree = 0;
        return BuddyNode;
    }

    if (BuddyNode->left == NULL && BuddyNode->right == NULL && BuddyNode->size > size && BuddyNode->isFree == 1)
    {
        splitBuddyNode(BuddyNode);
    }

    if (BuddyNode->left == NULL && BuddyNode->right == NULL)
    {
        return NULL;
    }

    struct BuddyNode *leftBuddyNode = findBuddyNode(BuddyNode->left, size);
    if (leftBuddyNode != NULL)
    {
        return leftBuddyNode;
    }
    return findBuddyNode(BuddyNode->right, size);
}

// Merge two buddies into a parent BuddyNode
void mergeBuddyNodes(struct BuddyNode *BuddyNode)
{
    BuddyNode->left->parent = NULL;
    BuddyNode->right->parent = NULL;
    BuddyNode->left = NULL;
    BuddyNode->right = NULL;
    if (BuddyNode->parent == NULL)
    {
        return;
    }
    free(BuddyNode->left);
    free(BuddyNode->right);
    BuddyNode->isFree = 1;
   
    if (BuddyNode->parent->left->isFree == 1 && BuddyNode->parent->right->isFree == 1)
        mergeBuddyNodes(BuddyNode->parent);
}

// Free a block of memory and merge its buddies if they are also free
void freeBuddyNode(struct BuddyNode *BuddyNode)
{
    BuddyNode->isFree = 1;
    if (BuddyNode->parent == NULL)
    {
        return;
    }
    if (BuddyNode->parent->left->isFree == 1 && BuddyNode->parent->right->isFree == 1)
    {
        mergeBuddyNodes(BuddyNode->parent);
    }
}
#endif