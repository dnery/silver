/*==== Public interface for linked list ADT ==================================*/

#ifndef __LLIST_H__
#define __LLIST_H__

/* Structure definitions */

typedef struct LNode_ {

        void          *data;
        struct LNode_ *next;

} LNode;

typedef struct LList_ {

        long size;

        void (*kill)(void *);

        LNode *head;
        LNode *tail;

} LList;

/* Operation prototypes (read source file) */

void l_init(LList *, void (*kill)(void *data));

void l_drop(LList *);

int l_ins_next(LList *, LNode *, const void *);

int l_rem_next(LList *, LNode *, void **);

/* Compiler symbols */

#define l_size(list) ((list)->size)

#define l_head(list) ((list)->head)

#define l_tail(list) ((list)->tail)

#define l_isHead(list, node) ((node) == (list)->head ? 1 : 0)

#define l_isTail(list, node) ((node) == (list)->tail ? 1 : 0)

#define l_data(node) ((node)->data)

#define l_next(node) ((node)->next)

#endif // !__LIST_H__
