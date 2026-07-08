# include "fs.h"
#define NULL ((void *)0)

unsigned int read_fs(fs_node_t *node, unsigned int offset, unsigned int size, char *buffer) {
    // Now you can actually use 'node', 'offset', 'size', and 'buffer' in here!
    if (node->read) {
        return node->read(node, offset, size, buffer);
    }

    return 0; // Return 0 if the read function is not defined   

}

unsigned int write_fs(fs_node_t *node, unsigned int offset, unsigned int size, char *buffer) {
    if (node->write) {
        return node->write(node, offset, size, buffer);
    }

    return 0; // Return 0 if the write function is not defined
}

// open fs and closed fs
void open_fs(fs_node_t *node) {
    if (node->open) {
        node->open(node);
    }
}

void close_fs(fs_node_t *node) {
    if (node->close) {
        node->close(node);
    }
}