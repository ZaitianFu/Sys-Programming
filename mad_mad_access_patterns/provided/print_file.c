/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "tree.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void printHelp();
BinaryTreeNode *readNode(const char *data, uint32_t offset);
void traverseTree(const char *data, uint32_t node_offset);

int main(int argc, char **argv) {
    if (argc != 2)
        printHelp();

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        return 1;
    }

    struct stat filestats;
    fstat(fd, &filestats);

    const char *data =
        mmap(NULL, filestats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!data) {
        fprintf(stderr, "Failed to map %s\n", argv[1]);
        return 1;
    }
    close(fd);

    uint32_t node_offset = BINTREE_ROOT_NODE_OFFSET;

    traverseTree(data, node_offset);

    munmap((void *)data, filestats.st_size);

    return 0;
}

void printHelp() {
    printf("\n  traverse <data_file>\n\n");
    exit(1);
}

BinaryTreeNode *readNode(const char *data, uint32_t offset) {
    return (BinaryTreeNode *)(data + offset);
}

void traverseTree(const char *data, uint32_t offset) {
    if (offset == 0)
        return;

    BinaryTreeNode *node = readNode(data, offset);

    traverseTree(data, node->left_child);

    printf("offset %d: \"%s\", count=%u, price=$%.2f, left=%u, right=%u\n",
           offset, node->word, node->count, node->price, node->left_child,
           node->right_child);

    traverseTree(data, node->right_child);
}
