#include <stdlib.h>
#include <string.h>

#include "afx/ilist.h"

/*
 * Initialize list structure.
 *
 * No error checks, drop_node points to a function for freeing a node's data.
 * If drop_node is null, the data is not freed upon calling fl_drop.
 */
void fl_init(IList * list, void(*drop_node)(void *)) {

    list->size = 0;

    list->array = NULL;

    list->drop_node = drop_node;

    return;
}

/*
 * Destroy all data in the list.
 *
 * No error checks, region of list is zeroed with memset.
 */
void fl_drop(IList * list) {

    void *data;

    while (list->size > 0) {

        if (fl_remove(list, &data) == 0 && list->drop_node != NULL)
            list->drop_node(data);
    }

    memset(list, 0, sizeof(IList));

    return;
}

/*
 * Insert new node on list.
 *
 * data holds the data to be stored in the new node.
 * The node is always inserted at the end of the list.
 * Exits on error if no space can be allocated for newNode.
 */
int fl_insert(IList * list, const void * data) {

    INode *newNode;

    if ((newNode = malloc(sizeof(*newNode))) == NULL)
        return -1;

    newNode->data = (void *)data;

    list->array = realloc(list->array, (list->size + 1) * sizeof(*list->array));

    list->array[list->size] = newNode;

    list->size++;

    return 0;
}

/*
 * Remove node from list.
 *
 * data will point to the data of the removed node.
 * The node is always removed from the end of the list.
 * Exits on error is list list is empty.
 */
int fl_remove(IList * list, void ** data) {

    INode *tail;

    if (list->size == 0)
        return -1;

    tail = list->array[list->size - 1];

    *data = tail->data;

    free(tail);

    list->array = realloc(list->array, (list->size - 1) * sizeof(*list->array));

    list->size--;

    return 0;
}

/*
 * Get data from indexed node.
 *
 * index adresses the target node.
 * Exits on error if index is out of bounds.
 */
void * fl_at(IList * list, uint64_t index) {

    // Index cannot be negative ;D
    if (index >= list->size)
        return NULL;

    return list->array[index]->data;
}