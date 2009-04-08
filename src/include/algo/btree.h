#ifndef _BTREE_H
#define _BTREE_H

typedef void* btree_keytype;
typedef void* btree_valtype;

typedef int (*btree_cmptype)(btree_keytype, btree_keytype);

struct btree_node;
struct btree
{
    btree_cmptype cmp;
    struct btree_node *head;
};

struct btree_node
{
    btree_keytype key;
    btree_valtype val;
    struct btree_node *left, *right;
};

struct btree *btree_create(btree_cmptype);

void btree_insert(struct btree*, btree_keytype, btree_valtype);
btree_valtype btree_lookup(struct btree*, btree_keytype);
void btree_delete(struct btree*, btree_keytype);
void btree_clear(struct btree*);
void btree_delete_recursivly(struct btree*, btree_keytype);

#endif
