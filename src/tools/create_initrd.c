#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include "create_initrd.h"

/* globals {{{ */
struct stat stat_buf;
FILE* output;
u64 total_nodes = 0;
u64 offset;
/* }}} */

void print_syntax() // {{{
{
    printf("create_initrd <root dir> <destination>\n");
} // }}}

/* read_header_* {{{ */
initrd_node_t *read_header_file(char* path)
{
    initrd_node_t *node = (initrd_node_t*) calloc(sizeof(initrd_node_t), 1);
    node->type = TYPE_FILE;
    node->inode = ++total_nodes;

    strncpy(node->name, path, NAME_LEN);

    FILE* fp = fopen(node->name, "r");
    fseek(fp, 0, SEEK_END);
    node->size = ftell(fp);
    fclose(fp);

    return node;
}

initrd_node_t *read_header_dir(char* path, initrd_node_t *parent)
{
    chdir(path);

    initrd_node_t *node = (initrd_node_t*) calloc(sizeof(initrd_node_t), 1);
    node->type = TYPE_DIR;
    node->size = 0;
    node->data = (u64)calloc(sizeof(u32) * 1024, 1);

    strncpy(node->name, path, NAME_LEN);
    node->inode = ++total_nodes;


    DIR* dir = opendir(".");
    struct dirent *dirent;

    while ((dirent = readdir(dir)) != 0)
    {
        if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, ".."))
            continue;

        if (stat(dirent->d_name, &stat_buf))
        {
            perror("Error fetching info about entry");
            exit(3);
        }

        initrd_node_t *child;
        if (S_ISDIR(stat_buf.st_mode))
        {
            child = read_header_dir(dirent->d_name, node);
        }
        else if (S_ISREG(stat_buf.st_mode))
        {
            child = read_header_file(dirent->d_name);
        }
        ((u64 *) node->data)[node->size++] = (u64)child;
    }

    if (parent)
        chdir("..");
    return node;
}
/* }}} */
/* write_header_* {{{ */
void write_node(initrd_node_t *node)
{
    fwrite(&node->inode, sizeof(u32), 1, output);
    fwrite(&node->type, sizeof(u8), 1, output);
    fwrite(&node->size, sizeof(u32), 1, output);
    u32 data = (u32)node->data;
    fwrite(&data, sizeof(u32), 1, output);
    fwrite(&node->name, sizeof(u8) * NAME_LEN, 1, output);
}

void write_header_file(initrd_node_t *node)
{
    node->data = (u64) offset;
    offset += node->size;

    write_node(node);
}

void write_header_dir(initrd_node_t *node)
{
    u64 tmp = node->data;
    node->data = (u64) offset;
    write_node(node);
    node->data = tmp;

    offset += node->size * sizeof(u32);

    u32 i;
    for (i = 0; i < node->size; ++i)
    {
        initrd_node_t *child = ((initrd_node_t **)node->data)[i];
        if( child->type == TYPE_FILE)
        {
            write_header_file(child);
        }
        else
        {
            write_header_dir(child);
        }
    }

}
/* }}} */
/* write_data_* {{{ */
void write_data_file(initrd_node_t *node)
{
    node->data = (u64)offset;
    offset += node->size;

    FILE *input;
    
    if (!(input = fopen(node->name, "r")))
    {
        perror("Error opening file");
        exit(5);
    }

    char buf[1024];
    while (node->size)
    {
        int bytes = fread(buf, 1, node->size, input);
        if (!bytes)
        {
            perror("Error writing file");
        exit(6);
        }
        node->size -= bytes;
        fwrite(buf, 1, bytes, output);
    }
}

void write_data_dir(initrd_node_t *node)
{
    chdir(node->name);
    u32 i;
    for (i = 0; i < node->size; ++i)
    {
        initrd_node_t *child = ((initrd_node_t **)node->data)[i];
        fwrite(&child->inode, sizeof(u32), 1, output);
    }

    for (i = 0; i < node->size; ++i)
    {
        initrd_node_t *child = ((initrd_node_t **)node->data)[i];
        if( child->type == TYPE_FILE)
        {
            write_data_file(child);
        }
        else
        {
            write_data_dir(child);
        }
    }
    chdir("..");
}
/* }}} */

int main(int argc, char **argv) // {{{
{
    if (argc != 3)
    {
        fprintf(stderr, "Invalid argumentcount\n");
        print_syntax();
        exit(1);
    }

    char *src = argv[1],
         *dst = argv[2];

    initrd_node_t *root;

    if(!(output = fopen(dst, "w")))
    {
        perror("Couldn't open destination");
        exit(5);
    }

    if (!stat(src, &stat_buf))
    {
        if (S_ISDIR(stat_buf.st_mode))
        {
            chdir(src);
            root = read_header_dir(".", 0);
        }
        else
        {
            fprintf(stderr, "Argument must be a directory\n");
            print_syntax();
            exit(4);
        }
    }
    else
    {
        perror("Couldn't read path\n");
        print_syntax();
        exit(2);
    }

    fseek(output, 4, 0);

    fwrite(&total_nodes, sizeof(u32), 1, output);
    write_header_dir(root);
    write_data_dir(root);

    u32 totalsize = ftell(output);
    rewind(output);
    fwrite(&totalsize, sizeof(u32), 1, output);
    fclose(output);

    return 0;
} // }}}
