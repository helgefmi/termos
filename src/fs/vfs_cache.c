/* 
 * This file is part of TermOS.
 *
 * TermOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TermOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TermOS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fs/vfs.h>
#include <fs/vfs_cache.h>
#include <lib/string.h>
#include <lib/stdio.h>
#include <kernel/common.h>
#include <mm/mem.h>
#include <algo/btree.h>

struct btree *v_cache;

struct vnode *vfs_vname(char* path)
{
    char delim[] = "/";

    struct vnode *parent = 0;
    if (*path == '/')
        parent = v_root;
    else
        PANIC("path[0] should be /");

    char *key = 0,
         *part = strtok(path, delim);
    int keyLen = 0;
    while (part != NULL)
    {
        int partLen = strlen(part);
        keyLen += partLen + 1;

        char *key2 = (char*) kmalloc(keyLen + 2);
        if (key)
        {
            strcpy(key2, key);
        }

        key = key2;

        key[keyLen - partLen - 1] = '/';
        strcpy(&key[keyLen - partLen], part);
        key[keyLen + 1] = '\0';

        char *tmp = key;
        while (*tmp++)
            printf("x");

        struct vnode *node = 0; //(struct vnode*) btree_lookup(v_cache, key);
        if (node == NULL)
        {
            node = vfs_lookup(parent, part);
            if (node == NULL)
            {
                kfree(key);
                return NULL;
            }

            /* Remove when caching works */
            kfree(key);
            //btree_insert(v_cache, key, node);
        }

        parent = node;

        part = strtok(NULL, delim);
    }

    printf("%s\n", key);
    printf("hm: %x\n", key);

    return parent;
}

void vget(struct vnode *node)
{
    //printf("vget: %s->%d\n", node->v_name, node->v_count);
    ++node->v_count;
}

void vput(struct vnode *node)
{
    //printf("vput: %s->%d\n", node->v_name, node->v_count);

    ASSERT(node->v_count > 0);
    --node->v_count;

    if (!node->v_count)
    {
        ASSERT(node->v_vfs);
        kfree(node);
    }
}

int v_cache_cmp(btree_keytype a, btree_keytype b)
{
    struct vnode *v_a = (struct vnode *)a,
                 *v_b = (struct vnode *)b;

    return strcmp(v_a->v_name, v_b->v_name);
}

void vfs_cache_init()
{
    v_cache = btree_create(&v_cache_cmp);
}
