# include "fs.h"


unsigned int read_fs(fs_node_t *node, unsigned int offset, unsigned int size, char *buffer) {
    // Now you can actually use 'node', 'offset', 'size', and 'buffer' in here!
    if (node->read) {
        return node->read(node, offset, size, buffer);
    }

    return 0; // Return 0 if the read function is not defined   

}