#include "initrd.h"
#include "fs.h"
#include "string.h"

unsigned int initrd_blob_start;

// Assuming your allocator header is included here

// We need these to stick around permanently in RAM so the VFS can access them later!
fs_node_t *initrd_root;             // The Trunk (Root Directory)
fs_node_t *initrd_file_nodes;       // The Array of Leaves (All the files)
unsigned int initrd_num_files;      // How many files we have

// We must declare these at the top so we can assign them to the pointers.
// (You will write the actual logic inside these later)
unsigned int initrd_read(fs_node_t *node, unsigned int offset, unsigned int size, char *buffer);
struct dirent *initrd_readdir(fs_node_t *node, unsigned int index);
fs_node_t *initrd_finddir(fs_node_t *node, char *name);


fs_node_t *initialize_initrd(unsigned int location) {

    // Read the Main Header
    main_header_t *header = (main_header_t *)location;
    initrd_num_files = header->num_of_files;

    // Find exactly where the file headers start in physical memory
    file_header_t *file_headers = (file_header_t *)(location + sizeof(main_header_t));

    // ALLOCATE THE TRUNK (The Root Directory)
    initrd_root = (fs_node_t *)malloc(sizeof(fs_node_t));
    
    // Copy the name "initrd" into the root node
    strncpy(initrd_root->name, "initrd", sizeof(initrd_root->name) - 1);
    initrd_root->name[sizeof(initrd_root->name) - 1] = '\0';
    
    initrd_root->flags = 2; // e.g., 2 = FS_DIRECTORY flag
    initrd_root->length = 0;
    initrd_root->inode = 0;
    
    // Assign directory-specific functions
    initrd_root->read = NULL;       // Directories can't be read like normal files
    initrd_root->write = NULL;
    initrd_root->open = NULL;
    initrd_root->close = NULL;
    initrd_root->readdir = initrd_readdir; // Point to custom readdir
    initrd_root->finddir = initrd_finddir; // Point to custom finddir


    // ALLOCATE THE LEAVES (The permanent array of files)
    initrd_file_nodes = (fs_node_t *)malloc(sizeof(fs_node_t) * initrd_num_files);

    // LOOP AND POPULATE
    for (unsigned int i = 0; i < initrd_num_files; i++) {
        
        file_header_t *file_header = &file_headers[i];
        
        // Grab the current node from our permanently allocated array
        fs_node_t *node = &initrd_file_nodes[i];

        strncpy(node->name, file_header->file_name, sizeof(node->name) - 1);
        node->name[sizeof(node->name) - 1] = '\0'; 
        
        node->flags = 1; // e.g., 1 = FS_FILE flag
        node->length = file_header->file_length;
        node->inode = i; // Assign unique inode number

        // THE CRITICAL FIX: Assign the actual custom read function, not NULL!
        node->read = initrd_read; 
        node->write = NULL;   // It's a read-only RAM disk, so write is NULL
        node->open = NULL; 
        node->close = NULL; 
        node->readdir = NULL; // Files don't have files inside them
        node->finddir = NULL; // Files can't be searched like directories
    }
    
    // RETURN THE TRUNK
    // Hand this back to kmain so it can plug it into your VFS switchboard!
    return initrd_root;
}


unsigned int initrd_read(fs_node_t *node, unsigned int offset, unsigned int size, char *buffer){

    //a BLOB is a binary large object , BLOB is the thing that actual data like file header and main header are stored .
    
    // 1. Jump to the raw QEMU blob, skip the main header, and grab this file's header using the inode index
    file_header_t *blob_headers = (file_header_t *)(initrd_blob_start + sizeof(main_header_t));
    file_header_t *file_header = &blob_headers[node->inode];
    
    // 2. Calculate the starting address of the file data in the raw blob memory
    unsigned int file_data_start = initrd_blob_start + file_header->starting_offset;

    // Ensure we don't read beyond the end of the file
    if (offset > file_header->file_length) {
        return 0; // Offset is beyond the end of the file
    }
    
    if (offset + size > file_header->file_length) {
        size = file_header->file_length - offset; // Adjust size to read only up to the end of the file
    }

    // Perform the actual read operation
    memcpy(buffer, (char *)(file_data_start + offset), size);

    return size; // Return the number of bytes read
}

// We need a single dirent_t struct to hold our answer. 
// We make it 'static' so it doesn't get destroyed when the function ends, 
// allowing us to safely return a pointer to it!
static dirent_t dirent; 

struct dirent *initrd_readdir(fs_node_t *node, unsigned int index) {
    // 1. Safety Check: Did the OS ask for an index that is out of bounds?
    // If we only have 5 files, and it asks for index 5 (the 6th file), we return NULL to stop the loop.
    if (index >= initrd_num_files) {
        return NULL; 
    }

    // 2. Grab the heavyweight file node from our global array
    fs_node_t *target_file = &initrd_file_nodes[index];

    // 3. Copy the name into our lightweight dirent struct
    // (You will need to implement strcpy in your string.c file!)
    strcpy(dirent.name, target_file->name);
    
    // 4. Copy the inode (the ID number)
    dirent.inode = target_file->inode;

    // 5. Return the pointer to our populated Post-it note
    return &dirent;
}