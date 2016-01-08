#include <stdlib.h>
#include <string.h>

#include "afx/llist.h"

/*
 * Initialize list structure.
 *
 * No error checks, kill points to a function for freeing a node's data.
 * If kill is null, the data is not freed upon calling l_drop.
 */
void l_init(LList *l, void(*kill)(void *)) {

    l->size = 0;

    l->kill = kill;

    l->head = NULL;
    l->tail = NULL;

    return;
}

/*
 * Destroy all data in the list.
 *
 * No error checks, region of l is zeroed with memset.
 */
void l_drop(LList *l) {

    void *data;

    while (l_size(l) > 0) {

        if (l_rem_next(l, NULL, &data) == 0 && l->kill != NULL)
            l->kill(data);
    }

    memset(l, 0, sizeof(LList));

    return;
}

/*
 * Insert a node succeeding another node.
 *
 * n precedes the node to be inserted, data holds data to be stored in the
 * inserted node. If n is null, the node is inserted at the head.
 * Exits on error if no space can be allocated for newNode.
 */
int l_ins_next(LList *l, LNode *n, const void *data) {

    LNode *newNode;

    if ((newNode = malloc(sizeof(*newNode))) == NULL)
        return -1;

    newNode->data = (void *)data;

    if (n == NULL) {

        if (l_size(l) == 0)
            l->tail = newNode;

        newNode->next = l->head;
        l->head = newNode;

    } else {

        if (n->next == NULL)
            l->tail = newNode;

        newNode->next = n->next;
        n->next = newNode;
    }

    l->size++;

    return 0;
}

/*
 * Remove node succeeding another node.
 *
 * n precedes the node to be removed, data will point to the data
 * of the removed node. If n is null, the node at the head is removed.
 * Exits on error if l is empty or if removal is attempted at the list tail.
 */
int l_rem_next(LList *l, LNode *n, void **data) {

    LNode *oldNode;

    if (l_size(l) == 0)
        return -1;

    if (n == NULL) {
        oldNode = l->head;
        l->head = l->head->next;

        if (l_size(l) == 1)
            l->tail = NULL;

    } else {

        if (n->next == NULL)
            return -1;

        oldNode = n->next;
        n->next = n->next->next;

        if (n->next == NULL)
            l->tail = n;
    }

    *data = oldNode->data;
    free(oldNode);

    l->size--;

    return 0;
}