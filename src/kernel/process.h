

typedef struct {
    unsigned int id;                  // A unique ID for the process (PID)
    unsigned int *page_directory;     // Pointer to this program's private Page Directory
    unsigned int stack_physical_addr; // Where the stack actually lives in RAM
    unsigned int code_physical_addr;  // Where the code actually lives in RAMd

} process_t;