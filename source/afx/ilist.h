/*==== Public interface for fixed-length list ADT ============================*/

#ifndef __ILIST_H__
#define __ILIST_H__

#include <stdint.h>

/* Structure definitions */

typedef struct INode_ {

    void *data;

} INode;

typedef struct IList_ {

    uint64_t size;

    INode **array;

    void(*drop_node)(void *);

} IList;

/* Operation prototypes (read source file) */

void fl_init(IList *list, void(*drop_node)(void *data));

void fl_drop(IList *list);

int fl_insert(IList *list, const void *data);

int fl_remove(IList *list, void **data);

void *fl_at(IList *list, uint64_t index);

/* Compiler symbols */

#define fl_data(node) ((node)->data)

#define fl_size(list) ((list)->size)

#define fl_head(list) ((list)->array[0])

#define fl_tail(list) ((list)->array[(list)->size - 1])

#define fl_empty(list) ((list)->size == 0)

#define fl_array(list) ((list)->array)

#endif // !__FLLIST_H__