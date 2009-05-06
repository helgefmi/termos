#include <lib/stdio.h>
#include <mm/mem.h>
#include <algo/btree.h>

static struct btree_node *create_btree_node(btree_keytype key, btree_valtype val)
{
    struct btree_node *node = (struct btree_node*) kmalloc(sizeof(struct btree_node));
    node->key = key;
    node->val = val;
    node->left = node->right = 0;
    return node;
}

static struct btree_node **btree_int_traverse(struct btree_node **node, btree_keytype key, btree_cmptype cmp, struct btree_node **parent)
{
    while (*node)
    {
        int result = cmp(key, (*node)->key);

        if (result)
        {
            if (parent)
                *parent = *node;

            if (result > 0)
                node = &(*node)->right;
            else if (result < 0)
                node = &(*node)->left;
        }
        else
            break;
    }

    return node;
}

void btree_insert(struct btree *root, btree_keytype key, btree_valtype val)
{
    if (!root->head)
    {
        root->head = create_btree_node(key, val);
    }
    else
    {
        struct btree_node **match = btree_int_traverse(&root->head, key, root->cmp, 0);

        if (!*match)
            *match = create_btree_node(key, val);
        else
            PANIC("Something's terribly wrong!");
    }
}

btree_valtype btree_lookup(struct btree* root, btree_keytype key)
{
    if (!root->head)
        return 0;

    struct btree_node **match = btree_int_traverse(&root->head, key, root->cmp, 0);

    if (*match)
        return (*match)->val;

    return 0;
}

void btree_delete(struct btree *root, btree_keytype key)
{
    struct btree_node *parent = 0;
    struct btree_node **match = btree_int_traverse(&root->head, key, root->cmp, &parent);

    if (*match)
    {
        if (!(*match)->left)
        {
            struct btree_node *to_delete = (*match);
            (*match) = (*match)->right;
            kfree(to_delete);
        }
        else if (!(*match)->right)
        {
            struct btree_node *to_delete = (*match);
            (*match) = (*match)->left;
            kfree(to_delete);
        }
        else
        {
            struct btree_node *right = (*match)->right,
                              *left = (*match)->left,
                              *right_leftmost = right,
                              *to_delete = *match;

            while (right_leftmost->left)
                right_leftmost = right_leftmost->left;

            right_leftmost->left = left;

            if (parent)
            {
                if (parent->left == *match)
                    parent->left = right;
                else if (parent->right == *match)
                    parent->right = right;
                else
                    PANIC("Something's terribly wrong!");

            }
            else
            {
                root->head = right;
            }
            kfree(to_delete);
        }
    }
    else
        PANIC("Something's terribly wrong!");
}

static void btree_int_delete_recursivly(struct btree_node *node)
{
    if (node->left)
        btree_int_delete_recursivly(node->left);
    if (node->right)
        btree_int_delete_recursivly(node->right);

    kfree(node);
}

void btree_delete_recursivly(struct btree *root, btree_keytype key)
{
    struct btree_node *parent = 0;
    struct btree_node **match = btree_int_traverse(&root->head, key, root->cmp, &parent);

    if (!*match)
        return;

    btree_int_delete_recursivly(*match);

    if (parent)
    {
        if (parent->left == *match)
            parent->left = 0;
        else if (parent->right == *match)
            parent->right = 0;
        else
            PANIC("Something's terribly wrong!");
    }
    else
    {
        root->head = 0;
    }
}

void btree_clear(struct btree *root)
{
    if (root->head)
        btree_int_delete_recursivly(root->head);

    root->head = 0;
}

struct btree *btree_create(btree_cmptype cmp)
{
    struct btree *btree = (struct btree*) kmalloc(sizeof(struct btree));
    btree->cmp = cmp;

    return btree;
}
