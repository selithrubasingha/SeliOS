# include "fs.h"

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



// readdir and finddir
dirent_t *readdir_fs(fs_node_t *node, unsigned int index) {
    if (node->readdir) {
        return node->readdir(node, index);
    }       

    return NULL; // Return NULL if the readdir function is not defined
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
    if (node->finddir) {
        return node->finddir(node, name);
    }

    return NULL; // Return NULL if the finddir function is not defined
}