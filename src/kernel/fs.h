// Forward Declarations (The Promise) for safety bruh . 
struct fs_node;
struct dirent;

// Typedefs with _t to prevent naming collisions . (you know since it is a TYPE)
// Notice we use 'struct fs_node*' and 'struct dirent*' here
typedef unsigned int (*read_type_t)(struct fs_node*, unsigned int, unsigned int, char*);
typedef unsigned int (*write_type_t)(struct fs_node*, unsigned int, unsigned int, char*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);

//  Pointer returns so we can return NULL on failure/end of directory
// to use NULL . we must return pointers? 
typedef struct dirent * (*readdir_type_t)(struct fs_node*, unsigned int);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*, char*);

//  The actual struct definitions
typedef struct fs_node {
    char name[128];
    unsigned int flags;
    unsigned int length;
    unsigned int inode;
    
    // Using the safe type names!
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
} fs_node_t; // Appended _t here as a best practice for the final type

typedef struct dirent {
    char name[128];
    unsigned int inode;
} dirent_t;